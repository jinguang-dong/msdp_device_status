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

#include "motion_callback_stub.h"

#include <message_parcel.h>

#include "motion_common.h"
#include "motion_callback_proxy.h"

namespace OHOS {
namespace Msdp {
int MotionCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "cmd = %{public}d, flags= %{public}d", code, option.GetFlags());
    std::u16string descripter = MotionCallbackStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "MotionCallbackStub::OnRemoteRequest failed, descriptor mismatch");
        return E_MOTION_GET_SERVICE_FAILED;
    }

    switch (code) {
        case static_cast<int>(ImotionCallback::MOTION_CHANGE): {
            return OnMotionChangedStub(data);
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_OK;
}

int32_t MotionCallbackStub::OnMotionChangedStub(MessageParcel& data)
{
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "Enter");
    int32_t type;
    int32_t value;
    int32_t status;
    int32_t action;
    int32_t rotateAction;
    double move;

    MOTION_READ_PARCEL_WITH_RET(data, Int32, type, E_MOTION_READ_PARCEL_ERROR);
    MOTION_READ_PARCEL_WITH_RET(data, Int32, value, E_MOTION_READ_PARCEL_ERROR);
    MOTION_READ_PARCEL_WITH_RET(data, Int32, status, E_MOTION_READ_PARCEL_ERROR);
    MOTION_READ_PARCEL_WITH_RET(data, Int32, action, E_MOTION_READ_PARCEL_ERROR);
    MOTION_READ_PARCEL_WITH_RET(data, Int32, rotateAction, E_MOTION_READ_PARCEL_ERROR);
    MOTION_READ_PARCEL_WITH_RET(data, Double, move, E_MOTION_READ_PARCEL_ERROR);
    MotionDataUtils::MotionData motionData = { static_cast<MotionDataUtils::MotionType>(type), \
        static_cast<MotionDataUtils::MotionValue>(value),static_cast<MotionDataUtils::Status>(status), \
        static_cast<MotionDataUtils::Action>(action),static_cast<MotionDataUtils::RotateAction>(rotateAction),move};
    MOTION_HILOGE(MOTION_MODULE_SERVICE, "Motiondata:%{public}d",static_cast<MotionDataUtils::MotionType>(type));
    MOTION_HILOGE(MOTION_MODULE_SERVICE, "Motiondata:%{public}d",static_cast<MotionDataUtils::MotionValue>(value));
    MOTION_HILOGE(MOTION_MODULE_SERVICE, "Motiondata:%{public}d",static_cast<MotionDataUtils::Status>(status));
    MOTION_HILOGE(MOTION_MODULE_SERVICE, "Motiondata:%{public}d",static_cast<MotionDataUtils::Action>(action));
    MOTION_HILOGE(MOTION_MODULE_SERVICE, "Motiondata:%{public}f",move);
    
    OnMotionChanged(motionData);
    return ERR_OK;
}
} // namespace Msdp
} // namespace OHOS
