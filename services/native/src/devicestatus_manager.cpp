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

#include "devicestatus_data_utils.h"
#include "devicestatus_define.h"
#include "bytrace_adapter.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace OHOS::HiviewDFX;
namespace {
constexpr int32_t INVALID_PID = -1;
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
    auto channels = GetChannels(devicestatusData.type);
    for (const auto& channel : channels) {
        channel->OnDeviceStatusChanged(devicestatusData);
    }
    return RET_OK;
}

bool DeviceStatusManager::UpdateClientInfo(int32_t pid, Type type, std::shared_ptr<ClientInfo> clientInfo)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (type == TYPE_INVALID || pid <= INVALID_PID) {
        DEV_HILOGE(SERVICE, "params are invalid");
        return false;
    }
    std::lock_guard<std::mutex> clientLock(clientInfoMutex_);
    auto iter = clientInfos_.find(type);
    if (iter == clientInfos_.end()) {
        std::map<int32_t, std::shared_ptr<ClientInfo>> pidMap;
        auto pidRet = pidMap.insert(std::make_pair(pid, clientInfo));
        auto clientRet = clientInfos_.insert(std::make_pair(type, pidMap));
        return pidRet.second && clientRet.second;
    }
    auto pidIt = iter->second.find(pid);
    if (pidIt == iter->second.end()) {
        auto ret = iter->second.insert(std::make_pair(pid, clientInfo));
        return ret.second;
    }
    iter->second[pid] = clientInfo;
    DEV_HILOGI(SERVICE, "Subscribe success,Exit");
    return true;
}

bool DeviceStatusManager::DestoryInfo(int32_t pid, Type type, std::shared_ptr<ClientInfo> clientInfo)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (type == TYPE_INVALID || pid <= INVALID_PID) {
        DEV_HILOGE(SERVICE, "params are invalid");
        return false;
    }
    std::lock_guard<std::mutex> clientLock(clientInfoMutex_);
    auto it = clientInfos_.find(type);
    if (it == clientInfos_.end()) {
        DEV_HILOGW(SERVICE, "type not exist");
        return false;
    }
    auto pidIt = it->second.find(pid);
    if (pidIt != it->second.end()) {
        it->second.erase(pidIt);
    }
    DEV_HILOGI(SERVICE, "Unsubscribe success,Exit");
    return true;
}

bool DeviceStatusManager::IsOnlyCurPidSubscribe(Type type, int32_t pid)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (OnlyCurPidSubscribe(type, pid)) {
        DEV_HILOGW(SERVICE, "Only current client using this algo");
        return false;
    }
    ClearCurPidClientInfo(type, pid);
    return true;
}

bool DeviceStatusManager::OnlyCurPidSubscribe(Type type, int32_t pid)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (type == TYPE_INVALID || pid <= INVALID_PID) {
        DEV_HILOGE(SERVICE, "params are invalid");
        return false;
    }
    std::lock_guard<std::mutex> clientLock(clientInfoMutex_);
    auto iter = clientInfos_.find(type);
    if (iter == clientInfos_.end()) {
        DEV_HILOGE(SERVICE, "cannot find type:%{public}d", type);
        return false;
    }
    bool ret = false;
    for (const auto &pidIt : iter->second) {
        if (pidIt.first != pid) {
            ret = false;
        } else {
            ret = true;
        }
    }
    return ret;
}

bool DeviceStatusManager::ClearClientInfo(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (type == TYPE_INVALID) {
        DEV_HILOGE(SERVICE, "type is invalid");
        return false;
    }
    std::lock_guard<std::mutex> clientLock(clientInfoMutex_);
    auto iter = clientInfos_.find(type);
    if (iter == clientInfos_.end()) {
        DEV_HILOGE(SERVICE, "type not exist, no need to clear it");
        return true;
    }
    clientInfos_.erase(iter);
    return true;
}

bool DeviceStatusManager::ClearCurPidClientInfo(Type type, int32_t pid)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (type == TYPE_INVALID || pid <= INVALID_PID) {
        DEV_HILOGE(SERVICE, "params are invalid");
        return false;
    }
    std::lock_guard<std::mutex> clientLock(clientInfoMutex_);
    auto it = clientInfos_.find(type);
    if (it == clientInfos_.end()) {
        DEV_HILOGE(SERVICE, "type not exist, no need to clear it");
        return false;
    }
    auto pidIt = it->second.find(pid);
    if (pidIt == it->second.end()) {
        DEV_HILOGE(SERVICE, "pid not exist, no need to clear it");
        return false;
    }
    pidIt = it->second.erase(pidIt);
    if (it->second.size() == 0) {
        it = clientInfos_.erase(it);
    }
    return true;
}

