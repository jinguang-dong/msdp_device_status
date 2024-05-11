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

#ifndef COOPERATE_HISYSEVENT_H
#define COOPERATE_HISYSEVENT_H

#include <map>
#include <string>

#include "devicestatus_define.h"
#include "hisysevent.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr int32_t SUB_LEN { 6 };
constexpr int32_t INIT_SIGN { 0 };
constexpr int32_t SESS_SIGN { 1 };
constexpr int32_t STATUS_SIGN { 2 };
} //namespace
enum CooperateType : int32_t {
    ENABLE_SUCC = 0,
    ENABLE_FAIL = 1,
    DISENABLE_SUCC = 2,
    DISENABLE_FAIL = 3,
    ACTIVATE_SUCC0 = 4,
    ACTIVATE_FAIL0 = 5,
    ACTIVATE_SUCC1 = 6,
    ACTIVATE_FAIL1 = 7,
    DISACTIVATE_SUCC0 = 8,
    DISACTIVATE_FAIL0 = 9,
    DISACTIVATE_SUCC1 = 10,
    DISACTIVATE_FAIL1 = 11,
    OPENSESSION_SUCC = 12,
    OPENSESSION_SUCC = 13,
    DEACTIVATE_SUCC = 14,
    DEACTIVATE_FAIL = 15,
    DEACTIVATE_RESULT = 16,
    COOP_DRAG_SUCC = 32,
    COOP_DRAG_FAIL = 33,
    COOP_DRAG_RESULT_SUCC = 34,
    COOP_DRAG_RESULT_FAIL = 35,
};
enum CooperateState : size_t {
    COOPRERATE_STATE_FREE = 0,
    COOPRERATE_STATE_OUT,
    COOPRERATE_STATE_IN,
    N_COOPERATE_STATUS,
}

class CooperateDFX {
public:

    static int32_t WriteEnable(OHOS::HiviewDFX::HiSysEvent::EventType type);
    static int32_t WriteDisenable(OHOS::HiviewDFX::HiSysEvent::EventType type);
    static int32_t WriteLocalStart(OHOS::HiviewDFX::HiSysEvent::EventType type);    
    static int32_t WriteLocalStop(OHOS::HiviewDFX::HiSysEvent::EventType type);
    static int32_t WriteRemoteStart(OHOS::HiviewDFX::HiSysEvent::EventType type);
    static int32_t WriteRemoteStop(OHOS::HiviewDFX::HiSysEvent::EventType type);
    static int32_t WriteOpenSession(OHOS::HiviewDFX::HiSysEvent::EventType type);
    static int32_t WriteDeactivate(const std::string &remoteNetworkId, std::map<std::string, int32_t> sessionDevMap_,
        OHOS::HiviewDFX::HiSysEvent::EventType type);
    static int32_t WriteDeactivateResult(const std::string &remoteNetworkId,
        std::map<std::string, int32_t> sessionDevMap_);
    static int32_t WriteCooperateDrag(const std::string &remoteNetworkId, CooperateState currentSta);
    static int32_t WriteCooperateDrag(const std::string &remoteNetworkId, CooperateState previousSta,
        CooperateState updateSta);
    static int32_t WriteCooperateDragResult(const std::string &remoteNetworkId, const CooperateState &currentSta,
        OHOS::HiviewDFX::HiSysEvent::EventType type);
    template<typename... Types>
    static int32_t WriteInputFunc(const CooperateType &cooperateType, Types... paras);

private:
    static std::map<CooperateState, std::string> CooperateState_;
    static std::map<CooperaeType, std::pair<std::string, std::string>> serialStr_;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // COOPERATE_HISYSEVENT_H
