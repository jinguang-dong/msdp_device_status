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

#ifndef INPUT_EVENT_INTERCEPTOR_H
#define INPUT_EVENT_INTERCEPTOR_H

#include <atomic>

#include "nocopyable.h"

#include "channel.h"
#include "cooperate_events.h"
#include "i_context.h"
#include "input_event_transmission/input_event_sampler.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace Cooperate {
class Context;

class InputEventInterceptor final {
public:
    InputEventInterceptor(IContext *env) : env_(env) {}
    ~InputEventInterceptor();
    DISALLOW_COPY_AND_MOVE(InputEventInterceptor);

    void Enable(Context &context);
    void Disable();
    void Update(Context &context);
     void ReportAddInterceptor(BizCooperateStage stageRes, CooperateRadarErrCode errCode, const std::string &funcName,
        const std::string &packageName);   

private:
    void OnPointerEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent);
    void OnNotifyCrossDrag(std::shared_ptr<MMI::PointerEvent> pointerEvent);
    void OnKeyEvent(std::shared_ptr<MMI::KeyEvent> keyEvent);
    void ReportPointerEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent);
    void TurnOffChannelScan();
    void TurnOnChannelScan();
    void ExecuteInner();
    void HandleStopTimer();
    int32_t SetWifiScene(unsigned int scene);
    void RefreshActivity();
    void HeartBeatSend();

    IContext *env_ { nullptr };
    int32_t interceptorId_ { -1 };
    bool scanState_ { true };
    std::atomic<int32_t> heartTimer_ { -1 };
    int32_t pointerEventTimer_ { -1 };
    std::string remoteNetworkId_;
    Channel<CooperateEvent>::Sender sender_;
    InputEventSampler inputEventSampler_;
    static std::set<int32_t> filterKeys_;
    static std::set<int32_t> filterPointers_;
};
} // namespace Cooperate
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // INPUT_EVENT_INTERCEPTOR_H
