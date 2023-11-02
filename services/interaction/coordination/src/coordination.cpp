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
#include "coordination_hotarea.h"
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

Coordination::Coordination()
{
    CALL_DEBUG_ENTER;
}

Coordination::~Coordination()
{
    CALL_DEBUG_ENTER;
}

void Coordination::PrepareCoordination()
{
    COOR_SM->PrepareCoordination();
}

void Coordination::UnprepareCoordination()
{
    COOR_SM->UnprepareCoordination();
}

int32_t Coordination::ActivateCoordination(SessionPtr sess, int32_t userData,
    const std::string& remoteNetworkId, int32_t startDeviceId)
{
    CALL_DEBUG_ENTER;
    CHKPR(sess, RET_ERR);
    sptr<CoordinationEventManager::EventInfo> event = new (std::nothrow) CoordinationEventManager::EventInfo();
    CHKPR(event, RET_ERR);
    event->type = CoordinationEventManager::EventType::START;
    event->sess = sess;
    event->msgId = MessageId::COORDINATION_MESSAGE;
    event->userData = userData;
    if (COOR_SM->GetCurrentCoordinationState() == CoordinationState::STATE_OUT ||
        (COOR_SM->GetCurrentCoordinationState() == CoordinationState::STATE_FREE &&
        COOR_DEV_MGR->IsRemote(startDeviceId))) {
        FI_HILOGW("It is currently worn out");
        NetPacket pkt(event->msgId);
        pkt << userData << "" << static_cast<int32_t>(CoordinationMessage::ACTIVATE_SUCCESS);
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
    COOR_EVENT_MGR->AddCoordinationEvent(event);
    int32_t ret = COOR_SM->ActivateCoordination(remoteNetworkId, startDeviceId);
    if (ret != RET_OK) {
        FI_HILOGE("On activate coordination failed, ret:%{public}d", ret);
    }
    return ret;
}

int32_t Coordination::DeactivateCoordination(SessionPtr sess, int32_t userData, bool isUnchained)
{
    CALL_DEBUG_ENTER;
    CHKPR(sess, RET_ERR);
    sptr<CoordinationEventManager::EventInfo> event = new (std::nothrow) CoordinationEventManager::EventInfo();
    CHKPR(event, RET_ERR);
    event->type = CoordinationEventManager::EventType::STOP;
    event->sess = sess;
    event->msgId = MessageId::COORDINATION_MESSAGE;
    event->userData = userData;
    COOR_EVENT_MGR->AddCoordinationEvent(event);
    int32_t ret = COOR_SM->DeactivateCoordination(isUnchained);
    if (ret != RET_OK) {
        FI_HILOGE("On deactivate coordination failed, ret:%{public}d", ret);
        COOR_EVENT_MGR->OnErrorMessage(event->type, CoordinationMessage(ret));
    }
    return ret;
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
    COOR_EVENT_MGR->AddCoordinationEvent(event);
    int32_t ret = COOR_SM->GetCoordinationState(deviceId);
    if (ret != RET_OK) {
        FI_HILOGE("Get coordination state failed");
    }
    return ret;
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
    COOR_EVENT_MGR->AddCoordinationEvent(event);
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
    COOR_EVENT_MGR->RemoveCoordinationEvent(event);
    return RET_OK;
}

int32_t Coordination::AddHotAreaListener(SessionPtr sess)
{
    CALL_DEBUG_ENTER;
    CHKPR(sess, RET_ERR);
    sptr<CoordinationHotArea::HotAreaInfo> event = new (std::nothrow) CoordinationHotArea::HotAreaInfo();
    CHKPR(event, RET_ERR);
    event->sess = sess;
    event->msgId = MessageId::HOT_AREA_ADD_LISTENER;
    HOT_AREA->AddHotAreaListener(event);
    return RET_OK;
}

void Coordination::Dump(int32_t fd)
{
    COOR_SM->Dump(fd);
}

void Coordination::OnSessionLost(SessionPtr session)
{
    CALL_DEBUG_ENTER;
    COOR_SM->OnSessionLost(session);
}

ICoordination* CreateCoordination(IContext *context)
{
    CALL_DEBUG_ENTER;
    CHKPP(context);
    COOR_EVENT_MGR->SetIContext(context);
    ICoordination *coor = new (std::nothrow) Coordination();
    CHKPP(coor);
    COOR_SM->Init();
    return coor;
}

void ReleaseCoordination(ICoordination* coor)
{
    CALL_DEBUG_ENTER;
    COOR_DEV_MGR->RemoveObserver();
    if (coor != nullptr) {
        delete coor;
    }
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS