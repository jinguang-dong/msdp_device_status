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

#ifndef MOTION_SENSOR_INTERFACE_H
#define MOTION_SENSOR_INTERFACE_H

#include <string>
#include <memory>
#include <map>
#include <errors.h>
#include "motion_data_utils.h"
#include "motion_algorithm_common.h"

namespace OHOS {
namespace Msdp {
class MotionSensorInterface {
public:
    MotionSensorInterface() {}
    virtual ~MotionSensorInterface() {}

    class MotionSensorHdiCallback {
    public:
        MotionSensorHdiCallback() = default;
        virtual ~MotionSensorHdiCallback() = default;
        virtual void OnResult(MotionDataUtils::MotionData& data) = 0;
    };

    virtual ErrCode RegisterCallback(std::shared_ptr<MotionSensorHdiCallback>& callback) = 0;
    virtual ErrCode UnregisterCallback() = 0;
    virtual ErrCode Enable(const MotionDataUtils::MotionType& type) = 0;
    virtual ErrCode Disable(const MotionDataUtils::MotionType& type) = 0;
    virtual ErrCode DisableCount(const MotionDataUtils::MotionType& type) = 0;
    virtual std::vector<MsdpAlgorithmCapability> GetCapability(const MotionDataUtils::MotionType& type) = 0;
};

struct SensorHdiHandle {
    void* handle;
    MotionSensorInterface* (*create)();
    void* (*destroy)(MotionSensorInterface*);
    MotionSensorInterface* pAlgorithm;
    SensorHdiHandle() : handle(nullptr), create(nullptr), destroy(nullptr), pAlgorithm(nullptr) {}
    ~SensorHdiHandle() {}
    void Clear()
    {
        handle = nullptr;
        create = nullptr;
        destroy = nullptr;
        pAlgorithm = nullptr;
    }
};
}
}
#endif // MOTION_SENSOR_INTERFACE_H
