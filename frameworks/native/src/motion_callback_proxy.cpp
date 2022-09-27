/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "motion_callback_proxy.h"

#include <ipc_types.h>
#include <message_parcel.h>

#include "motion_common.h"

namespace OHOS {
namespace Msdp {
void MotionCallbackProxy::OnMotionChanged(const MotionDataUtils::MotionData& motionData)
{
    sptr<IRemoteObject> remote = Remote();
    MOTION_RETURN_IF(remote == nullptr);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(MotionCallbackProxy::GetDescriptor())) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "Write descriptor failed");
        return;
    }

    MOTION_WRITE_PARCEL_NO_RET(data, Int32, static_cast<int32_t>(motionData.type));
    MOTION_WRITE_PARCEL_NO_RET(data, Int32, static_cast<int32_t>(motionData.value));
    MOTION_WRITE_PARCEL_NO_RET(data, Int32, static_cast<int32_t>(motionData.status));
    MOTION_WRITE_PARCEL_NO_RET(data, Int32, static_cast<int32_t>(motionData.action));
    MOTION_WRITE_PARCEL_NO_RET(data, Int32, static_cast<int32_t>(motionData.rotateAction));
    MOTION_WRITE_PARCEL_NO_RET(data, Double, motionData.move);
    
    int ret = remote->SendRequest(static_cast<int>(ImotionCallback::MOTION_CHANGE), data, reply, option);
    if (ret != ERR_OK) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
    }
}
} // Msdp
} // OHOS