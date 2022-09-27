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

#include "motion_gesture.h"
#include "motion_common.h"
#include <unistd.h>

namespace OHOS {
namespace Msdp {
void MotionGesture::Init()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    state_ = STATE_IDLE;
    int32_t ret = sem_init(&sem_, 0, 0);
    if (ret == -1) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "sem init failed");
        return;
    }
    ReportinfoInit();
    fingerCount_ = 0;
    pointsMap_.clear();
    buffer_.clear();
    std::make_unique<std::thread>(&MotionGesture::Loop, this)->detach();
    PointsEventCallback callback = std::bind(&MotionGesture::OnPointEventChanged, this, std::placeholders::_1);
    pointCallback_->SubscribePointEvent(callback);
}

void MotionGesture::ReportinfoInit()
{
    reportInfo_.type = MotionDataUtils::TYPE_INVALID;
    reportInfo_.value = MotionDataUtils::VALUE_INVALID;
    reportInfo_.status = MotionDataUtils::STATUS_INVALID;
    reportInfo_.action = MotionDataUtils::ACTION_INVALID;
    reportInfo_.move = 0;
}

void MotionGesture::Loop()
{
    while (true) {
        sem_wait(&sem_);
        HandleTouchEvent();
    }
}

void MotionGesture::OnPointEventChanged(const std::shared_ptr<PointerEvent> &pointEvent)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s enter", __func__);
    GetPointerInfo(pointEvent);
}

FingerEvent MotionGesture::DetermineFingerEvent(PointEventInfo pointInfo)
{
    FingerEvent event;
    PointerAction pointerAction = static_cast<PointerAction>(pointInfo.pointAction);
    switch (pointerAction) {
        case POINTER_ACTION_CANCEL: {
            MOTION_HILOGI(MOTION_MODULE_SERVICE, "CANCEL");
            event = EVENT_FINGER_EVENT_CANCEL;
            break;
        }
        case POINTER_ACTION_DOWN: {
            if (fingerCount_ == 0 && pointInfo.pointId == 0) {
                event = EVENT_FIRST_FINGER_DOWN;
                fingerCount_ = 1;
            } else {
                event = EVENT_OTHER_FINGER_DOWN;
                fingerCount_++;
            }
            break;
        }
        case POINTER_ACTION_MOVE: {
            event = EVENT_FINGER_EVENT_MOVE;
            break;
        }
        case POINTER_ACTION_UP: {
            if (fingerCount_ == 1) {
                event = EVENT_ALL_FINGERS_UP;
                fingerCount_--;
            } else {
                event = EVENT_OTHER_FINGER_UP;
                fingerCount_--;
            }
            break;
        }
        default: {
            event = EVENT_FINGER_EVENT_INVALID;
            break;
        }
    }
    return event;
}

MotionGestureKey MotionGesture::CreateFingerKey(PointEventInfo pointInfo)
{
    MotionGestureKey motiongesturekey;
    motiongesturekey.pointId = pointInfo.pointId;
    motiongesturekey.pointAction = pointInfo.pointAction;
    return motiongesturekey;
}

MotionGestureKey MotionGesture::CreateFingerDownKey(PointEventInfo pointInfo)
{
    MotionGestureKey motiongesturekey;
    motiongesturekey.pointId = pointInfo.pointId;
    motiongesturekey.pointAction = EVENT_DOWN;
    return motiongesturekey;
}

MotionGestureKey MotionGesture::CreateFingerMoveKey(PointEventInfo pointInfo)
{
    MotionGestureKey motiongesturekey;
    motiongesturekey.pointId = pointInfo.pointId;
    motiongesturekey.pointAction = EVENT_MOVE;
    return motiongesturekey;
}

void MotionGesture::CreateFingerMoveKey(MotionGestureKey firstIndex, MotionGestureKey &secondIndex)
{
    secondIndex.pointId = firstIndex.pointId;
    secondIndex.pointAction = EVENT_MOVE;
}

void MotionGesture::GetFingerMoveKey(MotionGestureKey &index)
{
    for (std::map<MotionGestureKey, PointEventInfo>::iterator it = pointsMap_.begin(); it != pointsMap_.end(); ++it) {
        if (it->first.pointAction == EVENT_MOVE) {
            index = it->first;
        }
    }
}

void MotionGesture::GetFingerDownKey(MotionGestureKey &index)
{
    for (std::map<MotionGestureKey, PointEventInfo>::iterator it = pointsMap_.begin(); it != pointsMap_.end(); ++it) {
        if (it->first.pointAction == EVENT_DOWN) {
            index = it->first;
        }
    }
}

void MotionGesture::GetFingerDownKey(MotionGestureKey &firstFingerDown, MotionGestureKey &secondFingerDown)
{
    for (std::map<MotionGestureKey, PointEventInfo>::iterator it = pointsMap_.begin(); it != pointsMap_.end(); ++it) {
        if (it->first.pointAction == EVENT_DOWN) {
            firstFingerDown.pointAction == NULL ? firstFingerDown = it->first : secondFingerDown = it->first;
        }
    }
}

void MotionGesture::GetFingerDownKey(MotionGestureKey &firstFingerDown, MotionGestureKey &secondFingerDown,
    MotionGestureKey &thirdFingerDown)
{
    for (std::map<MotionGestureKey, PointEventInfo>::iterator it = pointsMap_.begin(); it != pointsMap_.end(); ++it) {
        if (it->first.pointAction == EVENT_DOWN) {
            if (firstFingerDown.pointAction == NULL) {
                firstFingerDown = it->first;
            } else if (secondFingerDown.pointAction == NULL) {
                secondFingerDown = it->first;
            } else {
                thirdFingerDown = it->first;
            }
        }
    }
}

void MotionGesture::HandleIdleState(PointEventInfo pointInfo)
{
    if (DetermineFingerEvent(pointInfo) == EVENT_FIRST_FINGER_DOWN) {
        pointsMap_.insert(std::make_pair(CreateFingerKey(pointInfo), pointInfo));
        state_ = STATE_ONE_FINGER_DOWN;
    }
}

void MotionGesture::CalculateInitialDistanceAngle(PointEventInfo pointInfo)
{
    MotionGestureKey firstFingerDown = {NULL};
    MotionGestureKey secondFingerDown = {NULL};
    GetFingerDownKey(firstFingerDown);
    secondFingerDown = CreateFingerKey(pointInfo);
    pointsMap_.insert(std::make_pair(secondFingerDown, pointInfo));
    initialDistance_ = CalculateDistance(firstFingerDown, secondFingerDown);
    initialAngle_ = CalculateAngle(firstFingerDown, secondFingerDown);
}

void MotionGesture::CalculateInitialDistanceAngleMove(PointEventInfo pointInfo)
{
    MotionGestureKey firstFingerMove = {NULL};
    MotionGestureKey firstFingerDown = {NULL};
    MotionGestureKey secondFingerDown = {NULL};
    GetFingerDownKey(firstFingerDown);
    GetFingerMoveKey(firstFingerMove);
    pointsMap_[firstFingerDown].x = pointsMap_[firstFingerMove].x;
    pointsMap_[firstFingerDown].y = pointsMap_[firstFingerMove].y;
    pointsMap_.erase(firstFingerMove);
    secondFingerDown = CreateFingerKey(pointInfo);
    pointsMap_[secondFingerDown] = pointInfo;
    initialDistance_ = CalculateDistance(firstFingerDown, secondFingerDown);
    initialAngle_ = CalculateAngle(firstFingerDown, secondFingerDown);
}

void MotionGesture::HandleOneFingerDownState(PointEventInfo pointInfo)
{
    switch (DetermineFingerEvent(pointInfo)) {
        case EVENT_FIRST_FINGER_DOWN: {
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            break;
        }
        case EVENT_OTHER_FINGER_DOWN: {
            CalculateInitialDistanceAngle(pointInfo);
            state_ = STATE_TWO_FINGERS_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_UP: {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            pointsMap_.clear();
            state_ = STATE_ERROR;
            break;
        }
        case EVENT_ALL_FINGERS_UP: {
            pointsMap_.clear();
            state_ = STATE_IDLE;
            break;
        }
        case EVENT_FINGER_EVENT_MOVE: {
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_ONE_FINGER_MOVE;
            break;
        }
        case EVENT_FINGER_EVENT_CANCEL: {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            reportInfo_.status = MotionDataUtils::STATUS_CANCEL;
            state_ = STATE_END;
            break;
        }
        default: {
            break;
        }
    }
}

void MotionGesture::HandleOneFingerMoveState(PointEventInfo pointInfo)
{
    switch (DetermineFingerEvent(pointInfo)) {
        case EVENT_FIRST_FINGER_DOWN: {
            pointsMap_.clear();
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_ONE_FINGER_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_DOWN: {
            CalculateInitialDistanceAngleMove(pointInfo);
            state_ = STATE_TWO_FINGERS_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_UP: {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            pointsMap_.clear();
            MOTION_HILOGE(MOTION_MODULE_SERVICE, "To ERROR");
            state_ = STATE_ERROR;
            break;
        }
        case EVENT_ALL_FINGERS_UP: {
            pointsMap_.clear();
            state_ = STATE_IDLE;
            break;
        }
        case EVENT_FINGER_EVENT_MOVE: {
            SwipeDetect(pointInfo);
            break;
        }
        case EVENT_FINGER_EVENT_CANCEL: {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            reportInfo_.status = MotionDataUtils::STATUS_CANCEL;
            state_ = STATE_END;
            break;
        }
        default: {
            break;
        }
    }
}

void MotionGesture::HandleSwipeState(PointEventInfo pointInfo)
{
    switch (DetermineFingerEvent(pointInfo)) {
        case EVENT_FIRST_FINGER_DOWN: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            pointsMap_.clear();
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_ONE_FINGER_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_DOWN: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            CalculateInitialDistanceAngleMove(pointInfo);
            state_ = STATE_TWO_FINGERS_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_UP: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            pointsMap_.clear();
            MOTION_HILOGE(MOTION_MODULE_SERVICE, "To ERROR");
            state_ = STATE_ERROR;
            break;
        }
        case EVENT_ALL_FINGERS_UP: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            pointsMap_.clear();
            state_ = STATE_IDLE;
            break;
        }
        case EVENT_FINGER_EVENT_MOVE: {
            MotionGestureKey firstFingerDown = {NULL};
            MotionGestureKey firstFingerMove = {NULL};
            GetFingerDownKey(firstFingerDown);
            firstFingerMove = CreateFingerKey(pointInfo);
            pointsMap_[firstFingerMove] = pointInfo;
            int32_t swipeDistance = CalculateDistance(firstFingerDown, firstFingerMove);
            reportInfo_.status = MotionDataUtils::STATUS_PROCESS;
            if ((swipeDistance - reportInfo_.move) > REPORT_DISTANCE_THRESHOLD) {
                reportInfo_.move = swipeDistance;
                Report();
            }
            break;
        }
        case EVENT_FINGER_EVENT_CANCEL: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            state_ = STATE_END;
            break;
        }
        default: {
            break;
        }
    }
}

void MotionGesture::HandleTwoFingersDownState(PointEventInfo pointInfo)
{
    switch (DetermineFingerEvent(pointInfo)) {
        case EVENT_FIRST_FINGER_DOWN: {
            pointsMap_.clear();
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_ONE_FINGER_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_DOWN: {
            TwoFingersToThreeFingers(pointInfo);
            state_ = STATE_THREE_FINGERS_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_UP: {
            TwoFingersToOneFinger(pointInfo);
            state_ = STATE_ONE_FINGER_DOWN;
            break;
        }
        case EVENT_ALL_FINGERS_UP: {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            pointsMap_.clear();
            MOTION_HILOGE(MOTION_MODULE_SERVICE, "To ERROR");
            state_ = STATE_ERROR;
            break;
        }
        case EVENT_FINGER_EVENT_MOVE: {
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_TWO_FINGERS_MOVE;
            break;
        }
        case EVENT_FINGER_EVENT_CANCEL: {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            reportInfo_.status = MotionDataUtils::STATUS_CANCEL;
            state_ = STATE_END;
            break;
        }
        default: {
            break;
        }
    }
}

void MotionGesture::HandleTwoFingersMoveState(PointEventInfo pointInfo)
{
    switch (DetermineFingerEvent(pointInfo)) {
        case EVENT_FIRST_FINGER_DOWN: {
            pointsMap_.clear();
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_ONE_FINGER_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_DOWN: {
            TwoFingersToThreeFingers(pointInfo);
            state_ = STATE_THREE_FINGERS_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_UP: {
            TwoFingersToOneFinger(pointInfo);
            state_ = STATE_ONE_FINGER_DOWN;
            break;
        }
        case EVENT_ALL_FINGERS_UP: {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            pointsMap_.clear();
            MOTION_HILOGE(MOTION_MODULE_SERVICE, "To ERROR");
            state_ = STATE_ERROR;
            break;
        }
        case EVENT_FINGER_EVENT_MOVE: {
            PinchDetect(pointInfo);
            break;
        }
        case EVENT_FINGER_EVENT_CANCEL: {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            reportInfo_.status = MotionDataUtils::STATUS_CANCEL;
            state_ = STATE_END;
            break;
        }
        default: {
            break;
        }
    }
}

void MotionGesture::HandlePinchState(PointEventInfo pointInfo)
{
    switch (DetermineFingerEvent(pointInfo)) {
        case EVENT_FIRST_FINGER_DOWN: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            pointsMap_.clear();
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_ONE_FINGER_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_DOWN: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            TwoFingersToThreeFingers(pointInfo);
            state_ = STATE_THREE_FINGERS_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_UP: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            TwoFingersToOneFinger(pointInfo);
            state_ = STATE_ONE_FINGER_DOWN;
            break;
        }
        case EVENT_ALL_FINGERS_UP: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            pointsMap_.clear();
            MOTION_HILOGE(MOTION_MODULE_SERVICE, "To ERROR");
            state_ = STATE_ERROR;
            break;
        }
        case EVENT_FINGER_EVENT_MOVE: {
            reportInfo_.status = MotionDataUtils::STATUS_PROCESS;
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            int32_t movingAngle = 0;
            bool flag = CalculateMovingAngle(movingAngle);
            if (!flag)
            {
                return;
            }
            
            if (abs(movingAngle - initialAngle_) > PINCH_ANGLE_THRESHOLD) {
                MOTION_HILOGI(MOTION_MODULE_SERVICE, "It's not in the Angle");
                reportInfo_.value = MotionDataUtils::VALUE_EXIT;
                reportInfo_.status = MotionDataUtils::STATUS_FINISH;
                Report();
                state_ = STATE_END;
                return;
            }
            double scaleRate = CalculatePinchScale();
            if (abs(reportInfo_.move - scaleRate) > INTERVAL_REPORT_RATIO) {
                scaleRate = double((int)(scaleRate * KEEP_TWO_DIGITS)) / RESERVED_DIGITS;
                reportInfo_.move = scaleRate;
                Report();
            }
            break;
        }
        case EVENT_FINGER_EVENT_CANCEL: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            state_ = STATE_END;
            break;
        }
        default: {
            break;
        }
    }
}

bool MotionGesture::CalculateMovingAngle(int32_t& movingAngle)
{
    auto iter = pointsMap_.find(firstFingerMoveKey_);
    if (iter == pointsMap_.end()) {
        auto iter = pointsMap_.find(secondFingerMoveKey_);
        if (iter == pointsMap_.end()) {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            pointsMap_.clear();
            state_ = STATE_ERROR;
            return false;
        } else {
            movingAngle = CalculateAngle(firstFingerMoveKey_, secondFingerMoveKey_);
            return true;
        }
    } else {
        auto iter = pointsMap_.find(secondFingerMoveKey_);
        if (iter == pointsMap_.end()) {
            movingAngle = CalculateAngle(firstFingerMoveKey_, secondFingerMoveKey_);
            return true;
        } else {
            movingAngle = CalculateAngle(firstFingerMoveKey_, secondFingerMoveKey_);
            return true;
        }
    }
}

void MotionGesture::HandleThreeFingersDownState(PointEventInfo pointInfo)
{
    switch (DetermineFingerEvent(pointInfo)) {
        case EVENT_FIRST_FINGER_DOWN: {
            pointsMap_.clear();
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_ONE_FINGER_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_DOWN: {
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_END;
            break;
        }
        case EVENT_OTHER_FINGER_UP: {
            ThreeFingersToTwoFingers(pointInfo);
            state_ = STATE_TWO_FINGERS_DOWN;
            break;
        }
        case EVENT_ALL_FINGERS_UP: {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            pointsMap_.clear();
            MOTION_HILOGE(MOTION_MODULE_SERVICE, "To ERROR");
            state_ = STATE_ERROR;
            break;
        }
        case EVENT_FINGER_EVENT_MOVE: {
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_THREE_FINGERS_MOVE;
            break;
        }
        case EVENT_FINGER_EVENT_CANCEL: {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            reportInfo_.status = MotionDataUtils::STATUS_CANCEL;
            state_ = STATE_END;
            break;
        }
        default: {
            break;
        }
    }
}

void MotionGesture::HandleThreeFingersMoveState(PointEventInfo pointInfo)
{
    switch (DetermineFingerEvent(pointInfo)) {
        case EVENT_FIRST_FINGER_DOWN: {
            pointsMap_.clear();
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_ONE_FINGER_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_DOWN: {
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_END;
            break;
        }
        case EVENT_OTHER_FINGER_UP: {
            ThreeFingersToTwoFingers(pointInfo);
            state_ = STATE_TWO_FINGERS_DOWN;
            break;
        }
        case EVENT_ALL_FINGERS_UP: {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            pointsMap_.clear();
            MOTION_HILOGE(MOTION_MODULE_SERVICE, "To ERROR");
            state_ = STATE_ERROR;
            break;
        }
        case EVENT_FINGER_EVENT_MOVE: {
            SlideDetect(pointInfo);
            break;
        }
        case EVENT_FINGER_EVENT_CANCEL: {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            reportInfo_.status = MotionDataUtils::STATUS_CANCEL;
            state_ = STATE_END;
            break;
        }
        default: {
            break;
        }
    }
}

void MotionGesture::HandleSlideState(PointEventInfo pointInfo)
{
    switch (DetermineFingerEvent(pointInfo)) {
        case EVENT_FIRST_FINGER_DOWN: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            pointsMap_.clear();
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_ONE_FINGER_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_DOWN: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_END;
            break;
        }
        case EVENT_OTHER_FINGER_UP: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            ThreeFingersToTwoFingers(pointInfo);
            state_ = STATE_TWO_FINGERS_DOWN;
            break;
        }
        case EVENT_ALL_FINGERS_UP: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            pointsMap_.clear();
            MOTION_HILOGE(MOTION_MODULE_SERVICE, "To ERROR");
            state_ = STATE_ERROR;
            break;
        }
        case EVENT_FINGER_EVENT_MOVE: {
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            MotionGestureKey firstFingerDown = {NULL};
            MotionGestureKey secondFingerDown = {NULL};
            MotionGestureKey thirdFingerDown = {NULL};
            GetFingerDownKey(firstFingerDown, secondFingerDown, thirdFingerDown);
            MotionGestureKey firstFingerMove = {NULL};
            MotionGestureKey secondFingerMove = {NULL};
            MotionGestureKey thirdFingerMove = {NULL};
            CreateFingerMoveKey(firstFingerDown, firstFingerMove);
            CreateFingerMoveKey(secondFingerDown, secondFingerMove);
            CreateFingerMoveKey(thirdFingerDown, thirdFingerMove);
            int32_t distanceMean = CalculateDistanceMean(firstFingerDown, secondFingerDown, thirdFingerDown,
                firstFingerMove, secondFingerMove, thirdFingerMove);
            reportInfo_.status = MotionDataUtils::STATUS_PROCESS;
            if (abs(distanceMean - reportInfo_.move) > REPORT_DISTANCE_THRESHOLD) {
                reportInfo_.move = distanceMean;
                Report();
            }
            break;
        }
        case EVENT_FINGER_EVENT_CANCEL: {
            reportInfo_.value = MotionDataUtils::VALUE_EXIT;
            reportInfo_.status = MotionDataUtils::STATUS_FINISH;
            Report();
            ReportinfoInit();
            state_ = STATE_END;
            break;
        }
        default: {
            break;
        }
    }
}

void MotionGesture::HandleEndState(PointEventInfo pointInfo)
{
    switch (DetermineFingerEvent(pointInfo)) {
        case EVENT_FIRST_FINGER_DOWN: {
            pointsMap_.clear();
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            state_ = STATE_ONE_FINGER_DOWN;
            break;
        }
        case EVENT_OTHER_FINGER_DOWN: {
            if (fingerCount_ == 2) {
                MotionGestureKey firstFingerMove = {NULL};
                MotionGestureKey firstFingerDown = {NULL};
                MotionGestureKey secondFingerDown = {NULL};
                GetFingerDownKey(firstFingerDown);
                GetFingerMoveKey(firstFingerMove);
                auto iter = pointsMap_.find(firstFingerMove);
                if (iter != pointsMap_.end()) {
                    pointsMap_[firstFingerDown].x = pointsMap_[firstFingerMove].x;
                    pointsMap_[firstFingerDown].y = pointsMap_[firstFingerMove].y;
                    pointsMap_.erase(firstFingerMove);
                }
                secondFingerDown = CreateFingerKey(pointInfo);
                pointsMap_[secondFingerDown] = pointInfo;
                initialDistance_ = CalculateDistance(firstFingerDown, secondFingerDown);
                initialAngle_ = CalculateAngle(firstFingerDown, secondFingerDown);
                state_ = STATE_TWO_FINGERS_DOWN;
            } else if (fingerCount_ == 3) {
                TwoFingersToThreeFingers(pointInfo);
                state_ = STATE_THREE_FINGERS_DOWN;
            } else {
                pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            }
            break;
        }    
        case EVENT_OTHER_FINGER_UP: {
            if (fingerCount_ == 5) {
                pointsMap_.erase(CreateFingerDownKey(pointInfo));
                pointsMap_.erase(CreateFingerMoveKey(pointInfo));
            } else if (fingerCount_ == 4) {
                FourFingersToThreeFingers(pointInfo);
                state_ = STATE_THREE_FINGERS_DOWN;
            } else if (fingerCount_ == 3) {
                ThreeFingersToTwoFingers(pointInfo);
                state_ = STATE_TWO_FINGERS_DOWN;
            } else if (fingerCount_ == 2) {
                TwoFingersToOneFinger(pointInfo);
                state_ = STATE_ONE_FINGER_DOWN;
            } else {
                MOTION_HILOGE(MOTION_MODULE_SERVICE, "To ERROR");
                pointsMap_.clear();
                state_ = STATE_ERROR;
            }
            break;
        }
        case EVENT_ALL_FINGERS_UP: {
            pointsMap_.clear();
            state_ = STATE_IDLE;
            break;
        }
        case EVENT_FINGER_EVENT_MOVE: {
            pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
            break;
        }
        case EVENT_FINGER_EVENT_CANCEL: {
            break;
        }
        default: {
            break;
        }
    }
}

void MotionGesture::HandleErrorState(PointEventInfo pointInfo)
{
    reportInfo_.value = MotionDataUtils::VALUE_INVALID;
    fingerCount_ = 0;
    if (DetermineFingerEvent(pointInfo) == EVENT_FIRST_FINGER_DOWN) {
        pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
        state_ = STATE_ONE_FINGER_DOWN;
    }
}

int32_t MotionGesture::CalculateDistance(MotionGestureKey firstFingerDown, MotionGestureKey secondFingerDown)
{
    return static_cast<int32_t>(sqrt(pow((pointsMap_[firstFingerDown].x - pointsMap_[secondFingerDown].x), 2)
        + pow((pointsMap_[firstFingerDown].y - pointsMap_[secondFingerDown].y), 2)));
}

int32_t MotionGesture::CalculateAngle(MotionGestureKey firstFingerDown, MotionGestureKey secondFingerDown)
{
    return static_cast<int32_t>(atan(abs(pointsMap_[firstFingerDown].y - pointsMap_[secondFingerDown].y)
        / abs(pointsMap_[firstFingerDown].x - pointsMap_[secondFingerDown].x))*(180 / PI));
}

double MotionGesture::CalculateScale(int32_t firstValue, int32_t secondValue)
{
    return static_cast<double>(firstValue) / static_cast<double>(secondValue);
}

void MotionGesture::TwoFingersToOneFinger(PointEventInfo pointInfo)
{
    pointsMap_.erase(CreateFingerDownKey(pointInfo));
    pointsMap_.erase(CreateFingerMoveKey(pointInfo));
    MotionGestureKey firstFingerDown = {NULL};
    MotionGestureKey firstFingerMove = {NULL};
    GetFingerDownKey(firstFingerDown);
    CreateFingerMoveKey(firstFingerDown, firstFingerMove);
    auto iter = pointsMap_.find(firstFingerMove);
    if (iter != pointsMap_.end()) {
        pointsMap_[firstFingerDown].x = pointsMap_[firstFingerMove].x;
        pointsMap_[firstFingerDown].y = pointsMap_[firstFingerMove].y;
        pointsMap_.erase(firstFingerMove);
    }
}

void MotionGesture::TwoFingersToThreeFingers(PointEventInfo pointInfo)
{
    MotionGestureKey firstFingerDown = {NULL};
    MotionGestureKey secondFingerDown = {NULL};
    MotionGestureKey firstFingerMove = {NULL};
    MotionGestureKey secondFingerMove = {NULL};
    GetFingerDownKey(firstFingerDown, secondFingerDown);
    CreateFingerMoveKey(firstFingerDown, firstFingerMove);
    CreateFingerMoveKey(secondFingerDown, secondFingerMove);
    auto firstIter = pointsMap_.find(firstFingerMove);
    if (firstIter != pointsMap_.end()) {
        pointsMap_[firstFingerDown].x = pointsMap_[firstFingerMove].x;
        pointsMap_[firstFingerDown].y = pointsMap_[firstFingerMove].y;
        pointsMap_.erase(firstFingerMove);
    }
    auto secondIter = pointsMap_.find(secondFingerMove);
    if (secondIter != pointsMap_.end()) {
        pointsMap_[secondFingerDown].x = pointsMap_[secondFingerMove].x;
        pointsMap_[secondFingerDown].y = pointsMap_[secondFingerMove].y;
        pointsMap_.erase(secondFingerMove);
    }
    pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
}

void MotionGesture::ThreeFingersToTwoFingers(PointEventInfo pointInfo)
{
    pointsMap_.erase(CreateFingerDownKey(pointInfo));
    pointsMap_.erase(CreateFingerMoveKey(pointInfo));
    MotionGestureKey firstFingerDown = {NULL};
    MotionGestureKey secondFingerDown = {NULL};
    MotionGestureKey firstFingerMove = {NULL};
    MotionGestureKey secondFingerMove = {NULL};        
    GetFingerDownKey(firstFingerDown, secondFingerDown);
    CreateFingerMoveKey(firstFingerDown, firstFingerMove);
    CreateFingerMoveKey(secondFingerDown, secondFingerMove);
    auto firstIter = pointsMap_.find(firstFingerMove);
    if (firstIter != pointsMap_.end()) {
        pointsMap_[firstFingerDown].x = pointsMap_[firstFingerMove].x;
        pointsMap_[firstFingerDown].y = pointsMap_[firstFingerMove].y;
        pointsMap_.erase(firstFingerMove);
    }
    auto secondIter = pointsMap_.find(secondFingerMove);
    if (secondIter != pointsMap_.end()) {
        pointsMap_[secondFingerDown].x = pointsMap_[secondFingerMove].x;
        pointsMap_[secondFingerDown].y = pointsMap_[secondFingerMove].y;
        pointsMap_.erase(secondFingerMove);
    }
    initialDistance_ = CalculateDistance(firstFingerDown, secondFingerDown);
    initialAngle_ = CalculateAngle(firstFingerDown, secondFingerDown);
}

