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

#include "motion_near_ear.h"
#include "motion_common.h"
#include <cstdio>
#include <cmath>

namespace OHOS {
namespace Msdp {
MotionNearEar::~MotionNearEar() {}

void MotionNearEar::Init()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = MotionDataUtils::TYPE_INVALID;
    reportInfo_.value = MotionDataUtils::VALUE_INVALID;
    reportInfo_.status = MotionDataUtils::STATUS_INVALID;
    reportInfo_.action = MotionDataUtils::ACTION_INVALID;
    reportInfo_.move = 0;
    dataCallback_ = std::bind(&MotionNearEar::StartAlgorithm, this, std::placeholders::_1, std::placeholders::_2);
    sensorCallback_->SubscribeSensorEvent(dataCallback_);
}

void MotionNearEar::HandleIdle()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    if ((acc_z_ < PICKUP_AND_EAR_HRZ_THRESHOLD) &&
        (abs(acc_x_) < PICKUP_AND_EAR_HRZ_ABS_X_THRESHOLD) &&
        (abs(acc_y_) < PICKUP_AND_EAR_HRZ_ABS_Y_THRESHOLD)) {
        horizontalCounter--;
        if (horizontalCounter == 0) {
            horizontalCounter = initThreshold;
            previousState_ = state_;
            state_ = HORIZONTAL;
        }
    } else {
        horizontalCounter = initThreshold;
    }

    if ((abs(acc_x_) > PICKUP_AND_EAR_SHORT_VTC_ABS_X_THRESHOLD) &&
        (abs(acc_y_) < PICKUP_AND_EAR_SHORT_VTC_ABS_Y_THRESHOLD) &&
        (abs(acc_z_) < PICKUP_AND_EAR_SHORT_VTC_ABS_Z_THRESHOLD)) {
        verticalShortCounter--;
        if (verticalShortCounter == 0) {
            verticalShortCounter = initThreshold;
            previousState_ = state_;
            state_ = SHORT_VERTICAL;
        }
    } else {
        verticalShortCounter = initThreshold;
    }

    if ((acc_y_ > PICKUP_AND_EAR_LONG_VTC_Y_THRESHOLD) &&
        (abs(acc_x_) < PICKUP_AND_EAR_LONG_VTC_ABS_X_THRESHOLD) &&
        (abs(acc_z_) < PICKUP_AND_EAR_LONG_VTC_ABS_Z_THRESHOLD)) {
        verticalLongCounter--;
        if (verticalLongCounter == 0) {
            verticalLongCounter = initThreshold;
            previousState_ = state_;
            state_ = LONG_VERTICAL;
        }
    } else {
        verticalLongCounter = initThreshold;
    }

    if ((abs(acc_z_) < PICKUP_AND_EAR_FLIPPED_ABS_Z_THRESHOLD) &&
        (abs(acc_x_) < PICKUP_AND_EAR_FLIPPED_ABS_X_THRESHOLD) &&
        (abs(acc_y_) < PICKUP_AND_EAR_FLIPPED_ABS_Y_THRESHOLD)) {
        flippedCounter--;
        if (flippedCounter == 0) {
            flippedCounter = initThreshold;
            previousState_ = state_;
            state_ = FLIPPED;
        }
    } else {
        flippedCounter = initThreshold;
    }
}

void MotionNearEar::HandleHorizontal()
{
    if (previousState_ != IDLE) {
        return;
    }
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    if ((vectorMagnitude_ >= PICKUP_AND_EAR_VECTOR_MODULE_UP_THRESHOLD) ||
        (vectorMagnitude_ <= PICKUP_AND_EAR_VECTOR_MODULE_DOWN_THRESHOLD)) {
        previousState_ = state_;
        state_ = NON_ABSOLUTE_STILL;
    }
}

void MotionNearEar::HandleShortVertical()
{
    if (previousState_ != IDLE) {
        return;
    }
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    if ((vectorMagnitude_ >= PICKUP_AND_EAR_VECTOR_MODULE_UP_THRESHOLD) ||
        (vectorMagnitude_ <= PICKUP_AND_EAR_VECTOR_MODULE_DOWN_THRESHOLD)) {
        previousState_ = state_;
        state_ = NON_ABSOLUTE_STILL;
    }
}

