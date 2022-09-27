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

#ifndef MOTION_GESTURE_H
#define MOTION_GESTURE_H

#include <cmath>
#include <memory>
#include "point_event_callback.h"
#include "motion_data_utils.h"
#include "motion_data_define.h"
#include "data_define.h"
#include "motion_msdp_interface.h"
#include <map>
#include <list>
#include <thread>
#include <semaphore.h>

namespace OHOS {
namespace Msdp {
using namespace OHOS::MMI;
class MotionGesture {
public:
    MotionGesture(const std::shared_ptr<PointEventCallback> &pointCallback) : pointCallback_(pointCallback) {};
    ~MotionGesture() {};
    void Init();
    void RegisterCallback(std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> &callback);

protected:
    void OnPointEventChanged(const std::shared_ptr<PointerEvent>&);
    void GetPointerInfo(const std::shared_ptr<PointerEvent> &pointEvent);
    void HandleTouchEvent();
    void ReportinfoInit();
    MotionDataUtils::MotionData Report();
    void Loop();

    MotionGestureKey CreateFingerKey(PointEventInfo pointInfo);
    MotionGestureKey CreateFingerDownKey(PointEventInfo pointInfo);
    MotionGestureKey CreateFingerMoveKey(PointEventInfo pointInfo);
    void CreateFingerMoveKey(MotionGestureKey firstIndex, MotionGestureKey &secondIndex);
    void GetFingerMoveKey(MotionGestureKey &index);
    void GetFingerDownKey(MotionGestureKey &index);
    void GetFingerDownKey(MotionGestureKey &firstFingerDown, MotionGestureKey &secondFingerDown);
    void GetFingerDownKey(MotionGestureKey &firstFingerDown, MotionGestureKey &secondFingerDown,
        MotionGestureKey &thirdFingerDown);

    FingerEvent DetermineFingerEvent(PointEventInfo pointInfo);
    void CalculateInitialDistanceAngle(PointEventInfo pointInfo);
    void CalculateInitialDistanceAngleMove(PointEventInfo pointInfo);
    int32_t CalculateDistance(MotionGestureKey firstFingerDown, MotionGestureKey secondFingerDown);
    int32_t CalculateAngle(MotionGestureKey firstFingerDown, MotionGestureKey secondFingerDown);
    double CalculateScale(int32_t firstValue, int32_t secondValue);
    double CalculatePinchScale();
    bool CalculateMovingAngle(int32_t& movingAngle);
    int32_t CalculateDistanceMean(MotionGestureKey firstFingerDown, MotionGestureKey secondFingerDown,
        MotionGestureKey thirdFingerDown, MotionGestureKey firstFingerMove, MotionGestureKey secondFingerMove,
        MotionGestureKey thirdFingerMove);

    void PinchDetect(PointEventInfo pointInfo);
    void SwipeDetect(PointEventInfo pointInfo);
    bool JudgeDirection(MotionGestureKey firstFingerDown, MotionGestureKey secondFingerDown,
        MotionGestureKey thirdFingerDown,MotionGestureKey firstFingerMove, MotionGestureKey secondFingerMove,
        MotionGestureKey thirdFingerMove);
    void SlideDetect(PointEventInfo pointInfo);

    void HandleIdleState(PointEventInfo pointInfo);
    void HandleOneFingerDownState(PointEventInfo pointInfo);
    void HandleOneFingerMoveState(PointEventInfo pointInfo);
    void HandleSwipeState(PointEventInfo pointInfo);
    void HandleTwoFingersDownState(PointEventInfo pointInfo);
    void HandleTwoFingersMoveState(PointEventInfo pointInfo);
    void HandlePinchState(PointEventInfo pointInfo);
    void HandleThreeFingersDownState(PointEventInfo pointInfo);
    void HandleThreeFingersMoveState(PointEventInfo pointInfo);
    void HandleSlideState(PointEventInfo pointInfo);
    void HandleEndState(PointEventInfo pointInfo);
    void HandleErrorState(PointEventInfo pointInfo);

    void TwoFingersToOneFinger(PointEventInfo pointInfo);
    void TwoFingersToThreeFingers(PointEventInfo pointInfo);
    void ThreeFingersToTwoFingers(PointEventInfo pointInfo);
    void FourFingersToThreeFingers(PointEventInfo pointInfo);

private:
    std::shared_ptr<PointerEvent> pointEvent_;
    std::shared_ptr<PointEventCallback> pointCallback_;
    std::shared_ptr<MotionMsdpInterface::MsdpAlgorithmCallback> callbackImpl_;
    States state_;
    MotionDataUtils::MotionData reportInfo_;
    std::map<MotionGestureKey, PointEventInfo> pointsMap_;
    std::list<PointEventInfo> buffer_;
    int32_t fingerCount_;
    int32_t initialDistance_;
    int32_t initialAngle_;
    MotionGestureKey firstFingerMoveKey_;
    MotionGestureKey secondFingerMoveKey_;
    sem_t sem_;
};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_GESTURE_H