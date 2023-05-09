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
    bool fixConflict = true; // 该参数含义上不清晰
    int64_t tag = 0; // 该参数具体是干啥的
    if (AAFwk::AbilityManagerClient::GetInstance()->StartSyncRemoteMissions(deviceId, fixConflict, tag) != ERR_OK) {
        FI_HILOGE("RegisterMissionListener failed");
        return;
    }
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

int32_t AcrossAbilityAdapter::ContinueMission(const std::string &srcDeviceId, const std::string &dstDeviceId,
    int32_t missionId, const sptr<IRemoteObject> &callback, AAFwk::WantParams &wantParams)
{
    CALL_DEBUG_ENTER;
    (void) srcDeviceId;
    (void) dstDeviceId;
    (void) missionId;
    (void) callback;
    (void) wantParams;
    return RET_OK;
}

int32_t AcrossAbilityAdapter::GetMissionIdToContinue(const std::string &bundleName, const std::string &abilityName)
{
    CALL_DEBUG_ENTER;
    (void) bundleName;
    (void) abilityName;
    return -1;
}

void AcrossAbilityAdapter::LaunchAbility(const std::string &deviceId, const std::string &bundleName,
    const std::string &abilityName)
{
    CALL_INFO_TRACE;
    AAFwk::Want want;
    want.SetElementName(deviceId, bundleName, abilityName);
    want.SetAction("");
    want.SetUri("");
    want.SetType("");
    for (const auto &entity : { "" }) {
        want.AddEntity(entity);
    }
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
    for (const auto& elem : missionInfos_) {
        FI_HILOGD("MissionId:%{public}d, label:%{public}s", elem.id, elem.label.c_str());
    }
}

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS