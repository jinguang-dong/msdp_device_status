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

#include "point_event_callback.h"

#include <thread>
#include <chrono>

//
//#include "motion_common.h"
#include "devicestatus_common.h"

namespace OHOS {
namespace Msdp {
PointEventCallback::PointEventCallback()
{
}

int32_t PointEventCallback::SubscribePointEvent()
{
    pointEventCb_ = std::bind(&PointEventCallback::OnPointEventChange, this, std::placeholders::_1);
    DEV_HILOGI(SERVICE, "SubscribePointEvent enter");
    monitorId_ = MMI::InputManager::GetInstance()->AddMonitor(pointEventCb_);
    return monitorId_;
}

int32_t PointEventCallback::SubscribePointEvent(PointsEventCallback callback)
{
    callback_ = callback;
    return ERR_OK;
}

int32_t PointEventCallback::UnsubscribePointEvent()
{
    callback_ = nullptr;
    pointEventCb_ = nullptr;
    MMI::InputManager::GetInstance()->RemoveMonitor(monitorId_);
    return ERR_OK;
}

void PointEventCallback::OnPointEventChange(std::shared_ptr<OHOS::MMI::PointerEvent> pointEvent)
{
    DEV_HILOGD(SERVICE, "OnPointEventChange received pointEvent");
    int32_t pointId = pointEvent->GetPointerId();

    OHOS::MMI::PointerEvent::PointerItem pointerItem;
    bool ret = pointEvent->GetPointerItem(pointId, pointerItem);
    if (!ret) {
        DEV_HILOGE(SERVICE, "failed to get pointer item");
        return;
    }
    int32_t pointAction = pointEvent->GetPointerAction();

    DEV_HILOGI(SERVICE, "pointAction: %{public}d, desc:%{public}s", pointAction, pointEvent->DumpPointerAction());
    // DEV_HILOGI(SERVICE, "OnPointEventChange pointId:%{public}d coordinate.x: %{public}d, coordinate.y: %{public}d",
    //     pointId, pointerItem.GetGlobalX(), pointerItem.GetGlobalY());
    DEV_HILOGI(SERVICE, "OnPointEventChange pointId:%{public}d coordinate.x: %{public}d, coordinate.y: %{public}d",
        pointId, pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
    if (callback_ != nullptr) {
        callback_(pointEvent);
    }
}
}
}