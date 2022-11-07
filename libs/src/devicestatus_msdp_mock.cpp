/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cerrno>
#include <string>
#include <unistd.h>

#include <linux/netlink.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>

#include "devicestatus_common.h"
#include "devicestatus_msdp_mock.h"

using namespace OHOS::NativeRdb;
namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
const std::string DATABASE_NAME = "/data/MsdpStub.db";
constexpr int32_t TIMER_INTERVAL = 3;
constexpr int32_t ERR_INVALID_FD = -1;
constexpr int32_t READ_RDB_WAIT_TIME = 30;
std::unique_ptr<DeviceStatusMsdpMock> g_msdpRdb = std::make_unique<DeviceStatusMsdpMock>();
constexpr int32_t ERR_NG = -1;
DeviceStatusMsdpMock* g_rdb = nullptr;
}

bool DeviceStatusMsdpMock::Init()
{
    DEV_HILOGD(SERVICE, "DeviceStatusMsdpMockInit: Enter");
    InitRdbStore();
    InitTimer();
    StartThread();
    DEV_HILOGD(SERVICE, "DeviceStatusMsdpMockInit: Exit");
    return true;
}

void DeviceStatusMsdpMock::InitRdbStore() {}

ErrCode DeviceStatusMsdpMock::RegisterCallback(std::shared_ptr<MsdpAlgoCallback> callback)
{
    std::lock_guard lock(mutex_);
    callback_ = callback;
    return ERR_OK;
}

ErrCode DeviceStatusMsdpMock::UnregisterCallback()
{
    std::lock_guard lock(mutex_);
    callback_ = nullptr;
    return ERR_OK;
}

