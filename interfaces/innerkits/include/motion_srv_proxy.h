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

#ifndef MOTION_SRV_PROXY_H
#define MOTION_SRV_PROXY_H

#include <nocopyable.h>

#include "imotion.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Msdp {
class MotionSrvProxy : public IRemoteProxy<Imotion> {
public:
    explicit MotionSrvProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<Imotion>(impl) {}
    ~MotionSrvProxy() = default;
    DISALLOW_COPY_AND_MOVE(MotionSrvProxy);

    virtual void Subscribe(const MotionDataUtils::MotionType& type, const sptr<ImotionCallback>& callback) override;
    virtual void UnSubscribe(const MotionDataUtils::MotionType& type, const sptr<ImotionCallback>& callback) override;
    virtual MotionDataUtils::MotionData GetCache(const MotionDataUtils::MotionType& type) override;

private:
    static inline BrokerDelegator<MotionSrvProxy> delegator_;
};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_SRV_PROXY_H
