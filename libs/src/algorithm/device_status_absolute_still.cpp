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
#include "devicestatus_common.h"
#include <cstdio>
#include <cmath>

namespace OHOS {
namespace Msdp {
AbsoluteStill::~AbsoluteStill() {}

void AbsoluteStill::Init()
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = DevicestatusDataUtils::TYPE_INVALID;
    reportInfo_.value = DevicestatusDataUtils::VALUE_INVALID; 
    reportInfo_.status = DevicestatusDataUtils::STATUS_INVALID;
    reportInfo_.action = DevicestatusDataUtils::ACTION_INVALID;
    reportInfo_.move = 0;
    dataCallback_ = std::bind(&AbsoluteStill::StartAlgorithm, this, std::placeholders::_1, std::placeholders::_2);
    sensorCallback_->SubscribeSensorEvent(dataCallback_);
}

void AbsoluteStill::HandleStill()
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    if (previousState_ != state_) {
        Report();
    }
}
void AbsoluteStill::HandleUnstill() 
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
}

void AbsoluteStill::StartAlgorithm(int32_t sensorTypeId, void* sensorData) 
{
	DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    if (sensorTypeId == SENSOR_TYPE_ID_ACCELEROMETER) {
        AccelData* data = (AccelData*)sensorData;
        x_ = data->y;
        y_ = -(data->x);
        z_ = data->z;
    } else {
        return;
    }
    DEV_HILOGI(SERVICE,
        "acc_x_: %{public}f, acc_y_: %{public}f, acc_z_: %{public}f",
        x_, y_, z_);
    
    if ((abs(x_) < ACCELERATION_VALID_THRESHOLD) && (abs(y_) < ACCELERATION_VALID_THRESHOLD) && 
        (abs(z_) < ACCELERATION_VALID_THRESHOLD)) {

        vectorModule_ = sqrt((x_ * x_) + (y_ * y_) + (z_ * z_));

        if ((vectorModule_ > VECTEOR_MODULE_LOW_THRESHOLD) && (vectorModule_ < VECTEOR_MODULE_HIGH_THRESHOLD)) {
            DEV_HILOGI(SERVICE,  "vectorModule_ : %{public}f", vectorModule_);
            absoluteStillCounter_--;
            if (absoluteStillCounter_ == 0) {
                absoluteStillCounter_ = COUNTER_THRESHOLD;
                previousState_ = state_;
                state_ = STILL;
            } else if (absoluteStillCounter_) {
                DEV_HILOGI(SERVICE,  "absoluteStillCounter_ : %{public}d", absoluteStillCounter_);
                previousState_ = state_;
                return;
            }
        } else {
            absoluteStillCounter_ = COUNTER_THRESHOLD;
            previousState_ = state_;
            state_ = UNSTILL;
            DEV_HILOGI(SERVICE,  "state_ : %{public}d", state_);
        }
        
        switch (state_) {
            case STILL:
                HandleStill();
                break;
            case UNSTILL:
                HandleUnstill();
                break;
            default: 
                break;
        }
    }
}

void  AbsoluteStill::RegisterCallback(std::shared_ptr \
    <DevicestatusAlgorithmManagerInterface::DevicestatusAlgorithmCallback> &callback)
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    callbackImpl_ = callback;
}

DevicestatusDataUtils::DevicestatusData AbsoluteStill::Report()
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = DevicestatusDataUtils::TYPE_STILL;
    reportInfo_.value = DevicestatusDataUtils::VALUE_ENTER;
    reportInfo_.action = DevicestatusDataUtils::ACTION_ENLARGE;
    reportInfo_.status = DevicestatusDataUtils::STATUS_START;
    reportInfo_.move = 0.0;
    DEV_HILOGI(SERVICE, "%{public}s: deviceStatusData.type:%{public}d, \
        deviceStatusData.status: %{public}d, deviceStatusData.action: %{public}d, deviceStatusData.move: %{public}f",\
        __func__,  static_cast<int>(reportInfo_.type), \
        static_cast<int>(reportInfo_.status), static_cast<int>(reportInfo_.action), reportInfo_.move);
    if (callbackImpl_ != nullptr) {
        callbackImpl_->OnAlogrithmResult(reportInfo_);
    } else {
        DEV_HILOGI(SERVICE, "callbackImpl_ is null"); 
    }
    return reportInfo_;
}
}
}