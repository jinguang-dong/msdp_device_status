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

#include "motion_pickup.h"
#include <cstdio>
#include <cmath>

#include "devicestatus_common.h"

namespace OHOS {
namespace Msdp {
MotionPickup::~MotionPickup() {}

void MotionPickup::Init()
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = MotionDataUtils::TYPE_INVALID;
    reportInfo_.value = MotionDataUtils::VALUE_INVALID;
    reportInfo_.status = MotionDataUtils::STATUS_INVALID;
    reportInfo_.action = MotionDataUtils::ACTION_INVALID;
    reportInfo_.move = 0;
    dataCallback_ = std::bind(&MotionPickup::StartAlgorithm, this, std::placeholders::_1, std::placeholders::_2);
    sensorCallback_->SubscribeSensorEvent(dataCallback_);
}

void MotionPickup::HandleIdle()
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
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

void MotionPickup::HandleHorizontal()
{
    if (previousState_ != IDLE) {
        return;
    }
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    if ((vectorMagnitude_ >= PICKUP_AND_EAR_VECTOR_MODULE_UP_THRESHOLD) ||
        (vectorMagnitude_ <= PICKUP_AND_EAR_VECTOR_MODULE_DOWN_THRESHOLD)) {
        previousState_ = state_;
        state_ = NON_ABSOLUTE_STILL;
    }
}

void MotionPickup::HandleShortVertical()
{
    if (previousState_ != IDLE) {
        return;
    }
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    if ((vectorMagnitude_ >= PICKUP_AND_EAR_VECTOR_MODULE_UP_THRESHOLD) ||
        (vectorMagnitude_ <= PICKUP_AND_EAR_VECTOR_MODULE_DOWN_THRESHOLD)) {
        previousState_ = state_;
        state_ = NON_ABSOLUTE_STILL;
    }
}

void MotionPickup::HandleLongVertical()
{
    if (previousState_ != IDLE) {
        return;
    }
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    if ((vectorMagnitude_ >= PICKUP_AND_EAR_VECTOR_MODULE_UP_THRESHOLD) ||
        (vectorMagnitude_ <= PICKUP_AND_EAR_VECTOR_MODULE_DOWN_THRESHOLD)) {
        previousState_ = state_;
        state_ = NON_ABSOLUTE_STILL;
    }
}

void MotionPickup::HandleFlipped()
{
    if (previousState_ != IDLE) {
        return;
    }
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    if ((vectorMagnitude_ >= PICKUP_AND_EAR_VECTOR_MODULE_UP_THRESHOLD) ||
        (vectorMagnitude_ <= PICKUP_AND_EAR_VECTOR_MODULE_DOWN_THRESHOLD)) {
        previousState_ = state_;
        state_ = NON_ABSOLUTE_STILL;
    }
}

void MotionPickup::HandleNonAbsoluteStill()
{
    if ((previousState_ != HORIZONTAL) &&
        (previousState_ != SHORT_VERTICAL) &&
        (previousState_ != LONG_VERTICAL) &&
        (previousState_ != FLIPPED)) {
        return;
    }
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    if (pitch_ > PICKUP_AND_EAR_PITCH_DOWN_THRESHOLD) {
        pickupCounter--;
        if (pickupCounter == 0) {
            pickupCounter = pickupThreshold;
            previousState_ = state_;
            state_ = PICKUP;
            DEV_HILOGI(SERVICE, "Report_PICKUP");
            Report();
        }
    } else {
        pickupCounter = pickupThreshold;
    }
}

void MotionPickup::HandlePickup()
{
    if (previousState_ != NON_ABSOLUTE_STILL) {
        return;
    }
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    if (pitch_ <= HANDLE_PICKUP_PITCH_THRESHOLD) {
        previousState_ = state_;
        state_ = IDLE;
    }
}

void MotionPickup::StartAlgorithm(int32_t sensorTypeId, void* sensorData)
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    if (sensorTypeId == SENSOR_TYPE_ID_ACCELEROMETER) {
        AccelData* data = (AccelData*)sensorData;
        acc_x_ = data->y;
        acc_y_ = -(data->x);
        acc_z_ = data->z;
    } else {
        return;
    }
    DEV_HILOGI(SERVICE,
        "acc_x_: %{public}f, acc_y_: %{public}f, acc_z_: %{public}f",
        acc_x_, acc_y_, acc_z_);

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
        case PICKUP:
            HandlePickup();
            break;
        default:
            break;
    }
}

void MotionPickup::RegisterCallback(std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> &callback)
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    callbackImpl_ = callback;
}

MotionDataUtils::MotionData MotionPickup::Report()
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = MotionDataUtils::TYPE_PICKUP;
    reportInfo_.value = MotionDataUtils::VALUE_ENTER;
    reportInfo_.action = MotionDataUtils::ACTION_ENLARGE;
    reportInfo_.status = MotionDataUtils::STATUS_START;
    reportInfo_.move = 0.0;
    DEV_HILOGI(SERVICE, "%{public}s: motionData.type:%{public}d, \
        motionData.status: %{public}d, motionData.action: %{public}d, motionData.move: %{public}f",\
        __func__,  static_cast<int>(reportInfo_.type), \
        static_cast<int>(reportInfo_.status), static_cast<int>(reportInfo_.action), reportInfo_.move);
    return reportInfo_;
}
} // namespace Msdp
} // namespace OHOS


