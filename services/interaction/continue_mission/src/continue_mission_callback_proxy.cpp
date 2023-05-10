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

#include "continue_mission_callback_proxy.h"

#include <message_parcel.h>

#include "iremote_object.h"
#include "message_option.h"

#include "devicestatus_common.h"
#include "devicestatus_define.h"
#include "fi_log.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "ContinueMissionCallbackProxy" };
}  // namespace

int32_t ContinueMissionCallbackProxy::OnContinueMission(const ContinueMissionParam& continueMissionParam)
{
    sptr<IRemoteObject> remote = Remote();
    CHKPR(remote, RET_ERR);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(ContinueMissionCallbackProxy::GetDescriptor())) {
        FI_HILOGE("Write descriptor failed");
        return RET_ERR;
    }
    // Marshalling continueMissionParam
    int32_t ret = remote->SendRequest(static_cast<int32_t>(IContinueMissionCallback::CONTINUE_MISSION), data, reply, option);
    if (ret != RET_OK) {
        FI_HILOGE("SendRequest is failed, error code: %{public}d", ret);
    }
    return ret;
}
} // namespace DeviceStatus
} // Msdp
} // OHOS