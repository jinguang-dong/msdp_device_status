/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "standard_event_handler.h"
#include <cinttypes>

namespace OHOS {
namespace MMI {
    namespace {
        constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "StandardEventHandler" };
    }
} // namespace MMI
} // namespace OHOS

OHOS::MMI::StandardEventHandler::StandardEventHandler() {}

OHOS::MMI::StandardEventHandler::~StandardEventHandler() {}

void OHOS::MMI::StandardEventHandler::StandardTouchEvent(libinput_event *event, StandardTouchStruct& data)
{
    CHKPV(event);
    enum libinput_event_type eventType = {};
    eventType = libinput_event_get_type(event);
    switch (eventType) {
        case libinput_event_type::LIBINPUT_EVENT_POINTER_BUTTON: {
            PointerPressedStandard(event, data);
            break;
        }

        case libinput_event_type::LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE: {
            PointerAbsoluteStandardEvent(event, data);
            break;
        }

        case libinput_event_type::LIBINPUT_EVENT_POINTER_MOTION: {
            PointerMotionStandardEvent(event, data);
            break;
        }

        case libinput_event_type::LIBINPUT_EVENT_TABLET_TOOL_TIP: {
            TipStandardEvent(event, data);
            break;
        }

        case libinput_event_type::LIBINPUT_EVENT_TABLET_TOOL_AXIS: {
            TipMotionStandardEvent(event, data);
            break;
        }

        default: {
            data.curRventType = RET_ERR;
            MMI_LOGW("Unknown event type:%{public}d", eventType);
            break;
        }
    }
}

void OHOS::MMI::StandardEventHandler::PointerPressedStandard(libinput_event *event, StandardTouchStruct& data)
{
    CHKPV(event);
    auto szPoint = libinput_event_get_pointer_event(event);
    CHKPV(szPoint);
    leftButton_ = libinput_event_pointer_get_button(szPoint);
    data.buttonType = leftButton_;
    leftButtonState_ = libinput_event_pointer_get_button_state(szPoint);
    data.buttonState = leftButtonState_;
    MMI_LOGD("Event:buttonCode:%{public}d,buttonState:%{public}d",
             data.buttonType, data.buttonState);
    if (data.buttonType == BTN_LEFT && data.buttonState == LIBINPUT_BUTTON_STATE_RELEASED) {
        PointerReleasedStandardEvent(*szPoint, data);
    } else if (data.buttonType == BTN_LEFT && data.buttonState == LIBINPUT_BUTTON_STATE_PRESSED) {
        PointerPressedStandardEvent(*szPoint, data);
    } else {
        data.curRventType = RET_ERR;
    }
}

void OHOS::MMI::StandardEventHandler::PointerReleasedStandardEvent(struct libinput_event_pointer& szPoint,
                                                                   StandardTouchStruct& data)
{
    data.msgType = LIBINPUT_EVENT_TOUCH_UP;
    data.reRventType = LIBINPUT_EVENT_POINTER_BUTTON;
    data.curRventType = LIBINPUT_EVENT_TOUCH_UP;
    data.buttonType = BTN_LEFT;
    data.buttonState = LIBINPUT_BUTTON_STATE_RELEASED;
    data.time = libinput_event_pointer_get_time_usec(&szPoint);
    MMI_LOGD("3.standard event:pointLeftLiftEvent:reRventType:%{public}d,curRventType:%{public}d,"
             "buttonCode:%{public}d,buttonState:%{public}d",
             data.reRventType, data.curRventType, data.buttonType, data.buttonState);
}

void OHOS::MMI::StandardEventHandler::PointerPressedStandardEvent(struct libinput_event_pointer& szPoint,
                                                                  StandardTouchStruct& data)
{
    data.msgType = LIBINPUT_EVENT_TOUCH_DOWN;
    data.reRventType = LIBINPUT_EVENT_POINTER_BUTTON;
    data.curRventType = LIBINPUT_EVENT_TOUCH_DOWN;
    data.buttonType = BTN_LEFT;
    data.buttonState = LIBINPUT_BUTTON_STATE_PRESSED;
    data.time = libinput_event_pointer_get_time_usec(&szPoint);
    MMI_LOGD("3.standard event:pointLeftPressEvent:reRventType:%{public}d,curRventType:%{public}d,"
             "buttonCode:%{public}d,buttonState:%{public}d",
             data.reRventType, data.curRventType, data.buttonType, data.buttonState);
}

void OHOS::MMI::StandardEventHandler::PointerAbsoluteStandardEvent(libinput_event *event, StandardTouchStruct& data)
{
    CHKPV(event);
    auto szPoint = libinput_event_get_pointer_event(event);
    CHKPV(szPoint);
    data.time = libinput_event_pointer_get_time_usec(szPoint);
    data.x = libinput_event_pointer_get_absolute_x(szPoint);
    data.y = libinput_event_pointer_get_absolute_y(szPoint);
    MMI_LOGD("Event:time:%{public}" PRId64
            ", x:%{public}f, y:%{public}f",
            data.time, data.x, data.y);
    if (leftButtonState_ == LIBINPUT_BUTTON_STATE_PRESSED && leftButton_ == BTN_LEFT) {
        data.msgType = LIBINPUT_EVENT_TOUCH_MOTION;
        data.curRventType = LIBINPUT_EVENT_TOUCH_MOTION;
        data.reRventType = LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE;
        data.buttonType = BTN_LEFT;
        data.buttonState = LIBINPUT_BUTTON_STATE_PRESSED;
        MMI_LOGD("3.standard event:pointAbsLeftMoveEvent:reRventType:%{public}d,curRventType:%{public}d,"
                 "buttonCode:%{public}d,buttonState:%{public}d,x:%{public}f,y:%{public}f",
                 data.reRventType, data.curRventType, data.buttonType, data.buttonState, data.x, data.y);
    } else {
        data.curRventType = RET_ERR;
    }
}

