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

#include <if_system_ability_manager.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "iremote_broker.h"
#include "iremote_object.h"

#include "devicestatus_client.h"
#include "devicestatus_common.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
DeviceStatusClient::DeviceStatusClient() {}
DeviceStatusClient::~DeviceStatusClient()
{
    if (devicestatusProxy_ != nullptr) {
        auto remoteObject = devicestatusProxy_->AsObject();
        if (remoteObject != nullptr) {
            remoteObject->RemoveDeathRecipient(deathRecipient_);
        }
    }
}

ErrCode DeviceStatusClient::Connect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (devicestatusProxy_ != nullptr) {
        DEV_HILOGE(INNERKIT, "devicestatusProxy_ is nut nullptr");
        return ERR_OK;
    }

    sptr<ISystemAbilityManager> sa = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sa == nullptr) {
        DEV_HILOGE(INNERKIT, "GetSystemAbilityManager failed");
        return E_DEVICESTATUS_GET_SYSTEM_ABILITY_MANAGER_FAILED;
    }

    sptr<IRemoteObject> remoteObject = sa->CheckSystemAbility(MSDP_DEVICESTATUS_SERVICE_ID);
    if (remoteObject == nullptr) {
        DEV_HILOGE(INNERKIT, "CheckSystemAbility failed");
        return E_DEVICESTATUS_GET_SERVICE_FAILED;
    }

    deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new (std::nothrow) DeviceStatusDeathRecipient());
    if (deathRecipient_ == nullptr) {
        DEV_HILOGE(INNERKIT, "Failed to create DeviceStatusDeathRecipient");
        return ERR_NO_MEMORY;
    }

    if (remoteObject->IsProxyObject()) {
        if (!remoteObject->AddDeathRecipient(deathRecipient_)) {
            DEV_HILOGE(INNERKIT, "Add death recipient to DeviceStatus service failed");
            return E_DEVICESTATUS_ADD_DEATH_RECIPIENT_FAILED;
        }
    }

    devicestatusProxy_ = iface_cast<Idevicestatus>(remoteObject);
    DEV_HILOGD(INNERKIT, "Connecting DeviceStatusService success");
    return ERR_OK;
}

void DeviceStatusClient::ResetProxy(const wptr<IRemoteObject>& remote)
{
    std::lock_guard<std::mutex> lock(mutex_);
    DEV_RET_IF_NULL(devicestatusProxy_ == nullptr);

    auto serviceRemote = devicestatusProxy_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        devicestatusProxy_ = nullptr;
    }
}

void DeviceStatusClient::DeviceStatusDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        DEV_HILOGE(INNERKIT, "OnRemoteDied failed, remote is nullptr");
        return;
    }

    DeviceStatusClient::GetInstance().ResetProxy(remote);
    DEV_HILOGD(INNERKIT, "Recv death notice");
}

void DeviceStatusClient::SubscribeCallback(Type type,
    ActivityEvent event,
    ReportLatencyNs latency,
    sptr<IRemoteDevStaCallbck> callback)
{
    DEV_HILOGI(INNERKIT, "Enter event:%{public}d,latency:%{public}d", event, latency);
    typeMap_.insert(std::make_pair(type,1));
    DEV_HILOGD(INNERKIT, "typeMap_ %{public}d, type: %{public}d", typeMap_[type], type);
    DEV_RET_IF_NULL((callback == nullptr) || (Connect() != ERR_OK));
    if (devicestatusProxy_ == nullptr) {
        DEV_HILOGE(SERVICE, "devicestatusProxy_ is nullptr");
        return;
    }
    if (type > Type::TYPE_INVALID
        && type <= Type::TYPE_LID_OPEN) {
        devicestatusProxy_->Subscribe(type, event, latency, callback);
    }
    return;
}

void DeviceStatusClient::UnSubscribeCallback(Type type,
    ActivityEvent event,
    sptr<IRemoteDevStaCallbck> callback)
{
    DEV_HILOGI(INNERKIT, "UNevent: %{public}d", event);
    typeMap_.erase(type);
    DEV_HILOGD(INNERKIT, "typeMap_ %{public}d", typeMap_[type]);
    DEV_RET_IF_NULL((callback == nullptr) || (Connect() != ERR_OK));
    if (devicestatusProxy_ == nullptr) {
        DEV_HILOGE(SERVICE, "devicestatusProxy_ is nullptr");
        return;
    }
    if (type > Type::TYPE_INVALID && type <= Type::TYPE_LID_OPEN) {
        devicestatusProxy_->UnSubscribe(type, event, callback);
    }
    if (event > ActivityEvent::EVENT_INVALID && event <= ActivityEvent::ENTER_EXIT) {
        DEV_HILOGE(SERVICE, "event is failed");
    }
    return;
    DEV_HILOGD(INNERKIT, "Exit");
}

Data DeviceStatusClient::GetDeviceStatusData(Type type)
{
    DEV_HILOGD(INNERKIT, "Enter");
    Data devicestatusData;
    devicestatusData.type = Type::TYPE_INVALID;
    devicestatusData.value = OnChangedValue::VALUE_INVALID;

    DEV_RET_IF_NULL_WITH_RET((Connect() != ERR_OK), devicestatusData);
    if (devicestatusProxy_ == nullptr) {
        DEV_HILOGE(SERVICE, "devicestatusProxy_ is nullptr");
        return devicestatusData;
    }
    if (type > Type::TYPE_INVALID
        && type <= Type::TYPE_LID_OPEN) {
        devicestatusData = devicestatusProxy_->GetCache(type);
    }
    DEV_HILOGD(INNERKIT, "Exit");
    return devicestatusData;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
