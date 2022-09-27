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

#ifndef MOTION_GESTURE_MANAGER_H
#define MOTION_GESTURE_MANAGER_H

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
class MotionGestureManager : public MotionMsdpInterface {
public:
    MotionGestureManager() {}
    virtual ~MotionGestureManager() {}
    bool Init();
    ErrCode Enable(const MotionDataUtils::MotionType& type) override;
    ErrCode Disable(const MotionDataUtils::MotionType& type) override;
    ErrCode DisableCount(const MotionDataUtils::MotionType& type) override;
    ErrCode RegisterCallback(std::shared_ptr<MsdpAlgorithmCallback>& callback) override;
    ErrCode UnregisterCallback() override;
    std::shared_ptr<MsdpAlgorithmCallback> GetCallbacksImpl()
    {
        std::unique_lock lock(mutex_);
        return callbacksImpl_;
    }
private:
    int32_t in_type[MotionDataUtils::MotionType::TYPE_THREE_FINGERS_SLIDE + 1] = {0};
    std::shared_ptr<MsdpAlgorithmCallback> callbacksImpl_;
    std::mutex mutex_;
    std::shared_ptr<PointEventCallback> pointEventCb_ {nullptr};
    std::shared_ptr<MotionGesture> gesture_ {nullptr};
    MotionDataUtils::MotionType type_;
    std::shared_ptr<SensorDataCallback> sensorEventCb_ {nullptr};
    std::shared_ptr<MotionShake> shake_ {nullptr};
    std::shared_ptr<MotionFlip> flip_ {nullptr};
    std::shared_ptr<MotionRotate> rotate_ {nullptr};
    std::shared_ptr<MotionPocket> pocket_ {nullptr};
    std::shared_ptr<MotionPickup> pickup_ {nullptr};
    std::shared_ptr<MotionNearEar> nearEar_ {nullptr};
};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_GESTURE_MANAGER_H