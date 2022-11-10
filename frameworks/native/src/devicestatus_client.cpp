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

#include "devicestatus_client.h"

#include <if_system_ability_manager.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "load_devicestatus_callback.h"

#include "devicestatus_common.h"

#include "iremote_broker.h"
#include "iremote_object.h"

namespace {
constexpr uint32_t WAIT_MS = 500;
}

namespace OHOS {
namespace Msdp {
DevicestatusClient::DevicestatusClient() {}
DevicestatusClient::~DevicestatusClient()
{
    if (devicestatusProxy_ != nullptr) {
        auto remoteObject = devicestatusProxy_->AsObject();
        if (remoteObject != nullptr) {
            remoteObject->RemoveDeathRecipient(deathRecipient_);
        }
    }
}

ErrCode DevicestatusClient::Connect()
{
    if (devicestatusProxy_ != nullptr) {
        DEV_HILOGE(INNERKIT, "devicestatusProxy_ is nut nullptr");
        return ERR_OK;
    }

    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        DEV_HILOGE(INNERKIT, "GetSystemAbilityManager failed");
        return E_DEVICESTATUS_GET_SYSTEM_ABILITY_MANAGER_FAILED;
    }

    sptr<IRemoteObject> remoteObject_ = sam->CheckSystemAbility(MSDP_DEVICESTATUS_SERVICE_ID);
    if (remoteObject_ == nullptr) {
        DEV_HILOGE(INNERKIT, "CheckSystemAbility failed");
        return E_DEVICESTATUS_GET_SERVICE_FAILED;
    }

    deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new DevicestatusDeathRecipient());
    if (deathRecipient_ == nullptr) {
        DEV_HILOGE(INNERKIT, "Failed to create DevicestatusDeathRecipient");
        return ERR_NO_MEMORY;
    }

    if ((remoteObject_->IsProxyObject()) && (!remoteObject_->AddDeathRecipient(deathRecipient_))) {
        DEV_HILOGE(INNERKIT, "Add death recipient to Devicestatus service failed");
        return E_DEVICESTATUS_ADD_DEATH_RECIPIENT_FAILED;
    }

    devicestatusProxy_ = iface_cast<Idevicestatus>(remoteObject_);
    DEV_HILOGD(INNERKIT, "Connecting DevicestatusService success");
    return ERR_OK;
}

void DevicestatusClient::ResetProxy(const wptr<IRemoteObject>& remote)
{
    DEVICESTATUS_RETURN_IF(devicestatusProxy_ == nullptr);

    auto serviceRemote = devicestatusProxy_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        devicestatusProxy_ = nullptr;
    }
}

void DevicestatusClient::DevicestatusDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        DEV_HILOGE(INNERKIT, "OnRemoteDied failed, remote is nullptr");
        return;
    }

    DevicestatusClient::GetInstance().ResetProxy(remote);
    DEV_HILOGD(INNERKIT, "Recv death notice");
}

void DevicestatusClient::SubscribeCallback(const DevicestatusDataUtils::DevicestatusType& type, \
    const sptr<IdevicestatusCallback>& callback)
{
    DEV_HILOGD(INNERKIT, "Enter");
    DEVICESTATUS_RETURN_IF(callback == nullptr);
    CheckConnect();
    if (devicestatusProxy_ == nullptr) {
        DEV_HILOGE(SERVICE, "devicestatusProxy_ is nullptr");
        return;
    }
    if (type > DevicestatusDataUtils::DevicestatusType::TYPE_INVALID
        && type <= DevicestatusDataUtils::DevicestatusType::TYPE_LID_OPEN) {
        devicestatusProxy_->Subscribe(type, callback);
    }
    return;
    DEV_HILOGD(INNERKIT, "Exit");
}

