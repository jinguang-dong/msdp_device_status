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

#include "drag_stop_callback_stub.h"

#include <message_parcel.h>

#include "devicestatus_common.h"
#include "devicestatus_define.h"
#include "fi_log.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "DragStopCallbackStub" };
}  // namespace
int32_t DragStopCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    FI_HILOGD("cmd = %{public}u, flags= %{public}d", code, option.GetFlags());
    std::u16string descripter = DragStopCallbackStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        FI_HILOGE("DragCallbackStub::OnRemoteRequest failed, descriptor mismatch");
        return E_DEVICESTATUS_GET_SERVICE_FAILED;
    }
    if (code == static_cast<int32_t>(IDragStopCallback::DRAG_CHANGE)) {
        return OnDragChangedStub(data);
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t DragStopCallbackStub::OnDragChangedStub(MessageParcel &data)
{
    CALL_DEBUG_ENTER;
    int32_t result;
    DragNotifyMsg notifyMsg;
    READINT32(data, notifyMsg.displayX, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READINT32(data, notifyMsg.displayY, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READINT32(data, notifyMsg.targetPid, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READINT32(data, result, E_DEVICESTATUS_READ_PARCEL_ERROR);
    notifyMsg.result =  static_cast<DragResult>(result);
    return OnDragChanged(notifyMsg);
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
