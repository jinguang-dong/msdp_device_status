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

#include "devicestatus_sensor_mock.h"

#include <string>
#include <cerrno>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <linux/netlink.h>

#include "devicestatus_common.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr int32_t TIMER_INTERVAL = 3;
constexpr int32_t ERR_INVALID_FD = -1;
constexpr int32_t ERR_NG = -1;
DeviceStatusSensorMock* g_sensorMock = nullptr;
} // namespace

bool DeviceStatusSensorMock::Init()
{
    DEV_HILOGI(SERVICE, "Enter");
    if (dataParse_ == nullptr) {
        dataParse_ = std::make_unique<DeviceStatusDataParse>();
        dataParse_->CreateJsonFile();
    }
    InitMockStore();
    InitTimer();
    StartThread();
    DEV_HILOGI(SERVICE, "Exit");
    return true;
}

void DeviceStatusSensorMock::InitMockStore() {}

ErrCode DeviceStatusSensorMock::RegisterCallback(const std::shared_ptr<SensorHdiCallback>& callback)
{
    std::lock_guard lock(mutex_);
    callback_ = callback;
    return ERR_OK;
}

ErrCode DeviceStatusSensorMock::UnregisterCallback()
{
    std::lock_guard lock(mutex_);
    callback_ = nullptr;
    return ERR_OK;
}

ErrCode DeviceStatusSensorMock::Enable(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    int32_t item = int32_t(type);
    enabledType_[item] = int32_t(TypeValue::VALID);
    Init();
    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DeviceStatusSensorMock::Disable(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    scFlag_ = false;
    CloseTimer();
    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DeviceStatusSensorMock::DisableCount(const Type& type)
{
    DEV_HILOGD(SERVICE, "Enter");
    enabledType_[type] = int32_t(TypeValue::INVALID);
    dataParse_->DisableCount(type);
    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DeviceStatusSensorMock::NotifyMsdpImpl(const Data& data)
{
    DEV_HILOGI(SERVICE, "Enter");
    if (g_sensorMock == nullptr) {
        DEV_HILOGI(SERVICE, "g_sensorMock is nullptr");
        return ERR_NG;
    }
    if (g_sensorMock->GetCallbackImpl() == nullptr) {
        DEV_HILOGI(SERVICE, "callbacksImpl is nullptr");
        return ERR_NG;
    }
    g_sensorMock->GetCallbackImpl()->OnSensorHdiResult(data);

    return ERR_OK;
}

void DeviceStatusSensorMock::InitTimer()
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
    callbacks_.insert(std::make_pair(timerFd_, &DeviceStatusSensorMock::TimerCallback));
    if (RegisterTimerCallback(timerFd_, EVENT_TIMER_FD)) {
        DEV_HILOGE(SERVICE, "register timer fd failed");
    }
}

void DeviceStatusSensorMock::SetTimerInterval(int32_t interval)
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

void DeviceStatusSensorMock::CloseTimer()
{
    DEV_HILOGD(SERVICE, "Enter");
    close(timerFd_);
    DEV_HILOGD(SERVICE, "Exit");
}

void DeviceStatusSensorMock::TimerCallback()
{
    uint64_t timers;
    if (read(timerFd_, &timers, sizeof(timers)) == -1) {
        DEV_HILOGE(SERVICE, "read timer fd failed");
        return;
    }
    GetDeviceStatusData();
}

void DeviceStatusSensorMock::GetDeviceStatusData()
{
    Data Data;
    for (int32_t n = int(Type::TYPE_STILL); n < Type::TYPE_MAX; ++n) {
        if (enabledType_[n] == TypeValue::VALID) {
            Type type = Type(n);
            dataParse_->ParseDeviceStatusData(Data, type);
            NotifyMsdpImpl(Data);
        }
    }
}

int32_t DeviceStatusSensorMock::RegisterTimerCallback(const int32_t fd, const EventType et)
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
        DEV_HILOGE(SERVICE, "epoll_ctl failed, error num:%{public}d", errno);
        return ERR_NG;
    }

    return ERR_OK;
}

void DeviceStatusSensorMock::SubscribeHallSensor()
{
    DEV_HILOGI(SERVICE, "Enter");
    DEV_HILOGI(SERVICE, "Exit");
}

void DeviceStatusSensorMock::UnSubscribeHallSensor()
{
    DEV_HILOGI(SERVICE, "Enter");
    DEV_HILOGI(SERVICE, "Exit");
}

void DeviceStatusSensorMock::StartThread()
{
    DEV_HILOGI(SERVICE, "Enter");
    std::make_unique<std::thread>(&DeviceStatusSensorMock::LoopingThreadEntry, this)->detach();
}

void DeviceStatusSensorMock::LoopingThreadEntry()
{
    if (callbacks_.empty()) {
        DEV_HILOGD(SERVICE, "callbacks_ is empty");
        return;
    }
    size_t cbct = callbacks_.size();
    struct epoll_event events[cbct];

    while (alive_) {
        int32_t timeout = -1;

        int32_t nevents = epoll_wait(epFd_, events, cbct, timeout);
        if (nevents == -1) {
            continue;
        }
        for (int32_t n = 0; n < nevents; ++n) {
            if (events[n].data.ptr) {
                DeviceStatusSensorMock *func = const_cast<DeviceStatusSensorMock *>(this);
                (callbacks_.find(events[n].data.fd)->second)(func);
            }
        }
    }
}

extern "C" ISensor *Create(void)
{
    DEV_HILOGI(SERVICE, "Enter");
    g_sensorMock = new(std::nothrow) DeviceStatusSensorMock();
    return g_sensorMock;
}

extern "C" void Destroy(const ISensor* algorithm)
{
    DEV_HILOGI(SERVICE, "Enter");
    delete algorithm;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