void DevicestatusClient::UnSubscribeCallback(const DevicestatusDataUtils::DevicestatusType& type, \
    const sptr<IdevicestatusCallback>& callback)
{
    DEV_HILOGD(INNERKIT, "Enter");
    DEVICESTATUS_RETURN_IF(callback == nullptr);
    CheckConnect();
    if (devicestatusProxy_ == nullptr) {
        DEV_HILOGE(SERVICE, "devicestatusProxy_ is nullptr");
        return;
    }
    if (type > DevicestatusDataUtils::DevicestatusType::TYPE_INVALID
        && type <= DevicestatusDataUtils::DevicestatusType::TYPE_LID_OPEN) {
        devicestatusProxy_->UnSubscribe(type, callback);
    }
    return;
    DEV_HILOGD(INNERKIT, "Exit");
}

DevicestatusDataUtils::DevicestatusData DevicestatusClient::GetDevicestatusData(const \
    DevicestatusDataUtils::DevicestatusType& type)
{
    DEV_HILOGD(INNERKIT, "Enter");
    DevicestatusDataUtils::DevicestatusData devicestatusData;
    devicestatusData.type = DevicestatusDataUtils::DevicestatusType::TYPE_INVALID;
    devicestatusData.value = DevicestatusDataUtils::DevicestatusValue::VALUE_INVALID;
    CheckConnect();
    if (devicestatusProxy_ == nullptr) {
        DEV_HILOGE(SERVICE, "devicestatusProxy_ is nullptr");
        return devicestatusData;
    }
    if (type > DevicestatusDataUtils::DevicestatusType::TYPE_INVALID
        && type <= DevicestatusDataUtils::DevicestatusType::TYPE_LID_OPEN) {
        devicestatusData = devicestatusProxy_->GetCache(type);
    }
    DEV_HILOGD(INNERKIT, "Exit");
    return devicestatusData;
}
void DevicestatusClient::CheckConnect()
{
    DEV_HILOGI(INNERKIT, "Enter");
    if(Connect() != ERR_OK) {
        DEV_HILOGE(INNERKIT, "Failed to get service");
        if (LoadService() != ERR_OK) {
            DEV_HILOGE(INNERKIT, "Failed to load service");
            return;
        }
    }
    DEV_HILOGI(INNERKIT, "Exit");
}
int32_t DevicestatusClient::LoadService()
{
    DEV_HILOGI(INNERKIT, "Enter");
    std::unique_lock lock(mutex_);
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        DEV_HILOGI(INNERKIT, "GetSystemAbilityManager is null");
        return E_DEVICESTATUS_GET_SYSTEM_ABILITY_MANAGER_FAILED;
    }
    sptr<LoadDeviceStatusCallback> loadCallback = new (std::nothrow) LoadDeviceStatusCallback();
    int32_t result = sm->LoadSystemAbility(MSDP_DEVICESTATUS_SERVICE_ID, loadCallback);
    if (result != ERR_OK) {
       DEV_HILOGE(INNERKIT, "LoadSystemAbility failed");
       return E_LOAD_SYSTEM_ABILITY_FAILED;
    }

    auto waitStatus = proxyConVar_.wait_for(lock, std::chrono::milliseconds(WAIT_MS), [this]() {
        if (this->Connect() != ERR_OK) {
            DEV_HILOGE(INNERKIT, "failed to get service Id, load service");
            return false;
        }
        return this->devicestatusProxy_ != nullptr;
    });
    if (!waitStatus) {
        DEV_HILOGE(INNERKIT, "timeline load timeout");
        return E_LOAD_TIMEOUT;
    }
    DEV_HILOGI(INNERKIT, "Exit");
    return ERR_OK;
}

void DevicestatusClient::LoadServiceSuccess()
{
    DEV_HILOGI(INNERKIT, "Enter");
    std::lock_guard<std::mutex> lock(mutex_);
    proxyConVar_.notify_all();
}

void DevicestatusClient::LoadServiceFail()
{
    DEV_HILOGI(INNERKIT, "Enter");
    std::lock_guard<std::mutex> lock(mutex_);
    proxyConVar_.notify_all();
    LoadService();
}
} // namespace Msdp
} // namespace OHOS
