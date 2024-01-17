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

#include "cooperate_out.h"

#include "devicestatus_define.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace Cooperate {
namespace {
constexpr int32_t DEFAULT_TIMEOUT { 3000 };
constexpr int32_t REPEAT_ONCE { 1 };
constexpr uint32_t P2P_SESSION_CLOSED { 1 };
constexpr HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "CooperateOut" };
} // namespace

CooperateOut::CooperateOut(IContext *env)
    : env_(env)
{
    auto initial = std::make_shared<Initial>(*this);
    Initial::BuildChains(initial, *this);
    current_ = initial;
}

void CooperateOut::OnEvent(Context &context, const CooperateEvent &event)
{
    CALL_DEBUG_ENTER;
    current_->OnEvent(context, event);
}

void CooperateOut::OnEnterState(Context &context)
{
    CALL_DEBUG_ENTER;
    MMI::InputManager::GetInstance()->SetPointerVisible(false);
    CHKPV(env_);
    interceptorId_ = env_->GetInput().AddInterceptor(
        [sender = context.Sender()](std::shared_ptr<MMI::PointerEvent> pointerEvent) {},
        [sender = context.Sender()](std::shared_ptr<MMI::KeyEvent> keyEvent) {});
}

void CooperateOut::OnLeaveState(Context &context)
{
    CALL_DEBUG_ENTER;
    CHKPV(env_);
    env_->GetInput().RemoveInterceptor(interceptorId_);
    interceptorId_ = -1;
}

void CooperateOut::RegisterDInputSessionCb(Context &context)
{
    CALL_DEBUG_ENTER;
    CHKPV(env_);
    env_->GetDInput().RegisterSessionStateCb(
        [sender = context.Sender()](uint32_t status) mutable {
            if (status == P2P_SESSION_CLOSED) {
                sender.Send(CooperateEvent(CooperateEventType::DINPUT_SESSION_CLOSED));
            }
        });
}

CooperateOut::Initial::Initial(CooperateOut &parent) : ICooperateStep(parent, nullptr)
{}

void CooperateOut::Initial::OnEvent(Context &context, const CooperateEvent &event)
{}

void CooperateOut::Initial::OnProgress(Context &context, const CooperateEvent &event)
{}

void CooperateOut::Initial::OnReset(Context &context, const CooperateEvent &event)
{}

void CooperateOut::Initial::BuildChains(std::shared_ptr<Initial> self, CooperateOut &parent)
{
    auto s1 = std::make_shared<StopRemoteInput>(parent, self);
    self->stop_ = s1;
    auto s2 = std::make_shared<UnprepareRemoteInput>(parent, self);
    s1->SetNext(s2);
    s2->SetNext(self);

    auto s21 = std::make_shared<RemoteStart>(parent, self);
    self->remoteStart_ = s21;
    s21->SetNext(self);
}

void CooperateOut::Initial::RemoveChains(std::shared_ptr<Initial> self)
{
    if (self->stop_ != nullptr) {
        self->stop_->SetNext(nullptr);
        self->stop_ = nullptr;
    }
    if (self->remoteStart_ != nullptr) {
        self->remoteStart_->SetNext(nullptr);
        self->remoteStart_ = nullptr;
    }
}

CooperateOut::StopRemoteInput::StopRemoteInput(CooperateOut &parent, std::shared_ptr<ICooperateStep> prev)
    : ICooperateStep(parent, prev)
{}

void CooperateOut::StopRemoteInput::OnEvent(Context &context, const CooperateEvent &event)
{
    CALL_DEBUG_ENTER;
}

void CooperateOut::StopRemoteInput::OnProgress(Context &context, const CooperateEvent &event)
{
    CALL_DEBUG_ENTER;
}

void CooperateOut::StopRemoteInput::OnReset(Context &context, const CooperateEvent &event)
{
    CALL_DEBUG_ENTER;
}