std::shared_ptr<DeviceStatusManager::ClientInfo> DeviceStatusManager::GetClientInfo(Type type, int32_t pid)
{
    DEV_HILOGD(SERVICE, "Enter");
    ActivityEvent event = EVENT_INVALID;
    ReportLatencyNs latency = ReportLatencyNs::Latency_INVALID;
    std::shared_ptr<ClientInfo> clientInfo = std::make_shared<ClientInfo>(event, latency);
    if (type == TYPE_INVALID || pid <= INVALID_PID) {
        DEV_HILOGE(SERVICE, "params are invalid");
        return clientInfo;
    }
    std::lock_guard<std::mutex> clientLock(clientInfoMutex_);
    auto it = clientInfos_.find(type);
    if (it == clientInfos_.end()) {
        DEV_HILOGE(SERVICE, "cannot find sensorId : %{public}d", type);
        return clientInfo;
    }
    auto pidIt = it->second.find(pid);
    if (pidIt == it->second.end()) {
        DEV_HILOGE(SERVICE, "cannot find pid : %{public}d", pid);
        return clientInfo;
    }
    clientInfo->SetEvent(pidIt->second->GetEvent());
    clientInfo->SetLatency(pidIt->second->GetLatency());
    DEV_HILOGD(SERVICE, "end");
    return clientInfo;
}

std::list<sptr<IRemoteDevStaCallback>> DeviceStatusManager::GetChannels(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (type == TYPE_INVALID) {
        DEV_HILOGE(SERVICE, "type is invalid");
        return {};
    }
    std::lock_guard<std::mutex> clientLock(clientInfoMutex_);
    auto clientIt = clientInfos_.find(type);
    if (clientIt == clientInfos_.end()) {
        DEV_HILOGE(SERVICE, "there is no channel belong to sensorId : %{public}d", type);
        return {};
    }
    std::list<sptr<IRemoteDevStaCallback>> afeChannel;
    for (const auto &clientInfoIt : clientIt->second) {
        std::lock_guard<std::mutex> channelLock(channelMutex_);
        auto channelIt = dataChannels_.find(clientInfoIt.first);
        if (channelIt == dataChannels_.end()) {
            continue;
        }
        afeChannel.push_back(channelIt->second);
    }
    return afeChannel;
}

bool DeviceStatusManager::CheckEnable(Type type, int32_t pid)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (type == TYPE_INVALID || pid <= INVALID_PID) {
        DEV_HILOGE(SERVICE, "params are invalid");
        return false;
    }
    std::lock_guard<std::mutex> clientLock(clientInfoMutex_);
    auto it = clientInfos_.find(type);
    if (it == clientInfos_.end()) {
        DEV_HILOGE(SERVICE, "type not exist, no need to clear it");
        return false;
    }
    auto pidIt = it->second.find(pid);
    if (pidIt == it->second.end()) {
        DEV_HILOGE(SERVICE, "pid not exist, no need to clear it");
        return false;
    }
    return pidIt->second->GetEnabled();
}

int32_t DeviceStatusManager::UpdateDataChannel(int32_t pid, sptr<IRemoteDevStaCallback> callback)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (pid < INVALID_PID) {
        DEV_HILOGE(SERVICE, "Invalid pid");
        return RET_ERR;
    }
    std::lock_guard<std::mutex> channelLock(channelMutex_);
    auto iter = dataChannels_.find(pid);
    if (iter == dataChannels_.end()) {
        auto ret = dataChannels_.insert(std::make_pair(pid, callback));
        DEV_HILOGD(SERVICE, "ret.second:%{public}d", ret.second);
        if (!ret.second) {
            DEV_HILOGE(SERVICE, "Failed to insert");
            return RET_ERR;
        }
    }
    dataChannels_[pid] = callback;
    return RET_OK;
}

int32_t DeviceStatusManager::DestoryDataChannel(int32_t pid, sptr<IRemoteDevStaCallback> callback)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (pid < INVALID_PID) {
        DEV_HILOGE(SERVICE, "Invalid pid");
        return RET_ERR;
    }

    std::lock_guard<std::mutex> clientLock(clientInfoMutex_);
    for (auto it = clientInfos_.begin(); it != clientInfos_.end();) {
        auto pidIt = it->second.find(pid);
        if (pidIt == it->second.end()) {
            it++;
            continue;
        }
        pidIt = it->second.erase(pidIt);
        if (it->second.size() != 0) {
            it++;
            continue;
        }
        it = clientInfos_.erase(it);
    }

    std::lock_guard<std::mutex> channelLock(channelMutex_);
    auto iter = dataChannels_.find(pid);
    if (iter == dataChannels_.end()) {
        DEV_HILOGE(SERVICE, "Failed to finc channel");
        return RET_ERR;
    }
    dataChannels_.erase(iter);
    return RET_OK;
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

ActivityEvent DeviceStatusManager::ClientInfo::GetEvent() const
{
    return event_;
}

ReportLatencyNs DeviceStatusManager::ClientInfo::GetLatency() const
{
    return latency_;
}

void DeviceStatusManager::ClientInfo::SetEvent(ActivityEvent event)
{
    event_ = event;
}

void DeviceStatusManager::ClientInfo::SetLatency(ReportLatencyNs latency)
{
    latency_ = latency;
}

void DeviceStatusManager::ClientInfo::SetEnabled(bool enabled)
{
    enabled_ = enabled;
}

bool DeviceStatusManager::ClientInfo::GetEnabled() const
{
    return enabled_;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
