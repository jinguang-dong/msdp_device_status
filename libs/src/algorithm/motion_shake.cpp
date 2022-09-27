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

#include "motion_shake.h"
#include "motion_common.h"
#include <cstdio>
#include <cmath>

namespace OHOS {
namespace Msdp {
MotionShake::~MotionShake() {}

void MotionShake::Init()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = MotionDataUtils::TYPE_INVALID;
    reportInfo_.value = MotionDataUtils::VALUE_INVALID;
    reportInfo_.status = MotionDataUtils::STATUS_INVALID;
    reportInfo_.action = MotionDataUtils::ACTION_INVALID;
    reportInfo_.move = 0;
    dataCallback_ = std::bind(&MotionShake::StartAlgorithm, this, std::placeholders::_1, std::placeholders::_2);
    sensorCallback_->SubscribeSensorEvent(dataCallback_);
}

void MotionShake::HandleShake()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    Report();
}

void MotionShake::HandleNonShake()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
}

void MotionShake::StartAlgorithm(int32_t sensorTypeId, void* sensorData)
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

    if ((abs(acc_x_) > SHAKE_ABS_X_THRESHOLD) ||
        (abs(acc_y_) > SHAKE_ABS_Y_THRESHOLD) ||
        (abs(acc_z_) > SHAKE_ABS_Z_THRESHOLD)) {
        previousState_ = state_;
        state_ = SHAKE;
    } else {
        previousState_ = state_;
        state_ = NON_SHAKE;
    }

    switch (state_) {
        case SHAKE:
            if (previousState_ != state_) {
                HandleShake();
            }
            break;
        case NON_SHAKE: {
            if (previousState_ != state_) {
                HandleNonShake();
            }
            break;
        }
        default:
            break;
        }
}

void  MotionShake::RegisterCallback(std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> &callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    callbackImpl_ = callback;
}

MotionDataUtils::MotionData MotionShake::Report()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    reportInfo_.type = MotionDataUtils::TYPE_SHAKE;
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