ErrCode DeviceStatusMsdpMock::Enable(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    Init();
    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DeviceStatusMsdpMock::Disable(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    scFlag = false;
    CloseTimer();
    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DeviceStatusMsdpMock::NotifyMsdpImpl(const Data& data)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (g_rdb == nullptr) {
        DEV_HILOGE(SERVICE, "g_rdb is nullptr");
        return ERR_NG;
    }
    if (g_rdb->GetCallbacksImpl() == nullptr) {
        DEV_HILOGE(SERVICE, "callbacksImpl is nullptr");
        return ERR_NG;
    }
    g_rdb->GetCallbacksImpl()->OnResult(data);

    return ERR_OK;
}

Data DeviceStatusMsdpMock::SaveRdbData(const Data& data)
{
    for (auto iter = rdbDataMap_.begin(); iter != rdbDataMap_.end(); ++iter) {
        if (iter->first == data.type) {
            if (iter->second != data.value) {
                notifyFlag_ = true;
                iter->second = data.value;
            }
            DEV_HILOGI(SERVICE, "data is not changed");
            return data;
        }
    }

    rdbDataMap_.insert(std::make_pair(data.type, data.value));
    notifyFlag_ = true;

    DEV_HILOGI(SERVICE, "devicestatusType_ : %{public}d, devicestatusStatus_ : %{public}d",
        devicestatusType_, devicestatusStatus_);

    return data;
}

int32_t DeviceStatusMsdpMock::TriggerData(const std::unique_ptr<NativeRdb::ResultSet> &resultSet)
{
    if (resultSet == nullptr) {
        DEV_HILOGE(SERVICE, "resultSet is nullptr");
        return ERR_NG;
    }
    int32_t columnIndex;
    int32_t ret = resultSet->GetColumnIndex("ID", columnIndex);
    DEV_HILOGI(SERVICE, "TriggerDatabaseObserver GetColumnIndex = %{public}d", columnIndex);
    if (ret != ERR_OK) {
        DEV_HILOGE(SERVICE, "CheckID: GetColumnIndex failed");
        return ERR_NG;
    }
    int32_t intVal;
    ret = resultSet->GetInt(columnIndex, intVal);
    DEV_HILOGI(SERVICE, "ret = %{public}d, id = %{public}d", ret, intVal);
    if (ret != ERR_OK) {
        DEV_HILOGE(SERVICE, "CheckID: GetValue failed");
        return ERR_NG;
    }

    ret = resultSet->GetColumnIndex("DEVICESTATUS_TYPE", columnIndex);
    DEV_HILOGI(SERVICE, "DEVICESTATUS_TYPE GetColumnIndex = %{public}d", columnIndex);
    if (ret != ERR_OK) {
        DEV_HILOGE(SERVICE, "CheckDeviceStatusType: GetColumnIndex failed");
        return ERR_NG;
    }
    ret = resultSet->GetInt(columnIndex, intVal);
    DEV_HILOGI(SERVICE, "ret = %{public}d, Type = %{public}d", ret, intVal);
    devicestatusType_ = intVal;
    if (ret != ERR_OK) {
        DEV_HILOGE(SERVICE, "CheckDeviceStatusType: GetValue failed");
        return ERR_NG;
    }

    ret = resultSet->GetColumnIndex("DEVICESTATUS_STATUS", columnIndex);
    DEV_HILOGI(SERVICE, "DEVICESTATUS_STATUS GetColumnIndex = %{public}d", columnIndex);
    if (ret != ERR_OK) {
        DEV_HILOGE(SERVICE, "CheckDeviceStatusStatus: GetColumnIndex failed");
        return ERR_NG;
    }
    ret = resultSet->GetInt(columnIndex, intVal);
    DEV_HILOGI(SERVICE, "ret = %{public}d, DeviceStatusStatus = %{public}d", ret, intVal);
    devicestatusStatus_ = intVal;
    if (ret != ERR_OK) {
        DEV_HILOGE(SERVICE, "CheckDeviceStatusStatus: GetValue failed");
        return ERR_NG;
    }

    return ERR_OK;
}

int32_t DeviceStatusMsdpMock::TriggerDatabaseObserver()
{
    DEV_HILOGD(SERVICE, "Enter");

    if (store_ == nullptr) {
        sleep(READ_RDB_WAIT_TIME);
        InitRdbStore();
        return ERR_NG;
    }

    std::unique_ptr<ResultSet> resultSet =
        store_->QuerySql("SELECT * FROM DEVICESTATUSSENSOR WHERE ID = (SELECT max(ID) from DEVICESTATUSSENSOR)");

    if (resultSet == nullptr) {
        DEV_HILOGE(SERVICE, "database is not exist");
        return ERR_NG;
    }

    int32_t ret = resultSet->GoToFirstRow();
    DEV_HILOGI(SERVICE, "GoToFirstRow = %{public}d", ret);
    if (ret != ERR_OK) {
        sleep(READ_RDB_WAIT_TIME);
        DEV_HILOGE(SERVICE, "database observer is null");
        return ERR_NG;
    }

    if (TriggerData(resultSet) != ERR_OK) {
        DEV_HILOGE(SERVICE, "triger data failed");
        return ERR_NG;
    }

    ret = resultSet->Close();
    if (ret != ERR_OK) {
        DEV_HILOGE(SERVICE, "close database observer failed");
        return ERR_NG;
    }

    Data data;
    data.type = static_cast<Type>(devicestatusType_);
    data.value = (OnChangedValue)devicestatusStatus_;

    SaveRdbData(data);
    DEV_HILOGI(SERVICE, "notifyFlag_ is %{public}d", notifyFlag_);
    if (notifyFlag_) {
        NotifyMsdpImpl(data);
        notifyFlag_ = false;
    }

    return ERR_OK;
}

void DeviceStatusMsdpMock::InitTimer()
{
    DEV_HILOGD(SERVICE, "Enter");
    epFd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epFd_ == -1) {
        DEV_HILOGE(SERVICE, "create epoll fd failed");
        return;
    }
    timerFd_ = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timerFd_ == ERR_INVALID_FD) {
        DEV_HILOGE(SERVICE, "create timer fd failed");
        return;
    }
    SetTimerInterval(TIMER_INTERVAL);
    fcntl(timerFd_, F_SETFL, O_NONBLOCK);
    callbacks_.insert(std::make_pair(timerFd_, &DeviceStatusMsdpMock::TimerCallback));
    if (RegisterTimerCallback(timerFd_, EVENT_TIMER_FD)) {
        DEV_HILOGE(SERVICE, "register timer fd failed");
    }
}

