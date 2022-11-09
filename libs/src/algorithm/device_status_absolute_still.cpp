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

#include "device_status_absolute_still.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {

bool AbsoluteStill::Init(Type type)
{
    DEV_HILOGD(SERVICE, "enter");
    algoCallback_ = std::bind(&AbsoluteStill::StartAlgorithm, this, std::placeholders::_1,
        std::placeholders::_2);
    if (algoCallback_ != nullptr) {
        sensorCallback_->SubscribeSensorEvent(type, algoCallback_);
    }
    DEV_HILOGE(SERVICE, "sensorCallback is null");
    return true;
}

void AbsoluteStill::StartAlgorithm(int32_t sensorTypeId, AccelData* sensorData)
{
    DEV_HILOGD(SERVICE, "enter");
    GetData(sensorTypeId, sensorData);
    ExecuteAlgorithm();
}

void AbsoluteStill::ExecuteAlgorithm()
{
    DEV_HILOGD(SERVICE, "enter");

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
            Report(VALUE_ENTER, STILL, TYPE_STILL);
        }
    } else {
        counter_ = COUNTER_THRESHOLD;
        if (state_ == UNSTILL) {
            return;
        }
        Report(VALUE_EXIT, UNSTILL, TYPE_STILL);
    }
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
