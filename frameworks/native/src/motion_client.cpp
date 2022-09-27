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

#include "motion_client.h"

#include <iservice_registry.h>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <system_ability_definition.h>

namespace OHOS {
namespace Msdp {
MotionClient::MotionClient() {}
MotionClient::~MotionClient()
{
    if (motionProxy_ != nullptr) {
        auto remoteObject = motionProxy_->AsObject();
        if (remoteObject != nullptr) {
            remoteObject->RemoveDeathRecipient(deathRecipient_);
        }
    }
}

ErrCode MotionClient::Connect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (motionProxy_ != nullptr) {
        return ERR_OK;
    }

    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "GetSystemAbilityManager failed");
        return E_MOTION_GET_SYSTEM_ABILITY_MANAGER_FAILED;
    }

    sptr<IRemoteObject> remoteObject_ = sam->CheckSystemAbility(MSDP_MOTION_SERVICE_ID);
    if (remoteObject_ == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "CheckSystemAbility failed");
        return E_MOTION_GET_SERVICE_FAILED;
    }

    deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new MotionDeathRecipient());
    if (deathRecipient_ == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "Failed to create MotionDeathRecipient");
        return ERR_NO_MEMORY;
    }

    if ((remoteObject_->IsProxyObject()) && (!remoteObject_->AddDeathRecipient(deathRecipient_))) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "Add death recipient to Motion service failed");
        return E_MOTION_ADD_DEATH_RECIPIENT_FAILED;
    }

    motionProxy_ = iface_cast<Imotion>(remoteObject_);
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Connecting MotionService success");
    return ERR_OK;
}

void MotionClient::ResetProxy(const wptr<IRemoteObject>& remote)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MOTION_RETURN_IF(motionProxy_ == nullptr);

    auto serviceRemote = motionProxy_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        motionProxy_ = nullptr;
    }
}

void MotionClient::MotionDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "OnRemoteDied failed, remote is nullptr");
        return;
    }

    MotionClient::GetInstance().ResetProxy(remote);
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Recv death notice");
}

void MotionClient::SubscribeCallback(const MotionDataUtils::MotionType& type, const sptr<ImotionCallback>& callback)
{
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Enter");
    MOTION_RETURN_IF((callback == nullptr) || (Connect() != ERR_OK));
    motionProxy_->Subscribe(type, callback);
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Exit");
}

void MotionClient::UnSubscribeCallback(const MotionDataUtils::MotionType& type, const sptr<ImotionCallback>& callback)
{
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Enter");
    MOTION_RETURN_IF((callback == nullptr) || (Connect() != ERR_OK));
    motionProxy_->UnSubscribe(type, callback);
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Exit");
}

MotionDataUtils::MotionData MotionClient::GetMotionData(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Enter");
    MotionDataUtils::MotionData motionData;
    motionData.type = MotionDataUtils::MotionType::TYPE_INVALID;
    motionData.value = MotionDataUtils::MotionValue::VALUE_INVALID;

    MOTION_RETURN_IF_WITH_RET((Connect() != ERR_OK), motionData);
    motionData = motionProxy_->GetCache(type);
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Exit");
    return motionData;
}
} // namespace Msdp
} // namespace OHOS
