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

#ifndef MOTION_EAR_H
#define MOTION_EAR_H

#include <iostream>
#include "motion_data_utils.h"
#include "motion_data_define.h"
#include "sensor_data_callback.h"
#include "motion_msdp_interface.h"

namespace OHOS {
namespace Msdp {
class MotionNearEar {
public:
    MotionNearEar(const std::shared_ptr<SensorDataCallback> &sensorCallback) : sensorCallback_(sensorCallback) {};
    ~MotionNearEar();
    void Init();
    void Finish();
    void HandleIdle();
    void HandleHorizontal();
    void HandleShortVertical();
    void HandleLongVertical();
    void HandleFlipped();
    void HandleNonAbsoluteStill();
    void HandleEar();
    void StartAlgorithm(int32_t sensorTypeId, void* sensorData);
    void RegisterCallback(std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> &callback);
    MotionDataUtils::MotionData Report();
private:
    enum RotateState {
        IDLE = -1,
        HORIZONTAL,
        SHORT_VERTICAL,
        LONG_VERTICAL,
        FLIPPED,
        NON_ABSOLUTE_STILL,
        EAR
    };

    float acc_x_ = 0;
    float acc_y_ = 0;
    float acc_z_ = 0;

    bool sensor_proxitiy_value_ = false;
    double pitch_ = 0;
    double vectorMagnitude_ = 0;

    int state_ = IDLE;
    int previousState_ = IDLE;

    int horizontalCounter = 0;
    int verticalShortCounter = 0;
    int verticalLongCounter = 0;
    int flippedCounter = 0;
    int nearEarCounter = 0;

    int initThreshold = COUNTER_THRESHOLD;
    int nearEarThreshold = NEAR_EAR_THRESHOLD;

    MotionDataUtils::MotionData reportInfo_;
    SensorCallback dataCallback_;
    std::shared_ptr<SensorDataCallback> sensorCallback_;
    std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> callbackImpl_;
};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_EAR_H
