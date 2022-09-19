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

#include "motion_rotate.h"
#include <cstdio>
#include <cmath>

//
//#include "motion_common.h"
#include "devicestatus_common.h"

namespace OHOS {
namespace Msdp {
MotionRotate::~MotionRotate() {}

void MotionRotate::Init()
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = MotionDataUtils::TYPE_INVALID;
    reportInfo_.value = MotionDataUtils::VALUE_INVALID;
    reportInfo_.status = MotionDataUtils::STATUS_INVALID;
    reportInfo_.action = MotionDataUtils::ACTION_INVALID;
    reportInfo_.move = 0;
    reportInfo_.rotateAction = MotionDataUtils::ROTATE_ACTION_INVALID;
    dataCallback_ = std::bind(&MotionRotate::StartAlgorithm, this, std::placeholders::_1, std::placeholders::_2);
    sensorCallback_->SubscribeSensorEvent(dataCallback_);
}

void MotionRotate::CalculateDisplayAngle()
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    angle_ = atan2(-acc_y_, acc_x_) * (ANGLE_ONE_HUNDRED_AND_EIGHTY_DEGREE / PI);
    DEV_HILOGI(SERVICE, "angle_ : %{public}f", angle_);
    orientation_ = ANGLE_NITETY_DEGREE - angle_;
    if (orientation_ > ANGLE_THREE_HUNDRED_AND_SIXTY_DEGREE) {
        orientation_ = orientation_ - ANGLE_THREE_HUNDRED_AND_SIXTY_DEGREE;
        DEV_HILOGI(SERVICE, "orientation_bigger_than_360");
    } else if (orientation_ < 0) {
        orientation_ = orientation_ + ANGLE_THREE_HUNDRED_AND_SIXTY_DEGREE;
        DEV_HILOGI(SERVICE, "orientation_is_negative");
    } else if ((orientation_ > 0) && (orientation_ < ANGLE_FORTY_FIVE_DEGREE)) {
        display_ = VERTICAL_90;
    } else if ((orientation_ > ANGLE_FORTY_FIVE_DEGREE) &&
                (orientation_ < ANGLE_ONE_HUNDRED_AND_THIRTY_FIVE_DEGREE)) {
        display_ = HORIZONTAL_ZERO;
    } else if ((orientation_ > ANGLE_ONE_HUNDRED_AND_THIRTY_FIVE_DEGREE) &&
                (orientation_ < ANGLE_TWO_HUNDRED_AND_TWENTY_FIVE_DEGREE)) {
        display_ = VERTICAL_270;
    } else if ((orientation_ > ANGLE_TWO_HUNDRED_AND_TWENTY_FIVE_DEGREE) &&
                (orientation_ < ANGLE_THREE_HUNDRED_AND_FIFTEEN_DEGREE)) {
        display_ = HORIZONTAL_180;
    } else if ((orientation_ > ANGLE_THREE_HUNDRED_AND_FIFTEEN_DEGREE) &&
                (orientation_ < ANGLE_THREE_HUNDRED_AND_SIXTY_DEGREE)) {
        display_ = VERTICAL_90;
    }

    switch (display_) {
        case VERTICAL_90:
            DEV_HILOGI(SERVICE, "VERTICAL90_display, display_ : %{public}d", display_);
            Report();
            break;
        case HORIZONTAL_ZERO: {
            DEV_HILOGI(SERVICE, "HORIZONTAL_ZERO_display, display_ : %{public}d", display_);
            Report();
            break;
        }
        case VERTICAL_270: {
            DEV_HILOGI(SERVICE, "VERTICAL270_display, display_ : %{public}d", display_);
            Report();
            break;
        }
        case HORIZONTAL_180: {
            DEV_HILOGI(SERVICE, "HORIZONTAL180_display, display_ : %{public}d", display_);
            Report();
            break;
        }
        default:
            break;
    }
}

void MotionRotate::HandleIdle()
{
    DEV_HILOGI(SERVICE, "%{public}s enter, \
        acc_x_ : %{public}f, acc_y_ : %{public}f, acc_z_ : %{public}f",
        __func__, acc_x_, acc_y_, acc_z_);
    if ((((acc_x_ * acc_x_) + (acc_y_ * acc_y_)) * CONSTANT_FOUR) >= (acc_z_ * acc_z_)) {
        state_ = ROTATING;
    }
}

