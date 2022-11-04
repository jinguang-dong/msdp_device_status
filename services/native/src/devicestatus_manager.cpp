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

#include "devicestatus_manager.h"
#include "bytrace_adapter.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace OHOS::HiviewDFX;
namespace {
constexpr int32_t ERR_OK = 0;
constexpr int32_t ERR_NG = -1;
}
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
        }
    }

    msdpImpl_ = std::make_unique<DeviceStatusMsdpClientImpl>();
    if (msdpImpl_ == nullptr) {
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
        return {type, OnChangedValue::VALUE_INVALID};
    }
    std::map<Type, OnChangedValue>::iterator iter;
    iter = msdpData_.find(data.type);
    if(iter != msdpData_.end()){
        data.value = iter->second;
        return {type, OnChangedValue::VALUE_INVALID};
        }
    return {type, OnChangedValue::VALUE_INVALID};
}

bool DeviceStatusManager::Enable(Type type)
{
    DEV_HILOGE(SERVICE, "Enter");
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
    DEV_HILOGE(SERVICE, "Enter");
    if (msdpImpl_ == nullptr) {
        DEV_HILOGE(SERVICE, "disable rdb failed, msdpImpl is nullptr");
        return false;
    }

    if (msdpImpl_->Disable(type) != ERR_OK) {
        DEV_HILOGE(SERVICE, "disable msdp impl failed");
        return false;
    }

    return true;
}

bool DeviceStatusManager::InitAlgoMngrInterface(Type type)
{
    DEV_HILOGE(SERVICE, "Enter");
    if (msdpImpl_ == nullptr) {
        DEV_HILOGE(SERVICE, "msdpImpl_ is nullptr");
        return false;
    }

    if (msdpImpl_->InitMsdpImpl(type) != ERR_OK) {
        DEV_HILOGE(SERVICE, "init msdp impl failed");
        return false;
    };
    return true;
}

int32_t DeviceStatusManager::InitDataCallback()
{
    DEV_HILOGE(SERVICE, "Enter");
    if (msdpImpl_ == nullptr) {
        DEV_HILOGE(SERVICE, "msdpImpl_ is nullptr");
        return false;
    }
    DeviceStatusMsdpClientImpl::CallbackManager callback =
        std::bind(&DeviceStatusManager::MsdpDataCallback, this, std::placeholders::_1);
    if (msdpImpl_->RegisterImpl(callback) == ERR_NG) {
        DEV_HILOGE(SERVICE, "register impl failed");
    }
    return true;
}

int32_t DeviceStatusManager::MsdpDataCallback(const Data& data)
{
    NotifyDeviceStatusChange(data);
    return ERR_OK;
}

int32_t DeviceStatusManager::SensorDataCallback(const struct SensorEvents *event)
{
    DEV_HILOGD(SERVICE, "Enter");
    Data data = {Type::TYPE_STILL, OnChangedValue::VALUE_ENTER};
    if (!NotifyDeviceStatusChange(data)) {
        return ERR_NG;
    }
    return ERR_OK;
}

int32_t DeviceStatusManager::NotifyDeviceStatusChange(const Data& devicestatusData)
{
    DEV_HILOGD(SERVICE, "Enter");

    std::set<const sptr<IRemoteDevStaCallbck>, classcomp> listeners;
    bool isExists = false;
    std::map<Type, std::set<const sptr<IRemoteDevStaCallbck>, classcomp>>::iterator iter;
    iter = listenerMap_.find(devicestatusData.type);
    if(iter != listenerMap_.end()){
        isExists = true;
        listeners = (std::set<const sptr<IRemoteDevStaCallbck>, classcomp>)(iter->second);
    }
    if (!isExists) {
        DEV_HILOGI(SERVICE, "Type:%{public}d", devicestatusData.type);
        DEV_HILOGD(SERVICE, "Exit");
        return false;
    }
    for (auto& listener : listeners) {
        if (listener == nullptr) {
            DEV_HILOGE(SERVICE, "Listener is nullptr");
            return false;
        }
    DEV_HILOGI(SERVICE, "type:%{public}d,arrs_:%{public}d", devicestatusData.type, arrs_[devicestatusData.type]);
    switch (arrs_[devicestatusData.type]) {
        case ENTER: {
            if (devicestatusData.value == VALUE_ENTER) {
                listener->OnDeviceStatusChanged(devicestatusData);
            }
        }
            break;
        case EXIT: {
            if (devicestatusData.value == VALUE_EXIT) {
                listener->OnDeviceStatusChanged(devicestatusData);
            }
        }
            break;
        case ENTER_EXIT: {
            listener->OnDeviceStatusChanged(devicestatusData);
        }
            break;
        default:
            break;
    }
    }
    return ERR_OK;
}

