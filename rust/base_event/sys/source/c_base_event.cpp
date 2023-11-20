/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "c_base_event.h"

#include <vector>

#include "input_manager.h"
#include "pixel_map.h"

#include "c_base_event_internal.h"
#include "devicestatus_define.h"

using namespace OHOS;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, Msdp::MSDP_DOMAIN_ID, "CBaseEvent" };
#define INPUT_MANAGER OHOS::MMI::InputManager::GetInstance()

DragMonitorConsumer::DragMonitorConsumer(void (*cb)(CPointerEvent *)) : callback_(cb)
{
}

void DragMonitorConsumer::OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const
{
}

void DragMonitorConsumer::OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
}

void DragMonitorConsumer::OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    CALL_DEBUG_ENTER;
    CHKPV(pointerEvent);
    struct CPointerEvent cPointerEvent { pointerEvent };
    CHKPV(callback_);
    callback_(&cPointerEvent);
}

int32_t CGetPointerId(const CPointerEvent* cPointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(cPointerEvent, RET_ERR);
    CHKPR(cPointerEvent->event, RET_ERR);
    return cPointerEvent->event->GetPointerId();
}

int32_t CGetPointerAction(const CPointerEvent* cPointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(cPointerEvent, RET_ERR);
    CHKPR(cPointerEvent->event, RET_ERR);
    return cPointerEvent->event->GetPointerAction();
}

int32_t CGetTargetWindowId(const CPointerEvent* cPointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(cPointerEvent, RET_ERR);
    CHKPR(cPointerEvent->event, RET_ERR);
    return cPointerEvent->event->GetTargetWindowId();
}

int32_t CGetSourceType(const CPointerEvent* cPointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(cPointerEvent, RET_ERR);
    CHKPR(cPointerEvent->event, RET_ERR);
    return cPointerEvent->event->GetSourceType();
}

int32_t CGetTargetDisplayId(const CPointerEvent* cPointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(cPointerEvent, RET_ERR);
    CHKPR(cPointerEvent->event, RET_ERR);
    return cPointerEvent->event->GetTargetDisplayId();
}

int32_t CGetDisplayX(const CPointerEvent* cPointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(cPointerEvent, RET_ERR);
    CHKPR(cPointerEvent->event, RET_ERR);
    MMI::PointerEvent::PointerItem pointerItem;
    cPointerEvent->event->GetPointerItem(CGetPointerId(cPointerEvent), pointerItem);
    return pointerItem.GetDisplayX();
}

int32_t CGetDisplayY(const CPointerEvent* cPointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(cPointerEvent, RET_ERR);
    CHKPR(cPointerEvent->event, RET_ERR);
    MMI::PointerEvent::PointerItem pointerItem;
    cPointerEvent->event->GetPointerItem(CGetPointerId(cPointerEvent), pointerItem);
    return pointerItem.GetDisplayY();
}

int32_t CAddMonitor(void (*callback)(CPointerEvent *))
{
    CALL_DEBUG_ENTER;
    CHKPR(callback, RET_ERR);
    auto consumer = std::make_shared<DragMonitorConsumer>(callback);
    auto ret = INPUT_MANAGER->AddMonitor(consumer);
    if (ret < 0) {
        FI_HILOGE("Failed to add monitor");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t CGetWindowPid(const CPointerEvent* cPointerEvent)
{
    CALL_DEBUG_ENTER;
    CHKPR(cPointerEvent, RET_ERR);
    CHKPR(cPointerEvent->event, RET_ERR);
    return INPUT_MANAGER->GetWindowPid(CGetTargetWindowId(cPointerEvent));
}

int32_t CGetPointerStyle(CPointerStyle* cPointerStyle)
{
    CALL_DEBUG_ENTER;
    CHKPR(cPointerStyle, RET_ERR);
    MMI::PointerStyle pointerStyle;
    if (INPUT_MANAGER->GetPointerStyle(MMI::GLOBAL_WINDOW_ID, pointerStyle) != RET_OK) {
        FI_HILOGE("Failed to get pointer style");
        return RET_ERR;
    }

    cPointerStyle->size = pointerStyle.size;
    cPointerStyle->color.r = pointerStyle.color.r;
    cPointerStyle->color.g = pointerStyle.color.g;
    cPointerStyle->color.b = pointerStyle.color.b;
    cPointerStyle->id = pointerStyle.id;
    return RET_OK;
}

void CAppendExtraData(CExtraData cExtraData)
{
    CALL_DEBUG_ENTER;
    CHKPV(cExtraData.buffer);
    uint8_t* uData = cExtraData.buffer;
    CHKPV(uData);
    MMI::ExtraData extraData;
    extraData.appended = cExtraData.appended;
    extraData.pointerId = cExtraData.pointerId;
    extraData.sourceType = cExtraData.sourceType;
    for (size_t i = 0; i < cExtraData.bufferSize; ++i) {
        extraData.buffer.emplace_back(*uData);
        ++uData;
    }
    INPUT_MANAGER->AppendExtraData(extraData);
}

int32_t CSetPointerVisible(bool visible)
{
    CALL_DEBUG_ENTER;
    if (INPUT_MANAGER->SetPointerVisible(visible) != RET_OK) {
        FI_HILOGE("Failed to set pointer visible");
        return RET_ERR;
    }
    return RET_OK;
}