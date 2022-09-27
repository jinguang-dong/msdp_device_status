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

#ifndef MOTION_CLIENT_H
#define MOTION_CLIENT_H

#include <singleton.h>
#include "imotion.h"
#include "imotion_callback.h"
#include "motion_data_utils.h"
#include "motion_common.h"

namespace OHOS {
namespace Msdp {
class MotionClient final : public DelayedRefSingleton<MotionClient> {
    DECLARE_DELAYED_REF_SINGLETON(MotionClient)

public:
    DISALLOW_COPY_AND_MOVE(MotionClient);

    void SubscribeCallback(const MotionDataUtils::MotionType& type, const sptr<ImotionCallback>& callback);
    void UnSubscribeCallback(const MotionDataUtils::MotionType& type, const sptr<ImotionCallback>& callback);
    MotionDataUtils::MotionData GetMotionData(const MotionDataUtils::MotionType& type);

private:
    class MotionDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        MotionDeathRecipient() = default;
        ~MotionDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remote);
    private:
        DISALLOW_COPY_AND_MOVE(MotionDeathRecipient);
    };

    ErrCode Connect();
    sptr<Imotion> motionProxy_ {nullptr};
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ {nullptr};
    void ResetProxy(const wptr<IRemoteObject>& remote);
    std::mutex mutex_;
};
} // namespace Msdp
} // namespace OHOS
#endif // IMOTION_H