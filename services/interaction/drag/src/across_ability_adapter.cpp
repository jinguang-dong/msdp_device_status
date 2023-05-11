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

#include "across_ability_adapter.h"

#include "ability_manager_errors.h"
#include "want.h"

#include "devicestatus_define.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "AcrossAbilityAdapter" };
} // namespace

AcrossAbilityAdapter *AcrossAbilityAdapter::instance_ = new (std::nothrow) AcrossAbilityAdapter();

AcrossAbilityAdapter *AcrossAbilityAdapter::GetInstance()
{
    return instance_;
}

void AcrossAbilityAdapter::MissionListenerCallback::NotifyMissionsChanged(const std::string& deviceId)
{
    CALL_DEBUG_ENTER;
    FI_HILOGD("deviceId:%{public}s", deviceId.c_str());
    if (AcrossAbilityAdapter::GetInstance()->UpdateMissionInfos(deviceId) != RET_OK) {
        FI_HILOGE("UpdateMissionInfos failed");
    }
    AcrossAbilityAdapter::GetInstance()->PrintCurrentMissionInfo();
}

void AcrossAbilityAdapter::MissionListenerCallback::NotifySnapshot(const std::string& deviceId, int32_t missionId)
{
    FI_HILOGD("deviceId:%{public}s, missionId:%{public}d", deviceId.c_str(), missionId);
}

void AcrossAbilityAdapter::MissionListenerCallback::NotifyNetDisconnect(const std::string& deviceId, int32_t state)
{
    FI_HILOGD("deviceId:%{public}s, state:%{public}d", deviceId.c_str(), state);
}

int32_t AcrossAbilityAdapter::RegisterMissionListener(const std::string &deviceId)
{
    CALL_DEBUG_ENTER;
    FI_HILOGD("deviceId:%{public}s", deviceId.c_str());
    sptr<AAFwk::IRemoteMissionListener> listener = new (std::nothrow) AcrossAbilityAdapter::MissionListenerCallback();
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->Connect();
    if (ret != ERR_OK) {
        FI_HILOGE("Connect sa failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    ret = AAFwk::AbilityManagerClient::GetInstance()->RegisterMissionListener(deviceId, listener);
    if (ret != ERR_OK) {
        FI_HILOGE("RegisterMissionListener failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    bool fixConflict = true; // 该参数含义上不清晰
    int64_t tag = 0; // 该参数具体是干啥的
    if (AAFwk::AbilityManagerClient::GetInstance()->StartSyncRemoteMissions(deviceId, fixConflict, tag) != ERR_OK) {
        FI_HILOGE("StartSyncRemoteMissions failed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t AcrossAbilityAdapter::UnRegisterMissionListener(const std::string &deviceId)
{
    CALL_DEBUG_ENTER;
    FI_HILOGD("deviceId:%{public}s", deviceId.c_str());
    sptr<AAFwk::IRemoteMissionListener> listener = new (std::nothrow) AcrossAbilityAdapter::MissionListenerCallback();
    int32_t ret = AAFwk::AbilityManagerClient::GetInstance()->UnRegisterMissionListener(deviceId, listener);
    if (ret != ERR_OK) {
        FI_HILOGE("UnRegisterMissionListener failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

int32_t AcrossAbilityAdapter::UpdateMissionInfos(const std::string &deviceId)
{
    CALL_DEBUG_ENTER;
    int32_t ret = AAFwk::AbilityManagerClient::GetInstance()->GetMissionInfos(deviceId, MAX_MISSION_NUM, missionInfos_);
    if (ret != ERR_OK) {
        FI_HILOGE("GetMissionInfos failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

int32_t AcrossAbilityAdapter::ContinueMission(const AAFwk::MissionInfo &missionInfo)
{
    CALL_DEBUG_ENTER;
    if (!missionInfo.continuable) {
        FI_HILOGE("ContinueMission failed, this mission is unsupported to continue");
        return RET_ERR;
    }
    AAFwk::WantParams wantParams = missionInfo.want.GetParams();
    sptr<IRemoteObject> callback = new (std::nothrow) AcrossAbilityAdapter::ContinueMissionCallback();
    if (int32_t ret = AAFwk::AbilityManagerClient::GetInstance()->ContinueMission(
        srcDeviceId_, dstDeviceId_, missionInfo.id, callback, wantParams); ret != ERR_OK) {
        FI_HILOGE("ContinueMission failed, %{public}d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

AAFwk::MissionInfo AcrossAbilityAdapter::GetMissionInfoToContinue(const std::string &bundleName, const std::string &abilityName)
{
    CALL_DEBUG_ENTER;
    auto iter = find_if(missionInfos_.begin(), missionInfos_.end(),
        [&bundleName, &abilityName](const auto &missionInfo) {
            return missionInfo.want.GetElement().GetBundleName() == bundleName &&
                   missionInfo.want.GetElement().GetAbilityName() == abilityName;
        }
    );
    return (iter != missionInfos_.cend() ? *iter : AAFwk::MissionInfo());
}

void AcrossAbilityAdapter::LaunchAbility(const std::string &deviceId, const std::string &bundleName,
    const std::string &abilityName)
{
    CALL_DEBUG_ENTER;
    AAFwk::Want want;
    want.SetElementName(deviceId, bundleName, abilityName);
    FI_HILOGD("Start launch ability, bundleName:%{public}s", bundleName.c_str());
    int32_t err = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    if (err != ERR_OK) {
        FI_HILOGE("LaunchAbility failed, bundleName:%{public}s, err:%{public}d", bundleName.c_str(), err);
    }
}

void AcrossAbilityAdapter::PrintCurrentMissionInfo()
{
    CALL_DEBUG_ENTER;
    FI_HILOGD("MissionNum:%{public}d", missionInfos_.size());
    for (const auto& missionInfo : missionInfos_) {
        FI_HILOGD("MissionId:%{public}d, label:%{public}s, bundleName:%{public}s, abilityName:%{public}s",
            missionInfo.id, missionInfo.label.c_str(),
            missionInfo.want.GetElement().GetBundleName().c_str(),
            missionInfo.want.GetElement().GetAbilityName().c_str()
        );
    }
}

int32_t AcrossAbilityAdapter::ContinueFirstMission()
{
    CALL_DEBUG_ENTER;
    if (missionInfos_.empty()) {
        FI_HILOGE("No remote missionInfo");
        return RET_ERR;
    }
    if (ContinueMission(missionInfos_[0]) != RET_OK) {
        FI_HILOGE("ContinueMission failed");
        return RET_ERR;
    }
    return RET_OK;
}

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS