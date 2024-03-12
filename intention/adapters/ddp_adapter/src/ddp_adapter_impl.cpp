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
#include "distributed_device_profile_client.h"
#include "json_parser.h"
#include "parameter.h"
#include "utility.h"

#undef LOG_TAG
#define LOG_TAG "DDPAdapterImpl"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
using namespace OHOS::DistributedDeviceProfile;
const std::string SERVICE_ID { "deviceStatus" };
const std::string SERVICE_TYPE { "deviceStatus" };
const std::string CROSSING_SWITCH_STATE { "crossingSwitchState" };
const std::string CHARACTERISTIC_VALUE { "characteristicValue" };
constexpr int32_t DEVICE_STATUS_SA_ID { 2902 };
constexpr int32_t DEVICE_UDID_LENGTH { 65 };
constexpr int32_t DP_ERROR_CODE_DATA_EXIST { 98566164 };
} // namespace

#define DDP_CLIENT  DistributedDeviceProfile::DistributedDeviceProfileClient::GetInstance()

void DDPAdapterImpl::OnProfileChanged(const std::string &networkId)
{
    std::lock_guard guard(mutex_);
    FI_HILOGI("Profile of \'%{public}s\' has changed", Utility::Anonymize(networkId));
    for (const auto &item : observers_) {
        std::shared_ptr<IDeviceProfileObserver> observer = item.Lock();
        if (observer != nullptr) {
            FI_HILOGD("Notify profile change: \'%{public}s\'", Utility::Anonymize(networkId));
            observer->OnProfileChanged(networkId);
        }
    }
}

void DDPAdapterImpl::AddObserver(std::shared_ptr<IDeviceProfileObserver> observer)
{
    CALL_DEBUG_ENTER;
    std::lock_guard guard(mutex_);
    CHKPV(observer);
    observers_.erase(Observer());
    observers_.emplace(observer);
}

void DDPAdapterImpl::RemoveObserver(std::shared_ptr<IDeviceProfileObserver> observer)
{
    CALL_DEBUG_ENTER;
    std::lock_guard guard(mutex_);
    if (auto iter = observers_.find(Observer(observer)); iter != observers_.end()) {
        observers_.erase(iter);
    }
    observers_.erase(Observer());
}

void DDPAdapterImpl::AddWatch(const std::string &networkId, const std::string &udId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard guard(mutex_);
    FI_HILOGD("Add watch \'%{public}s\'", Utility::Anonymize(networkId));
    RegisterProfileListener(networkId);
    networkId2UdId_[udId] = networkId;
    udId2NetworkId_[networkId] = udId;
}

void DDPAdapterImpl::RemoveWatch(const std::string &networkId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard guard(mutex_);
    FI_HILOGD("Remove watch \'%{public}s\'", Utility::Anonymize(networkId));
    if (networkId2UdId_.find(networkId) != networkId2UdId_.end()) {
        udId2NetworkId_.erase(networkId2UdId_[networkId]);
    }
    networkId2UdId_.erase(networkId);
    UnregisterProfileListener(networkId);
}

int32_t DDPAdapterImpl::RegisterProfileListener(const std::string &networkId)
{
    CALL_DEBUG_ENTER;
    SubscribeInfo subscribeInfo;
    subscribeInfo.SetSaId(DEVICE_STATUS_SA_ID);
    std::string udid = GetUdIdByNetworkId(networkId);
    subscribeInfo.SetSubscribeKey(udid, SERVICE_ID, CROSSING_SWITCH_STATE, CHARACTERISTIC_VALUE);
    subscribeInfo.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_ADD);
    subscribeInfo.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_UPDATE);
    subscribeInfo.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_DELETE);
    sptr<IProfileChangeListener> subscribeDPChangeListener =
        new (std::nothrow) SubscribeDPChangeListener(shared_from_this());
    CHKPR(subscribeDPChangeListener, RET_ERR);
    subscribeInfo.SetListener(subscribeDPChangeListener);
    if (int32_t ret = DDP_CLIENT.SubscribeDeviceProfile(subscribeInfo) != RET_OK) {
        FI_HILOGE("SubscribeDeviceProfile failed, ret:%{public}d, udid:%{public}s",
            ret, Utility::Anonymize(udid));
        return RET_ERR;
    }
    crossingSwitchSubscribeInfo_.emplace(networkId, subscribeInfo);
    return RET_OK;
}

