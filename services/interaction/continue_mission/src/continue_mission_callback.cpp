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

#include "continue_mission_callback.h"
#include "devicestatus_define.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {


namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "ContinueMissionCallback" };
} // namespace

int32_t ContinueMissionCallback::StartRemoteAbility(const OHOS::AAFwk::Want& want,
    int32_t callerUid, int32_t requestCode, uint32_t accessToken)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::StartAbilityFromRemote(const OHOS::AAFwk::Want& want,
    const OHOS::AppExecFwk::AbilityInfo& abilityInfo, int32_t requestCode, const DistributedSchedule::CallerInfo& callerInfo,
    const AccountInfo& accountInfo)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::SendResultFromRemote(OHOS::AAFwk::Want& want, int32_t requestCode,
    const DistributedSchedule::CallerInfo& callerInfo, const AccountInfo& accountInfo, int32_t resultCode)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::ContinueMission(const std::string& srcDeviceId, const std::string& dstDeviceId,
    int32_t missionId, const sptr<IRemoteObject>& callback, const OHOS::AAFwk::WantParams& wantParams)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::StartContinuation(const OHOS::AAFwk::Want& want, int32_t missionId,
    int32_t callerUid, int32_t status, uint32_t accessToken)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

void ContinueMissionCallback::NotifyCompleteContinuation(const std::u16string& devId, int32_t sessionId, bool isSuccess)
{
    CALL_DEBUG_ENTER;
    (void)isSuccess;
}

int32_t ContinueMissionCallback::NotifyContinuationResultFromRemote(int32_t sessionId, bool isSuccess)
{
    CALL_DEBUG_ENTER;
    (void)isSuccess;
    return RET_OK;
}

int32_t ContinueMissionCallback::ConnectRemoteAbility(const OHOS::AAFwk::Want& want, const sptr<IRemoteObject>& connect,
    int32_t callerUid, int32_t callerPid, uint32_t accessToken)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::DisconnectRemoteAbility(const sptr<IRemoteObject>& connect, int32_t callerUid,
    uint32_t accessToken)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::ConnectAbilityFromRemote(const OHOS::AAFwk::Want& want,
    const AppExecFwk::AbilityInfo& abilityInfo, const sptr<IRemoteObject>& connect,
    const DistributedSchedule::CallerInfo& callerInfo, const AccountInfo& accountInfo)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::DisconnectAbilityFromRemote(const sptr<IRemoteObject>& connect,
    int32_t uid, const std::string& sourceDeviceId)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::NotifyProcessDiedFromRemote(const DistributedSchedule::CallerInfo& callerInfo)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

#ifdef SUPPORT_DISTRIBUTED_MISSION_MANAGER
int32_t ContinueMissionCallback::StartSyncRemoteMissions(const std::string& devId, bool fixConflict, int64_t tag)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::RegisterMissionListener(const std::u16string& devId, const sptr<IRemoteObject>& obj)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::UnRegisterMissionListener(const std::u16string& devId,
    const sptr<IRemoteObject>& obj)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::GetRemoteMissionSnapshotInfo(const std::string& networkId, int32_t missionId,
    std::unique_ptr<MissionSnapshot>& missionSnapshot)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::NotifyMissionsChangedFromRemote(const std::vector<DstbMissionInfo>& missionInfos,
    const DistributedSchedule::CallerInfo& callerInfo)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::GetMissionInfos(const std::string& deviceId, int32_t numMissions,
    std::vector<MissionInfo>& missionInfos)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::StartSyncMissionsFromRemote(const DistributedSchedule::CallerInfo& callerInfo,
    std::vector<DstbMissionInfo>& missionInfos)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::StopSyncMissionsFromRemote(const DistributedSchedule::CallerInfo& callerInfo)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::StopSyncRemoteMissions(const std::string& devId)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}
#endif

int32_t ContinueMissionCallback::StartRemoteAbilityByCall(const OHOS::AAFwk::Want& want,
    const sptr<IRemoteObject>& connect, int32_t callerUid, int32_t callerPid, uint32_t accessToken)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::ReleaseRemoteAbility(const sptr<IRemoteObject>& connect,
    const AppExecFwk::ElementName &element)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::StartAbilityByCallFromRemote(const OHOS::AAFwk::Want& want,
    const sptr<IRemoteObject>& connect, const DistributedSchedule::CallerInfo& callerInfo, const AccountInfo& accountInfo)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::ReleaseAbilityFromRemote(const sptr<IRemoteObject>& connect,
    const AppExecFwk::ElementName &element, const DistributedSchedule::CallerInfo& callerInfo)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::NotifyStateChangedFromRemote(int32_t abilityState, int32_t missionId,
    const AppExecFwk::ElementName& element)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::GetDistributedComponentList(std::vector<std::string>& distributedComponents)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::StartRemoteFreeInstall(const OHOS::AAFwk::Want &want, int32_t callerUid,
    int32_t requestCode, uint32_t accessToken, const sptr<IRemoteObject> &callback)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::StartFreeInstallFromRemote(const IDistributedSched::FreeInstallInfo &info, int64_t taskId)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::NotifyCompleteFreeInstallFromRemote(int64_t taskId, int32_t resultCode)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::NotifyCompleteFreeInstall(
    const IDistributedSched::FreeInstallInfo &info, int64_t taskId, int32_t resultCode)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

#ifdef SUPPORT_DISTRIBUTED_FORM_SHARE
int32_t ContinueMissionCallback::StartRemoteShareForm(const std::string& remoteDeviceId,
    const OHOS::AppExecFwk::FormShareInfo& formShareInfo)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

int32_t ContinueMissionCallback::StartShareFormFromRemote(
    const std::string& remoteDeviceId, const OHOS::AppExecFwk::FormShareInfo& formShareInfo)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}
#endif
int32_t ContinueMissionCallback::StopRemoteExtensionAbility(
    const OHOS::AAFwk::Want& want, int32_t callerUid, uint32_t accessToken, int32_t extensionType)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}
int32_t ContinueMissionCallback::StopExtensionAbilityFromRemote(const OHOS::AAFwk::Want& want,
    const DistributedSchedule::CallerInfo& callerInfo, const AccountInfo& accountInfo, int32_t extensionType)
{
    CALL_DEBUG_ENTER;
    return RET_OK;
}

} // namespace DeviceStatus
} // Msdp
} // namespace OHOS