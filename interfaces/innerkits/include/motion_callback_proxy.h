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

#ifndef MOTION_CALLBACK_PROXY_H
#define MOTION_CALLBACK_PROXY_H

#include <iremote_proxy.h>
#include <nocopyable.h>

#include "imotion_callback.h"
#include "motion_data_utils.h"

namespace OHOS {
namespace Msdp {
class MotionCallbackProxy : public IRemoteProxy<ImotionCallback> {
public:
    explicit MotionCallbackProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<ImotionCallback>(impl) {}
    ~MotionCallbackProxy() = default;
    DISALLOW_COPY_AND_MOVE(MotionCallbackProxy);
    virtual void OnMotionChanged(const MotionDataUtils::MotionData& motionData) override;

private:
    static inline BrokerDelegator<MotionCallbackProxy> delegator_;
};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_CALLBACK_PROXY_H
