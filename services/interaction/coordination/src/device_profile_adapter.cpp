/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "device_profile_adapter.h"

#include <algorithm>
#include <mutex>

#include "coordination_sm.h"
#include "coordination_util.h"
#include "devicestatus_define.h"
#include "distributed_device_profile_client.h"
#include "json_parser.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace OHOS::DistributedDeviceProfile;
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DeviceProfileAdapter" };
const std::string SERVICE_ID { "deviceStatus" };
const std::string SERVICE_TYPE { "deviceStatus" };
const std::string CURRENT_STATUS { "currentStatus" };
constexpr int32_t DEVICE_STATUS_SA_ID { 2902 };
} // namespace

DeviceProfileAdapter::DeviceProfileAdapter() {}

DeviceProfileAdapter::~DeviceProfileAdapter() {}

int32_t DeviceProfileAdapter::UpdateCrossingSwitchState(bool state)
{
    CALL_INFO_TRACE;
    DistributedDeviceProfile::ServiceProfile serviceProfile;
    serviceProfile.SetDeviceId(COORDINATION::GetLocalUdid());
    serviceProfile.SetServiceName(SERVICE_ID);
    serviceProfile.SetServiceType(SERVICE_TYPE);
    if (DP_CLIENT.PutServiceProfile(serviceProfile) != RET_OK) {
        FI_HILOGE("PutServiceProfile failed");
        return RET_ERR;
    }

    DistributedDeviceProfile::CharacteristicProfile characteristicProfile;
    characteristicProfile.SetDeviceId(COORDINATION::GetLocalUdid());
    characteristicProfile.SetServiceName(SERVICE_ID);
    characteristicProfile.SetCharacteristicKey(CURRENT_STATUS);
    characteristicProfile.SetCharacteristicValue(std::to_string(state));
    if (DP_CLIENT.PutCharacteristicProfile(characteristicProfile) != RET_OK) {
        FI_HILOGE("PutCharacteristicProfile failed");
        return RET_ERR;
    }
    return RET_OK;
}

bool DeviceProfileAdapter::GetCrossingSwitchState(const std::string &networkId)
{
    CALL_INFO_TRACE;
    std::string udid = GetUdidByNetworkId(networkId);
    DistributedDeviceProfile::CharacteristicProfile profile;
    if (DP_CLIENT.GetCharacteristicProfile(udid, SERVICE_ID, CURRENT_STATUS, profile) != RET_OK) {
        FI_HILOGE("GetCharacteristicProfile failed");
    }
    return (profile.GetCharacteristicValue() == "1" ? true : false);
}

