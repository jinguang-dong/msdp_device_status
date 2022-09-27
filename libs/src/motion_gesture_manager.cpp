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

#include "motion_gesture_manager.h"
#include <cerrno>
#include <linux/netlink.h>
#include <string>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "motion_common.h"

namespace OHOS {
namespace Msdp {
namespace {
MotionGestureManager* g_rdb;
}

bool MotionGestureManager::Init()
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "MotionMsdpRdbInit: enter");
    if (!pointEventCb_) {
        pointEventCb_ = std::make_shared<PointEventCallback>();
        pointEventCb_->SubscribePointEvent();
    }

    if (!sensorEventCb_) {
        sensorEventCb_ = SensorDataCallback::GetInstance();
        sensorEventCb_->Initiate();
    }

    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "MotionMsdpRdbInit: exit");
    return true;
}

ErrCode MotionGestureManager::RegisterCallback(std::shared_ptr<MsdpAlgorithmCallback>& callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, " enter");
    if(type_ == MotionDataUtils::TYPE_LEFT_SIDE_SWIPE || type_ == MotionDataUtils::TYPE_RIGHT_SIDE_SWIPE
    || type_ == MotionDataUtils::TYPE_UP_SIDE_SWIPE || type_ == MotionDataUtils::TYPE_TWO_FINGERS_PINCH
    || type_ == MotionDataUtils::TYPE_THREE_FINGERS_SLIDE) {
        if (!gesture_) {
            gesture_= std::make_shared<MotionGesture>(pointEventCb_);
            gesture_->RegisterCallback(callback);
            gesture_->Init();
        }
    } else if(type_ == MotionDataUtils::TYPE_SHAKE) {
        if (!shake_) {
            shake_= std::make_shared<MotionShake>(sensorEventCb_);
            shake_->RegisterCallback(callback);
            shake_->Init();
        }
    } else if(type_ == MotionDataUtils::TYPE_FLIP) {
        if (!flip_) {
            flip_ = std::make_shared<MotionFlip>(sensorEventCb_);
            flip_->RegisterCallback(callback);
            flip_->Init();
        }
    } else if(type_ == MotionDataUtils::TYPE_ROTATE) {
        if (!rotate_) {
            rotate_ = std::make_shared<MotionRotate>(sensorEventCb_);
            rotate_->RegisterCallback(callback);
            rotate_->Init();
        }
    } else if(type_ == MotionDataUtils::TYPE_POCKET) {
        MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "TYPE_POCKET: enter");
        if (!pocket_) {
            MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "MotionMsdpRdbInit_pocket: enter");
            pocket_ = std::make_shared<MotionPocket>(sensorEventCb_);
            pocket_->RegisterCallback(callback);
            pocket_->Init();
        }
    } else if(type_ == MotionDataUtils::TYPE_PICKUP) {
        if (!pickup_) {
            pickup_ = std::make_shared<MotionPickup>(sensorEventCb_);
            pickup_->RegisterCallback(callback);
            pickup_->Init();
        }
    } else if(type_ == MotionDataUtils::TYPE_CLOSE_TO_EAR) {
        if (!nearEar_) {
            nearEar_ = std::make_shared<MotionNearEar>(sensorEventCb_);
            nearEar_->RegisterCallback(callback);
            nearEar_->Init();
        }
    }
    return ERR_OK;
}

ErrCode MotionGestureManager::UnregisterCallback()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "UnregisterCallback enter");
    callbacksImpl_ = nullptr;
    return ERR_OK;
}

ErrCode MotionGestureManager::DisableCount(const MotionDataUtils::MotionType& type)
{
    return ERR_OK;
}

ErrCode MotionGestureManager::Enable(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Enable enter");
    type_ = type;
    int motionItem = int(type);
    in_type[motionItem] = int(MotionDataUtils::MotionTypeValue::VALID);
    Init();
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "Enable Exit");
    return ERR_OK;
}

ErrCode MotionGestureManager::Disable(const MotionDataUtils::MotionType& type)
{
    return ERR_OK;
}

extern "C" MotionMsdpInterface *Create(void)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    g_rdb = new MotionGestureManager();
    return g_rdb;
}

extern "C" void Destroy(const MotionMsdpInterface* algorithm)
{
    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "enter");
    delete algorithm;
}
} // namespace Msdp
} // namespace OHOS
