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

#include "cooperate_server.h"

#include "cooperate_params.h"
#include "default_params.h"
#include "devicestatus_define.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "CooperateServer" };
} // namespace

CooperateServer::CooperateServer(IContext *context)
    : context_(context)
{}

int32_t CooperateServer::Enable(CallingContext &context, MessageParcel &data, MessageParcel &reply)
{
    CALL_DEBUG_ENTER;
    DefaultParam param;
    if (!param.Unmarshalling(data)) {
        FI_HILOGE("DefaultParam::Unmarshalling fail");
        return RET_ERR;
    }
    CHKPR(context_, RET_ERR);
    ICooperate* cooperate = context_->GetPluginManager().LoadCooperate();
    CHKPR(cooperate, RET_ERR);
    cooperate->Enable(context.pid, param.userData);
    return RET_OK;
}

int32_t CooperateServer::Disable(CallingContext &context, MessageParcel &data, MessageParcel &reply)
{
    CALL_DEBUG_ENTER;
    DefaultParam param;
    if (!param.Unmarshalling(data)) {
        FI_HILOGE("DefaultParam::Unmarshalling fail");
        return RET_ERR;
    }
    CHKPR(context_, RET_ERR);
    ICooperate* cooperate = context_->GetPluginManager().LoadCooperate();
    CHKPR(cooperate, RET_ERR);
    cooperate->Disable(context.pid, param.userData);
    return RET_OK;
}

int32_t CooperateServer::Start(CallingContext &context, MessageParcel &data, MessageParcel &reply)
{
    CALL_DEBUG_ENTER;
    StartCooperateParam param;
    if (!param.Unmarshalling(data)) {
        FI_HILOGE("StartCooperateParam::Unmarshalling fail");
        return RET_ERR;
    }
    CHKPR(context_, RET_ERR);
    ICooperate* cooperate = context_->GetPluginManager().LoadCooperate();
    CHKPR(cooperate, RET_ERR);
    return cooperate->Start(context.pid, param.userData, param.remoteNetworkId, param.startDeviceId);
}

int32_t CooperateServer::Stop(CallingContext &context, MessageParcel &data, MessageParcel &reply)
{
    CALL_DEBUG_ENTER;
    StopCooperateParam param;
    if (!param.Unmarshalling(data)) {
        FI_HILOGE("StopCooperateParam::Unmarshalling fail");
        return RET_ERR;
    }
    CHKPR(context_, RET_ERR);
    ICooperate* cooperate = context_->GetPluginManager().LoadCooperate();
    CHKPR(cooperate, RET_ERR);
    return cooperate->Stop(context.pid, param.userData, param.isUnchained);
}

int32_t CooperateServer::AddWatch(CallingContext &context, uint32_t id, MessageParcel &data, MessageParcel &reply)
{
    CALL_DEBUG_ENTER;
    switch (id) {
        case CooperateRequestID::REGISTER_LISTENER: {
            CHKPR(context_, RET_ERR);
            ICooperate* cooperate = context_->GetPluginManager().LoadCooperate();
            CHKPR(cooperate, RET_ERR);
            return cooperate->RegisterListener(context.pid);
        }
        case CooperateRequestID::REGISTER_HOTAREA_LISTENER: {
            CHKPR(context_, RET_ERR);
            ICooperate* cooperate = context_->GetPluginManager().LoadCooperate();
            CHKPR(cooperate, RET_ERR);
            return cooperate->RegisterHotAreaListener(context.pid);
        }
        default: {
            FI_HILOGE("Unexpected request ID (%{public}u)", id);
            return RET_ERR;
        }
    }
}

int32_t CooperateServer::RemoveWatch(CallingContext &context, uint32_t id, MessageParcel &data, MessageParcel &reply)
{
    CALL_DEBUG_ENTER;
    switch (id) {
        case CooperateRequestID::UNREGISTER_LISTENER: {
            CHKPR(context_, RET_ERR);
            ICooperate* cooperate = context_->GetPluginManager().LoadCooperate();
            CHKPR(cooperate, RET_ERR);
            return cooperate->UnregisterListener(context.pid);
        }
        case CooperateRequestID::UNREGISTER_HOTAREA_LISTENER: {
            CHKPR(context_, RET_ERR);
            ICooperate* cooperate = context_->GetPluginManager().LoadCooperate();
            CHKPR(cooperate, RET_ERR);
            return cooperate->UnregisterHotAreaListener(context.pid);
        }
        default: {
            FI_HILOGE("Unexpected request ID (%{public}u)", id);
            return RET_ERR;
        }
    }
}

int32_t CooperateServer::SetParam(CallingContext &context, uint32_t id, MessageParcel &data, MessageParcel &reply)
{
    CALL_DEBUG_ENTER;
    return RET_ERR;
}

int32_t CooperateServer::GetParam(CallingContext &context, uint32_t id, MessageParcel &data, MessageParcel &reply)
{
    CALL_DEBUG_ENTER;
    switch (id) {
        case CooperateRequestID::GET_COOPERATE_STATE: {
            GetCooperateStateParam param;
            if (!param.Unmarshalling(data)) {
                FI_HILOGE("GetCooperateStateParam::Unmarshalling fail");
                return RET_ERR;
            }
            CHKPR(context_, RET_ERR);
            ICooperate* cooperate = context_->GetPluginManager().LoadCooperate();
            CHKPR(cooperate, RET_ERR);
            return cooperate->GetCooperateState(context.pid, param.userData, param.networkId);
        }
        default: {
            FI_HILOGE("Unexpected request ID (%{public}u)", id);
            return RET_ERR;
        }
    }
}

int32_t CooperateServer::Control(CallingContext &context, uint32_t id, MessageParcel &data, MessageParcel &reply)
{
    CALL_DEBUG_ENTER;
    return RET_ERR;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS