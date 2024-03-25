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

#include "cooperate.h"

#include "devicestatus_define.h"

#undef LOG_TAG
#define LOG_TAG "Cooperate"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace Cooperate {
namespace {
    const std::string COOPERATE_SWITCH { "currentStatus" };
} // namespace

Cooperate::Cooperate(IContext *env)
    : context_(env), sm_(env)
{
    auto [sender, receiver] = Channel<CooperateEvent>::OpenChannel();
    receiver_ = receiver;
    context_.AttachSender(sender);
    context_.Enable();
}

Cooperate::~Cooperate()
{
    StopWorker();
    context_.Disable();
}

int32_t Cooperate::RegisterListener(int32_t pid)
{
    CALL_DEBUG_ENTER;
    context_.Sender().Send(CooperateEvent(
        CooperateEventType::REGISTER_LISTENER,
        RegisterListenerEvent {
            .pid = pid
        }));
    return RET_OK;
}

int32_t Cooperate::UnregisterListener(int32_t pid)
{
    CALL_DEBUG_ENTER;
    context_.Sender().Send(CooperateEvent(
        CooperateEventType::UNREGISTER_LISTENER,
        UnregisterListenerEvent {
            .pid = pid
        }));
    return RET_OK;
}

int32_t Cooperate::RegisterHotAreaListener(int32_t pid)
{
    CALL_DEBUG_ENTER;
    context_.Sender().Send(CooperateEvent(
        CooperateEventType::REGISTER_HOTAREA_LISTENER,
        RegisterHotareaListenerEvent {
            .pid = pid
        }));
    return RET_OK;
}

int32_t Cooperate::UnregisterHotAreaListener(int32_t pid)
{
    CALL_DEBUG_ENTER;
    context_.Sender().Send(CooperateEvent(
        CooperateEventType::UNREGISTER_HOTAREA_LISTENER,
        UnregisterHotareaListenerEvent {
            .pid = pid
        }));
    return RET_OK;
}

int32_t Cooperate::Enable(int32_t pid, int32_t userData)
{
    CALL_DEBUG_ENTER;
    StartWorker();
    context_.Sender().Send(CooperateEvent(
        CooperateEventType::ENABLE,
        EnableCooperateEvent {
            .pid = pid,
            .userData = userData,
        }));
    return RET_OK;
}

int32_t Cooperate::Disable(int32_t pid, int32_t userData)
{
    CALL_DEBUG_ENTER;
    context_.Sender().Send(CooperateEvent(
        CooperateEventType::DISABLE,
        DisableCooperateEvent {
            .pid = pid,
            .userData = userData,
        }));
    StopWorker();
    return RET_OK;
}

int32_t Cooperate::Start(int32_t pid, int32_t userData, const std::string &remoteNetworkId, int32_t startDeviceId)
{
    CALL_DEBUG_ENTER;
    context_.Sender().Send(CooperateEvent(
        CooperateEventType::START,
        StartCooperateEvent {
            .pid = pid,
            .userData = userData,
            .remoteNetworkId = remoteNetworkId,
            .startDeviceId = startDeviceId,
        }));
    return RET_OK;
}

int32_t Cooperate::Stop(int32_t pid, int32_t userData, bool isUnchained)
{
    CALL_DEBUG_ENTER;
    context_.Sender().Send(CooperateEvent(
        CooperateEventType::STOP,
        StopCooperateEvent {
            .pid = pid,
            .userData = userData,
            .isUnchained = isUnchained,
        }));
    return RET_OK;
}

int32_t Cooperate::GetCooperateState(int32_t pid, int32_t userData, const std::string &networkId)
{
    CALL_DEBUG_ENTER;
    context_.Sender().Send(CooperateEvent(
        CooperateEventType::GET_COOPERATE_STATE,
        GetCooperateStateEvent {
            .pid = pid,
            .userData = userData,
            .networkId = networkId,
        }));
    return RET_OK;
}

int32_t Cooperate::GetCooperateState(int32_t pid, const std::string &udId, bool &state)
{
    CALL_DEBUG_ENTER;
    context_.GetDP.GetProperty(udId,COOPERATE_SWITCH,state);
    return RET_OK;
}

void Cooperate::Dump(int32_t fd)
{
    CALL_DEBUG_ENTER;
    context_.Sender().Send(CooperateEvent(
        CooperateEventType::DUMP,
        DumpEvent {
            .fd = fd
        }));
}

void Cooperate::Loop()
{
    CALL_DEBUG_ENTER;
    bool running = true;

    while (running) {
        CooperateEvent event = receiver_.Receive();
        switch (event.type) {
            case CooperateEventType::NOOP: {
                break;
            }
            case CooperateEventType::QUIT: {
                FI_HILOGI("Skip out of loop");
                running = false;
                break;
            }
            default: {
                sm_.OnEvent(context_, event);
                break;
            }
        }
    }
}

void Cooperate::StartWorker()
{
    CALL_DEBUG_ENTER;
    std::lock_guard guard(lock_);
    if (!workerStarted_) {
        workerStarted_ = true;
        worker_ = std::thread(std::bind(&Cooperate::Loop, this));
    }
}

void Cooperate::StopWorker()
{
    CALL_DEBUG_ENTER;
    std::lock_guard guard(lock_);
    if (workerStarted_) {
        context_.Sender().Send(CooperateEvent(CooperateEventType::QUIT));
        if (worker_.joinable()) {
            worker_.join();
        }
        workerStarted_ = false;
    }
}

extern "C" ICooperate* CreateInstance(IContext *env)
{
    CHKPP(env);
    return new Cooperate(env);
}

extern "C" void DestroyInstance(ICooperate *instance)
{
    if (instance != nullptr) {
        delete instance;
    }
}
} // namespace Cooperate
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS