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

#include "motion_msdp_mock.h"
#include <cerrno>
#include <linux/netlink.h>
#include <string>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "motion_common.h"

namespace OHOS {
namespace Msdp {
namespace {
constexpr int TIMER_INTERVAL = 1;
constexpr int ERR_INVALID_FD = -1;
constexpr int32_t ERR_NG = -1;
MotionMsdpMock* g_rdb;
}

bool MotionMsdpMock::Init()
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "MotionMsdpRdbInit: enter");
    if (dataParse_ == nullptr) {
        dataParse_ = std::make_unique<MotionDataParse>();
        dataParse_->CreateJsonFile();
    }

    InitTimer();
    StartThread();
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "MotionMsdpRdbInit: exit");
    return true;
}


ErrCode MotionMsdpMock::RegisterCallback(std::shared_ptr<MsdpAlgorithmCallback>& callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    callbacksImpl_ = callback;
    return ERR_OK;
}

ErrCode MotionMsdpMock::UnregisterCallback()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    callbacksImpl_ = nullptr;
    return ERR_OK;
}

ErrCode MotionMsdpMock::Enable(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    int motionItem = int(type);
    in_type[motionItem] = int(MotionDataUtils::MotionTypeValue::VALID);
    Init();
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Exit");
    return ERR_OK;
}

ErrCode MotionMsdpMock::Disable(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    CloseTimer();
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Exit");
    return ERR_OK;
}

ErrCode MotionMsdpMock::DisableCount(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    in_type[type] = int(MotionDataUtils::MotionTypeValue::INVALID);
    dataParse_->DisableCount(type);
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Exit");
    return ERR_OK;
}

ErrCode MotionMsdpMock::NotifyMsdpImpl(MotionDataUtils::MotionData& data)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (g_rdb->GetCallbacksImpl() == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "callbacksImpl is nullptr");
        return ERR_NG;
    }
    g_rdb->GetCallbacksImpl()->OnResult(data);

    return ERR_OK;
}

void MotionMsdpMock::GetMotionData()
{

    for (int n = 0; n <= MotionDataUtils::MotionType::TYPE_WRIST_TILT; n++) {
        if (in_type[n] == MotionDataUtils::MotionTypeValue::VALID) {
            MotionDataUtils::MotionData motionData;
            MotionDataUtils::MotionType type = MotionDataUtils::MotionType(n);
            dataParse_->ParseMotionData(motionData, type);
            NotifyMsdpImpl(motionData);
        }
    }
}

void MotionMsdpMock::InitTimer()
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    epFd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epFd_ == -1) {
        MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "create epoll fd fail.");
    }
    timerFd_ = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timerFd_ == ERR_INVALID_FD) {
        MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "create timer fd fail.");
    }
    SetTimerInterval(TIMER_INTERVAL);
    fcntl(timerFd_, F_SETFL, O_NONBLOCK);
    callbacks_.insert(std::make_pair(timerFd_, &MotionMsdpMock::TimerCallback));
    if (RegisterTimerCallback(timerFd_, EVENT_TIMER_FD)) {
        MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "register timer fd fail.");
    }
}

void MotionMsdpMock::SetTimerInterval(int interval)
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    struct itimerspec itval;

    if (timerFd_ == ERR_INVALID_FD) {
        return;
    }

    timerInterval_ = interval;

    if (interval < 0) {
        interval = 0;
    }

    itval.it_interval.tv_sec = interval;
    itval.it_interval.tv_nsec = 0;
    itval.it_value.tv_sec = interval;
    itval.it_value.tv_nsec = 0;

    if (timerfd_settime(timerFd_, 0, &itval, nullptr) == -1) {
        MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "set timer failed");
    }

    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    return;
}

void MotionMsdpMock::CloseTimer()
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Enter");
    close(timerFd_);
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Exit");
}

void MotionMsdpMock::TimerCallback()
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    unsigned long long timers;
    if (read(timerFd_, &timers, sizeof(timers)) == -1) {
        MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "read timer fd fail.");
        return;
    }
    GetMotionData();
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "exit");
}

int MotionMsdpMock::RegisterTimerCallback(const int fd, const EventType et)
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    struct epoll_event ev;

    ev.events = EPOLLIN;
    if (et == EVENT_TIMER_FD) {
        ev.events |= EPOLLWAKEUP;
    }

    ev.data.ptr = reinterpret_cast<void*>(this);
    ev.data.fd = fd;
    if (epoll_ctl(epFd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
        MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "epoll_ctl failed, error num =%{public}d", errno);
        return -1;
    }

    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "exit");
    return 0;
}

void MotionMsdpMock::StartThread()
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    std::make_unique<std::thread>(&MotionMsdpMock::LoopingThreadEntry, this)->detach();
}

void MotionMsdpMock::LoopingThreadEntry()
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    int nevents = 0;
    size_t cbct = callbacks_.size();
    struct epoll_event events[cbct];

    while (true) {
        int timeout = -1;

        nevents = epoll_wait(epFd_, events, cbct, timeout);
        if (nevents == -1) {
            continue;
        }
        for (int n = 0; n < nevents; ++n) {
            if (events[n].data.ptr) {
                MotionMsdpMock *func = const_cast<MotionMsdpMock *>(this);
                (callbacks_.find(events[n].data.fd)->second)(func);
            }
        }
    }
}

extern "C" MotionMsdpInterface *Create(void)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    g_rdb = new MotionMsdpMock();
    return g_rdb;
}

extern "C" void Destroy(const MotionMsdpInterface* algorithm)
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    delete algorithm;
}
}
}
