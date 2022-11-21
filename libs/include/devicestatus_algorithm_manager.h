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

#ifndef DEVICESTATUS_ALGORITHM_MANAGER_H
#define DEVICESTATUS_ALGORITHM_MANAGER_H

#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <map>

#include "devicestatus_data_utils.h"
#include "devicestatus_msdp_interface.h"
#include "devicestatus_data_define.h"
#include "sensor_data_callback.h"
#include "algo_absolute_still.h"
#include "algo_horizontal.h"
#include "algo_vertical.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class AlgoMgr final : public IMsdp {
public:
    AlgoMgr() = default;
    virtual ~AlgoMgr() = default;

    bool Init();
    ErrCode RegisterCallback(const std::shared_ptr<MsdpAlgoCallback>& callback) override;
    ErrCode UnregisterCallback() override;
    ErrCode Enable(Type type) override;
    ErrCode Disable(Type type) override;
    ErrCode DisableCount(const Type& type) override;
    ErrCode UnregisterSensor(Type type);
    std::shared_ptr<MsdpAlgoCallback> GetCallbackImpl()
    {
        std::unique_lock lock(mutex_);
        return callback_;
    }
    bool CheckSensorTypeId(int32_t sensorTypeId);
    bool SensorStart(Type type);
    int32_t GetSensorTypeId(Type type);
private:
    int32_t type_[Type::TYPE_MAX] = {0};
    std::shared_ptr<MsdpAlgoCallback> callback_ {nullptr};
    std::mutex mutex_;
    std::shared_ptr<SensorDataCallback> sensorEventCb_ {nullptr};
    std::shared_ptr<AlgoAbsoluteStill> still_ {nullptr};
    std::shared_ptr<AlgoHorizontal> horizontalPosition_ {nullptr};
    std::shared_ptr<AlgoVertical> verticalPosition_ {nullptr};
    std::map<Type, int32_t> callAlgoNum_ = {};
    Type algoType_;
    int32_t ERR_NG = -1;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_ALGORITHM_MANAGER_H