void MotionGesture::FourFingersToThreeFingers(PointEventInfo pointInfo)
{
    pointsMap_.erase(CreateFingerDownKey(pointInfo));
    pointsMap_.erase(CreateFingerMoveKey(pointInfo));
    MotionGestureKey firstFingerDown = {NULL};
    MotionGestureKey secondFingerDown = {NULL};
    MotionGestureKey thirdFingerDown = {NULL};
    MotionGestureKey firstFingerMove = {NULL};
    MotionGestureKey secondFingerMove = {NULL};
    MotionGestureKey thirdFingerMove = {NULL};
    GetFingerDownKey(firstFingerDown, secondFingerDown, thirdFingerDown);
    CreateFingerMoveKey(firstFingerDown, firstFingerMove);
    CreateFingerMoveKey(secondFingerDown, secondFingerMove);
    CreateFingerMoveKey(thirdFingerDown, thirdFingerMove);
    auto firstIter = pointsMap_.find(firstFingerMove);
    if (firstIter != pointsMap_.end()) {
        pointsMap_[firstFingerDown].x = pointsMap_[firstFingerMove].x;
        pointsMap_[firstFingerDown].y = pointsMap_[firstFingerMove].y;
        pointsMap_.erase(firstFingerMove);
    }
    auto secondIter = pointsMap_.find(secondFingerMove);
    if (secondIter != pointsMap_.end()) {
        pointsMap_[secondFingerDown].x = pointsMap_[secondFingerMove].x;
        pointsMap_[secondFingerDown].y = pointsMap_[secondFingerMove].y;
        pointsMap_.erase(secondFingerMove);
    }
    auto thirdIter = pointsMap_.find(thirdFingerMove);
    if (thirdIter != pointsMap_.end()) {
        pointsMap_[thirdFingerDown].x = pointsMap_[thirdFingerMove].x;
        pointsMap_[thirdFingerDown].y = pointsMap_[thirdFingerMove].y;
        pointsMap_.erase(thirdFingerMove);
    }
}


void MotionGesture::SwipeDetect(PointEventInfo pointInfo)
{
    MotionGestureKey firstFingerDown = {NULL};
    MotionGestureKey firstFingerMove = CreateFingerKey(pointInfo);
    firstFingerDown.pointId = firstFingerMove.pointId;
    firstFingerDown.pointAction = EVENT_DOWN;
    int32_t swipeDistance = CalculateDistance(firstFingerDown, firstFingerMove);
    if (swipeDistance < SWIPE_DISTANCE_THRESHOLD) {
        pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
        return;
    }
    int32_t dertx = pointsMap_[firstFingerMove].x - pointsMap_[firstFingerDown].x;
    int32_t derty = pointsMap_[firstFingerMove].y - pointsMap_[firstFingerDown].y;
    if ((abs(dertx) > abs(derty)) && dertx > 0) {
        if (pointsMap_[firstFingerDown].x > 0 && pointsMap_[firstFingerDown].x < LEFT_BOUNDARY) {
            reportInfo_.type = MotionDataUtils::TYPE_RIGHT_SIDE_SWIPE;
            reportInfo_.value = MotionDataUtils::VALUE_ENTER;
            reportInfo_.action = MotionDataUtils::ACTION_RIGHT;
            reportInfo_.status = MotionDataUtils::STATUS_START;
            reportInfo_.move = pointsMap_[firstFingerMove].x - pointsMap_[firstFingerDown].x;
            Report();
            state_ = STATE_SWIPE;
        } else {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            state_ = STATE_END;
            return;
        }
    } else if ((abs(dertx) > abs(derty)) && dertx < 0) {
        if (pointsMap_[firstFingerDown].x > RIGHT_BOUNDARY && pointsMap_[firstFingerDown].x < PIXEL_X_MAX) {
            reportInfo_.type = MotionDataUtils::TYPE_LEFT_SIDE_SWIPE;
            reportInfo_.value = MotionDataUtils::VALUE_ENTER;
            reportInfo_.action = MotionDataUtils::ACTION_LEFT;
            reportInfo_.status = MotionDataUtils::STATUS_START;
            reportInfo_.move = pointsMap_[firstFingerDown].x - pointsMap_[firstFingerMove].x;
            Report();
            state_ = STATE_SWIPE;
        } else {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            state_ = STATE_END;
            return;
        }
    } else if ((abs(dertx) < abs(derty)) && derty < 0) {
        if (pointsMap_[firstFingerDown].y > DOWN_BOUNDARY && pointsMap_[firstFingerDown].y < PIXEL_Y_MAX) {
            reportInfo_.type = MotionDataUtils::TYPE_UP_SIDE_SWIPE;
            reportInfo_.value = MotionDataUtils::VALUE_ENTER;
            reportInfo_.action = MotionDataUtils::ACTION_UP;
            reportInfo_.status = MotionDataUtils::STATUS_START;
            reportInfo_.move = pointsMap_[firstFingerDown].y - pointsMap_[firstFingerMove].y;
            Report();
            state_ = STATE_SWIPE;
        } else {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            state_ = STATE_END;
            return;
        }
    } else {
        state_ = STATE_END;
    }
}

double MotionGesture::CalculatePinchScale()
{
    int32_t movingDistance;
    double scaleRate;
    MotionGestureKey firstFingerDown = {NULL};
    MotionGestureKey secondFingerDown = {NULL};
    MotionGestureKey firstFingerMove = {NULL};
    MotionGestureKey secondFingerMove = {NULL};
    GetFingerDownKey(firstFingerDown, secondFingerDown);
    CreateFingerMoveKey(firstFingerDown, firstFingerMove);
    CreateFingerMoveKey(secondFingerDown, secondFingerMove);
    auto iter = pointsMap_.find(firstFingerMove);
    if (iter == pointsMap_.end()) {
        auto iter = pointsMap_.find(secondFingerMove);
        if (iter == pointsMap_.end()) {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            pointsMap_.clear();
            state_ = STATE_ERROR;
            return NULL;
        } else {
            movingDistance = CalculateDistance(firstFingerDown, secondFingerMove);
        }
    } else {
        auto iter = pointsMap_.find(secondFingerMove);
        if (iter == pointsMap_.end()) {
            movingDistance = CalculateDistance(firstFingerMove, secondFingerDown);
        } else {
            movingDistance = CalculateDistance(firstFingerMove, secondFingerMove);
        }
    }  
    scaleRate = CalculateScale(movingDistance, initialDistance_);
    if (scaleRate > 1) {
        reportInfo_.action = MotionDataUtils::ACTION_ENLARGE;
    } else {
        reportInfo_.action = MotionDataUtils::ACTION_REDUCE;

    }
    return scaleRate;
}

