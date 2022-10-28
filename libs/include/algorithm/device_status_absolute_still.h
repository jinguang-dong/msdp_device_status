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

#ifndef DEVICE_STATUS_ABSOLUTE_STILL_H
#define DEVICE_STATUS_ABSOLUTE_STILL_H
#include <iostream>
#include "devicestatus_data_utils.h"
#include "devicestatus_data_define.h"
#include "sensor_data_callback.h"
#include "devicestatus_algorithm_manager_interface.h"

namespace OHOS {
namespace Msdp {
class AbsoluteStill {
public:
    AbsoluteStill(const std::shared_ptr<SensorDataCallback> &sensorCallback) : sensorCallback_(sensorCallback) {};
    ~AbsoluteStill();
    void Init();

    void StartAlgorithm(int32_t sensorTypeId, void* sensorData);
    void GetData(int32_t sensorTypeId, void* sensorData);
    void LogicJudgment();

    void RegisterCallback(std::shared_ptr<DevicestatusAlgorithmManagerInterface::DevicestatusAlgorithmCallback> \
        &callback);
    void Report();
private:
    enum EventType {
        IDLE = -1,
        STILL,
        UNSTILL
    };

    float x_ = 0.0;
    float y_ = 0.0;
    float z_ = 0.0;

    double vectorModule_ = 0.0;
    int32_t state_ = IDLE;
    int32_t previousState_ = UNSTILL;
    int32_t counter_ = COUNTER_THRESHOLD;

    DevicestatusDataUtils::DevicestatusData reportInfo_;
    SensorCallback dataCallback_;
    std::shared_ptr<SensorDataCallback> sensorCallback_;
    std::shared_ptr<DevicestatusAlgorithmManagerInterface::DevicestatusAlgorithmCallback> callbackImpl_;
};
} // namespace Msdp
} // namespace OHOS
#endif	// DEVICE_STATUS_ABSOLUTE_STILL_H