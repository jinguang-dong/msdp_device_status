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

#include "motion_pocket.h"
#include "motion_common.h"
#include <cstdio>
#include <cmath>

namespace OHOS {
namespace Msdp {
MotionPocket::~MotionPocket() {}

void MotionPocket::Init()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = MotionDataUtils::TYPE_INVALID;
    reportInfo_.value = MotionDataUtils::VALUE_INVALID;
    reportInfo_.status = MotionDataUtils::STATUS_INVALID;
    reportInfo_.action = MotionDataUtils::ACTION_INVALID;
    reportInfo_.move = 0;
    dataCallback_ = std::bind(&MotionPocket::StartAlgorithm, this, std::placeholders::_1, std::placeholders::_2);
    sensorCallback_->SubscribeSensorEvent(dataCallback_);
}

void MotionPocket::HandlePocket()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    Report();
}

void MotionPocket::HandleNonPocket()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
}

void MotionPocket::StartAlgorithm(int32_t sensorTypeId, void* sensorData)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    if (sensorTypeId == SENSOR_TYPE_ID_ACCELEROMETER) {
        AccelData* data = (AccelData*)sensorData;
        acc_x_ = data->y;
        acc_y_ = -(data->x);
        acc_z_ = data->z;
    } else if (sensorTypeId == SENSOR_TYPE_ID_PROXIMITY) {
        float distance = *((float*)sensorData);
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "distance: %{public}f", distance);
        if (distance == 0.0) {
            sensor_proxitiy_value_ = true;
        } else {
            sensor_proxitiy_value_ = false;
        }
    } else if (sensorTypeId == SENSOR_TYPE_ID_AMBIENT_LIGHT) {
        sensor_als_value_ = *((float*)sensorData);
    } else {
        return;
    }
    MOTION_HILOGI(MOTION_MODULE_SERVICE,
        "acc_x_: %{public}f, acc_y_: %{public}f, acc_z_: %{public}f, \
        sensor_proxitiy_value_: %{public}d, sensor_als_value_: %{public}f",
        acc_x_, acc_y_, acc_z_, sensor_proxitiy_value_, sensor_als_value_);

    if ((sensor_proxitiy_value_ == true) && (sensor_als_value_ < POCKET_ALS_THRESHOLD)
        && (abs(acc_z_) > POCKET_ABS_Z_THRESHOLD)) {
        previousState_ = state_;
        state_ = POCKET;
    } else {
        previousState_ = state_;
        state_ = NON_POCKET;
    }

    switch (state_) {
        case POCKET:
            if (previousState_ != state_) {
                HandlePocket();
            }
            break;
        case NON_POCKET: {
            if (previousState_ != state_) {
                HandleNonPocket();
            }
            break;
        }
        default:
            break;
    }
}

void MotionPocket::RegisterCallback(std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> &callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    callbackImpl_ = callback;
}

MotionDataUtils::MotionData MotionPocket::Report()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = MotionDataUtils::TYPE_POCKET;
    reportInfo_.value = MotionDataUtils::VALUE_ENTER;
    reportInfo_.action = MotionDataUtils::ACTION_ENLARGE;
    reportInfo_.status = MotionDataUtils::STATUS_START;
    reportInfo_.move = 0.0;
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "%{public}s: motionData.type:%{public}d, \
        motionData.status: %{public}d, motionData.action: %{public}d, motionData.move: %{public}f",\
        __func__,  static_cast<int>(reportInfo_.type), \
        static_cast<int>(reportInfo_.status), static_cast<int>(reportInfo_.action), reportInfo_.move);
    if (callbackImpl_ != nullptr) {
        callbackImpl_->OnResult(reportInfo_);
    } else {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "callbackImpl_ is null");
    }
    return reportInfo_;
}
} // namespace Msdp
} // namespace OHOS