void MotionRotate::Rotating()
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    if ((vectorMagnitude_ > VECTEOR_MODULE_LOW_THRESHOLD) && (vectorMagnitude_ < VECTEOR_MODULE_HIGH_THRESHOLD)) {
        if ((((acc_x_ * acc_x_) + (acc_y_ * acc_y_)) * CONSTANT_FOUR) >= (acc_z_ * acc_z_)) {
            CalculateDisplayAngle();
            state_ = ROTATED;
        } else {
            state_ = IDLE;
        }
    }
}

void MotionRotate::Rotated()
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    if ((vectorMagnitude_ < VECTEOR_MODULE_LOW_THRESHOLD) || (vectorMagnitude_ > VECTEOR_MODULE_HIGH_THRESHOLD)) {
        state_ = ROTATING;
    }
}

void MotionRotate::StartAlgorithm(int32_t sensorTypeId, void* sensorData)
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

    if ((abs(acc_x_) < ACCELERATION_VALID_THRESHOLD) &&
        (abs(acc_y_) < ACCELERATION_VALID_THRESHOLD) &&
        (abs(acc_z_) < ACCELERATION_VALID_THRESHOLD)) {
        DEV_HILOGI(SERVICE, "StartAlgorithm");
        pitch_ = atan2(acc_y_, acc_z_) * (ANGLE_ONE_HUNDRED_AND_EIGHTY_DEGREE / PI);
        roll_ = atan2(acc_x_, acc_z_) * (ANGLE_ONE_HUNDRED_AND_EIGHTY_DEGREE / PI);
        DEV_HILOGI(SERVICE, "ROTATE_pitch_ : %{public}f, ROTATE_roll_ : %{public}f",pitch_, roll_);
        vectorMagnitude_ = sqrt((acc_x_ * acc_x_) + (acc_y_ * acc_y_) + (acc_z_ * acc_z_));
        DEV_HILOGI(SERVICE, "ROTATE_vectorMagnitude_ : %{public}f", vectorMagnitude_);

        switch (state_) {
            case IDLE:
                HandleIdle();
                break;
            case ROTATING: {
                Rotating();
                break;
            }
            case ROTATED: {
                Rotated();
                break;
            }
            default:
                HandleIdle();
                break;
        }
    }
}

void MotionRotate::RegisterCallback(std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> &callback)
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    callbackImpl_ = callback;
}

MotionDataUtils::MotionData MotionRotate::Report()
{
    DEV_HILOGI(SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = MotionDataUtils::TYPE_ROTATE;
    reportInfo_.value = MotionDataUtils::VALUE_ENTER;
    reportInfo_.action = MotionDataUtils::ACTION_ENLARGE;
    reportInfo_.status = MotionDataUtils::STATUS_START;
    reportInfo_.move = 0.0;

    switch (display_) {
        case VERTICAL_90:
            reportInfo_.rotateAction = MotionDataUtils::TYPE_VERTICAL_90;
            break;
        case HORIZONTAL_ZERO:
            reportInfo_.rotateAction = MotionDataUtils::TYPE_HORIZONTAL_ZERO;
            break;
        case VERTICAL_270:
            reportInfo_.rotateAction = MotionDataUtils::TYPE_VERTICAL_270;
            break;
        case HORIZONTAL_180:
            reportInfo_.rotateAction = MotionDataUtils::TYPE_HORIZONTAL_180;
            break;
        default:
            reportInfo_.rotateAction = MotionDataUtils::ROTATE_ACTION_INVALID;
            break;
    }

    DEV_HILOGI(SERVICE, "%{public}s: motionData.type:%{public}d, \
        motionData.status: %{public}d, motionData.action: %{public}d, \
        motionData.move: %{public}f, motionData.rotateAction: %{public}d", \
        __func__,  static_cast<int>(reportInfo_.type), \
        static_cast<int>(reportInfo_.status), static_cast<int>(reportInfo_.action), \
        reportInfo_.move, reportInfo_.rotateAction);
    return reportInfo_;
}
} // namespace Msdp
} // namespace OHOS