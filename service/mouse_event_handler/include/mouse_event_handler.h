/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_MULTIMDOALINPUT_MOUSE_EVENT_HANDLER_H
#define OHOS_MULTIMDOALINPUT_MOUSE_EVENT_HANDLER_H

#include "libinput.h"
#include "pointer_event.h"
#include "input_windows_manager.h"
#include "input_event_handler.h"
#include <memory>

namespace OHOS {
namespace MMI {
class MouseEventHandler : public PointerEvent {
public:
    virtual ~MouseEventHandler();
    void SetMouseData(libinput_event *event, int32_t deviceId);
    static std::shared_ptr<MouseEventHandler> Create()
    {
        return std::make_shared<MouseEventHandler>(InputEvent::EVENT_TYPE_POINTER);
    }
    static int32_t SetMouseEndData(std::shared_ptr<PointerEvent> pointerEvent, int32_t deviceId);
protected:
    explicit MouseEventHandler(int32_t eventType);

    void SetMouseMotion(PointerEvent::PointerItem& pointerItem);
    void SetMouseButon(PointerEvent::PointerItem &pointerItem, struct libinput_event_pointer& pointEventData);
    void SetMouseAxis(struct libinput_event_pointer& pointEventData);
    void CalcMovedCoordinate(struct libinput_event_pointer &pointEventData);
    void AdjustCoordinate(double &coordinateX, double &coordinateY);
};
}
} // namespace OHOS::MMI
#endif // OHOS_MULTIMDOALINPUT_POINTER_EVENT_H
