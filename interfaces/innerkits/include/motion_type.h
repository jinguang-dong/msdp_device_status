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

#ifndef MOTION_TYPE_H
#define MOTION_TYPE_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define NAME_MAX_LEN 48

/**
 * @brief Enumerates device status types.
 *
 * @since 5
 */
typedef enum MotionTypeId {
    MOTION_TYPE_ID_NONE = 0,                    /**< None */
    MOTION_TYPE_ID_PICKUP = 1,                  /**< Pick up */
    MOTION_TYPE_ID_TAKE_OFF = 2,                /**< Take off */
    MOTION_TYPE_ID_CLOSE_TO_EAR = 3,            /**< Close to ear */
    MOTION_TYPE_ID_FLIP = 4,                    /**< Flip */
    MOTION_TYPE_ID_WAVE = 5,                    /**< Wave */
    MOTION_TYPE_ID_SHAKE = 6,                   /**< Shake */ 
    MOTION_TYPE_ID_ROTATE = 7,                  /**< Rotate */
    MOTION_TYPE_ID_POCKET = 8,                  /**< POCKET */
    MOTION_TYPE_ID_WRIST_TILT = 9,              /**< Wrist Tilt */
    MOTION_TYPE_ID_LEFT_SIDE_SWIPE  = 10,       /**< Left Side Swipe */
    MOTION_TYPE_ID_RIGHT_SIDE_SWIPE  = 11,      /**< Right Side Swipe */
    MOTION_TYPE_ID_UP_SIDE_SWIPE  = 12,         /**< Up Side Swipe */
    MOTION_TYPE_ID_TWO_FINGERS_PINCH = 13,      /**< Two Fingers Pinch */
    MOTION_TYPE_ID_THREE_FINGERS_SLIDE = 14,    /**< Three Fingers Pinch */
    MOTION_ID_MAX = 30,                         /**< Maximum number of Device status type IDs*/
}MotionTypeId;

/**
 * @brief define the reported status
 *
 */
typedef enum MotionStatus {
    STATUS_INVALID = -1,
    STATUS_EXIT = 0,
    STATUS_ENTER = 1,
}MotionStatus;

/**
 * @brief Defines the reported data.
 *
 */
typedef struct MotionEvent {
    int32_t motionTypeId;   /**< Motion type identifier */
    int32_t result;         /**< Motion result */
    int32_t status;         /**< Motion status */
    int32_t action;         /**< Motion action */
    double  rate;           /**< Slip rate */
} MotionEvent;

/**
 * @brief Defines the callback for data reporting by the motion.
 *
 */
typedef void (*RecordMotionCallback)(MotionEvent *event);

/**
 * @brief Defines information about the motion subscriber.
 *
 */
typedef struct MotionUser {
    char name[NAME_MAX_LEN];  /**< Name of the motion subscriber */
    RecordMotionCallback callback;   /**< Callback for reporting motion result*/
} MotionUser;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif // MOTION_TYPE_H