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

#include "motion_flip.h"
#include "motion_common.h"
#include <cstdio>
#include <cmath>

namespace OHOS {
namespace Msdp {
MotionFlip::~MotionFlip() {}

void MotionFlip::Init()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = MotionDataUtils::TYPE_INVALID;
    reportInfo_.value = MotionDataUtils::VALUE_INVALID;
    reportInfo_.status = MotionDataUtils::STATUS_INVALID;
    reportInfo_.action = MotionDataUtils::ACTION_INVALID;
    reportInfo_.move = 0;
    dataCallback_ = std::bind(&MotionFlip::StartAlgorithm, this, std::placeholders::_1, std::placeholders::_2);
    sensorCallback_->SubscribeSensorEvent(dataCallback_);
}

void MotionFlip::HandleIdle()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter, vectorMagnitude_ : %{public}f, \
        roll_ : %{public}f, pitch_ : %{public}f", __func__, vectorMagnitude_, roll_, pitch_);
    if ((acc_z_ < 0) &&
        (vectorMagnitude_ > VECTEOR_MODULE_LOW_THRESHOLD) &&
        (vectorMagnitude_ < VECTEOR_MODULE_HIGH_THRESHOLD) &&
        ((abs(roll_) >= ROLL_HORIZONTAL_LOW_THRESHOLD) &&(abs(roll_) <= ROLL_HORIZONTAL_HIGH_THRESHOLD)) &&
        ((abs(pitch_) >= PITCH_HORIZONTAL_LOW_THRESHOLD) && (abs(pitch_) <= PITCH_HORIZONTAL_HIGH_THRESHOLD))) {
        state_ = STILL;
    }
}

void MotionFlip::HandleStill()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    timer_ = 0;
    if (((vectorMagnitude_ > VECTEOR_MODULE_HIGH_THRESHOLD) || (vectorMagnitude_ < VECTEOR_MODULE_LOW_THRESHOLD)) &&
        ((abs(roll_) > REVERSE_ROLL_HORIZONTAL_HIGH_THRESHOLD) && (abs(roll_) < ROLL_HORIZONTAL_LOW_THRESHOLD)) &&
        (((abs(pitch_) >= PITCH_HORIZONTAL_LOW_THRESHOLD) && (abs(pitch_) <= PITCH_HORIZONTAL_HIGH_THRESHOLD)) ||
        ((abs(pitch_) >= REVERSE_PITCH_HORIZONTAL_LOW_THRESHOLD) &&
        (abs(pitch_) <= REVERSE_PITCH_HORIZONTAL_HIGH_THRESHOLD)))) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "Still_up \
            vectorMagnitude_ : %{public}f, roll_ : %{public}f, pitch_ : %{public}f,", vectorMagnitude_, roll_, pitch_);
        state_ = FLIPPING;
    }
    if (((abs(pitch_) > REVERSE_PITCH_HORIZONTAL_HIGH_THRESHOLD) && (abs(pitch_) < PITCH_HORIZONTAL_LOW_THRESHOLD))) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "Still_down \
            vectorMagnitude_ : %{public}f, roll_ : %{public}f, pitch_ : %{public}f,", vectorMagnitude_, roll_, pitch_);
        state_ = IDLE;
    }
}

void MotionFlip::HandleFlipping()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    timer_ = timer_ + FLIPPING_INTERVAL;
    if (timer_ < FLIPPING_TIMER) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "timer_ : %{public}d, timer_related acc_z_ : %{public}f, \
            vectorMagnitude_ : %{public}f, roll_ : %{public}f, pitch_ : %{public}f",
            timer_, acc_z_, vectorMagnitude_, roll_, pitch_);
        if ((acc_z_ > 0) && (vectorMagnitude_ > VECTEOR_MODULE_LOW_THRESHOLD)
        && (vectorMagnitude_ < VECTEOR_MODULE_HIGH_THRESHOLD) &&
            ((abs(roll_) >= REVERSE_ROLL_HORIZONTAL_LOW_THRESHOLD) &&
            (abs(roll_) <= REVERSE_ROLL_HORIZONTAL_HIGH_THRESHOLD)) &&
            ((abs(pitch_) >= REVERSE_PITCH_HORIZONTAL_LOW_THRESHOLD) &&
            (abs(pitch_) <= REVERSE_PITCH_HORIZONTAL_HIGH_THRESHOLD))) {
            MOTION_HILOGI(MOTION_MODULE_SERVICE, "timer_here_if");
            state_ = FLIPPED;
            Report();
        }
    } else {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "timer_here_else");
        timer_ = 0;
        state_ = IDLE;
    }
}

void MotionFlip::HandleFlipped()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    if ((acc_z_ < 0) || (vectorMagnitude_ < VECTEOR_MODULE_LOW_THRESHOLD) ||
        (vectorMagnitude_ > VECTEOR_MODULE_HIGH_THRESHOLD) ||
        ((abs(roll_) > REVERSE_ROLL_HORIZONTAL_HIGH_THRESHOLD) &&
        (abs(roll_) <= ROLL_HORIZONTAL_HIGH_THRESHOLD)) ||
        ((abs(pitch_) > REVERSE_PITCH_HORIZONTAL_HIGH_THRESHOLD) &&
        (abs(pitch_) < PITCH_HORIZONTAL_LOW_THRESHOLD))) {
        state_ = IDLE;
    } else {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "Report_Flip_ivalid");
    }
}

void MotionFlip::StartAlgorithm(int32_t sensorTypeId, void* sensorData)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    if (sensorTypeId == SENSOR_TYPE_ID_ACCELEROMETER) {
        AccelData* data = (AccelData*)sensorData;
        acc_x_ = data->y;
        acc_y_ = -(data->x);
        acc_z_ = data->z;
    } else {
        return;
    }
    MOTION_HILOGI(MOTION_MODULE_SERVICE,
        "acc_x_: %{public}f, acc_y_: %{public}f, acc_z_: %{public}f",
        acc_x_, acc_y_, acc_z_);

    if ((abs(acc_x_) < ACCELERATION_VALID_THRESHOLD) &&
        (abs(acc_y_) < ACCELERATION_VALID_THRESHOLD) &&
        (abs(acc_z_) < ACCELERATION_VALID_THRESHOLD)) {
        pitch_ = -atan2(acc_y_, acc_z_) * (ANGLE_ONE_HUNDRED_AND_EIGHTY_DEGREE / PI);
        roll_ = atan2(acc_x_, acc_z_) * (ANGLE_ONE_HUNDRED_AND_EIGHTY_DEGREE / PI) ;
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "pitch_1: %{public}f, roll_1: %{public}f", pitch_, roll_);

        vectorMagnitude_ = sqrt((acc_x_ * acc_x_) + (acc_y_ * acc_y_) + (acc_z_ * acc_z_));

        switch (state_) {
            case IDLE:
                HandleIdle();
                break;
            case STILL: {
                HandleStill();
                break;
            }
            case FLIPPING: {
                HandleFlipping();
                break;
            }
            case FLIPPED: {
                HandleFlipped();
                break;
            }
            default:
                HandleIdle();
                break;
        }
    }
}

void MotionFlip::RegisterCallback(std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> &callback)
{
    callbackImpl_ = callback;
}

MotionDataUtils::MotionData MotionFlip::Report()
{
    reportInfo_.type = MotionDataUtils::TYPE_FLIP;
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