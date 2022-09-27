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

#include "motion_manager.h"

#include "motion_msdp_client_impl.h"

namespace OHOS {
namespace Msdp {
void MotionManager::MotionCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "OnRemoteDied failed, remote is nullptr");
        return;
    }
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "Recv death notice");
}

bool MotionManager::Init()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MotionManager: Init start");
    if (motionCBDeathRecipient_ == nullptr) {
        motionCBDeathRecipient_ = new MotionCallbackDeathRecipient();
    }

    msdpImpl_ = std::make_unique<MotionMsdpClientImpl>();
    if (msdpImpl_ == nullptr) {
        return false;
    }

    LoadAlgorithm();
    motionClientImplCallback_ = std::bind(&MotionManager::MsdpDataCallback, this, std::placeholders::_1);

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MotionManager: Init success");
    return true;
}

MotionDataUtils::MotionData MotionManager::GetLatestMotionData(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    MotionDataUtils::MotionData data = {type, MotionDataUtils::MotionValue::VALUE_EXIT};

    msdpData_ = msdpImpl_->GetObserverData();
    for (auto iter = msdpData_.begin(); iter != msdpData_.end(); ++iter) {
        if (data.type == iter->first) {
            data.value = iter->second;
            return data;
        }
    }

    data.value = MotionDataUtils::MotionValue::VALUE_INVALID;
    return data;
}

bool MotionManager::Enable(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGE(MOTION_MODULE_SERVICE, "enter");
    if (msdpImpl_ == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "msdpImpl_ is nullptr");
        return false;
    }
    if (msdpImpl_->EnableMsdpImpl(type) == -1) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "init msdp impl failed");
        return false;
    };
    return true;
}

bool MotionManager::Disable(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    if (msdpImpl_ == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "disable rdb failed, msdpImpl is nullptr");
        return false;
    }

    if (msdpImpl_->DisableMsdpImpl(type) == -1) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "disable msdp impl failed");
        return false;
    }
    return true;
}

bool MotionManager::DisableCount(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGE(MOTION_MODULE_SERVICE, "enter");
    msdpImpl_->DisableMsdpImplCount(type);
    return true;
}

bool MotionManager::RegisterCallbak(MotionMsdpClientImpl::CallbackManager& motionClientImplCallback)
{
    MOTION_HILOGE(MOTION_MODULE_SERVICE, "enter");
    if (msdpImpl_ == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "msdpImpl_ is nullptr");
        return false;
    }
    if (msdpImpl_->RegisterImpl(motionClientImplCallback) == -1) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "register impl failed");
    }
    return true;
}

bool MotionManager::UnRegisterCallbak()
{
    MOTION_HILOGE(MOTION_MODULE_SERVICE, "enter");
    if (msdpImpl_ == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "msdpImpl is nullptr");
        return false;
    }

    if (msdpImpl_->UnregisterImpl() == -1) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "unregister impl failed");
        return false;
    }
    return true;
}

int32_t MotionManager::MsdpDataCallback(MotionDataUtils::MotionData& data)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");

    NotifyMotionChange(data);
    return ERR_OK;
}

int32_t MotionManager::SensorDataCallback(const struct SensorEvents *event)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    // TO-DO, handle sensor event properly when we get the data details of sensor HDI.
    MotionDataUtils::MotionData data = {MotionDataUtils::MotionType::TYPE_PICKUP,
        MotionDataUtils::MotionValue::VALUE_ENTER};
    NotifyMotionChange(data);
    return ERR_OK;
}

void MotionManager::NotifyMotionChange(const MotionDataUtils::MotionData& motionData)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "type:%{public}d",motionData.type);
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "value:%{public}d",motionData.value);
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Status:%{public}d",motionData.status);
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Action:%{public}d",motionData.action);
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "move:%{public}f",motionData.move);

    // Call back for all listeners
    std::set<const sptr<ImotionCallback>, classcomp> listeners;
    bool isExists = false;
    listenerMap_.empty();

    for (auto it = listenerMap_.begin(); it != listenerMap_.end(); ++it) {
        //
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "listener type:%{public}d",static_cast<int32_t>(it->first));
        //
        if (it->first == motionData.type) {
            isExists = true;
            listeners = (std::set<const sptr<ImotionCallback>, classcomp>)(it->second);
            break;
        }
    }
    if (!isExists) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "No listener found for type: %{public}d", motionData.type);
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "Exit");
        return;
    }
    for (auto& listener : listeners) {
        listener->OnMotionChanged(motionData);
    }
}

void MotionManager::Subscribe(const MotionDataUtils::MotionType& type,
    const sptr<ImotionCallback>& callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    MOTION_RETURN_IF(callback == nullptr);
    auto object = callback->AsObject();
    MOTION_RETURN_IF(object == nullptr);
    if (!Enable(type)) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "Enable failed!");
        return;
    }
    if (!RegisterCallbak(motionClientImplCallback_)) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "RegisterCallbak failed!");
        return;
    }
    std::set<const sptr<ImotionCallback>, classcomp> listeners;
    std::lock_guard lock(mutex_);
    auto iter = listenerMap_.find(type);
    if (iter == listenerMap_.end()) {
        auto retIt = listeners.insert(callback);
        if (retIt.second) {
            object->AddDeathRecipient(motionCBDeathRecipient_);
        }
        //
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "Subscribe type:%{public}d",static_cast<int32_t>(type));
        //
        listenerMap_.insert(std::make_pair(type, listeners));
    } else {
        if (iter->second.insert(callback).second) {
            object->AddDeathRecipient(motionCBDeathRecipient_);
        }
    }
}

void MotionManager::UnSubscribe(const MotionDataUtils::MotionType& type, const sptr<ImotionCallback>& callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    std::lock_guard lock(mutex_);
    MOTION_RETURN_IF(callback == nullptr);
    auto object = callback->AsObject();
    MOTION_RETURN_IF(object == nullptr);

    auto callbackIter = listenerMap_.find(type);
    if (callbackIter == listenerMap_.end()) {
        return;
    }

    size_t eraseNum = callbackIter->second.erase(callback);
    if (eraseNum != 0) {
        object->RemoveDeathRecipient(motionCBDeathRecipient_);
    }

    if (callbackIter->second.size() == 0) {
        listenerMap_.erase(callbackIter);
    }

    DisableCount(type);

    if (listenerMap_.empty()) {
        if (!Disable(type)) {
            MOTION_HILOGE(MOTION_MODULE_SERVICE, "Disable failed!");
            return;
        }
        if (!UnRegisterCallbak()) {
            MOTION_HILOGE(MOTION_MODULE_SERVICE, "UnRegisterCallbak failed!");
            return;
        }
    }
}

int32_t MotionManager::LoadAlgorithm()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    if (msdpImpl_ != nullptr) {
        msdpImpl_->LoadAlgorithmLibrary();
        msdpImpl_->LoadSensorHdiLibrary();
    }

    return ERR_OK;
}

int32_t MotionManager::UnloadAlgorithm()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    if (msdpImpl_ != nullptr) {
        msdpImpl_->UnloadAlgorithmLibrary();
        msdpImpl_->UnloadSensorHdiLibrary();
    }

    return ERR_OK;
}
} // namespace Msdp
} // namespace OHOS