void MotionGesture::PinchDetect(PointEventInfo pointInfo)
{
    pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
    int32_t movingDistance;
    int32_t movingAngle;
    MotionGestureKey firstFingerDown = {NULL};
    MotionGestureKey secondFingerDown = {NULL};
    MotionGestureKey firstFingerMove = {NULL};
    MotionGestureKey secondFingerMove = {NULL};
    GetFingerDownKey(firstFingerDown, secondFingerDown);
    CreateFingerMoveKey(firstFingerDown, firstFingerMove);
    CreateFingerMoveKey(secondFingerDown, secondFingerMove);
    firstFingerMoveKey_ = firstFingerMove;
    secondFingerMoveKey_ = secondFingerMove;
    auto iter = pointsMap_.find(firstFingerMove);
    if (iter == pointsMap_.end()) {
        auto iter = pointsMap_.find(secondFingerMove);
        if (iter == pointsMap_.end()) {
            reportInfo_.value = MotionDataUtils::VALUE_INVALID;
            pointsMap_.clear();
            state_ = STATE_ERROR;
            return;
        } else {
            movingDistance = CalculateDistance(firstFingerDown, secondFingerMove);
            movingAngle = CalculateAngle(firstFingerDown, secondFingerMove);
        }
    } else {
        auto iter = pointsMap_.find(secondFingerMove);
        if (iter == pointsMap_.end()) {
            movingDistance = CalculateDistance(firstFingerMove, secondFingerDown);
            movingAngle = CalculateAngle(firstFingerMove, secondFingerDown);
        } else {
            movingDistance = CalculateDistance(firstFingerMove, secondFingerMove);
            movingAngle = CalculateAngle(firstFingerMove, secondFingerMove);
        }
    }
    if (abs(movingAngle - initialAngle_) > PINCH_ANGLE_THRESHOLD) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "It's not in the Angle");
        reportInfo_.value = MotionDataUtils::VALUE_INVALID;
        state_ = STATE_END;
        return;
    }
    if (abs(movingDistance - initialDistance_) < PINCH_DISTANCE_THRESHOLD) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "It's not in the Distance");
        return;
    }
    double scaleRate = CalculateScale(movingDistance, initialDistance_);
    if (scaleRate > 1) {
        reportInfo_.action = MotionDataUtils::ACTION_ENLARGE;
    } else {
        reportInfo_.action = MotionDataUtils::ACTION_REDUCE;
    }
    reportInfo_.type = MotionDataUtils::TYPE_TWO_FINGERS_PINCH;
    reportInfo_.value = MotionDataUtils::VALUE_ENTER;
    reportInfo_.status = MotionDataUtils::STATUS_START;
    scaleRate = double((int)(scaleRate * KEEP_TWO_DIGITS)) / RESERVED_DIGITS;
    reportInfo_.move = scaleRate;
    Report();
    state_ = STATE_PINCH;
}

int32_t MotionGesture::CalculateDistanceMean(MotionGestureKey firstFingerDown, MotionGestureKey secondFingerDown,
    MotionGestureKey thirdFingerDown, MotionGestureKey firstFingerMove, MotionGestureKey secondFingerMove,
    MotionGestureKey thirdFingerMove)
{
    int32_t firstFingerDistance = CalculateDistance(firstFingerDown, firstFingerMove);
    int32_t secondFingerDistance = CalculateDistance(secondFingerDown, secondFingerMove);
    int32_t thirdFingerDistance = CalculateDistance(thirdFingerDown, thirdFingerMove);
    return  (firstFingerDistance + secondFingerDistance + thirdFingerDistance) / 3;
}

bool MotionGesture::JudgeDirection(MotionGestureKey firstFingerDown, MotionGestureKey secondFingerDown,
    MotionGestureKey thirdFingerDown, MotionGestureKey firstFingerMove, MotionGestureKey secondFingerMove,
    MotionGestureKey thirdFingerMove) {
    int32_t firstDertX = pointsMap_[firstFingerMove].x - pointsMap_[firstFingerDown].x;
    int32_t firstDertY = pointsMap_[firstFingerMove].y - pointsMap_[firstFingerDown].y;
    int32_t secondDertX = pointsMap_[secondFingerMove].x - pointsMap_[secondFingerDown].x;
    int32_t secondDertY = pointsMap_[secondFingerMove].y - pointsMap_[secondFingerDown].y;
    int32_t thirdDertX = pointsMap_[thirdFingerMove].x - pointsMap_[thirdFingerDown].x;
    int32_t thirdDertY = pointsMap_[thirdFingerMove].y - pointsMap_[thirdFingerDown].y;
    if (abs(firstDertX) > abs(firstDertY) && abs(secondDertX) > abs(secondDertY) && abs(thirdDertX) > abs(thirdDertY)
    && firstDertX > 0 && secondDertX > 0 && thirdDertX > 0) {
        reportInfo_.action = MotionDataUtils::ACTION_RIGHT;
        return true;
    } else if (abs(firstDertX) > abs(firstDertY) && abs(secondDertX) > abs(secondDertY)
    && abs(thirdDertX) > abs(thirdDertY) && firstDertX < 0 && secondDertX < 0 && thirdDertX < 0) {
        reportInfo_.action = MotionDataUtils::ACTION_LEFT;
        return true;
    } else if (abs(firstDertX) < abs(firstDertY) && abs(secondDertX) < abs(secondDertY)
    && abs(thirdDertX) < abs(thirdDertY) && firstDertY < 0 && secondDertY < 0 && thirdDertY < 0) {
        reportInfo_.action = MotionDataUtils::ACTION_UP;
        return true;
    } else if (abs(firstDertX) < abs(firstDertY) && abs(secondDertX) < abs(secondDertY)
    && abs(thirdDertX) < abs(thirdDertY) && firstDertY > 0 && secondDertY > 0 && thirdDertY > 0) {
        reportInfo_.action = MotionDataUtils::ACTION_DOWN;
        return true;
    } else {
        state_ = STATE_END;
        return false;
    }
}

