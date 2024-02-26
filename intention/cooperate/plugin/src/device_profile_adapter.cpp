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

#include "device_profile_adapter.h"

#include <algorithm>
#include <mutex>

#include "distributed_device_profile_client.h"

#include "cooperate_util.h"
#include "devicestatus_define.h"
#include "json_parser.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace OHOS::DistributedDeviceProfile;
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DeviceProfileAdapter" };
const std::string SERVICE_ID { "deviceStatus" };
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
    characteristicProfile.SetCharacteristicKey("currentStatus");
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
    characteristicProfile.SetCharacteristicKey("currentStatus");
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
    std::string remoteDeviceId = udid;
    DistributedDeviceProfile::DistributedDeviceProfileClient::GetInstance().GetCharacteristicProfile(remoteDeviceId,
        SERVICE_ID, "currentStatus", profile);
    std::string profileValue = profile.GetCharacteristicValue();
    return profileValue == "1" ? true : false;
}
 
int32_t DeviceProfileAdapter::RegisterCrossingStateListener(const std::string &networkId, DPCallback callback)
        return RET_ERR;
    }
    std::lock_guard<std::mutex> guard(adapterLock_);

    int32_t ret = RegisterProfileListener(networkId);
    if (ret != RET_OK) {
        FI_HILOGE("Register profile listener failed");
    }
    dpCallback_ = callback;
    return ret;
}
 
int32_t DeviceProfileAdapter::UnregisterCrossingStateListener(const std::string &networkId)
{
    CALL_INFO_TRACE;
    int32_t unSubscribeRes = DistributedDeviceProfileClient::GetInstance().UnSubscribeDeviceProfile(subscribeInfo_);
    FI_HILOGE("UnregisterCrossingStateListener unsubscribeRes %d", unSubscribeRes);
    return unSubscribeRes;

}
 
int32_t DeviceProfileAdapter::RegisterProfileListener(const std::string &networkId)
{
    CALL_INFO_TRACE;
    int32_t saId = 2902;
    std::string remoteUdid = COORDINATION::GetUdidByNetworkId(networkId);
    if (subscribeDPChangeListener_ == nullptr) {
        subscribeDPChangeListener_ = new(std::nothrow) SubscribeDPChangeListener;
    }
    subscribeInfo_.SetSaId(saId);
    subscribeInfo_.SetSubscribeKey(remoteUdid, SERVICE_ID, "currentStatus", "characteristicKey");
    subscribeInfo_.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_ADD);
    subscribeInfo_.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_UPDATE);
    subscribeInfo_.AddProfileChangeType(ProfileChangeType::CHAR_PROFILE_DELETE);
    subscribeInfo_.SetListener(subscribeDPChangeListener_);
    int32_t subscribeRes = DistributedDeviceProfileClient::GetInstance().SubscribeDeviceProfile(subscribeInfo_);
    FI_HILOGE("RegisterProfileListener subscribeRes %d", subscribeRes);
    return subscribeRes;
}
 
void DeviceProfileAdapter::OnProfileChanged(const std::string &udid)
{
    std::lock_guard<std::mutex> guard(adapterLock_);
    auto state = GetCrossingSwitchState(udid);
    std::string networkId = COOR_SM->onlineDeviceMap_[udid];
    dpCallback_(networkId, state);
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
