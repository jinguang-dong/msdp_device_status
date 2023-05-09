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

#include <vector>

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
    DECLARE_SINGLETON(AcrossAbilityAdapter);

public:
    DISALLOW_MOVE(AcrossAbilityAdapter);
    class MissionListenerCallback final : public AAFwk::RemoteMissionListenerStub {
    public:
        void NotifyMissionsChanged(const std::string& deviceId)override;
        void NotifySnapshot(const std::string& deviceId, int32_t missionId)override;
        void NotifyNetDisconnect(const std::string& deviceId, int32_t state)override;
    };

    int32_t RegisterMissionListener(const std::string &deviceId);
    int32_t UnRegisterMissionListener(const std::string &deviceId);
    int32_t UpdateMissionInfos(const std::string &deviceId);
    int32_t ContinueMission(const std::string &srcDeviceId, const std::string &dstDeviceId, int32_t missionId,
        const sptr<IRemoteObject> &callback, AAFwk::WantParams &wantParams);
    int32_t GetMissionIdToContinue(const std::string &bundleName, const std::string &abilityName);
    void LaunchAbility(const std::string &deviceId, const std::string &bundleName, const std::string &abilityName);
    void PrintCurrentMissionInfo();
private:
    std::vector<AAFwk::MissionInfo> missionInfos_;
};

#define AcrossDragAbilityAdapter ::OHOS::Singleton<AcrossAbilityAdapter>::GetInstance()

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // ACROSS_ABILITY_ADAPTER