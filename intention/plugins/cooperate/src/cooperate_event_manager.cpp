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

#include "cooperate_event_manager.h"
#include "devicestatus_define.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "CoorperateEventManager" };
} // namespace

CoorperateEventManager::CoorperateEventManager() {}
CoorperateEventManager::~CoorperateEventManager() {}

void CoorperateEventManager::AddCoorperateEvent(sptr<EventInfo> event)
{
    CALL_DEBUG_ENTER;
    CHKPV(event);
    std::lock_guard<std::mutex> guard(lock_);
    if (event->type == EventType::LISTENER) {
        auto it = std::find_if(remoteCoorperateCallbacks_.begin(), remoteCoorperateCallbacks_.end(),
            [event] (auto info) {
                return (*info).sess == event->sess;
            });
        if (it != remoteCoorperateCallbacks_.end()) {
            *it = event;
        } else {
            remoteCoorperateCallbacks_.emplace_back(event);
        }
    } else {
        cooperateCallbacks_[event->type] = event;
    }
}

void CoorperateEventManager::RemoveCooperateEvent(sptr<EventInfo> event)
{
    CALL_DEBUG_ENTER;
    if (remoteCoorperateCallbacks_.empty() || event == nullptr) {
        FI_HILOGE("Remove listener failed");
        return;
    }
    for (auto it = remoteCoorperateCallbacks_.begin(); it != remoteCoorperateCallbacks_.end(); ++it) {
        if ((*it)->sess == event->sess) {
            remoteCoorperateCallbacks_.erase(it);
            return;
        }
    }
}

int32_t CoorperateEventManager::OnCoorperateMessage(CooperateMessage msg, const std::string &deviceId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(lock_);
    if (remoteCoorperateCallbacks_.empty()) {
        FI_HILOGW("The current listener is empty, unable to invoke the listening interface");
        return RET_ERR;
    }
    for (auto it = remoteCoorperateCallbacks_.begin(); it != remoteCoorperateCallbacks_.end(); ++it) {
        sptr<EventInfo> info = *it;
        CHKPC(info);
        NotifyCoorperateMessage(info->sess, info->msgId, info->userData, deviceId, msg);
    }
    return RET_OK;
}

void CoorperateEventManager::OnEnable(CooperateMessage msg, const std::string &deviceId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(lock_);
    sptr<EventInfo> info = cooperateCallbacks_[EventType::ENABLE];
    CHKPV(info);
    NotifyCoorperateMessage(info->sess, info->msgId, info->userData, deviceId, msg);
    cooperateCallbacks_[EventType::ENABLE] =  nullptr;
}

void CoorperateEventManager::OnStart(CooperateMessage msg, const std::string &deviceId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(lock_);
    sptr<EventInfo> info = cooperateCallbacks_[EventType::START];
    CHKPV(info);
    NotifyCoorperateMessage(info->sess, info->msgId, info->userData, deviceId, msg);
    cooperateCallbacks_[EventType::START] =  nullptr;
}

void CoorperateEventManager::OnStop(CooperateMessage msg, const std::string &deviceId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(lock_);
    sptr<EventInfo> info = cooperateCallbacks_[EventType::STOP];
    CHKPV(info);
    NotifyCoorperateMessage(info->sess, info->msgId, info->userData, deviceId, msg);
    cooperateCallbacks_[EventType::STOP] =  nullptr;
}

void CoorperateEventManager::OnGetCrossingSwitchState(bool state)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(lock_);
    sptr<EventInfo> info = cooperateCallbacks_[EventType::STATE];
    CHKPV(info);
    NotifyCoorperateState(info->sess, info->msgId, info->userData, state);
    cooperateCallbacks_[EventType::STATE] =  nullptr;
}

void CoorperateEventManager::OnErrorMessage(EventType type, CooperateMessage msg)
{
    std::lock_guard<std::mutex> guard(lock_);
    sptr<EventInfo> info = cooperateCallbacks_[type];
    CHKPV(info);
    NotifyCoorperateMessage(info->sess, info->msgId, info->userData, "", msg);
    cooperateCallbacks_[type] =  nullptr;
}

void CoorperateEventManager::SetIContext(IContext *context)
{
    context_ = context;
}

IContext* CoorperateEventManager::GetIContext() const
{
    return context_;
}

void CoorperateEventManager::NotifyCoorperateMessage(
    SessionPtr sess, MessageId msgId, int32_t userData, const std::string &deviceId, CooperateMessage msg)
{
    CALL_DEBUG_ENTER;
    CHKPV(sess);
    NetPacket pkt(msgId);
    pkt << userData << deviceId << static_cast<int32_t>(msg);
    if (pkt.ChkRWError()) {
        FI_HILOGE("Packet write data failed");
        return;
    }
    if (!sess->SendMsg(pkt)) {
        FI_HILOGE("Sending failed");
        return;
    }
}

void CoorperateEventManager::NotifyCoorperateState(SessionPtr sess, MessageId msgId, int32_t userData, bool state)
{
    CALL_DEBUG_ENTER;
    CHKPV(sess);
    NetPacket pkt(msgId);
    pkt << userData << state;
    if (pkt.ChkRWError()) {
        FI_HILOGE("Packet write data failed");
        return;
    }
    if (!sess->SendMsg(pkt)) {
        FI_HILOGE("Sending failed");
        return;
    }
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
