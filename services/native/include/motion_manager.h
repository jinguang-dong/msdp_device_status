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

#ifndef MOTION_MANAGER_H
#define MOTION_MANAGER_H

#include <set>
#include <map>
#include "sensor_if.h"

#include "motion_data_utils.h"
#include "imotion_algorithm.h"
#include "imotion_callback.h"
#include "motion_common.h"
#include "motion_msdp_client_impl.h"

namespace OHOS {
namespace Msdp {
class MotionService;
class MotionManager {
public:
    explicit MotionManager(const wptr<MotionService>& ms) : ms_(ms)
    {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "MotionManager instance is created.");
    }
    ~MotionManager() = default;

    class MotionCallbackDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        MotionCallbackDeathRecipient() = default;
        virtual void OnRemoteDied(const wptr<IRemoteObject> &remote);
        virtual ~MotionCallbackDeathRecipient() = default;
    };

    bool Init();
    bool Enable(const MotionDataUtils::MotionType& type);
    bool Disable(const MotionDataUtils::MotionType& type);
    bool RegisterCallbak(MotionMsdpClientImpl::CallbackManager& motionClientImplCallback);
    bool UnRegisterCallbak();
    void NotifyMotionChange(const MotionDataUtils::MotionData& motionData);
    void Subscribe(const MotionDataUtils::MotionType& type, const sptr<ImotionCallback>& callback);
    void UnSubscribe(const MotionDataUtils::MotionType& type, const sptr<ImotionCallback>& callback);
    MotionDataUtils::MotionData GetLatestMotionData(const MotionDataUtils::MotionType& type);
    int32_t SensorDataCallback(const struct SensorEvents *event);
    int32_t MsdpDataCallback(MotionDataUtils::MotionData& data);
    bool DisableCount(const MotionDataUtils::MotionType& type);
    int32_t LoadAlgorithm();
    int32_t UnloadAlgorithm();
private:
    struct classcomp {
        bool operator()(const sptr<ImotionCallback> &l, const sptr<ImotionCallback> &r) const
        {
            return l->AsObject() < r->AsObject();
        }
    };
    const wptr<MotionService> ms_;
    std::mutex mutex_;
    sptr<IRemoteObject::DeathRecipient> motionCBDeathRecipient_;
    std::unique_ptr<MotionMsdpClientImpl> msdpImpl_;
    MotionMsdpClientImpl::CallbackManager motionClientImplCallback_;
    std::map<MotionDataUtils::MotionType, MotionDataUtils::MotionValue> msdpData_;
    std::map<MotionDataUtils::MotionType, std::set<const sptr<ImotionCallback>, classcomp>> listenerMap_;
};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_MANAGER_H
