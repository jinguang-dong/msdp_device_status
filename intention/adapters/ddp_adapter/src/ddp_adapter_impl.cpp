/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ddp_adapter_impl.h"

#include "devicestatus_define.h"
#include "json_parser.h"
#include "utility.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DDPAdapterImpl" };
const std::string SERVICE_ID { "deviceStatus" };
const std::string SERVICE_TYPE { "deviceStatus" };
const std::string CURRENT_STATUS { "currentStatus" };
const std::string CHARACTERISTIC_VALUE { "characteristicValue" };
constexpr int32_t DEVICE_STATUS_SA_ID { 2902 };
const std::string PKG_NAME_PREFIX { "DBinderBus_Dms_" };
} // namespace

int32_t DDPAdapterImpl::OnProfileChanged(const CharacteristicProfile &profile)
{
    std::lock_guard guard(mutex_);
    std::string udid = profile.GetDeviceId();
    std::string networkId = GetNetworkIdByUdid(udid);
    if (networkId.empty()) {
        FI_HILOGE("Invalid networkId");
        return RET_ERR;
    }
    if (switchListener_.find(networkId) == switchListener_.end()) {
        FI_HILOGE("NetworkId:%{public}s is not founded in SwitchListener", GetAnonyString(networkId).c_str());
        return RET_ERR;
    }
    auto switchListener = switchListener_[networkId];
    CHKPR(switchListener.dpCallback, RET_ERR);
    bool state = (profile.GetCharacteristicValue() == "true" ? true : false);
    switchListener.dpCallback(networkId, state);
    return RET_OK;
}

void DDPAdapterImpl::AddObserver(std::shared_ptr<IDeviceProfileObserver> observer)
{
    CALL_INFO_TRACE;
    std::lock_guard guard(mutex_);
    CHKPV(observer);
    observers_.erase(Observer());
    observers_.emplace(observer);
}

void DDPAdapterImpl::RemoveObserver(std::shared_ptr<IDeviceProfileObserver> observer)
{
    CALL_INFO_TRACE;
    std::lock_guard guard(mutex_);
    if (auto iter = observers_.find(Observer(observer)); iter != observers_.end()) {
        observers_.erase(iter);
    }
    observers_.erase(Observer());
}

void DDPAdapterImpl::AddWatch(const std::string &networkId)
{
    CALL_INFO_TRACE;
    std::lock_guard guard(mutex_);
    FI_HILOGD("Add watch \'%{public}s\'", Utility::Anonymize(networkId));
    RegisterProfileListener(networkId,dpCallback);
    siblings_.insert(networkId);
}

void DDPAdapterImpl::RemoveWatch(const std::string &networkId)
{
    CALL_INFO_TRACE;
    std::lock_guard guard(mutex_);
    FI_HILOGD("Remove watch \'%{public}s\'", Utility::Anonymize(networkId));
    siblings_.erase(networkId);
    UnregisterProfileListener(networkId);
}

int32_t DDPAdapterImpl::RegisterProfileListener(const std::string &networkId, Observer::DPCallback callback)
{
    CALL_DEBUG_ENTER;
    SubscribeInfo subscribeInfo;
    subscribeInfo.SetSaId(DEVICE_STATUS_SA_ID);
    std::string udid = GetUdidByNetworkId(networkId);
    subscribeInfo.SetSubscribeKey(udid, SERVICE_ID, CURRENT_STATUS, CHARACTERISTIC_VALUE);
    subscribeInfo.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_ADD);
    subscribeInfo.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_UPDATE);
    subscribeInfo.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_DELETE);
    sptr<IProfileChangeListener> subscribeDPChangeListener = new (std::nothrow) SubscribeDPChangeListener;
    CHKPR(subscribeDPChangeListener, RET_ERR);
    subscribeInfo.SetListener(subscribeDPChangeListener);
    if (int32_t ret = DDP_CLIENT.SubscribeDeviceProfile(subscribeInfo) != RET_OK) {
        FI_HILOGE("SubscribeDeviceProfile failed, ret:%{public}d, udid:%{public}s", ret, GetAnonyString(udid).c_str());
        return RET_ERR;
    }
    SwitchListener switchListener = {
    .subscribeInfo = subscribeInfo,
    .dpCallback = callback
    };
    switchListener_.emplace(networkId, switchListener);
    return RET_OK;
}


void DDPAdapterImpl::UnregisterProfileListener(const std::string &networkId)
{
    CALL_DEBUG_ENTER;
    FI_HILOGD("Unregister profile listener for \'%{public}s\'", Utility::Anonymize(networkId));
   if (switchListener_.find(networkId) == switchListener_.end()) {
        FI_HILOGE("NetworkId:%{public}s is not founded in switchListener", GetAnonyString(networkId).c_str());
    }
    auto switchListener = switchListener_[networkId];
    if (int32_t ret = DDP_CLIENT.UnSubscribeDeviceProfile(switchListener.subscribeInfo) != RET_OK) {
        FI_HILOGE("UnSubscribeDeviceProfile failed, ret:%{public}d", ret);
    }
    switchListener_.erase(networkId);
}