void MotionNearEar::HandleLongVertical()
{
    if (previousState_ != IDLE) {
        return;
    }
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    if ((vectorMagnitude_ >= PICKUP_AND_EAR_VECTOR_MODULE_UP_THRESHOLD) ||
        (vectorMagnitude_ <= PICKUP_AND_EAR_VECTOR_MODULE_DOWN_THRESHOLD)) {
        previousState_ = state_;
        state_ = NON_ABSOLUTE_STILL;
    }
}

void MotionNearEar::HandleFlipped()
{
    if (previousState_ != IDLE) {
        return;
    }
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    if ((vectorMagnitude_ >= PICKUP_AND_EAR_VECTOR_MODULE_UP_THRESHOLD) ||
        (vectorMagnitude_ <= PICKUP_AND_EAR_VECTOR_MODULE_DOWN_THRESHOLD)) {
        previousState_ = state_;
        state_ = NON_ABSOLUTE_STILL;
    }
}

void MotionNearEar::HandleNonAbsoluteStill()
{
    if ((previousState_ != HORIZONTAL) &&
        (previousState_ != SHORT_VERTICAL) &&
        (previousState_ != LONG_VERTICAL) &&
        (previousState_ != FLIPPED)) {
        return;
    }
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    if (((pitch_ > PICKUP_AND_EAR_PITCH_DOWN_THRESHOLD) &&
        (pitch_ < PICKUP_AND_EAR_PITCH_UP_THRESHOLD)) &&
        (sensor_proxitiy_value_ == true)) {
        nearEarCounter--;
        if (nearEarCounter == 0) {
            nearEarCounter = nearEarThreshold;
            previousState_ = state_;
            state_ = EAR;
            MOTION_HILOGI(MOTION_MODULE_SERVICE, "Report_EAR");
            Report();
        }
    }
    else {
        nearEarCounter = nearEarThreshold;
    }
}

void MotionNearEar::HandleEar()
{
    if (previousState_ != NON_ABSOLUTE_STILL) {
        return;
    }
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    if (sensor_proxitiy_value_ == false) {
        previousState_ = state_;
        state_ = IDLE;
    }
}

/*
 * description: Algorithm entrance
 * parameter: sensorTypeId, sensorData
 * return: none
 */
void MotionNearEar::StartAlgorithm(int32_t sensorTypeId, void* sensorData)
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
    } else {
        return;
    }
    MOTION_HILOGI(MOTION_MODULE_SERVICE, 
        "acc_x_: %{public}f, acc_y_: %{public}f, acc_z_: %{public}f, sensor_proxitiy_value_: %{public}d",
        acc_x_, acc_y_, acc_z_, sensor_proxitiy_value_);

    pitch_ = -atan2(acc_y_, acc_z_) * (ANGLE_ONE_HUNDRED_AND_EIGHTY_DEGREE / PI);
    vectorMagnitude_ = sqrt((acc_x_ * acc_x_) + (acc_y_ * acc_y_) + (acc_z_ * acc_z_));
    //
    switch (state_) {
        case IDLE:
            HandleIdle();
            break;
        case HORIZONTAL:
            HandleHorizontal();
            break;
        case SHORT_VERTICAL:
            HandleShortVertical();
            break;
        case LONG_VERTICAL:
            HandleLongVertical();
            break;
        case FLIPPED:
            HandleFlipped();
            break;
        case NON_ABSOLUTE_STILL:
            HandleNonAbsoluteStill();
            break;
        case EAR:
            HandleEar();
            break;
        default:
            break;
    }
}

void MotionNearEar::RegisterCallback(std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> &callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    callbackImpl_ = callback;
}

MotionDataUtils::MotionData MotionNearEar::Report()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = MotionDataUtils::TYPE_CLOSE_TO_EAR;
    reportInfo_.value = MotionDataUtils::VALUE_ENTER;
    reportInfo_.action = MotionDataUtils::ACTION_ENLARGE;
    reportInfo_.status = MotionDataUtils::STATUS_START;
    reportInfo_.move = 0.0;
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "%{public}s: motionData.type:%{public}d, \
        motionData.status: %{public}d, motionData.action: %{public}d, motionData.move: %{public}f", \
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


