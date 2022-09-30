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

#include "device_status_vertical.h"
#include "devicestatus_common.h"
#include <cstdio>
#include <cmath>

namespace OHOS {
namespace Msdp {
DeviceStatusVertical::~DeviceStatusVertical() {}

void DeviceStatusVertical::Init()
{
    DEV_HILOGD(SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = DevicestatusDataUtils::TYPE_INVALID;
    reportInfo_.value = DevicestatusDataUtils::VALUE_INVALID; 
    reportInfo_.status = DevicestatusDataUtils::STATUS_INVALID;
    reportInfo_.action = DevicestatusDataUtils::ACTION_INVALID;
    reportInfo_.move = 0;
    dataCallback_ = std::bind(&DeviceStatusVertical::StartAlgorithm, this, std::placeholders::_1,
        std::placeholders::_2);
    sensorCallback_->SubscribeSensorEvent(dataCallback_);
}

void DeviceStatusVertical::StartAlgorithm(int32_t sensorTypeId, void* sensorData)
{
    DEV_HILOGD(SERVICE, "enter");
    GetData(sensorTypeId, sensorData);
    LogicJudgment();
}

void DeviceStatusVertical::GetData(int32_t sensorTypeId, void* sensorData)
{
    DEV_HILOGD(SERVICE, "enter");
    if (sensorTypeId != SENSOR_TYPE_ID_ACCELEROMETER) {
        return;
    }

    AccelData* data = static_cast<AccelData*>(sensorData);
    if ((abs(data->x) > ACCELERATION_VALID_THRESHOLD) ||
        (abs(data->y) > ACCELERATION_VALID_THRESHOLD) ||
        (abs(data->z) > ACCELERATION_VALID_THRESHOLD)) {
        return;
    }

    x_ = data->y;
    y_ = -(data->x);
    z_ = data->z;
    DEV_HILOGD(SERVICE,"acc_x_:%{public}f,acc_y_:%{public}f,acc_z_:%{public}f", x_, y_, z_);
}

void DeviceStatusVertical::LogicJudgment()
{
    DEV_HILOGD(SERVICE, "enter");

    pitch_ = -atan2(y_, z_) * (ANGLE_ONE_HUNDRED_AND_EIGHTY_DEGREE / PI);
    DEV_HILOGD(SERVICE, "pitch_:%{public}f", pitch_);

    if ((abs(pitch_) > ANGLE_EIGHTY_DEGREE) && (abs(pitch_) < ANGLE_ONE_HUNDRED_AND_TEN_DEGREE)) {
        if (state_ == VERTICAL) {
            return;
        }
        counter_--;
        if (counter_ == 0) {
            counter_ = COUNTER_THRESHOLD;
            previousState_ = state_;
            state_ = VERTICAL;
            Report();
        } else {
            previousState_ = state_;
            return;
        }
    } else {
        counter_ = COUNTER_THRESHOLD;
        previousState_ = state_;
        state_ = NON_VERTICAL;
    }
}

void DeviceStatusVertical::RegisterCallback(
    std::shared_ptr<DevicestatusAlgorithmManagerInterface::DevicestatusAlgorithmCallback> &callback)
{
    DEV_HILOGD(SERVICE, "enter");
    callbackImpl_ = callback;
}

void DeviceStatusVertical::Report()
{
    DEV_HILOGD(SERVICE, "enter");

    if (callbackImpl_ == nullptr) {
        DEV_HILOGD(SERVICE, "callbackImpl_ is null");
        return;
    }

    reportInfo_.type = DevicestatusDataUtils::TYPE_VERTICAL_POSITION;
    reportInfo_.value = DevicestatusDataUtils::VALUE_ENTER;
    reportInfo_.action = DevicestatusDataUtils::ACTION_ENLARGE;
    reportInfo_.status = DevicestatusDataUtils::STATUS_START;
    reportInfo_.move = 0.0;

    DEV_HILOGD(SERVICE, "type:%{public}d,value:%{public}d,action:%{public}d,status:%{public}d,move:%{public}f",
        static_cast<int>(reportInfo_.type), static_cast<int>(reportInfo_.value),
        static_cast<int>(reportInfo_.action), static_cast<int>(reportInfo_.status), reportInfo_.move);

    callbackImpl_->OnAlogrithmResult(reportInfo_);
}
}
}
