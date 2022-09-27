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

#ifndef DATA_DEFINE_H
#define DATA_DEFINE_H

#include <functional>
#include "cJSON.h"

namespace OHOS {
namespace Msdp {
constexpr double PI = 3.141592653589793;
constexpr int32_t VECTEOR_MODULE_LOW_THRESHOLD = 5;
constexpr int32_t VECTEOR_MODULE_HIGH_THRESHOLD = 12;
constexpr int32_t ACCELERATION_VALID_THRESHOLD = 100;
constexpr int32_t ANGLE_ONE_HUNDRED_AND_EIGHTY_DEGREE = 180;
constexpr int32_t VALID_TIME_THRESHOLD = 500;
constexpr int32_t ACC_SAMPLE_PERIOD = 100;
constexpr int32_t COUNTER_THRESHOLD = VALID_TIME_THRESHOLD / ACC_SAMPLE_PERIOD;


const std::string MSDP_MOTION_DATA_PATH = "/data/msdp/motion_data.json";
const std::string MSDP_DATA_DIR = "/data/msdp";

struct JsonParser {
    JsonParser() = default;
    ~JsonParser()
    {
        if (json_ != nullptr) {
            cJSON_Delete(json_);
        }
    }
    operator cJSON *()
    {
        return json_;
    }
    cJSON *json_ = nullptr;
};

// constexpr double INTERVAL_REPORT_RATIO = 0.01;
// constexpr int32_t SWIPE_DISTANCE_THRESHOLD = 50;
// constexpr int32_t PINCH_DISTANCE_THRESHOLD = 100;
// constexpr int32_t PINCH_ANGLE_THRESHOLD = 3;
// constexpr int32_t SLIDE_START_THRESHOLD = 25;
// constexpr int32_t SLIDE_DISTANCE_THRESHOLD = 50;
// constexpr int32_t SLIDE_ANGLE_THRESHOLD = 10;
// constexpr int32_t REPORT_DISTANCE_THRESHOLD = 2;

// constexpr int32_t CAPACITY = 1024;

// constexpr int32_t ANGLE_MIN = 0;
// constexpr int32_t ANGLE_MAX = 360;
// constexpr int32_t DIRECTION_LEFT_UP = 135;
// constexpr int32_t DIRECTION_RIGHT_UP = 45;
// constexpr int32_t DIRECTION_LEFT_DOWN = 225;
// constexpr int32_t DIRECTION_RIGHT_DOWN = 315;

// constexpr int32_t PIXEL_X_MAX = 720;
// constexpr int32_t PIXEL_Y_MAX = 1280;
// constexpr int32_t LEFT_BOUNDARY = PIXEL_X_MAX * 0.05;
// constexpr int32_t RIGHT_BOUNDARY = PIXEL_X_MAX * 0.95;
// constexpr int32_t DOWN_BOUNDARY = PIXEL_Y_MAX * 0.95;

// // universal
// constexpr int32_t VECTEOR_MODULE_LOW_THRESHOLD = 5;
// constexpr int32_t VECTEOR_MODULE_HIGH_THRESHOLD = 12;
// constexpr int32_t ACCELERATION_VALID_THRESHOLD = 100;
// constexpr int32_t ANGLE_ONE_HUNDRED_AND_EIGHTY_DEGREE = 180;
// constexpr int32_t SAMPLINGNTERVAL = 100000000;
// constexpr int32_t REPORTINTERVAL = 100000000;
// constexpr int32_t KEEP_TWO_DIGITS = 100;
// constexpr double RESERVED_DIGITS = 100.0;

// // absoluteStill, horizontal, vertical
// constexpr int32_t VALID_TIME_THRESHOLD = 500;
// constexpr int32_t ACC_SAMPLE_PERIOD = 100;
// constexpr int32_t COUNTER_THRESHOLD = VALID_TIME_THRESHOLD / ACC_SAMPLE_PERIOD;

// // flip
// constexpr int32_t FLIPPING_INTERVAL = 25;
// constexpr int32_t FLIPPING_TIMER = 500;
// constexpr int32_t ROLL_HORIZONTAL_LOW_THRESHOLD = 160;
// constexpr int32_t ROLL_HORIZONTAL_HIGH_THRESHOLD = 180;
// constexpr int32_t REVERSE_ROLL_HORIZONTAL_LOW_THRESHOLD = 0;
// constexpr int32_t REVERSE_ROLL_HORIZONTAL_HIGH_THRESHOLD = 20;
// constexpr int32_t PITCH_HORIZONTAL_LOW_THRESHOLD = 160;
// constexpr int32_t PITCH_HORIZONTAL_HIGH_THRESHOLD = 180;
// constexpr int32_t REVERSE_PITCH_HORIZONTAL_LOW_THRESHOLD = 0;
// constexpr int32_t REVERSE_PITCH_HORIZONTAL_HIGH_THRESHOLD = 20;

// // rotation
// constexpr int32_t CONSTANT_FOUR = 4;
// constexpr int32_t ANGLE_FORTY_FIVE_DEGREE = 45;
// constexpr int32_t ANGLE_NITETY_DEGREE = 90;
// constexpr int32_t ANGLE_ONE_HUNDRED_AND_THIRTY_FIVE_DEGREE = 135;
// constexpr int32_t ANGLE_TWO_HUNDRED_AND_TWENTY_FIVE_DEGREE = 225;
// constexpr int32_t ANGLE_THREE_HUNDRED_AND_FIFTEEN_DEGREE = 315;
// constexpr int32_t ANGLE_THREE_HUNDRED_AND_SIXTY_DEGREE = 360;

// // shake
// constexpr int32_t SHAKE_ABS_X_THRESHOLD = 11;
// constexpr int32_t SHAKE_ABS_Y_THRESHOLD = 11;
// constexpr int32_t SHAKE_ABS_Z_THRESHOLD = 11;

// // pocket
// constexpr int32_t POCKET_ALS_THRESHOLD = 6;
// constexpr int32_t POCKET_ABS_Z_THRESHOLD = 6;

// // pickup & ear
// constexpr int NEAR_EAR_THRESHOLD = 3;
// constexpr int PICKUP_THRESHOLD = 3;
// // HORIZONTAL
// constexpr int PICKUP_AND_EAR_HRZ_THRESHOLD = -9;
// constexpr int PICKUP_AND_EAR_HRZ_ABS_X_THRESHOLD = 1;
// constexpr int PICKUP_AND_EAR_HRZ_ABS_Y_THRESHOLD = 1;
// // SHORT_VERTICAL
// constexpr int PICKUP_AND_EAR_SHORT_VTC_ABS_X_THRESHOLD = 9;
// constexpr int PICKUP_AND_EAR_SHORT_VTC_ABS_Y_THRESHOLD = 1;
// constexpr int PICKUP_AND_EAR_SHORT_VTC_ABS_Z_THRESHOLD = 1;
// // LONG_VERTICAL
// constexpr int PICKUP_AND_EAR_LONG_VTC_Y_THRESHOLD = 9;
// constexpr int PICKUP_AND_EAR_LONG_VTC_ABS_X_THRESHOLD = 1;
// constexpr int PICKUP_AND_EAR_LONG_VTC_ABS_Z_THRESHOLD = 1;
// // FLIPPED
// constexpr int PICKUP_AND_EAR_FLIPPED_ABS_Z_THRESHOLD = 9;
// constexpr int PICKUP_AND_EAR_FLIPPED_ABS_X_THRESHOLD = 1;
// constexpr int PICKUP_AND_EAR_FLIPPED_ABS_Y_THRESHOLD = 1;
// // HandleHorizontal
// constexpr int PICKUP_AND_EAR_VECTOR_MODULE_UP_THRESHOLD = 12;
// constexpr int PICKUP_AND_EAR_VECTOR_MODULE_DOWN_THRESHOLD = 8;
// // HandleNonAbsoluteStill
// constexpr int PICKUP_AND_EAR_PITCH_DOWN_THRESHOLD = 45;
// constexpr int PICKUP_AND_EAR_PITCH_UP_THRESHOLD = 110;
// // HandlePickup
// constexpr int HANDLE_PICKUP_PITCH_THRESHOLD = 45;
} // namespace Msdp
} // namespace OHOS
#endif // DATA_DEFINE_H