void DeviceStatusMsdpMock::SetTimerInterval(int32_t interval)
{
    struct itimerspec itval;

    if (timerFd_ == ERR_INVALID_FD) {
        DEV_HILOGE(SERVICE, "create timer fd failed");
        return;
    }
    if (interval != 0) {
        timerInterval_ = interval;
    }
    if (interval < 0) {
        interval = 0;
    }
    itval.it_interval.tv_sec = interval;
    itval.it_interval.tv_nsec = 0;
    itval.it_value.tv_sec = interval;
    itval.it_value.tv_nsec = 0;
    if (timerfd_settime(timerFd_, 0, &itval, nullptr) == -1) {
        DEV_HILOGE(SERVICE, "set timer failed");
    }
}

void DeviceStatusMsdpMock::CloseTimer()
{
    DEV_HILOGD(SERVICE, "Enter");
    close(timerFd_);
    DEV_HILOGD(SERVICE, "Exit");
}

void DeviceStatusMsdpMock::TimerCallback()
{
    unsigned long long timers;
    if (read(timerFd_, &timers, sizeof(timers)) == -1) {
        DEV_HILOGE(SERVICE, "read timer fd failed");
        return;
    }
    TriggerDatabaseObserver();
}

int32_t DeviceStatusMsdpMock::RegisterTimerCallback(const int32_t fd, const EventType et)
{
    DEV_HILOGD(SERVICE, "Enter");
    struct epoll_event ev;

    ev.events = EPOLLIN;
    if (et == EVENT_TIMER_FD) {
        ev.events |= EPOLLWAKEUP;
    }

    ev.data.ptr = reinterpret_cast<void*>(this);
    ev.data.fd = fd;
    if (epoll_ctl(epFd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
        DEV_HILOGE(SERVICE, "epoll_ctl failed, error num =%{public}d", errno);
        return ERR_NG;
    }

    return ERR_OK;
}

void DeviceStatusMsdpMock::StartThread()
{
    DEV_HILOGD(SERVICE, "Enter");
    std::make_unique<std::thread>(&DeviceStatusMsdpMock::LoopingThreadEntry, this)->detach();
}

void DeviceStatusMsdpMock::LoopingThreadEntry()
{
    size_t cbct = callbacks_.size();
    struct epoll_event events[cbct];
    while (scFlag) {
        int32_t timeout = -1;
        int32_t nevents = epoll_wait(epFd_, events, cbct, timeout);
        if (nevents == -1) {
            continue;
        }
        if (scFlag == false) {
            break;
        }
        GetEpollEvents();
    }
}

void DeviceStatusMsdpMock::GetEpollEvents()
{
    for (int32_t n = 0; n < nevents; ++n) {
        if (events[n].data.ptr) {
            DeviceStatusMsdpMock *func = const_cast<DeviceStatusMsdpMock *>(this);
            if (callbacks_.empty()) {
                return;
            }
            (callbacks_.find(events[n].data.fd)->second)(func);
        }
    }
}

int32_t InsertOpenCallback::OnCreate(RdbStore &store)
{
    DEV_HILOGD(SERVICE, "Enter");
    return ERR_OK;
}

int32_t InsertOpenCallback::OnUpgrade(RdbStore &store, int32_t oldVersion, int32_t newVersion)
{
    DEV_HILOGD(SERVICE, "Enter");
    return ERR_OK;
}

extern "C" IMsdp *Create(void)
{
    DEV_HILOGD(SERVICE, "Enter");
    g_rdb = new(std::nothrow) DeviceStatusMsdpMock();
    return g_rdb;
}

extern "C" void Destroy(const IMsdp* algorithm)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (algorithm == nullptr) {
        delete algorithm;
    }
    DEV_HILOGD(SERVICE, "leave");
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
