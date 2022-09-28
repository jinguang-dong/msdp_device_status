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

#include "devicestatus_manager.h"
#include "bytrace_adapter.h"

namespace OHOS {
namespace Msdp {
using namespace OHOS::HiviewDFX;
namespace {
constexpr int32_t ERR_OK = 0;
constexpr int32_t ERR_NG = -1;
}
void DevicestatusManager::DevicestatusCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        DEV_HILOGE(SERVICE, "OnRemoteDied failed, remote is nullptr");
        return;
    }
    DEV_HILOGD(SERVICE, "Recv death notice");
}

bool DevicestatusManager::Init()
{
    DEV_HILOGI(SERVICE, "Enter");
    if (devicestatusCBDeathRecipient_ == nullptr) {
        devicestatusCBDeathRecipient_ = new DevicestatusCallbackDeathRecipient();
    }

    msdpImpl_ = std::make_unique<DevicestatusMsdpClientImpl>();
    if (msdpImpl_ == nullptr) {
        return false;
    }
    LoadAlgorithm(false);

    DEV_HILOGI(SERVICE, "Init success");
    return true;
}

DevicestatusDataUtils::DevicestatusData DevicestatusManager::GetLatestDevicestatusData(const \
    DevicestatusDataUtils::DevicestatusType& type)
{
    DEV_HILOGI(SERVICE, "Enter");
    DevicestatusDataUtils::DevicestatusData data = {type, DevicestatusDataUtils::DevicestatusValue::VALUE_EXIT};
    if (msdpImpl_ == nullptr) {
        DEV_HILOGI(SERVICE, "GetObserverData func is nullptr,return default!");
        data.value = DevicestatusDataUtils::DevicestatusValue::VALUE_INVALID;
        return data;
    }
    msdpData_ = msdpImpl_->GetObserverData();
    for (auto iter = msdpData_.begin(); iter != msdpData_.end(); ++iter) {
        if (data.type == iter->first) {
            data.value = iter->second;
            return data;
        }
    }

    data.value = DevicestatusDataUtils::DevicestatusValue::VALUE_INVALID;
    return data;
}

bool DevicestatusManager::EnableRdb(const DevicestatusDataUtils::DevicestatusType& type)
{
    DEV_HILOGE(SERVICE, "Enter");
    InitInterface(type); //START SENSOR
    InitDataCallback();
    return true;
}
bool DevicestatusManager::DisableRdb(const DevicestatusDataUtils::DevicestatusType& type)
{
    DEV_HILOGE(SERVICE, "Enter");
    if (msdpImpl_ == nullptr) {
        DEV_HILOGE(SERVICE, "disable rdb failed, msdpImpl is nullptr");
        return false;
    }

    if (msdpImpl_->DisableMsdpImpl(type) == ERR_NG) {
        DEV_HILOGE(SERVICE, "disable msdp impl failed");
        return false;
    }

    if (msdpImpl_->UnregisterImpl() == ERR_NG) {
        DEV_HILOGE(SERVICE, "unregister impl failed");
        return false;
    }

    return true;
}

bool DevicestatusManager::InitInterface(const DevicestatusDataUtils::DevicestatusType& type)
{
    DEV_HILOGE(SERVICE, "Enter");
    if (msdpImpl_ == nullptr) {
        DEV_HILOGE(SERVICE, "msdpImpl_ is nullptr");
        return false;
    }
    
    if (msdpImpl_->InitMsdpImpl(type) == ERR_NG) {
        DEV_HILOGE(SERVICE, "init msdp impl failed");
    };
    return true;
}

bool DevicestatusManager::InitDataCallback()
{
    DEV_HILOGE(SERVICE, "Enter");
    if (msdpImpl_ == nullptr) {
        DEV_HILOGE(SERVICE, "msdpImpl_ is nullptr");
        return false;
    }
    DevicestatusMsdpClientImpl::CallbackManager callback =
        std::bind(&DevicestatusManager::MsdpDataCallback, this, std::placeholders::_1);
    if (msdpImpl_->RegisterImpl(callback) == ERR_NG) {
        DEV_HILOGE(SERVICE, "register impl failed");
    }
    return true;
}

