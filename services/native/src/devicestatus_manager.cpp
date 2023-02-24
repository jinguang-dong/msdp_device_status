/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "devicestatus_manager.h"

#include <algorithm>

#include "devicestatus_define.h"
#include "bytrace_adapter.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace OHOS::HiviewDFX;
namespace {
static constexpr uint8_t ARG_4 = 4;
int arrs_ [ARG_4] {};
} // namespace
void DeviceStatusManager::DeviceStatusCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        DEV_HILOGE(SERVICE, "OnRemoteDied failed, remote is nullptr");
        return;
    }
    DEV_HILOGD(SERVICE, "Recv death notice");
}

bool DeviceStatusManager::Init()
{
    DEV_HILOGD(SERVICE, "Enter");
    if (devicestatusCBDeathRecipient_ == nullptr) {
        devicestatusCBDeathRecipient_ = new (std::nothrow) DeviceStatusCallbackDeathRecipient();
        if (devicestatusCBDeathRecipient_ == nullptr) {
            DEV_HILOGE(SERVICE, "devicestatusCBDeathRecipient_ failed");
            return false;
        }
    }

    msdpImpl_ = std::make_unique<DeviceStatusMsdpClientImpl>();
    if (msdpImpl_ == nullptr) {
        DEV_HILOGE(SERVICE, "msdpImpl_ is nullptr");
        return false;
    }

    DEV_HILOGD(SERVICE, "Init success");
    return true;
}

Data DeviceStatusManager::GetLatestDeviceStatusData(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    Data data = {type, OnChangedValue::VALUE_EXIT};
    if (msdpImpl_ == nullptr) {
        DEV_HILOGE(SERVICE, "GetObserverData func is nullptr,return default");
        data.value = OnChangedValue::VALUE_INVALID;
        return data;
    }
    msdpData_ = msdpImpl_->GetObserverData();
    for (auto iter = msdpData_.begin(); iter != msdpData_.end(); ++iter) {
        if (data.type == iter->first) {
            data.value = iter->second;
            return data;
        }
    }
    return {type, OnChangedValue::VALUE_INVALID};
}

bool DeviceStatusManager::Enable(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (type == Type::TYPE_INVALID) {
        DEV_HILOGE(SERVICE, "enable is failed");
        return false;
    }
    InitAlgoMngrInterface(type);
    InitDataCallback();
    return true;
}

bool DeviceStatusManager::Disable(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (msdpImpl_ == nullptr) {
        DEV_HILOGE(SERVICE, "disable failed, msdpImpl is nullptr");
        return false;
    }

    if (msdpImpl_->Disable(type) != RET_OK) {
        DEV_HILOGE(SERVICE, "disable msdp impl failed");
        return false;
    }

    return true;
}

bool DeviceStatusManager::InitAlgoMngrInterface(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (msdpImpl_ == nullptr) {
        DEV_HILOGE(SERVICE, "msdpImpl_ is nullptr");
        return false;
    }

    if (msdpImpl_->InitMsdpImpl(type) != RET_OK) {
        DEV_HILOGE(SERVICE, "init msdp impl failed");
        return false;
    };
    return true;
}

int32_t DeviceStatusManager::InitDataCallback()
{
    DEV_HILOGD(SERVICE, "Enter");
    if (msdpImpl_ == nullptr) {
        DEV_HILOGE(SERVICE, "msdpImpl_ is nullptr");
        return false;
    }
    DeviceStatusMsdpClientImpl::CallbackManager callback =
        std::bind(&DeviceStatusManager::MsdpDataCallback, this, std::placeholders::_1);
    if (msdpImpl_->RegisterImpl(callback) == RET_ERR) {
        DEV_HILOGE(SERVICE, "register impl failed");
    }
    return true;
}

int32_t DeviceStatusManager::MsdpDataCallback(const Data& data)
{
    NotifyDeviceStatusChange(data);
    return RET_OK;
}

