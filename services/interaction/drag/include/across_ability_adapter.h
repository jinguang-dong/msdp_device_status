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

#ifndef ACROSS_ABILITY_ADAPTER
#define ACROSS_ABILITY_ADAPTER

#include <cstddef>
#include <cstdint>
#include <vector>
#include <sys/types.h>

#include "ability_manager_client.h"
#include "distributed_sched_stub.h"
#include "iremote_stub.h"
#include "nocopyable.h"
#include "remote_mission_listener_stub.h"
#include "singleton.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr int32_t MAX_MISSION_NUM { 1000 };
}
class AcrossAbilityAdapter final {
public:
    static AcrossAbilityAdapter *GetInstance();
    virtual ~AcrossAbilityAdapter() = default;

    class MissionListenerCallback final : public AAFwk::RemoteMissionListenerStub {
    public:
        void NotifyMissionsChanged(const std::string& deviceId)override;
        void NotifySnapshot(const std::string& deviceId, int32_t missionId)override;
        void NotifyNetDisconnect(const std::string& deviceId, int32_t state)override;
    };

    class ContinueMissionCallback : public DistributedSchedule::DistributedSchedStub {
    public:
        explicit ContinueMissionCallback()
        {
        }

        int32_t StartRemoteAbility(const OHOS::AAFwk::Want& want, int32_t callerUid, int32_t requestCode,
            uint32_t accessToken) override { return 0; }
        int32_t StartAbilityFromRemote(const OHOS::AAFwk::Want& want,
            const OHOS::AppExecFwk::AbilityInfo& abilityInfo, int32_t requestCode, const AAFwk::CallerInfo& callerInfo,
            const AccountInfo& accountInfo) override { return 0; }
        int32_t SendResultFromRemote(OHOS::AAFwk::Want& want, int32_t requestCode,
            const AAFwk::CallerInfo& callerInfo, const AccountInfo& accountInfo, int32_t resultCode) override { return 0; }
        int32_t ContinueMission(const std::string& srcDeviceId, const std::string& dstDeviceId,
            int32_t missionId, const sptr<IRemoteObject>& callback, const OHOS::AAFwk::WantParams& wantParams) override { return 0; }
        int32_t StartContinuation(const OHOS::AAFwk::Want& want, int32_t missionId, int32_t callerUid,
            int32_t status, uint32_t accessToken) override { return 0; }
        void NotifyCompleteContinuation(const std::u16string& devId, int32_t sessionId, bool isSuccess) override { }
        int32_t NotifyContinuationResultFromRemote(int32_t sessionId, bool isSuccess) override { return 0; }
        int32_t ConnectRemoteAbility(const OHOS::AAFwk::Want& want, const sptr<IRemoteObject>& connect,
            int32_t callerUid, int32_t callerPid, uint32_t accessToken) override { return 0; }
        int32_t DisconnectRemoteAbility(const sptr<IRemoteObject>& connect, int32_t callerUid,
            uint32_t accessToken) override { return 0; }
        int32_t ConnectAbilityFromRemote(const OHOS::AAFwk::Want& want, const AppExecFwk::AbilityInfo& abilityInfo,
            const sptr<IRemoteObject>& connect, const AAFwk::CallerInfo& callerInfo, const AccountInfo& accountInfo) override { return 0; }
        int32_t DisconnectAbilityFromRemote(const sptr<IRemoteObject>& connect,
            int32_t uid, const std::string& sourceDeviceId) override { return 0; }
        int32_t NotifyProcessDiedFromRemote(const AAFwk::CallerInfo& callerInfo) override { return 0; }
    #ifdef SUPPORT_DISTRIBUTED_MISSION_MANAGER
        int32_t StartSyncRemoteMissions(const std::string& devId, bool fixConflict, int64_t tag) override { return 0; }
        int32_t StartSyncMissionsFromRemote(const AAFwk::CallerInfo& callerInfo,
            std::vector<DstbMissionInfo>& missionInfos) override { return 0; }
        int32_t StopSyncRemoteMissions(const std::string& devId) override { return 0; }
        int32_t StopSyncMissionsFromRemote(const AAFwk::CallerInfo& callerInfo) override { return 0; }
        int32_t RegisterMissionListener(const std::u16string& devId, const sptr<IRemoteObject>& obj) override { return 0; }
        int32_t UnRegisterMissionListener(const std::u16string& devId, const sptr<IRemoteObject>& obj) override { return 0; }
        int32_t GetMissionInfos(const std::string& deviceId, int32_t numMissions,
            std::vector<AAFwk::MissionInfo>& missionInfos) override { return 0; }
        int32_t GetRemoteMissionSnapshotInfo(const std::string& networkId, int32_t missionId,
            std::unique_ptr<AAFwk::MissionSnapshot>& missionSnapshot) override { return 0; }
        int32_t NotifyMissionsChangedFromRemote(const std::vector<DstbMissionInfo>& missionInfos,
            const AAFwk::CallerInfo& callerInfo) override { return 0; }
    #endif
        int32_t StartRemoteAbilityByCall(const OHOS::AAFwk::Want& want, const sptr<IRemoteObject>& connect,
            int32_t callerUid, int32_t callerPid, uint32_t accessToken) override { return 0; }
        int32_t ReleaseRemoteAbility(const sptr<IRemoteObject>& connect,
            const AppExecFwk::ElementName &element) override { return 0; }
        int32_t StartAbilityByCallFromRemote(const OHOS::AAFwk::Want& want, const sptr<IRemoteObject>& connect,
            const AAFwk::CallerInfo& callerInfo, const AccountInfo& accountInfo) override { return 0; }
        int32_t ReleaseAbilityFromRemote(const sptr<IRemoteObject>& connect, const AppExecFwk::ElementName &element,
            const AAFwk::CallerInfo& callerInfo) override { return 0; }
    #ifdef SUPPORT_DISTRIBUTED_FORM_SHARE
        int32_t StartRemoteShareForm(const std::string& remoteDeviceId,
            const OHOS::AppExecFwk::FormShareInfo& formShareInfo) override { return 0; }
        int32_t StartShareFormFromRemote(
            const std::string& remoteDeviceId, const OHOS::AppExecFwk::FormShareInfo& formShareInfo) override { return 0; }
    #endif
        int32_t NotifyStateChangedFromRemote(int32_t abilityState, int32_t missionId,
            const AppExecFwk::ElementName& element) override { return 0; }
        int32_t GetDistributedComponentList(std::vector<std::string>& distributedComponents) override { return 0; }
        int32_t StopRemoteExtensionAbility(
            const OHOS::AAFwk::Want& want, int32_t callerUid, uint32_t accessToken, int32_t extensionType) override { return 0; }
        int32_t StopExtensionAbilityFromRemote(const OHOS::AAFwk::Want& want,
            const AAFwk::CallerInfo& callerInfo, const AccountInfo& accountInfo, int32_t extensionType) override { return 0; }
    };

    int32_t RegisterMissionListener(const std::string &deviceId);
    int32_t UnRegisterMissionListener(const std::string &deviceId);
    int32_t UpdateMissionInfos(const std::string &deviceId);
    int32_t ContinueMission(const AAFwk::MissionInfo &missionInfo);
    AAFwk::MissionInfo GetMissionInfoToContinue(const std::string &bundleName, const std::string &abilityName);
    void LaunchAbility(const std::string &deviceId, const std::string &bundleName, const std::string &abilityName);
    void PrintCurrentMissionInfo();
    int32_t ContinueFirstMission();

private:
    std::vector<AAFwk::MissionInfo> missionInfos_;
    std::string srcDeviceId_;
    std::string dstDeviceId_;
    AcrossAbilityAdapter() = default;
    DISALLOW_COPY_AND_MOVE(AcrossAbilityAdapter);
    static AcrossAbilityAdapter *instance_;
};

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // ACROSS_ABILITY_ADAPTER