void MotionGesture::SlideDetect(PointEventInfo pointInfo)
{
    pointsMap_[CreateFingerKey(pointInfo)] = pointInfo;
    int32_t movingDistance;
    MotionGestureKey firstFingerDown = {NULL};
    MotionGestureKey secondFingerDown = {NULL};
    MotionGestureKey thirdFingerDown = {NULL};
    GetFingerDownKey(firstFingerDown, secondFingerDown, thirdFingerDown);
    MotionGestureKey firstFingerMove = {NULL};
    MotionGestureKey secondFingerMove = {NULL};
    MotionGestureKey thirdFingerMove = {NULL};
    CreateFingerMoveKey(firstFingerDown, firstFingerMove);
    CreateFingerMoveKey(secondFingerDown, secondFingerMove);
    CreateFingerMoveKey(thirdFingerDown, thirdFingerMove);
    if (CreateFingerKey(pointInfo) == firstFingerMove) {
        movingDistance = CalculateDistance(firstFingerDown, firstFingerMove);
    } else if (CreateFingerKey(pointInfo) == secondFingerMove) {
        movingDistance = CalculateDistance(secondFingerDown, secondFingerMove);
    } else {
        movingDistance = CalculateDistance(thirdFingerDown, thirdFingerMove);
    }
    if (movingDistance < SLIDE_START_THRESHOLD) {
        return;
    }
    auto firstIter = pointsMap_.find(firstFingerMove);
    auto secondIter = pointsMap_.find(secondFingerMove);
    auto thirdIter = pointsMap_.find(thirdFingerMove);
    if (firstIter == pointsMap_.end() && secondIter == pointsMap_.end() && thirdIter == pointsMap_.end()) {
        pointsMap_.clear();
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "To ERROR");
        state_ = STATE_ERROR;
        return;
    }
    bool flag = JudgeDirection(firstFingerDown, secondFingerDown, thirdFingerDown, firstFingerMove,
        secondFingerMove, thirdFingerMove);
    if (!flag) {
        return;
    }
    int32_t distanceMean = CalculateDistanceMean(firstFingerDown, secondFingerDown, thirdFingerDown, firstFingerMove,
        secondFingerMove, thirdFingerMove);
    if (distanceMean < SLIDE_DISTANCE_THRESHOLD) {
        return;
    }
    reportInfo_.type = MotionDataUtils::TYPE_THREE_FINGERS_SLIDE;
    reportInfo_.value = MotionDataUtils::VALUE_ENTER;
    reportInfo_.status = MotionDataUtils::STATUS_START;
    reportInfo_.move = distanceMean;
    Report();
    state_ = STATE_SLIDE;
}

void MotionGesture::GetPointerInfo(const std::shared_ptr<PointerEvent> &pointEvent)
{
    PointEventInfo pointInfo;
    pointInfo.pointId = pointEvent->GetPointerId();
    pointInfo.pointAction = pointEvent->GetPointerAction();
    PointerEvent::PointerItem pointerItem;
    bool ret = pointEvent->GetPointerItem(pointInfo.pointId, pointerItem);
    if (!ret) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "failed to get pointer item");
        return;
    }
     pointInfo.x = pointerItem.GetDisplayX();
     pointInfo.y = pointerItem.GetDisplayY();
    pointInfo.downTime = pointerItem.GetDownTime();
    buffer_.push_back(pointInfo);
    sem_post(&sem_);
}

void MotionGesture::HandleTouchEvent()
{
    PointEventInfo pointInfo;
    if (buffer_.size() == 0) {
        MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Buffer is empty");
        return;
    }
    pointInfo = buffer_.front();
    buffer_.pop_front();
    switch (state_) {
        case STATE_IDLE: {
            HandleIdleState(pointInfo);
            break;
        }
        case STATE_ONE_FINGER_DOWN: {
            HandleOneFingerDownState(pointInfo);
            break;
        }
        case STATE_ONE_FINGER_MOVE: {
            HandleOneFingerMoveState(pointInfo);
            break;
        }
        case STATE_SWIPE: {
            HandleSwipeState(pointInfo);
            break;
        }
        case STATE_TWO_FINGERS_DOWN: {
            HandleTwoFingersDownState(pointInfo);
            break;
        }
        case STATE_TWO_FINGERS_MOVE: {
            HandleTwoFingersMoveState(pointInfo);
            break;
        } 
        case STATE_PINCH: {
            HandlePinchState(pointInfo);
            break;
        }
        case STATE_THREE_FINGERS_DOWN: {
            HandleThreeFingersDownState(pointInfo);
            break;
        }
        case STATE_THREE_FINGERS_MOVE: {
            HandleThreeFingersMoveState(pointInfo);
            break;
        }
        case STATE_SLIDE: {
            HandleSlideState(pointInfo);
            break;
        }
        case STATE_END: {
            HandleEndState(pointInfo);
            break;
        }
        case STATE_ERROR: {
            HandleErrorState(pointInfo);
            break;
        }
        default: {
            MOTION_HILOGE(MOTION_MODULE_SERVICE, "Others");
            break;
        }
    }
}

void MotionGesture::RegisterCallback(std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> &callback)
{
    callbackImpl_ = callback;
}

MotionDataUtils::MotionData MotionGesture::Report()
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "%{public}s: motionData.type:%{public}d, motionData.status: %{public}d, \
        motionData.action: %{public}d, motionData.move: %{public}f",
        __func__,  static_cast<int>(reportInfo_.type), static_cast<int>(reportInfo_.status),
        static_cast<int>(reportInfo_.action), reportInfo_.move);
    if (callbackImpl_ != nullptr) {
        callbackImpl_->OnResult(reportInfo_);
    } else {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "callbackImpl_ is null");
    }
    return reportInfo_;
}
} // namespace Msdp
} // namespace OHOS