int32_t DeviceStatusManager::SensorDataCallback(struct SensorEvents *event)
{
    DEV_HILOGD(SERVICE, "Enter");
    Data data = {Type::TYPE_ABSOLUTE_STILL, OnChangedValue::VALUE_ENTER};
    if (!NotifyDeviceStatusChange(data)) {
        DEV_HILOGE(SERVICE, "NotifyDeviceStatusChange is failed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t DeviceStatusManager::NotifyDeviceStatusChange(const Data& devicestatusData)
{
    DEV_HILOGD(SERVICE, "Enter");
    std::lock_guard lock(mutex_);
    for (const auto &item : listenerMap_) {
        if (item.second == nullptr) {
            DEV_HILOGE(SERVICE, "Listener is nullptr");
            return false;
        }
        DEV_HILOGI(SERVICE, "type:%{public}d", item.second->GetType());
        switch (arrs_[item.second->GetType()]) {
            case ENTER: {
                if (devicestatusData.value == VALUE_ENTER) {
                    item.second->GetCallback()->OnDeviceStatusChanged(devicestatusData);
                }
                break;
            }
            case EXIT: {
                if (devicestatusData.value == VALUE_EXIT) {
                    item.second->GetCallback()->OnDeviceStatusChanged(devicestatusData);
                }
                break;
            }
            case ENTER_EXIT: {
                item.second->GetCallback()->OnDeviceStatusChanged(devicestatusData);
                break;
            }
            default: {
                DEV_HILOGE(SERVICE, "Exit");
                break;
            }
        }
    }
    return RET_OK;
}

void DeviceStatusManager::Subscribe(std::shared_ptr<ClientInfo> clientInfo)
{
    DEV_RET_IF_NULL(clientInfo->GetCallback() == nullptr);
    event_ = clientInfo->GetEvent();
    type_ = clientInfo->GetType();
    arrs_ [type_] = event_;
    DEV_HILOGI(SERVICE, "type:%{public}d,event:%{public}d", type_, event_);
    auto object = clientInfo->GetCallback()->AsObject();
    DEV_RET_IF_NULL(object == nullptr);
    object->AddDeathRecipient(devicestatusCBDeathRecipient_);
    if (!Enable(clientInfo->GetType())) {
        DEV_HILOGE(SERVICE, "Enable failed!");
        return;
    }
    std::lock_guard lock(mutex_);
    auto iter = listenerMap_.find(clientInfo->GetPid());
    if (iter == listenerMap_.end()) {
        listenerMap_.emplace(clientInfo->GetPid(), clientInfo);
    } else {
        iter->second = clientInfo;
    }
    auto pidIter = activeTypes_.find(clientInfo->GetPid());
    if (pidIter == activeTypes_.end()) {
        std::vector<Type> types;
        types.push_back(clientInfo->GetType());
        activeTypes_.emplace(clientInfo->GetPid(), types);
    } else {
        pidIter->second.push_back(clientInfo->GetType());
    }
    DEV_HILOGI(SERVICE, "lisnteners size:%{public}zu,active type size:%{public}zu, types:%{public}zu",
        listenerMap_.size(), activeTypes_.size(), activeTypes_[clientInfo->GetPid()].size());
    DEV_HILOGI(SERVICE, "Subscribe success,Exit");
}

void DeviceStatusManager::Unsubscribe(std::shared_ptr<ClientInfo> clientInfo)
{
    DEV_HILOGD(SERVICE, "Enter");
    DEV_RET_IF_NULL(clientInfo->GetCallback() == nullptr);
    auto object = clientInfo->GetCallback()->AsObject();
    DEV_RET_IF_NULL(object == nullptr);
    DEV_HILOGE(SERVICE, "type: %{public}d", clientInfo->GetType());
    std::lock_guard lock(mutex_);
    auto pidIter = activeTypes_.find(clientInfo->GetPid());
    if (pidIter != activeTypes_.end()) {
        auto typeIter = std::find(pidIter->second.begin(), pidIter->second.end(), clientInfo->GetType());
        if (typeIter != pidIter->second.end()) {
            pidIter->second.erase(typeIter);
        }
    }
    if (pidIter->second.empty()) {
        Disable(clientInfo->GetType());
        activeTypes_.erase(pidIter);
        auto iter = listenerMap_.find(clientInfo->GetPid());
        if (iter == listenerMap_.end()) {
            DEV_HILOGE(SERVICE, "Failed to find listener for type");
            return;
        }
        listenerMap_.erase(iter);
    }
    DEV_HILOGI(SERVICE, "lisnteners size:%{public}zu,active type size:%{public}zu, types:%{public}zu",
        listenerMap_.size(), activeTypes_.size(), activeTypes_[clientInfo->GetPid()].size());
    DEV_HILOGI(SERVICE, "Unsubscribe success,Exit");
}

int32_t DeviceStatusManager::LoadAlgorithm()
{
    DEV_HILOGI(SERVICE, "Enter");
    if (msdpImpl_ != nullptr) {
        msdpImpl_->LoadAlgoLibrary();
    }

    return RET_OK;
}

int32_t DeviceStatusManager::UnloadAlgorithm()
{
    DEV_HILOGI(SERVICE, "Enter");
    if (msdpImpl_ != nullptr) {
        msdpImpl_->UnloadAlgoLibrary();
    }

    return RET_OK;
}

int32_t DeviceStatusManager::GetPackageName(AccessTokenID tokenId, std::string &packageName)
{
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    switch (tokenType) {
        case ATokenTypeEnum::TOKEN_HAP: {
            HapTokenInfo hapInfo;
            if (AccessTokenKit::GetHapTokenInfo(tokenId, hapInfo) != 0) {
                DEV_HILOGE(SERVICE, "get hap token info fail");
                return RET_ERR;
            }
            packageName = hapInfo.bundleName;
            break;
        }
        case ATokenTypeEnum::TOKEN_NATIVE:
        case ATokenTypeEnum::TOKEN_SHELL: {
            NativeTokenInfo tokenInfo;
            if (AccessTokenKit::GetNativeTokenInfo(tokenId, tokenInfo) != 0) {
                DEV_HILOGE(SERVICE, "get native token info fail");
                return RET_ERR;
            }
            packageName = tokenInfo.processName;
            break;
        }
        default: {
            DEV_HILOGE(SERVICE, "token type not match");
            break;
        }
    }
    return RET_OK;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
