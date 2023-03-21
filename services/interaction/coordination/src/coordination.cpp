/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "coordination.h"

#include "coordination_device_manager.h"
#include "coordination_event_manager.h"
#include "coordination_sm.h"
#include "coordination_util.h"
#include "distributed_input_adapter.h"
#include "proto.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "Coordination" };
} // namespace

int32_t Coordination::EnableCoordination(SessionPtr sess, int32_t userData, bool enabled)
{
    CALL_DEBUG_ENTER;
    CHKPR(sess, RET_ERR);
    CooSM->EnableCoordination(enabled);
    std::string deviceId;
    CoordinationMessage msg = enabled ? CoordinationMessage::OPEN_SUCCESS : CoordinationMessage::CLOSE_SUCCESS;
    NetPacket pkt(MessageId::COORDINATION_MESSAGE);
    pkt << userData << deviceId << static_cast<int32_t>(msg);
    if (pkt.ChkRWError()) {
        FI_HILOGE("Packet write data failed");
        return RET_ERR;
    }
    if (!sess->SendMsg(pkt)) {
        FI_HILOGE("Sending failed");
        return MSG_SEND_FAIL;
    }
    return RET_OK;
}

int32_t Coordination::StartCoordination(SessionPtr sess, int32_t userData, const std::string& sinkDeviceId,
    int32_t srcDeviceId)
{
    CALL_DEBUG_ENTER;
    CHKPR(sess, RET_ERR);
    sptr<CoordinationEventManager::EventInfo> event = new (std::nothrow) CoordinationEventManager::EventInfo();
    CHKPR(event, RET_ERR);
    event->type = CoordinationEventManager::EventType::START;
    event->sess = sess;
    event->msgId = MessageId::COORDINATION_MESSAGE;
    event->userData = userData;
    if (CooSM->GetCurrentCoordinationState() == CoordinationState::STATE_OUT) {
        FI_HILOGW("It is currently worn out");
        NetPacket pkt(event->msgId);
        pkt << userData << "" << static_cast<int32_t>(CoordinationMessage::INFO_SUCCESS);
        if (pkt.ChkRWError()) {
            FI_HILOGE("Packet write data failed");
            return RET_ERR;
        }
        if (!sess->SendMsg(pkt)) {
            FI_HILOGE("Sending failed");
            return RET_ERR;
        }
        return RET_OK;
    }
    CoordinationEventMgr->AddCoordinationEvent(event);
    int32_t ret = CooSM->StartCoordination(sinkDeviceId, srcDeviceId);
    if (ret != RET_OK) {
        FI_HILOGE("StartCoordination failed, ret:%{public}d", ret);
        CoordinationEventMgr->OnErrorMessage(event->type, CoordinationMessage(ret));
        return ret;
    }
    return RET_OK;
}

int32_t Coordination::StopCoordination(SessionPtr sess, int32_t userData)
{
    CALL_DEBUG_ENTER;
    CHKPR(sess, RET_ERR);
    sptr<CoordinationEventManager::EventInfo> event = new (std::nothrow) CoordinationEventManager::EventInfo();
    CHKPR(event, RET_ERR);
    event->type = CoordinationEventManager::EventType::STOP;
    event->sess = sess;
    event->msgId = MessageId::COORDINATION_MESSAGE;
    event->userData = userData;
    CoordinationEventMgr->AddCoordinationEvent(event);
    int32_t ret = CooSM->StopCoordination();
    if (ret != RET_OK) {
        FI_HILOGE("StopCoordination failed, ret:%{public}d", ret);
        CoordinationEventMgr->OnErrorMessage(event->type, CoordinationMessage(ret));
        return ret;
    }
    return RET_OK;
}

int32_t Coordination::GetCoordinationState(SessionPtr sess, int32_t userData, const std::string &deviceId)
{
    CALL_DEBUG_ENTER;
    CHKPR(sess, RET_ERR);
    sptr<CoordinationEventManager::EventInfo> event = new (std::nothrow) CoordinationEventManager::EventInfo();
    CHKPR(event, RET_ERR);
    event->type = CoordinationEventManager::EventType::STATE;
    event->sess = sess;
    event->msgId = MessageId::COORDINATION_GET_STATE;
    event->userData = userData;
    CoordinationEventMgr->AddCoordinationEvent(event);
    CooSM->GetCoordinationState(deviceId);
    return RET_OK;
}

int32_t Coordination::RegisterCoordinationListener(SessionPtr sess)
{
    CALL_DEBUG_ENTER;
    CHKPR(sess, RET_ERR);
    sptr<CoordinationEventManager::EventInfo> event = new (std::nothrow) CoordinationEventManager::EventInfo();
    CHKPR(event, RET_ERR);
    event->type = CoordinationEventManager::EventType::LISTENER;
    event->sess = sess;
    event->msgId = MessageId::COORDINATION_ADD_LISTENER;
    CoordinationEventMgr->AddCoordinationEvent(event);
    return RET_OK;
}

int32_t Coordination::UnregisterCoordinationListener(SessionPtr sess)
{
    CALL_DEBUG_ENTER;
    CHKPR(sess, RET_ERR);
    sptr<CoordinationEventManager::EventInfo> event = new (std::nothrow) CoordinationEventManager::EventInfo();
    CHKPR(event, RET_ERR);
    event->type = CoordinationEventManager::EventType::LISTENER;
    event->sess = sess;
    CoordinationEventMgr->RemoveCoordinationEvent(event);
    return RET_OK;
}

void Coordination::Dump(int32_t fd)
{
    CALL_DEBUG_ENTER;
    CooSM->Dump(fd);
}

void Coordination::OnSessionLost(SessionPtr session)
{
    CALL_DEBUG_ENTER;
    CooSM->OnSessionLost(session);
}

ICoordination* CreateCoordination(IContext *context)
{
    CALL_DEBUG_ENTER;
    CHKPP(context);
    CoordinationEventMgr->SetIContext(context);
    ICoordination *coor = new (std::nothrow) Coordination();
    CHKPP(coor);
    CooSM->Init();
    return coor;
}

void ReleaseCoordination(ICoordination* coor)
{
    CALL_DEBUG_ENTER;
    CooDevMgr->RemoveObserver();
    if (coor != nullptr) {
        delete coor;
    }
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS