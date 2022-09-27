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

#ifndef MOTION_POCKET_H
#define MOTION_POCKET_H

#include <iostream>
#include "motion_data_utils.h"
#include "motion_data_define.h"
#include "data_define.h"
#include "sensor_data_callback.h"
#include "motion_msdp_interface.h"

namespace OHOS {
namespace Msdp {
class MotionPocket {
public:
    MotionPocket(const std::shared_ptr<SensorDataCallback> &sensorCallback) : sensorCallback_(sensorCallback) {};
    ~MotionPocket();
    void Init();
    void HandlePocket();
    void HandleNonPocket();
    void StartAlgorithm(int32_t sensorTypeId, void* sensorData);
    void RegisterCallback(std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> &callback);
    MotionDataUtils::MotionData Report();
private:
    enum EventType {
        POCKET = 1,
        NON_POCKET
    };

    float acc_x_ = 0;
    float acc_y_ = 0;
    float acc_z_ = 0;

    bool sensor_proxitiy_value_ = false;
    float sensor_als_value_ = 0;

    int state_ = NON_POCKET;
    int previousState_ = NON_POCKET;
    
    MotionDataUtils::MotionData reportInfo_;
    SensorCallback dataCallback_;
    std::shared_ptr<SensorDataCallback> sensorCallback_;
    std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> callbackImpl_;
};
} // namespace Msdp
} // namespace OHOS
#endif  //  MOTION_POCKET_H