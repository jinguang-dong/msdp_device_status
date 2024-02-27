/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "cooperate_util.h"
#include "devicestatus_define.h"
#include "distributed_device_profile_client.h"
#include "json_parser.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace OHOS::DistributedDeviceProfile;
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DeviceProfileAdapter" };
constexpr int32_t SAID { 2902 };
const std::string SERVICE_ID { "deviceStatus" };
const std::string CURRENT_STATUS { "currentStatus" };
} // namespace

DeviceProfileAdapter::DeviceProfileAdapter() {}

DeviceProfileAdapter::~DeviceProfileAdapter() {}

int32_t DeviceProfileAdapter::UpdateCrossingSwitchState(bool state, const std::vector<std::string> &deviceIds)
{
    CALL_INFO_TRACE;
    DistributedDeviceProfile::ServiceProfile serviceProfile;
    serviceProfile.SetDeviceId(COORDINATION::GetLocalUdid());
    serviceProfile.SetServiceName(SERVICE_ID);
    serviceProfile.SetServiceType(SERVICE_ID);
    DistributedDeviceProfile::DistributedDeviceProfileClient::GetInstance().PutServiceProfile(serviceProfile);
    DistributedDeviceProfile::CharacteristicProfile characteristicProfile;
    characteristicProfile.SetDeviceId(COORDINATION::GetLocalUdid());
    characteristicProfile.SetServiceName(SERVICE_ID);
    characteristicProfile.SetCharacteristicKey(CURRENT_STATUS);
    characteristicProfile.SetCharacteristicValue(std::to_string(state));
    DistributedDeviceProfile::DistributedDeviceProfileClient::GetInstance().
        PutCharacteristicProfile(characteristicProfile);
    DpSyncOptions dpsyncOptions;
    std::for_each(deviceIds.begin(), deviceIds.end(),
                  [&dpsyncOptions](auto &networkId) {
                      dpsyncOptions.AddDevice(networkId);
                      FI_HILOGI("Add device success");
                  });
    sptr<ISyncCompletedCallback> syncCallback = new(std::nothrow) SyncCallback;
    if (syncCallback == nullptr){
        FI_HILOGE("syncCallback is nullptr");
        return RET_ERR;
    }
    int32_t syncRes = DistributedDeviceProfileClient::GetInstance().SyncDeviceProfile(dpsyncOptions, syncCallback);
    FI_HILOGE("DeviceOnlineNotify SyncResult %{public}d", syncRes);
    return syncRes;
}
 
int32_t DeviceProfileAdapter::UpdateCrossingSwitchState(bool state)
{
    CALL_INFO_TRACE;
    DistributedDeviceProfile::ServiceProfile serviceProfile;
    serviceProfile.SetDeviceId(COORDINATION::GetLocalUdid());
    serviceProfile.SetServiceName(SERVICE_ID);
    serviceProfile.SetServiceType(SERVICE_ID);
    DistributedDeviceProfile::DistributedDeviceProfileClient::GetInstance().PutServiceProfile(serviceProfile);
    DistributedDeviceProfile::CharacteristicProfile characteristicProfile;
    characteristicProfile.SetDeviceId(COORDINATION::GetLocalUdid());
    characteristicProfile.SetServiceName(SERVICE_ID);
    characteristicProfile.SetCharacteristicKey(CURRENT_STATUS);
    characteristicProfile.SetCharacteristicValue(std::to_string(state));
    DistributedDeviceProfile::DistributedDeviceProfileClient::GetInstance().
        PutCharacteristicProfile(characteristicProfile);
    return DistributedDeviceProfile::DistributedDeviceProfileClient::GetInstance().
        PutCharacteristicProfile(characteristicProfile);
}
 
bool DeviceProfileAdapter::GetCrossingSwitchState(const std::string &udid)
{
    CALL_INFO_TRACE;
    DistributedDeviceProfile::CharacteristicProfile profile;
    DistributedDeviceProfile::DistributedDeviceProfileClient::GetInstance().GetCharacteristicProfile(udid,
        SERVICE_ID, CURRENT_STATUS, profile);
    std::string profileValue = profile.GetCharacteristicValue();
    return profileValue == "1" ? true : false;
}
 
int32_t DeviceProfileAdapter::RegisterCrossingStateListener(const std::string &networkId, DPCallback callback)
{
    std::lock_guard<std::mutex> guard(adapterLock_);

    if (RegisterProfileListener(networkId) != RET_OK) {
        FI_HILOGE("Register profile listener failed");
        return RET_ERR;
    }
    dpCallback_ = callback;
    return RET_OK;
}
 
int32_t DeviceProfileAdapter::UnregisterCrossingStateListener(const std::string &networkId)
{
    CALL_INFO_TRACE;
    int32_t unSubscribeRes = DistributedDeviceProfileClient::GetInstance().UnSubscribeDeviceProfile(subscribeInfo_);
    FI_HILOGE("UnregisterCrossingStateListener unsubscribeRes:%{public}d", unSubscribeRes);
    return unSubscribeRes;
}
 