CooperateOut::UnprepareRemoteInput::UnprepareRemoteInput(CooperateOut &parent, std::shared_ptr<ICooperateStep> prev)
    : ICooperateStep(parent, prev)
{}

void CooperateOut::UnprepareRemoteInput::OnEvent(Context &context, const CooperateEvent &event)
{
    CALL_DEBUG_ENTER;
}

void CooperateOut::UnprepareRemoteInput::OnProgress(Context &context, const CooperateEvent &event)
{
    CALL_DEBUG_ENTER;
}

void CooperateOut::UnprepareRemoteInput::OnReset(Context &context, const CooperateEvent &event)
{
    CALL_DEBUG_ENTER;
}

CooperateOut::RemoteStart::RemoteStart(CooperateOut &parent, std::shared_ptr<ICooperateStep> prev)
    : ICooperateStep(parent, prev), parent_(parent)
{}

void CooperateOut::RemoteStart::OnEvent(Context &context, const CooperateEvent &event)
{
    switch (event.type) {
        case CooperateEventType::DSOFTBUS_START_COOPERATE_FINISHED: {
            OnRemoteStartFinished(context, event);
            break;
        }
        default: {
            break;
        }
    }
}

void CooperateOut::RemoteStart::OnRemoteStartFinished(Context &context, const CooperateEvent &event)
{
    CALL_DEBUG_ENTER;
    DSoftbusStartCooperateFinished ev = std::get<DSoftbusStartCooperateFinished>(event.event);
    if (!context.IsPeer(ev.networkId)) {
        return;
    }
    CHKPV(parent_.env_);
    parent_.env_->GetTimerManager().RemoveTimer(timerId_);
    if (ev.success) {
        OnSuccess(context, ev);
        Proceed(context, event);
    } else {
        OnReset(context, event);
    }
}

void CooperateOut::RemoteStart::OnSuccess(Context &context, const DSoftbusStartCooperateFinished &event)
{
    parent_.RegisterDInputSessionCb(context);
    context.RemoteStartSuccess(event);
    context.eventMgr_.RemoteStartFinish(event);
    context.Sender().Send(CooperateEvent(
        CooperateEventType::UPDATE_STATE,
        UpdateStateEvent {
            .current = CooperateState::COOPERATE_STATE_FREE,
        }));
}

void CooperateOut::RemoteStart::OnProgress(Context &context, const CooperateEvent &event)
{
    CALL_DEBUG_ENTER;
    DSoftbusStartCooperate req = std::get<DSoftbusStartCooperate>(event.event);
    context.RemoteStart(req);

    DSoftbusStartCooperateResponse resp {
        .networkId = DSoftbusAdapter::GetLocalNetworkId(),
        .normal = true,
    };
    CHKPV(context.dsoftbus_);
    int32_t ret = context.dsoftbus_->StartCooperateResponse(req.networkId, resp);
    if (ret != RET_OK) {
        FI_HILOGE("Failed to answer \'%{public}s\'", req.networkId.c_str());
        OnReset(context, event);
        return;
    }
    CHKPV(parent_.env_);
    timerId_ = parent_.env_->GetTimerManager().AddTimer(DEFAULT_TIMEOUT, REPEAT_ONCE,
        [sender = context.Sender(), remoteNetworkId = req.networkId]() mutable {
            sender.Send(CooperateEvent(
                CooperateEventType::DSOFTBUS_START_COOPERATE_FINISHED,
                DSoftbusStartCooperateFinished {
                    .networkId = remoteNetworkId,
                    .success = false,
                }));
        });
}

void CooperateOut::RemoteStart::OnReset(Context &context, const CooperateEvent &event)
{
    DSoftbusStartCooperateFinished ev {
        .networkId = context.Peer(),
        .success = false,
    };
    context.eventMgr_.RemoteStartFinish(ev);
    Reset(context, event);
}
} // namespace Cooperate
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
