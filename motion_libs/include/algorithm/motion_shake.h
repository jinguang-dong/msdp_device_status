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

#ifndef MOTION_SHAKE_H
#define MOTION_SHAKE_H

#include <iostream>
#include "motion_data_utils.h"
#include "motion_data_define.h"
#include "sensor_data_callback.h"
#include "motion_msdp_interface.h"

namespace OHOS {
namespace Msdp {
class MotionShake {
public:
    MotionShake(const std::shared_ptr<SensorDataCallback> &sensorCallback) : sensorCallback_(sensorCallback) {};
    ~MotionShake();
    void Init();
    void HandleShake();
    void HandleNonShake();
    void StartAlgorithm(int32_t sensorTypeId, void* sensorData);
    void  RegisterCallback(std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> &callback);
    MotionDataUtils::MotionData Report();
private:
    enum EventType {
        SHAKE = 1,
        NON_SHAKE
    };

    float acc_x_ = 0;
    float acc_y_ = 0;
    float acc_z_ = 0;

    int state_ = NON_SHAKE;
    int previousState_ = NON_SHAKE;
    MotionDataUtils::MotionData reportInfo_;
    SensorCallback dataCallback_;
    std::shared_ptr<SensorDataCallback> sensorCallback_;
    std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> callbackImpl_;
};
} // namespace Msdp
} // namespace OHOS
#endif  //  MOTION_SHAKE_H