int32_t DevicestatusManager::MsdpDataCallback(const DevicestatusDataUtils::DevicestatusData& data)
{
    NotifyDevicestatusChange(data);
    return ERR_OK;
}

int32_t DevicestatusManager::SensorDataCallback(const struct SensorEvents *event)
{
    DEV_HILOGI(SERVICE, "Enter");
    // handle sensor event properly when we get the data details of sensor HDI.
    DevicestatusDataUtils::DevicestatusData data = {DevicestatusDataUtils::DevicestatusType::TYPE_HIGH_STILL,
        DevicestatusDataUtils::DevicestatusValue::VALUE_ENTER};
    NotifyDevicestatusChange(data);
    return ERR_OK;
}

void DevicestatusManager::NotifyDevicestatusChange(const DevicestatusDataUtils::DevicestatusData& devicestatusData)
{
    DEV_HILOGI(SERVICE, "Enter");

    // Call back for all listeners
    std::set<const sptr<IdevicestatusCallback>, classcomp> listeners;
    bool isExists = false;
    for (auto it = listenerMap_.begin(); it != listenerMap_.end(); ++it) {
        if (it->first == devicestatusData.type) {
            isExists = true;
            listeners = (std::set<const sptr<IdevicestatusCallback>, classcomp>)(it->second);
            break;
        }
    }
    if (!isExists) {
        DEV_HILOGI(SERVICE, "No listener found for type: %{public}d", \
            devicestatusData.type);
        DEV_HILOGI(SERVICE, "Exit");
        return;
    }
    for (auto& listener : listeners) {
        if (listener == nullptr) {
            DEV_HILOGI(SERVICE, "Listener is nullptr");
            return;
        }
    switch (arrs[devicestatusData.type]) {
        case DevicestatusDataUtils::ENTER:
            if (devicestatusData.value == DevicestatusDataUtils::VALUE_ENTER) {
                listener->OnDevicestatusChanged(devicestatusData);
            }
            break;
        case DevicestatusDataUtils::EXIT:
            if (devicestatusData.value == DevicestatusDataUtils::VALUE_EXIT) {
                listener->OnDevicestatusChanged(devicestatusData);
            }
            break;
        case DevicestatusDataUtils::ENTER_EXIT:
            listener->OnDevicestatusChanged(devicestatusData);
            break;
        default:
            break;
    }
    }
}

void DevicestatusManager::Subscribe(const DevicestatusDataUtils::DevicestatusType& type,
    const DevicestatusDataUtils::DevicestatusActivityEvent& event,
    const DevicestatusDataUtils::DevicestatusReportLatencyNs& latency,
    const sptr<IdevicestatusCallback>& callback)
{
    DEVICESTATUS_RETURN_IF(callback == nullptr);
    auto object = callback->AsObject();
    DEVICESTATUS_RETURN_IF(object == nullptr);
    std::set<const sptr<IdevicestatusCallback>, classcomp> listeners;
    DEV_HILOGI(SERVICE, "listenerMap_.size=%{public}zu", listenerMap_.size());
    event_ = event;
    type_ = type;
    if (!EnableRdb(type)) {
        DEV_HILOGE(SERVICE, "Enable failed!");
        return;
    }
    arrs [type_] = event_;
    std::lock_guard lock(mutex_);
    auto dtTypeIter = listenerMap_.find(type);
    if (dtTypeIter == listenerMap_.end()) {
        if (listeners.insert(callback).second) {
            DEV_HILOGI(SERVICE, "no found set list of type, insert success");
            object->AddDeathRecipient(devicestatusCBDeathRecipient_);
        }
        listenerMap_.insert(std::make_pair(type, listeners));
    } else {
        DEV_HILOGI(SERVICE, "callbacklist.size=%{public}zu",
            listenerMap_[dtTypeIter->first].size());
        auto iter = listenerMap_[dtTypeIter->first].find(callback);
        if (iter != listenerMap_[dtTypeIter->first].end()) {
            return;
        } else {
            if (listenerMap_[dtTypeIter->first].insert(callback).second) {
                DEV_HILOGI(SERVICE, "found set list of type, insert success");
                object->AddDeathRecipient(devicestatusCBDeathRecipient_);
            }
        }
    }
    DEV_HILOGI(SERVICE, "Subscribe success,Exit");
}

