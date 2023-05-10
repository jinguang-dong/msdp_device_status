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

    // class ContinueMissionCallback final : public IRemoteStub<IRemoteObject> {
    // public:
    //     DISALLOW_COPY_AND_MOVE(ContinueMissionCallback);
    //     ContinueMissionCallback() = default;
    //     virtual ~ContinueMissionCallback() = default;
    //     int32_t OnRemoteRequest(uint32_t code, MessageParcel &data,
    //         MessageParcel &reply, MessageOption &option) override {}
    // };

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