void DeviceStatusManager::Subscribe(const Type type,
    const ActivityEvent event,
    const ReportLatencyNs latency,
    const sptr<IRemoteDevStaCallbck> callback)
{
    DEV_RET_IF_NULL(callback == nullptr);
    auto object = callback->AsObject();
    DEV_RET_IF_NULL(object == nullptr);
    std::set<const sptr<IRemoteDevStaCallbck>, classcomp> listeners;
    DEV_HILOGI(SERVICE, "listenerMap_.size=%{public}zu", listenerMap_.size());
    event_ = event;
    type_ = type;
    if (!Enable(type)) {
        DEV_HILOGE(SERVICE, "Enable failed!");
        return;
    }
    arrs_ [type_] = event_;
    DEV_HILOGE(SERVICE, " arr save:%{public}d ,event:%{public}d", type_, event);
    std::lock_guard lock(mutex_);
    auto dtTypeIter = listenerMap_.find(type);
    if (dtTypeIter == listenerMap_.end()) {
        if (listeners.insert(callback).second) {
            DEV_HILOGI(SERVICE, "no found set list of type, insert success");
            object->AddDeathRecipient(devicestatusCBDeathRecipient_);
        }
        listenerMap_.insert(std::make_pair(type, listeners));
    } else {
        DEV_HILOGI(SERVICE, "callbacklist.size=%{public}zu", listenerMap_[dtTypeIter->first].size());
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

void DeviceStatusManager::UnSubscribe(const Type type,
    const ActivityEvent event,
    const sptr<IRemoteDevStaCallbck> callback)
{
    DEV_HILOGD(SERVICE, "Enter");
    DEV_HILOGE(SERVICE, "UNevent: %{public}d", event);
    std::lock_guard lock(mutex_);
    DEV_RET_IF_NULL(callback == nullptr);
    auto object = callback->AsObject();
    DEV_RET_IF_NULL(object == nullptr);
    DEV_HILOGE(SERVICE, "listenerMap_.size=%{public}zu,arrs_:%{public}d", listenerMap_.size(), arrs_ [type_]);

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
        Disable(type);
    } else {
        DEV_HILOGI(SERVICE, "other subscribe exist");
    }
    DEV_HILOGI(SERVICE, "UnSubscribe success,Exit");
}

int32_t DeviceStatusManager::GetPackageName(AccessTokenID tokenId, std::string &packageName)
{
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    switch (tokenType) {
        case ATokenTypeEnum::TOKEN_HAP: {
            HapTokenInfo hapInfo;
            if (AccessTokenKit::GetHapTokenInfo(tokenId, hapInfo) != 0) {
                DEV_HILOGE(SERVICE, "get hap token info fail");
                return ERR_NG;
            }
            packageName = hapInfo.bundleName;
            break;
        }
        case ATokenTypeEnum::TOKEN_NATIVE:
        case ATokenTypeEnum::TOKEN_SHELL: {
            NativeTokenInfo tokenInfo;
            if (AccessTokenKit::GetNativeTokenInfo(tokenId, tokenInfo) != 0) {
                DEV_HILOGE(SERVICE, "get native token info fail");
                return ERR_NG;
            }
            packageName = tokenInfo.processName;
            break;
        }
        default: {
            DEV_HILOGE(SERVICE, "token type not match");
            break;
        }
    }
    return ERR_OK;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
