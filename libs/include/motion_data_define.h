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

#ifndef MOTION_DATA_DEFINE_H
#define MOTION_DATA_DEFINE_H

#include <functional>
#include "pointer_event.h"
#include "motion_data_utils.h"
#include "sensor_agent.h"

namespace OHOS {
namespace Msdp {
// constexpr double PI = 3.141592653589793;
constexpr double INTERVAL_REPORT_RATIO = 0.01;
constexpr int32_t SWIPE_DISTANCE_THRESHOLD = 50;
constexpr int32_t PINCH_DISTANCE_THRESHOLD = 100;
constexpr int32_t PINCH_ANGLE_THRESHOLD = 3;
constexpr int32_t SLIDE_START_THRESHOLD = 25;
constexpr int32_t SLIDE_DISTANCE_THRESHOLD = 50;
constexpr int32_t SLIDE_ANGLE_THRESHOLD = 10;
constexpr int32_t REPORT_DISTANCE_THRESHOLD = 2;

constexpr int32_t CAPACITY = 1024;

constexpr int32_t ANGLE_MIN = 0;
constexpr int32_t ANGLE_MAX = 360;
constexpr int32_t DIRECTION_LEFT_UP = 135;
constexpr int32_t DIRECTION_RIGHT_UP = 45;
constexpr int32_t DIRECTION_LEFT_DOWN = 225;
constexpr int32_t DIRECTION_RIGHT_DOWN = 315;

constexpr int32_t PIXEL_X_MAX = 720;
constexpr int32_t PIXEL_Y_MAX = 1280;
constexpr int32_t LEFT_BOUNDARY = PIXEL_X_MAX * 0.05;
constexpr int32_t RIGHT_BOUNDARY = PIXEL_X_MAX * 0.95;
constexpr int32_t DOWN_BOUNDARY = PIXEL_Y_MAX * 0.95;

// universal
// constexpr int32_t VECTEOR_MODULE_LOW_THRESHOLD = 5;
// constexpr int32_t VECTEOR_MODULE_HIGH_THRESHOLD = 12;
// constexpr int32_t ACCELERATION_VALID_THRESHOLD = 100;
// constexpr int32_t ANGLE_ONE_HUNDRED_AND_EIGHTY_DEGREE = 180;
constexpr int32_t SAMPLINGNTERVAL = 100000000;
constexpr int32_t REPORTINTERVAL = 100000000;
constexpr int32_t KEEP_TWO_DIGITS = 100;
constexpr double RESERVED_DIGITS = 100.0;

// absoluteStill, horizontal, vertical
// constexpr int32_t VALID_TIME_THRESHOLD = 500;
// constexpr int32_t ACC_SAMPLE_PERIOD = 100;
// constexpr int32_t COUNTER_THRESHOLD = VALID_TIME_THRESHOLD / ACC_SAMPLE_PERIOD;

// flip
constexpr int32_t FLIPPING_INTERVAL = 25;
constexpr int32_t FLIPPING_TIMER = 500;
constexpr int32_t ROLL_HORIZONTAL_LOW_THRESHOLD = 160;
constexpr int32_t ROLL_HORIZONTAL_HIGH_THRESHOLD = 180;
constexpr int32_t REVERSE_ROLL_HORIZONTAL_LOW_THRESHOLD = 0;
constexpr int32_t REVERSE_ROLL_HORIZONTAL_HIGH_THRESHOLD = 20;
constexpr int32_t PITCH_HORIZONTAL_LOW_THRESHOLD = 160;
constexpr int32_t PITCH_HORIZONTAL_HIGH_THRESHOLD = 180;
constexpr int32_t REVERSE_PITCH_HORIZONTAL_LOW_THRESHOLD = 0;
constexpr int32_t REVERSE_PITCH_HORIZONTAL_HIGH_THRESHOLD = 20;

// rotation
constexpr int32_t CONSTANT_FOUR = 4;
constexpr int32_t ANGLE_FORTY_FIVE_DEGREE = 45;
constexpr int32_t ANGLE_NITETY_DEGREE = 90;
constexpr int32_t ANGLE_ONE_HUNDRED_AND_THIRTY_FIVE_DEGREE = 135;
constexpr int32_t ANGLE_TWO_HUNDRED_AND_TWENTY_FIVE_DEGREE = 225;
constexpr int32_t ANGLE_THREE_HUNDRED_AND_FIFTEEN_DEGREE = 315;
constexpr int32_t ANGLE_THREE_HUNDRED_AND_SIXTY_DEGREE = 360;

// shake
constexpr int32_t SHAKE_ABS_X_THRESHOLD = 11;
constexpr int32_t SHAKE_ABS_Y_THRESHOLD = 11;
constexpr int32_t SHAKE_ABS_Z_THRESHOLD = 11;

// pocket
constexpr int32_t POCKET_ALS_THRESHOLD = 6;
constexpr int32_t POCKET_ABS_Z_THRESHOLD = 6;

// pickup & ear
constexpr int NEAR_EAR_THRESHOLD = 3;
constexpr int PICKUP_THRESHOLD = 3;
// HORIZONTAL
constexpr int PICKUP_AND_EAR_HRZ_THRESHOLD = -9;
constexpr int PICKUP_AND_EAR_HRZ_ABS_X_THRESHOLD = 1;
constexpr int PICKUP_AND_EAR_HRZ_ABS_Y_THRESHOLD = 1;
// SHORT_VERTICAL
constexpr int PICKUP_AND_EAR_SHORT_VTC_ABS_X_THRESHOLD = 9;
constexpr int PICKUP_AND_EAR_SHORT_VTC_ABS_Y_THRESHOLD = 1;
constexpr int PICKUP_AND_EAR_SHORT_VTC_ABS_Z_THRESHOLD = 1;
// LONG_VERTICAL
constexpr int PICKUP_AND_EAR_LONG_VTC_Y_THRESHOLD = 9;
constexpr int PICKUP_AND_EAR_LONG_VTC_ABS_X_THRESHOLD = 1;
constexpr int PICKUP_AND_EAR_LONG_VTC_ABS_Z_THRESHOLD = 1;
// FLIPPED
constexpr int PICKUP_AND_EAR_FLIPPED_ABS_Z_THRESHOLD = 9;
constexpr int PICKUP_AND_EAR_FLIPPED_ABS_X_THRESHOLD = 1;
constexpr int PICKUP_AND_EAR_FLIPPED_ABS_Y_THRESHOLD = 1;
// HandleHorizontal
constexpr int PICKUP_AND_EAR_VECTOR_MODULE_UP_THRESHOLD = 12;
constexpr int PICKUP_AND_EAR_VECTOR_MODULE_DOWN_THRESHOLD = 8;
// HandleNonAbsoluteStill
constexpr int PICKUP_AND_EAR_PITCH_DOWN_THRESHOLD = 45;
constexpr int PICKUP_AND_EAR_PITCH_UP_THRESHOLD = 110;
// HandlePickup
constexpr int HANDLE_PICKUP_PITCH_THRESHOLD = 45;

constexpr int32_t EVENT_CANCEL = 1;
constexpr int32_t EVENT_DOWN = 2;
constexpr int32_t EVENT_MOVE = 3;
constexpr int32_t EVENT_UP = 4;

enum PointerAction {
    POINTER_ACTION_CANCEL = 1,
    POINTER_ACTION_DOWN,
    POINTER_ACTION_MOVE,
    POINTER_ACTION_UP
};

enum States {
    STATE_IDLE,
    STATE_ONE_FINGER_DOWN,
    STATE_ONE_FINGER_MOVE,
    STATE_SWIPE,
    STATE_TWO_FINGERS_DOWN,
    STATE_TWO_FINGERS_MOVE,
    STATE_PINCH,
    STATE_THREE_FINGERS_DOWN,
    STATE_THREE_FINGERS_MOVE,
    STATE_SLIDE,
    STATE_END,
    STATE_ERROR
};

enum FingerEvent {
    EVENT_FINGER_EVENT_INVALID,
    EVENT_FIRST_FINGER_DOWN,
    EVENT_OTHER_FINGER_DOWN,
    EVENT_OTHER_FINGER_UP,
    EVENT_ALL_FINGERS_UP,
    EVENT_FINGER_EVENT_MOVE,
    EVENT_FINGER_EVENT_CANCEL
};

struct PointEventInfo {
    int32_t pointId;
    int32_t pointAction;
    int32_t x;
    int32_t y;
    int64_t downTime;
};

struct MotionGestureKey {
    int32_t pointId;
    int32_t pointAction;
    bool operator< (MotionGestureKey const& motiongesturekey) const
    {
        if (pointId < motiongesturekey.pointId) {
            return true;
        }
        if (pointId == motiongesturekey.pointId) {
            if (pointAction < motiongesturekey.pointAction) {
                return true;
            }
        }
        return false;
    }

    bool operator== (MotionGestureKey const& motiongesturekey) const
    {
        if (pointId == motiongesturekey.pointId && pointAction == motiongesturekey.pointAction) {
            return true;
        }
        return false;
    }
};

using PointsEventCallback = std::function<void(std::shared_ptr<OHOS::MMI::PointerEvent> pointEvent)>;
using SensorCallback = std::function<void(int32_t, void*)>;
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_DATA_DEFINE_H