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

#ifndef MOTION_DATA_UTILS_H
#define MOTION_DATA_UTILS_H

namespace OHOS {
namespace Msdp {
class MotionDataUtils {
public:
    enum MotionType {
        TYPE_INVALID = -1,
        TYPE_PICKUP,
        TYPE_TAKE_OFF,
        TYPE_CLOSE_TO_EAR,
        TYPE_FLIP,
        TYPE_WAVE,
        TYPE_SHAKE,
        TYPE_ROTATE,
        TYPE_POCKET,
        TYPE_WRIST_TILT,
        TYPE_LEFT_SIDE_SWIPE,
        TYPE_RIGHT_SIDE_SWIPE,
        TYPE_UP_SIDE_SWIPE,
        TYPE_TWO_FINGERS_PINCH,
        TYPE_THREE_FINGERS_SLIDE,
    };

    enum MotionTypeValue {
        INVALID = 0,
        VALID,
    };

    enum MotionValue {
        VALUE_INVALID = -1,
        VALUE_EXIT,
        VALUE_ENTER
    };

    enum Status {
        STATUS_INVALID = -1,
        STATUS_CANCEL,
        STATUS_START,
        STATUS_PROCESS,
        STATUS_FINISH
    };

    enum Action {
        ACTION_INVALID = -1,
        ACTION_ENLARGE,
        ACTION_REDUCE,
        ACTION_UP,
        ACTION_LEFT,
        ACTION_DOWN,
        ACTION_RIGHT
    };

    enum RotateAction {
        ROTATE_ACTION_INVALID = -1,
        TYPE_HORIZONTAL_ZERO,
        TYPE_VERTICAL_90 = 90,
        TYPE_VERTICAL_270 = 270,
        TYPE_HORIZONTAL_180 = 180
    };

    struct MotionData {
        MotionType type;
        MotionValue value;
        Status status;
        Action action;
        RotateAction rotateAction;
        double move;

        bool operator!= (MotionData const& data) const {
            if (type == data.type && value == data.value
            && status == data.status && action == data.action && rotateAction == data.rotateAction && move == data.move) {
                return false;
            }
            return true;
        }
    };
};

typedef struct MotionJsonData {
    int Type;
    char Json[20];
}MotionJsonD;

static MotionJsonD MotionJson[] = {
    {MotionDataUtils::MotionType::TYPE_PICKUP, "PICKUP"},
    {MotionDataUtils::MotionType::TYPE_TAKE_OFF, "TAKEOFF"},
    {MotionDataUtils::MotionType::TYPE_CLOSE_TO_EAR, "CLOSE_TO_EAR"},
    {MotionDataUtils::MotionType::TYPE_FLIP, "FLIP"},
    {MotionDataUtils::MotionType::TYPE_WAVE, "WAVE"},
    {MotionDataUtils::MotionType::TYPE_SHAKE, "SHAKE"},
    {MotionDataUtils::MotionType::TYPE_ROTATE, "ROTATE"},
    {MotionDataUtils::MotionType::TYPE_POCKET, "POCKET"},
    {MotionDataUtils::MotionType::TYPE_WRIST_TILT, "WRIST_TILT"},
    {MotionDataUtils::MotionType::TYPE_LEFT_SIDE_SWIPE, "LEFT_SIDE_SWIPE"},
    {MotionDataUtils::MotionType::TYPE_RIGHT_SIDE_SWIPE, "RIGHT_SIDE_SWIPE"},
    {MotionDataUtils::MotionType::TYPE_UP_SIDE_SWIPE, "UP_SIDE_SWIPE"},
    {MotionDataUtils::MotionType::TYPE_TWO_FINGERS_PINCH, "TWO_FINGERS_PINCH"},
    {MotionDataUtils::MotionType::TYPE_THREE_FINGERS_SLIDE, "THREE_FINGERS_SLIDE"}
};

static int32_t in_vector_count[MotionDataUtils::MotionType::TYPE_THREE_FINGERS_SLIDE + 1] = {0};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_DATA_UTILS_H