void DevicestatusManager::UnSubscribe(const DevicestatusDataUtils::DevicestatusType& type,
    const DevicestatusDataUtils::DevicestatusActivityEvent& event,
    const sptr<IdevicestatusCallback>& callback)
{
    DEV_HILOGI(SERVICE, "Enter");
    DEV_HILOGE(SERVICE, "UNevent: %{public}d", event);
    std::lock_guard lock(mutex_);
    DEVICESTATUS_RETURN_IF(callback == nullptr);
    auto object = callback->AsObject();
    DEVICESTATUS_RETURN_IF(object == nullptr);
    arrs [type_] = 0;

    auto dtTypeIter = listenerMap_.find(type);
    if (dtTypeIter == listenerMap_.end()) {
        return;
    } else {
        DEV_HILOGI(SERVICE, "callbacklist.size=%{public}zu",
            listenerMap_[dtTypeIter->first].size());
        auto iter = listenerMap_[dtTypeIter->first].find(callback);
        if (iter != listenerMap_[dtTypeIter->first].end()) {
            if (listenerMap_[dtTypeIter->first].erase(callback) != 0) {
                object->RemoveDeathRecipient(devicestatusCBDeathRecipient_);
                if (listenerMap_[dtTypeIter->first].size() == 0) {
                    listenerMap_.erase(dtTypeIter);
                }
            }
        }
    }
    DEV_HILOGI(SERVICE, "listenerMap_.size = %{public}zu", listenerMap_.size());
    if (listenerMap_.empty()) {
        DisableRdb(type);
    } else {
        DEV_HILOGI(SERVICE, "other subscribe exist");
    }
    DEV_HILOGI(SERVICE, "UnSubscribe success,Exit");
}

int32_t DevicestatusManager::LoadAlgorithm(bool bCreate)
{
    DEV_HILOGI(SERVICE, "Enter");
    if (msdpImpl_ != nullptr) {
        msdpImpl_->LoadAlgorithmLibrary(bCreate);
        msdpImpl_->LoadSensorHdiLibrary(bCreate);
        msdpImpl_->LoadDevAlgorithmLibrary(bCreate);
    }

    return ERR_OK;
}

int32_t DevicestatusManager::UnloadAlgorithm(bool bCreate)
{
    DEV_HILOGI(SERVICE, "Enter");
    if (msdpImpl_ != nullptr) {
        msdpImpl_->UnloadAlgorithmLibrary(bCreate);
        msdpImpl_->UnloadSensorHdiLibrary(bCreate);
        msdpImpl_->UnloadDevAlgorithmLibrary(bCreate);
    }

    return ERR_OK;
}

void DevicestatusManager::GetPackageName(AccessTokenID tokenId, std::string &packageName)
{
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    switch (tokenType) {
        case ATokenTypeEnum::TOKEN_HAP: {
            HapTokenInfo hapInfo;
            if (AccessTokenKit::GetHapTokenInfo(tokenId, hapInfo) != 0) {
                DEV_HILOGE(SERVICE, "get hap token info fail");
                return;
            }
            packageName = hapInfo.bundleName;
            break;
        }
        // Native type and shell type get processname in the same way
        case ATokenTypeEnum::TOKEN_NATIVE:
        case ATokenTypeEnum::TOKEN_SHELL: {
            NativeTokenInfo tokenInfo;
            if (AccessTokenKit::GetNativeTokenInfo(tokenId, tokenInfo) != 0) {
                DEV_HILOGE(SERVICE, "get native token info fail");
                return;
            }
            packageName = tokenInfo.processName;
            break;
        }
        default: {
            DEV_HILOGE(SERVICE, "token type not match");
            break;
        }
    }
}
} // namespace Msdp
} // namespace OHOS