int32_t DDPAdapterImpl::GetProperty(const std::string &networkId, const std::string &name, bool &value)
{
    CALL_DEBUG_ENTER;
    return GetProperty(networkId, name, [&name, &value](cJSON *json) {
        FI_HILOGD("Get bool property: %{public}s", name.c_str());
        if (cJSON_IsBool(json)) {
            value = cJSON_IsTrue(json);
        } else if (cJSON_IsNumber(json)) {
            value = (static_cast<int32_t>(json->valuedouble) != 0);
        } else {
            FI_HILOGE("Unexpected data type");
            return RET_ERR;
        }
        return RET_OK;
    });
}

int32_t DDPAdapterImpl::GetProperty(const std::string &networkId, const std::string &name, int32_t &value)
{
    CALL_DEBUG_ENTER;
    return GetProperty(networkId, name, [&name, &value](cJSON *json) {
        FI_HILOGD("Get integer property: %{public}s", name.c_str());
        if (!cJSON_IsNumber(json)) {
            FI_HILOGE("Unexpected data type");
            return RET_ERR;
        }
        value = static_cast<int32_t>(json->valuedouble);
        return RET_OK;
    });
}

int32_t DDPAdapterImpl::GetProperty(const std::string &networkId, const std::string &name, std::string &value)
{
    CALL_DEBUG_ENTER;
    return GetProperty(networkId, name, [&name, &value](cJSON *json) {
        FI_HILOGD("Get string property: %{public}s", name.c_str());
        if (!cJSON_IsString(json) && !cJSON_IsRaw(json)) {
            FI_HILOGE("Unexpected data type");
            return RET_ERR;
        }
        CHKPR(json->valuestring, RET_ERR);
        value = json->valuestring;
        return RET_OK;
    });
}

int32_t DDPAdapterImpl::GetProperty(const std::string &networkId, const std::string &name,
    std::function<int32_t(cJSON *json)> parse)
{
    CALL_DEBUG_ENTER;
    DistributedDeviceProfile::CharacteristicProfile profile;
    std::string udid = "";
    udid = DDP_CLIENT.GetUuidByNetworkId(networkId);
    int32_t ret = DDP_CLIENT.GetCharacteristicProfile(udid, SERVICE_ID, SERVICE_ID , profile);
    if (ret != RET_OK) {
        FI_HILOGE("DP::GetCharacteristicProfile failed");
        return RET_ERR;
    }
    std::string jsonData = profile.GetCharacteristicValue();
    JsonParser parser;
    parser.json = cJSON_Parse(jsonData.c_str());
    if (!cJSON_IsObject(parser.json)) {
        FI_HILOGE("Unexpected data format");
        return RET_ERR;
    }
    cJSON* jsonValue = cJSON_GetObjectItem(parser.json, name.c_str());
    if (jsonValue == nullptr) {
        FI_HILOGE("Item \'%{public}s\' not found", name.c_str());
        return RET_ERR;
    }
    return parse(jsonValue);
}

int32_t DDPAdapterImpl::SetProperty(const std::string &name, bool value)
{
    DPValue dpVal(std::in_place_type<bool>, value);
    return SetProperty(name, dpVal);
}

int32_t DDPAdapterImpl::SetProperty(const std::string &name, int32_t value)
{
    DPValue dpVal(std::in_place_type<int32_t>, value);
    return SetProperty(name, dpVal);
}

int32_t DDPAdapterImpl::SetProperty(const std::string &name, const std::string &value)
{
    DPValue dpVal(std::in_place_type<std::string>, value);
    return SetProperty(name, dpVal);
}

int32_t DDPAdapterImpl::SetProperty(const std::string &name, const DPValue &value)
{
    CALL_DEBUG_ENTER;
    if (auto iter = properties_.find(name); iter != properties_.end()) {
        if (iter->second == value) {
            return RET_OK;
        }
        iter->second = value;
    } else {
        properties_.emplace(name, value);
    }

    PutProfile();
    return RET_OK;
}
std::string DDPAdapterImpl::GetCurrentPackageName()
{
    return PKG_NAME_PREFIX + std::to_string(getpid());
}

std::string DDPAdapterImpl::GetUdidByNetworkId(const std::string &networkId)
{
    std::string udid { "" };
    if (GetUdidByNetworkId(GetCurrentPackageName(), networkId, udid) != RET_OK) {
        FI_HILOGE("GetUdidByNetworkId failed, networkId:%{public}s, udid:%{public}s",
            GetAnonyString(networkId).c_str(), GetAnonyString(udid).c_str());
    }
    return udid;
}