int32_t DeviceProfileAdapter::RegisterProfileListener(const std::string &networkId)
{
    CALL_INFO_TRACE;
    std::string remoteUdid = COORDINATION::GetUdidByNetworkId(networkId);
    if (subscribeDPChangeListener_ == nullptr) {
        subscribeDPChangeListener_ = new(std::nothrow) SubscribeDPChangeListener;
    }
    subscribeInfo_.SetSaId(SAID);
    subscribeInfo_.SetSubscribeKey(remoteUdid, SERVICE_ID, CURRENT_STATUS, "characteristicKey");
    subscribeInfo_.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_ADD);
    subscribeInfo_.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_UPDATE);
    subscribeInfo_.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_DELETE);
    subscribeInfo_.SetListener(subscribeDPChangeListener_);
    int32_t subscribeRes = DistributedDeviceProfileClient::GetInstance().SubscribeDeviceProfile(subscribeInfo_);
    FI_HILOGE("RegisterProfileListener subscribeRes:%{public}d", subscribeRes);
    return subscribeRes;
}
 
void DeviceProfileAdapter::OnProfileChanged(const std::string &udid)
{
    std::lock_guard<std::mutex> guard(adapterLock_);
    auto state = GetCrossingSwitchState(udid);
    std::string networkId = COOR_SM->onlineDeviceMap_[udid];
    dpCallback_(networkId, state);
}
void DeviceProfileAdapter::SyncCallback::OnSyncCompleted(const std::map<std::string,
    OHOS::DistributedDeviceProfile::SyncStatus> &syncResults)
{
    for (const auto &item : syncResults) {
        FI_HILOGE("networkId: %{public}s, SyncStatus:%{public}d", item.first.c_str(), item.second);
    }
}
DeviceProfileAdapter::SubscribeDPChangeListener::SubscribeDPChangeListener()
{
    FI_HILOGE("Constructor");
}
DeviceProfileAdapter::SubscribeDPChangeListener::~SubscribeDPChangeListener()
{
    FI_HILOGE("Destructor");
}
int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnTrustDeviceProfileAdd(const TrustDeviceProfile &profile)
{
    FI_HILOGE("OnTrustDeviceProfileAdd");
return RET_OK;
}
int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnTrustDeviceProfileDelete(const TrustDeviceProfile &profile)
{
    FI_HILOGE("OnTrustDeviceProfileDelete");
    return RET_OK;
}
int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnTrustDeviceProfileUpdate(
    const TrustDeviceProfile &oldProfile, const TrustDeviceProfile &newProfile)
{
    FI_HILOGE("OnTrustDeviceProfileUpdate");
    return RET_OK;
}
int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnDeviceProfileAdd(const DeviceProfile &profile)
{
    FI_HILOGE("OnDeviceProfileAdd deviceId:%{public}s", profile.GetDeviceId().c_str());
    return RET_OK;
}
int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnDeviceProfileDelete(const DeviceProfile &profile)
{
    FI_HILOGE("OnDeviceProfileDelete, deviceId:%{public}s", profile.GetDeviceId().c_str());
    return RET_OK;
}
int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnDeviceProfileUpdate(const DeviceProfile &oldProfile,
    const DeviceProfile &newProfile)
{
    FI_HILOGE("OnDeviceProfileUpdate, oldDeviceId:%{public}s, newDeviceId:%{public}s",
        oldProfile.GetDeviceId().c_str(), newProfile.GetDeviceId().c_str());
    return RET_OK;
}
int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnServiceProfileAdd(const ServiceProfile &profile)
{
    FI_HILOGE("OnServiceProfileAdd");
    return RET_OK;
}
int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnServiceProfileDelete(const ServiceProfile &profile)
{
    FI_HILOGE("OnServiceProfileDelete");
    return RET_OK;
}
int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnServiceProfileUpdate(const ServiceProfile &oldProfile,
    const ServiceProfile& newProfile)
{
    FI_HILOGE("OnServiceProfileUpdate");
    return RET_OK;
}
int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnCharacteristicProfileAdd(
    const CharacteristicProfile &profile)
{
    FI_HILOGE("OnCharacteristicProfileAdd");
    std::string udid = profile.GetDeviceId();
    DP_ADAPTER->OnProfileChanged(udid);
    return RET_OK;
}
int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnCharacteristicProfileDelete(
    const CharacteristicProfile &profile)
{
    FI_HILOGE("OnCharacteristicProfileDelete");
    std::string udid = profile.GetDeviceId();
    DP_ADAPTER->OnProfileChanged(udid);
    return RET_OK;
}
int32_t DeviceProfileAdapter::SubscribeDPChangeListener::OnCharacteristicProfileUpdate(
    const CharacteristicProfile &oldProfile, const CharacteristicProfile &newProfile)
{
    FI_HILOGE("OnCharacteristicProfileUpdate");
    std::string udid = newProfile.GetDeviceId();
    DP_ADAPTER->OnProfileChanged(udid);
    return RET_OK;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
