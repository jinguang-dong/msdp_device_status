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

#ifndef POINT_EVENT_CALLBACK_H
#define POINT_EVENT_CALLBACK_H

#include "input_manager.h"
#include "motion_data_define.h"

namespace OHOS {
namespace Msdp {
class PointEventCallback {
public:
    using PointEventFunc = std::function<void(std::shared_ptr<OHOS::MMI::PointerEvent>)>;
    PointEventCallback();
    ~PointEventCallback() = default;
    int32_t SubscribePointEvent();
    int32_t SubscribePointEvent(PointsEventCallback callback);
    int32_t UnsubscribePointEvent();
    void OnPointEventChange(std::shared_ptr<OHOS::MMI::PointerEvent> monitor);
private:
    PointEventFunc pointEventCb_;
    PointsEventCallback callback_;
    int32_t monitorId_;
};
} //namespace Msdp
} //namespace OHOS
#endif // POINT_EVENT_CALLBACK_H