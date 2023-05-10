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

#ifndef CONTINUE_MISSION_CALLBACK_PROXY_H
#define CONTINUE_MISSION_CALLBACK_PROXY_H

#include <iremote_proxy.h>
#include <nocopyable.h>

#include "i_continue_mission_callback.h"
#include "continue_mission_param.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class ContinueMissionCallbackProxy : public IRemoteProxy<IContinueMissionCallback> {
public:
    explicit ContinueMissionCallbackProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IContinueMissionCallback>(impl) {}
    ~ContinueMissionCallbackProxy() = default;
    DISALLOW_COPY_AND_MOVE(ContinueMissionCallbackProxy);
    virtual int32_t OnContinueMission(const ContinueMissionParam& continueMissionParam) override;

private:
    static inline BrokerDelegator<ContinueMissionCallbackProxy> delegator_;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // CONTINUE_MISSION_CALLBACK_PROXY_H