void OHOS::MMI::StandardEventHandler::PointerMotionStandardEvent(libinput_event *event, StandardTouchStruct& data)
{
    CHKPV(event);
    auto szPoint = libinput_event_get_pointer_event(event);
    CHKPV(szPoint);
    data.time = libinput_event_pointer_get_time_usec(szPoint);
    data.x = libinput_event_pointer_get_dx_unaccelerated(szPoint);
    data.y = libinput_event_pointer_get_dy_unaccelerated(szPoint);
    double rawX = data.x;
    double rawY = data.y;
    data.x = libinput_event_pointer_get_dx(szPoint);
    data.y = libinput_event_pointer_get_dy(szPoint);
    MMI_LOGD("Event:time:%{public}" PRId64
            ", x:%{public}f,y:%{public}f",
            data.time, data.x, data.y);
    if (leftButtonState_ == LIBINPUT_BUTTON_STATE_PRESSED && leftButton_ == BTN_LEFT) {
        data.msgType = LIBINPUT_EVENT_TOUCH_MOTION;
        data.curRventType = LIBINPUT_EVENT_TOUCH_MOTION;
        data.reRventType = LIBINPUT_EVENT_POINTER_MOTION;
        data.buttonType = BTN_LEFT;
        data.buttonState = LIBINPUT_BUTTON_STATE_PRESSED;
        MMI_LOGD("3.standard event:pointLeftMoveEvent:reRventType:%{public}d,curRventType:%{public}d,"
                 "buttonCode:%{public}d,buttonState:%{public}d,x:%{public}f,y:%{public}f,rawX:%{public}f,"
                 "rawY:%{public}f",
                 data.reRventType, data.curRventType, data.buttonType, data.buttonState, data.x, data.y, rawX, rawY);
    } else {
        data.curRventType = RET_ERR;
    }
}

void OHOS::MMI::StandardEventHandler::TipStandardEvent(libinput_event *event, StandardTouchStruct& data)
{
    CHKPV(event);
    auto szPoint = libinput_event_get_tablet_tool_event(event);
    CHKPV(szPoint);
    data.tipState = libinput_event_tablet_tool_get_tip_state(szPoint);
    if (data.tipState == LIBINPUT_TABLET_TOOL_TIP_UP) {
        TipUpStandardEvent(*szPoint, data);
    } else if (data.tipState == LIBINPUT_TABLET_TOOL_TIP_DOWN) {
        TipDownStandardEvent(*szPoint, data);
    } else {
        data.curRventType = RET_ERR;
    }
}

void OHOS::MMI::StandardEventHandler::TipUpStandardEvent(struct libinput_event_tablet_tool& szPoint,
                                                         StandardTouchStruct& data)
{
    data.msgType = LIBINPUT_EVENT_TOUCH_UP;
    data.reRventType = LIBINPUT_EVENT_TABLET_TOOL_TIP;
    data.curRventType = LIBINPUT_EVENT_TOUCH_UP;
    data.time = libinput_event_tablet_tool_get_time_usec(&szPoint);
    data.x = libinput_event_tablet_tool_get_x(&szPoint);
    data.y = libinput_event_tablet_tool_get_y(&szPoint);
    MMI_LOGD("3.standard event:TipUpStandarEvent:reRventType:%{public}d,curRventType:%{public}d,"
             "tipState:%{public}d,x:%{public}f,y:%{public}f",
             data.reRventType, data.curRventType, data.tipState, data.x, data.y);
}

void OHOS::MMI::StandardEventHandler::TipDownStandardEvent(struct libinput_event_tablet_tool& szPoint,
                                                           StandardTouchStruct& data)
{
    data.time = libinput_event_tablet_tool_get_time_usec(&szPoint);
    data.x = libinput_event_tablet_tool_get_x(&szPoint);
    data.y = libinput_event_tablet_tool_get_y(&szPoint);
    data.msgType = LIBINPUT_EVENT_TOUCH_DOWN;
    data.reRventType = LIBINPUT_EVENT_TABLET_TOOL_TIP;
    data.curRventType = LIBINPUT_EVENT_TOUCH_DOWN;
    MMI_LOGD("3.standard event:TipDownStandarEvent:reRventType:%{public}d,curRventType:%{public}d,"
        "tipState:%{public}d,x:%{public}f,y:%{public}f",
        data.reRventType, data.curRventType, data.tipState, data.x, data.y);
}

void OHOS::MMI::StandardEventHandler::TipMotionStandardEvent(libinput_event *event, StandardTouchStruct& data)
{
    CHKPV(event);
    auto szPoint = libinput_event_get_tablet_tool_event(event);
    CHKPV(szPoint);
    data.x = libinput_event_tablet_tool_get_x(szPoint);
    data.y = libinput_event_tablet_tool_get_y(szPoint);
    data.tipState = libinput_event_tablet_tool_get_tip_state(szPoint);
    if (data.tipState == LIBINPUT_TABLET_TOOL_TIP_DOWN) {
        data.msgType = LIBINPUT_EVENT_TABLET_TOOL_AXIS;
        data.reRventType = LIBINPUT_EVENT_TABLET_TOOL_AXIS;
        data.curRventType = LIBINPUT_EVENT_TOUCH_MOTION;
        MMI_LOGD("3.standard event:TipMotionStandarEvent:reRventType:%{public}d,curRventType:%{public}d,"
            "tipState:%{public}d,x:%{public}f,y:%{public}f",
            data.reRventType, data.curRventType, data.tipState, data.x, data.y);
    } else {
        data.curRventType = RET_ERR;
    }
}
