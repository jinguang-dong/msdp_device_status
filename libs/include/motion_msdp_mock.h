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

#ifndef MOTION_MSDP_MOCK_H
#define MOTION_MSDP_MOCK_H

#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <errors.h>

#include "motion_data_utils.h"
#include "motion_msdp_interface.h"
#include "motion_data_parse.h"
#include "point_event_callback.h"
#include "motion_gesture.h"
#include "sensor_data_callback.h"
#include "motion_flip.h"
#include "motion_rotate.h"
#include "motion_shake.h"
#include "motion_pocket.h"
#include "motion_pickup.h" 
#include "motion_near_ear.h" 

namespace OHOS {
namespace Msdp {

class MotionMsdpMock : public MotionMsdpInterface {
public:
    enum EventType {
        EVENT_UEVENT_FD,
        EVENT_TIMER_FD,
    };

    MotionMsdpMock() {}
    virtual ~MotionMsdpMock() {}
    bool Init();
    void SetTimerInterval(int interval);
    void InitTimer();
    void CloseTimer();
    void TimerCallback();
    int RegisterTimerCallback(const int fd, const EventType et);
    void StartThread();
    void LoopingThreadEntry();
    ErrCode Enable(const MotionDataUtils::MotionType& type) override;
    ErrCode Disable(const MotionDataUtils::MotionType& type) override;
    ErrCode DisableCount(const MotionDataUtils::MotionType& type) override;
    ErrCode RegisterCallback(std::shared_ptr<MsdpAlgorithmCallback>& callback) override;
    ErrCode UnregisterCallback() override;
    ErrCode NotifyMsdpImpl(MotionDataUtils::MotionData& data);
    MotionDataUtils::MotionData SaveRdbData(MotionDataUtils::MotionData& data);
    std::shared_ptr<MsdpAlgorithmCallback> GetCallbacksImpl()
    {
        std::unique_lock lock(mutex_);
        return callbacksImpl_;
    }

private:
    void GetMotionData();
    using Callback = std::function<void(MotionMsdpMock*)>;
    std::shared_ptr<MsdpAlgorithmCallback> callbacksImpl_;
    std::map<int32_t, Callback> callbacks_;
    int timerInterval_ = -1;
    int32_t timerFd_ = -1;
    int32_t epFd_ = -1;
    std::mutex mutex_;
    std::unique_ptr<MotionDataParse> dataParse_;
    int32_t in_type[MotionDataUtils::MotionType::TYPE_HORIZONTAL_180 + 1] = {0};
    std::shared_ptr<PointEventCallback> pointEventCb_;
    std::shared_ptr<MotionGesture> gesture_;
    std::shared_ptr<SensorDataCallback> sensorEventCb_;
    MotionDataUtils::MotionData motionData_;

};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_MSDP_MOCK_H