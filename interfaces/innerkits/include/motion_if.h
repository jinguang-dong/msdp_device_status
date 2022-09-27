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

#ifndef MOTION_IF_H
#define MOTION_IF_H

#include "motion_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief Get the motion event in the system.
 *
 * @param motionTypeId Indicates the motion event type.
 * @param event Indicates the motion event
 * @return Returns <b>0</b> if the event is obtained; returns a non-zero value otherwise.
 */
int32_t GetMotionEvent(int32_t motionTypeId, MotionEvent* event);

/**
 * @brief Subscribes to motion event. The system will report the obtained motion event to the subscriber.
 *
 * @param motionTypeId Indicates the ID of a motion type. For details, see {@link MotionTypeId}.
 * @param user Indicates the pointer to the motion subscriber that requests motion event. For details,
 * see {@link MotionUser}.
 * @return Returns <b>0</b> if the event is obtained; returns a non-zero value otherwise.
 */
int32_t SubscribeMotionEvent(int32_t motionTypeId, const MotionUser* user);

/**
 * @brief Unsubscribes to motion event
 *
 * @param motionTypeId Indicates the ID of a motion type. For details, see {@link MotionTypeId}.
 * @param user Indicates the pointer to the motion subscriber that requests motion event. For details,
 * see {@link MotionUser}.
 * @return Returns <b>0</b> if the event is obtained; returns a non-zero value otherwise.
 */
int32_t UnsubscribeMotionEvent(int32_t motionTypeId, const MotionUser *user);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* MOTION_IF_H */