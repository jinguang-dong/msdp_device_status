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

#include "motion_sensor_mock.h"
#include <cerrno>
#include <linux/netlink.h>
#include <string>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

// #include "dummy_values_bucket.h"
#include "motion_common.h"

namespace OHOS {
namespace Msdp {
namespace {
const int TIMER_INTERVAL = 1;
const int ERR_INVALID_FD = -1;
const int32_t ERR_NG = -1;
MotionSensorMock* g_rdb;
}

bool MotionSensorMock::Init()
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "MotionSensorRdbInit: enter");
    InitTimer();
    StartThread();
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "MotionSensorRdbInit: exit");
    return true;
}

ErrCode MotionSensorMock::RegisterCallback(std::shared_ptr<MotionSensorHdiCallback>& callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    callbacksImpl_ = callback;
    return ERR_OK;
}

ErrCode MotionSensorMock::UnregisterCallback()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    callbacksImpl_ = nullptr;
    return ERR_OK;
}

ErrCode MotionSensorMock::Enable(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    int motionItem = int(type);
    in_type[motionItem] = int(MotionDataUtils::MotionTypeValue::VALID);
    Init();
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Exit");
    return ERR_OK;
}

ErrCode MotionSensorMock::Disable(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    int motionItem = int(type);
    in_type[motionItem] = int(MotionDataUtils::MotionTypeValue::INVALID);
    CloseTimer();
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Exit");
    return ERR_OK;
}

ErrCode MotionSensorMock::DisableCount(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    in_type[type] = int(MotionDataUtils::MotionTypeValue::INVALID);
    dataParse_->DisableCount(type);
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Exit");
    return ERR_OK;
}

std::vector<MsdpAlgorithmCapability> MotionSensorMock::GetCapability(const MotionDataUtils::MotionType& type)
{
    std::vector<MsdpAlgorithmCapability> capabilities;
    MsdpAlgorithmCapability capability;
    capability.type = type;
    capability.version = "1.0.0";
    capability.vendor = "com.vector";
    if (type >= MotionDataUtils::MotionType::TYPE_PICKUP && type <= MotionDataUtils::MotionType::TYPE_FLIP) {
        capability.description = "ProximityAlgorithm";
    } else {
        capability.description = "RemovedAlgorithm";
    }
    capability.mediumMap.insert(std::make_pair(TransmissionMedium::WIFI, MediumPriority::HIGH));
    capability.mediumMap.insert(std::make_pair(TransmissionMedium::ETHERNET, MediumPriority::MIDDLE));
    capabilities.push_back(capability);
    return capabilities;
}

ErrCode MotionSensorMock::NotifyMsdpImpl(MotionDataUtils::MotionData& data)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (g_rdb->GetCallbacksImpl() == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "callbacksImpl is nullptr");
        return ERR_NG;
    }
    g_rdb->GetCallbacksImpl()->OnResult(data);
    return ERR_OK;
}

void MotionSensorMock::InitTimer()
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
    callbacks_.insert(std::make_pair(timerFd_, &MotionSensorMock::TimerCallback));
    if (RegisterTimerCallback(timerFd_, EVENT_TIMER_FD)) {
        MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "register timer fd fail.");
    }
}

void MotionSensorMock::SetTimerInterval(int interval)
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

void MotionSensorMock::CloseTimer()
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Enter");
    close(timerFd_);
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Exit");
}

void MotionSensorMock::TimerCallback()
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

void MotionSensorMock::GetMotionData()
{
    MotionDataUtils::MotionData motionData;
    for (int n = 0; n <= MotionDataUtils::MotionType::TYPE_WRIST_TILT; n++){
        if(in_type[n] == MotionDataUtils::MotionTypeValue::VALID){
            MotionDataUtils::MotionType type = MotionDataUtils::MotionType(n);;
            dataParse_->ParseMotionData(motionData, type);
            NotifyMsdpImpl(motionData);
        }
    }
}

int MotionSensorMock::RegisterTimerCallback(const int fd, const EventType et)
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

void MotionSensorMock::StartThread()
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    std::make_unique<std::thread>(&MotionSensorMock::LoopingThreadEntry, this)->detach();
}

void MotionSensorMock::LoopingThreadEntry()
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
                MotionSensorMock *func = const_cast<MotionSensorMock *>(this);
                (callbacks_.find(events[n].data.fd)->second)(func);
            }
        }
    }
}

extern "C" MotionSensorInterface *Create(void)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    g_rdb = new MotionSensorMock();
    return g_rdb;
}

extern "C" void Destroy(const MotionSensorInterface* algorithm)
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    delete algorithm;
}
}
}