int32_t DDPAdapterImpl::UnregisterProfileListener(const std::string &networkId)
{
    CALL_DEBUG_ENTER;
    FI_HILOGD("Unregister profile listener for \'%{public}s\'", Utility::Anonymize(networkId));
    if (crossingSwitchSubscribeInfo_.find(networkId) == crossingSwitchSubscribeInfo_.end()) {
        FI_HILOGE("NetworkId:%{public}s is not founded in crossingSwitchSubscribeInfo",
            Utility::Anonymize(networkId));
        return RET_ERR;
    }
    auto subscribeInfo = crossingSwitchSubscribeInfo_[networkId];
    if (int32_t ret = DDP_CLIENT.UnSubscribeDeviceProfile(subscribeInfo) != RET_OK) {
        FI_HILOGE("UnSubscribeDeviceProfile failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    crossingSwitchSubscribeInfo_.erase(networkId);
    return RET_OK;
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
    std::string udid = GetUdIdByNetworkId(networkId);
    DistributedDeviceProfile::CharacteristicProfile profile;
    if (int32_t ret = DDP_CLIENT.GetCharacteristicProfile(udid, SERVICE_ID, CROSSING_SWITCH_STATE, profile)
        != RET_OK) {
        FI_HILOGE("GetCharacteristicProfile failed, ret:%{public}d, udid:%{public}s",
            ret, Utility::Anonymize(udid));
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
    std::string localUdid = GetLocalUdid();
    if (!isServiceProfileExist_) {
        DistributedDeviceProfile::ServiceProfile serviceProfile;
        serviceProfile.SetDeviceId(localUdid);
        serviceProfile.SetServiceName(SERVICE_ID);
        serviceProfile.SetServiceType(SERVICE_TYPE);
        int32_t ret = DDP_CLIENT.PutServiceProfile(serviceProfile);
        if (ret != RET_OK && ret != DP_ERROR_CODE_DATA_EXIST) {
            FI_HILOGE("PutServiceProfile failed, ret:%{public}d", ret);
            return RET_ERR;
        }
        FI_HILOGI("PutServiceProfile successfully, ret:%{public}d", ret);
        isServiceProfileExist_ = true;
    }
    DistributedDeviceProfile::CharacteristicProfile characteristicProfile;
    characteristicProfile.SetDeviceId(localUdid);
    characteristicProfile.SetServiceName(SERVICE_ID);
    characteristicProfile.SetCharacteristicKey(CROSSING_SWITCH_STATE);
    characteristicProfile.SetCharacteristicValue(sProfile);
    if (int32_t ret = DDP_CLIENT.PutCharacteristicProfile(characteristicProfile) != RET_OK) {
        FI_HILOGE("PutCharacteristicProfile failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

std::string DDPAdapterImpl::GetNetworkIdByUdId(const std::string &udid)
{
    if (udId2NetworkId_.find(udid) == udId2NetworkId_.end()) {
        FI_HILOGE("Udid:%{public}s is not founded in udId2NetworkId", Utility::Anonymize(udid));
        return {};
    }
    return udId2NetworkId_[udid];
}

std::string DDPAdapterImpl::GetUdIdByNetworkId(const std::string &networkId)
{
    if (networkId2UdId_.find(networkId) == networkId2UdId_.end()) {
        FI_HILOGE("NetworkId:%{public}s is not founded in networkId2UdId", Utility::Anonymize(networkId));
        return {};
    }
    return networkId2UdId_[networkId];
}

std::string DDPAdapterImpl::GetLocalUdid()
{
    char localDeviceId[DEVICE_UDID_LENGTH];
    if (GetDevUdid(localDeviceId, DEVICE_UDID_LENGTH) != RET_OK) {
        FI_HILOGE("GetDevUdid failed");
    }
    std::string localUdid { localDeviceId };
    FI_HILOGD("LocalUdid:%{public}s", Utility::Anonymize(localUdid));
    return localUdid;   
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
    FI_HILOGW("OnDeviceProfileAdd deviceId:%{public}s", Utility::Anonymize(profile.GetDeviceId()));
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnDeviceProfileDelete(const DeviceProfile &profile)
{
    FI_HILOGW("OnDeviceProfileDelete, deviceId:%{public}s", Utility::Anonymize(profile.GetDeviceId()));
    return RET_OK;
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnDeviceProfileUpdate(const DeviceProfile &oldProfile,
    const DeviceProfile &newProfile)
{
    FI_HILOGW("OnDeviceProfileUpdate, oldDeviceId:%{public}s, newDeviceId:%{public}s",
        Utility::Anonymize(oldProfile.GetDeviceId()), Utility::Anonymize(newProfile.GetDeviceId()));
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
    return OnProfileChanged(profile);
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnCharacteristicProfileDelete(
    const CharacteristicProfile &profile)
{
    return OnProfileChanged(profile);
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnCharacteristicProfileUpdate(
    const CharacteristicProfile &oldProfile, const CharacteristicProfile &newProfile)
{
    return OnProfileChanged(newProfile);
}

int32_t DDPAdapterImpl::SubscribeDPChangeListener::OnProfileChanged(const CharacteristicProfile &profile)
{
    CALL_INFO_TRACE;
    auto udid = profile.GetDeviceId();
    // std::string networkId = GetNetworkIdByUdId(udid);
    std::string networkId;
    if (networkId.empty()) {
        FI_HILOGE("Invalid networkId");
        return RET_ERR;
    }
    std::shared_ptr<DDPAdapterImpl> ddp = ddp_.lock();
    ddp->OnProfileChanged(networkId);
    return RET_OK;
}

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