int32_t DeviceProfileAdapter::RegisterCrossingStateListener(const std::string &networkId, DPCallback callback)
{
    CALL_INFO_TRACE;
    std::lock_guard<std::mutex> guard(adapterLock_);
    if (RegisterProfileListener(networkId, callback) != RET_OK) {
        FI_HILOGE("RegisterProfileListener failed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t DeviceProfileAdapter::UnregisterCrossingStateListener(const std::string &networkId)
{
    CALL_INFO_TRACE;
    std::lock_guard<std::mutex> guard(adapterLock_);
    if (UnregisterProfileListener(networkId) != RET_OK) {
        FI_HILOGE("UnregisterProfileListener failed");
        return RET_ERR;
    }
    return RET_OK;
}

void DeviceProfileAdapter::OnDeviceOnline(const std::string &networkId, const std::string &udid)
{
    CALL_INFO_TRACE;
    std::lock_guard<std::mutex> guard(adapterLock_);
    onlineDevUdid2NetworkId_.emplace(udid, networkId);
    onlineDevNetworkId2Udid_.emplace(networkId, udid);
}

void DeviceProfileAdapter::OnDeviceOffline(const std::string &networkId, const std::string &udid)
{
    CALL_INFO_TRACE;
    std::lock_guard<std::mutex> guard(adapterLock_);
    if (onlineDevUdid2NetworkId_.find(udid) != onlineDevUdid2NetworkId_.end()) {
        onlineDevUdid2NetworkId_.erase(udid);
    }
    if (onlineDevNetworkId2Udid_.find(networkId) != onlineDevNetworkId2Udid_.end()) {
        onlineDevNetworkId2Udid_.erase(networkId);
    }
}

int32_t DeviceProfileAdapter::RegisterProfileListener(const std::string &networkId, DPCallback callback)
{
    CALL_INFO_TRACE;
    SubscribeInfo subscribeInfo;
    subscribeInfo.SetSaId(DEVICE_STATUS_SA_ID);
    std::string udid = GetUdidByNetworkId(networkId);
    subscribeInfo.SetSubscribeKey(udid, SERVICE_ID, CURRENT_STATUS, "characteristicKey");
    subscribeInfo.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_ADD);
    subscribeInfo.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_UPDATE);
    subscribeInfo.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_DELETE);
    sptr<IProfileChangeListener> subscribeDPChangeListener = new (std::nothrow) SubscribeDPChangeListener;
    CHKPR(subscribeDPChangeListener, RET_ERR);
    subscribeInfo.SetListener(subscribeDPChangeListener);
    if (int32_t ret = DP_CLIENT.SubscribeDeviceProfile(subscribeInfo) != RET_OK) {
        FI_HILOGE("SubscribeDeviceProfile failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    CrossingSwitchListener switchListener =  {
        .subscribeInfo = subscribeInfo,
        .dpCallback = callback
    };
    crossingSwitchListener_.emplace(networkId, switchListener);
    return RET_OK;
}

int32_t DeviceProfileAdapter::UnregisterProfileListener(const std::string &networkId)
{
    if (crossingSwitchListener_.find(networkId) == crossingSwitchListener_.end()) {
        FI_HILOGE("UnregisterProfileListener failed, no subscribeListener stored in DeviceProfileAdapter");
        return RET_ERR;
    }
    auto switchListener = crossingSwitchListener_[networkId];
    if (int32_t ret = DP_CLIENT.UnSubscribeDeviceProfile(switchListener.subscribeInfo) != RET_OK) {
        FI_HILOGE("UnSubscribeDeviceProfile failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    crossingSwitchListener_.erase(networkId);
    return RET_OK;

}

std::string DeviceProfileAdapter::GetNetworkIdByUdid(const std::string &udid)
{
    if (onlineDevUdid2NetworkId_.find(udid) == onlineDevUdid2NetworkId_.end()) {
        FI_HILOGE("GetNetworkIdByUdid failed");
        return {};
    }
    return onlineDevUdid2NetworkId_[udid];
}

std::string DeviceProfileAdapter::GetUdidByNetworkId(const std::string &networkId)
{
    if (onlineDevNetworkId2Udid_.find(networkId) == onlineDevNetworkId2Udid_.end()) {
        FI_HILOGE("GetUdidByNetworkId failed");
        return {};
    }
    return onlineDevNetworkId2Udid_[networkId];
}

void DeviceProfileAdapter::OnProfileChanged(const std::string &udid)
{
    std::lock_guard<std::mutex> guard(adapterLock_);
    std::string networkId = GetNetworkIdByUdid(udid);
    if (networkId.empty()) {
        FI_HILOGE("Invalid networkId");
        return;
    }
    if (crossingSwitchListener_.find(networkId) == crossingSwitchListener_.end()) {
        FI_HILOGE("No crossingSwitchListener for networkId:%{public}s", AnonyNetworkId(networkId).c_str());
        return;
    }
    auto switchListener = crossingSwitchListener_[networkId];
    CHKPV(switchListener.dpCallback);
    switchListener.dpCallback(networkId, GetCrossingSwitchState(udid));
}

DeviceProfileAdapter::SubscribeDPChangeListener::SubscribeDPChangeListener()
{
    FI_HILOGW("Constructor");
}

DeviceProfileAdapter::SubscribeDPChangeListener::~SubscribeDPChangeListener()
{
    FI_HILOGW("Destructor");
}

int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnTrustDeviceProfileAdd(const TrustDeviceProfile &profile)
{
    FI_HILOGW("OnTrustDeviceProfileAdd");
    return RET_OK;
}

int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnTrustDeviceProfileDelete(const TrustDeviceProfile &profile)
{
    FI_HILOGW("OnTrustDeviceProfileDelete");
    return RET_OK;
}

int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnTrustDeviceProfileUpdate(
    const TrustDeviceProfile &oldProfile, const TrustDeviceProfile &newProfile)
{
    FI_HILOGW("OnTrustDeviceProfileUpdate");
    return RET_OK;
}

int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnDeviceProfileAdd(const DeviceProfile &profile)
{
    FI_HILOGW("OnDeviceProfileAdd deviceId:%{public}s", profile.GetDeviceId().c_str());
    return RET_OK;
}

int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnDeviceProfileDelete(const DeviceProfile &profile)
{
    FI_HILOGW("OnDeviceProfileDelete, deviceId:%{public}s", profile.GetDeviceId().c_str());
    return RET_OK;
}

int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnDeviceProfileUpdate(const DeviceProfile &oldProfile,
    const DeviceProfile &newProfile)
{
    FI_HILOGW("OnDeviceProfileUpdate, oldDeviceId:%{public}s, newDeviceId:%{public}s",
        oldProfile.GetDeviceId().c_str(), newProfile.GetDeviceId().c_str());
    return RET_OK;
}

int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnServiceProfileAdd(const ServiceProfile &profile)
{
    FI_HILOGW("OnServiceProfileAdd");
    return RET_OK;
}

int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnServiceProfileDelete(const ServiceProfile &profile)
{
    FI_HILOGW("OnServiceProfileDelete");
    return RET_OK;
}

int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnServiceProfileUpdate(const ServiceProfile &oldProfile,
    const ServiceProfile& newProfile)
{
    FI_HILOGW("OnServiceProfileUpdate");
    return RET_OK;
}

int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnCharacteristicProfileAdd(
    const CharacteristicProfile &profile)
{
    CALL_INFO_TRACE;
    std::string udid = profile.GetDeviceId();
    DP_ADAPTER->OnProfileChanged(udid);
    return RET_OK;
}

int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnCharacteristicProfileDelete(
    const CharacteristicProfile &profile)
{
    CALL_INFO_TRACE;
    std::string udid = profile.GetDeviceId();
    DP_ADAPTER->OnProfileChanged(udid);
    return RET_OK;
}

int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnCharacteristicProfileUpdate(
    const CharacteristicProfile &oldProfile, const CharacteristicProfile &newProfile)
{
    CALL_INFO_TRACE;
    std::string udid = newProfile.GetDeviceId();
    DP_ADAPTER->OnProfileChanged(udid);
    return RET_OK;
}

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