std::string DDPAdapterImpl::GetLocalUdid()
{
    auto packageName = GetCurrentPackageName();
    OHOS::DistributedHardware::DmDeviceInfo dmDeviceInfo;
    if (int32_t errCode = RET_OK; (errCode = GetLocalDeviceInfo(packageName, dmDeviceInfo)) != RET_OK) {
        FI_HILOGE("GetLocalBasicInfo failed, errCode:%{public}d", errCode);
        return {};
    }
    return GetUdidByNetworkId(dmDeviceInfo.networkId);
}
int32_t DDPAdapterImpl::PutProfile()
{
    CALL_DEBUG_ENTER;
    JsonParser parser;
    parser.json = cJSON_CreateObject();
    CHKPR(parser.json, RET_ERR);

    for (const auto &[name, value] : properties_) {
        JsonParser parser1;

        std::visit(
            [&parser1](const auto &arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr(std::is_same_v<T, bool>) {
                    parser1.json = cJSON_CreateNumber(arg);
                } else if constexpr(std::is_same_v<T, int32_t>) {
                    parser1.json = cJSON_CreateNumber(arg);
                } else if constexpr(std::is_same_v<T, std::string>) {
                    parser1.json = cJSON_CreateString(arg.c_str());
                }
            }, value);
        CHKPR(parser1.json, RET_ERR);

        if (!cJSON_AddItemToObject(parser.json, name.c_str(), parser1.json)) {
            FI_HILOGE("Failed to add \'%{public}s\' to object", name.c_str());
            return RET_ERR;
        }
        parser1.json = nullptr;
    }

    char *cProfile = cJSON_Print(parser.json);
    CHKPR(cProfile, RET_ERR);
    std::string sProfile(cProfile);
    cJSON_free(cProfile);

    DistributedDeviceProfile::CharacteristicProfile profile;
    profile.SetDeviceId(SERVICE_ID);
    profile.SetDeviveType(SERVICE_TYPE);
    profile.SetCharacteristicValue(sProfile);
    int32_t ret = DDP_CLIENT.PutDeviceProfile(profile);
    if (ret != RET_OK) {
        FI_HILOGE("DP::PutDeviceProfile fail");
        return RET_ERR;
    }
    return RET_OK;
}
DDPAdapterImpl::SubscribeDPChangeListener::SubscribeDPChangeListener()
{
    FI_HILOGW("Constructor");
}

DDPAdapterImpl::SubscribeDPChangeListener::~SubscribeDPChangeListener()
{
    FI_HILOGW("Destructor");
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnTrustDeviceProfileAdd(const TrustDeviceProfile &profile)
{
    FI_HILOGW("OnTrustDeviceProfileAdd");
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnTrustDeviceProfileDelete(const TrustDeviceProfile &profile)
{
    FI_HILOGW("OnTrustDeviceProfileDelete");
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnTrustDeviceProfileUpdate(
    const TrustDeviceProfile &oldProfile, const TrustDeviceProfile &newProfile)
{
    FI_HILOGW("OnTrustDeviceProfileUpdate");
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnDeviceProfileAdd(const DeviceProfile &profile)
{
    FI_HILOGW("OnDeviceProfileAdd deviceId:%{public}s", GetAnonyString(profile.GetDeviceId()).c_str());
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnDeviceProfileDelete(const DeviceProfile &profile)
{
    FI_HILOGW("OnDeviceProfileDelete, deviceId:%{public}s", GetAnonyString(profile.GetDeviceId()).c_str());
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnDeviceProfileUpdate(const DeviceProfile &oldProfile,
    const DeviceProfile &newProfile)
{
    FI_HILOGW("OnDeviceProfileUpdate, oldDeviceId:%{public}s, newDeviceId:%{public}s",
        GetAnonyString(oldProfile.GetDeviceId()).c_str(), GetAnonyString(newProfile.GetDeviceId()).c_str());
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnServiceProfileAdd(const ServiceProfile &profile)
{
    FI_HILOGW("OnServiceProfileAdd");
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnServiceProfileDelete(const ServiceProfile &profile)
{
    FI_HILOGW("OnServiceProfileDelete");
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnServiceProfileUpdate(const ServiceProfile &oldProfile,
    const ServiceProfile& newProfile)
{
    FI_HILOGW("OnServiceProfileUpdate");
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnCharacteristicProfileAdd(
    const CharacteristicProfile &profile)
{
    CALL_INFO_TRACE;
    std::string udid = profile.GetDeviceId();
    DDP_CLIENT.OnProfileChanged(udid);
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnCharacteristicProfileDelete(
    const CharacteristicProfile &profile)
{
    CALL_INFO_TRACE;
    std::string udid = profile.GetDeviceId();
    DDP_CLIENT.OnProfileChanged(udid);
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnCharacteristicProfileUpdate(
    const CharacteristicProfile &oldProfile, const CharacteristicProfile &newProfile)
{
    CALL_INFO_TRACE;
    std::string udid = newProfile.GetDeviceId();
    DDP_CLIENT.OnProfileChanged(udid);
    return RET_OK;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
