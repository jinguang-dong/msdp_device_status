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

#include "algo_absolute_still.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
bool AlgoAbsoluteStill::Init(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    algoCallback_ = std::bind(&AlgoAbsoluteStill::StartAlgorithm, this, std::placeholders::_1, std::placeholders::_2);
    if (algoCallback_ != nullptr) {
        sensorCallback_->SubscribeSensorEvent(type, algoCallback_);
        return true;
    }
    DEV_HILOGE(SERVICE, "algoCallback is null");
    return false;
}

bool AlgoAbsoluteStill::StartAlgorithm(int32_t sensorTypeId, AccelData* sensorData)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (!GetData(sensorTypeId, sensorData)) {
        DEV_HILOGE(SERVICE, "Failed to get data");
        return false;
    }
    ExecuteOperation();
    return true;
}

void AlgoAbsoluteStill::ExecuteOperation()
{
    DEV_HILOGD(SERVICE, "Enter");
    algoPara_.resultantAcc =
        sqrt((algoPara_.x * algoPara_.x) + (algoPara_.y * algoPara_.y) + (algoPara_.z * algoPara_.z));
    DEV_HILOGD(SERVICE, "resultantAcc:%{public}f", algoPara_.resultantAcc);
    if ((algoPara_.resultantAcc > RESULTANT_ACC_LOW_THRHD) && (algoPara_.resultantAcc < RESULTANT_ACC_UP_THRHD)) {
        if (state_ == STILL) {
            return;
        }
        counter_--;
        if (counter_ == 0) {
            counter_ = COUNTER_THRESHOLD;
            UpdateStateAndReport(VALUE_ENTER, STILL, TYPE_STILL);
        }
    } else {
        counter_ = COUNTER_THRESHOLD;
        if (state_ == UNSTILL) {
            return;
        }
        UpdateStateAndReport(VALUE_EXIT, UNSTILL, TYPE_STILL);
    }
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
