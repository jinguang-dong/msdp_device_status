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

#include "motion_srv_stub.h"

#include "message_parcel.h"

#include "motion_srv_proxy.h"
#include "motion_common.h"
#include "imotion_callback.h"
#include "motion_data_utils.h"
#include "motion_service.h"

namespace OHOS {
namespace Msdp {
int MotionSrvStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "cmd = %{public}d, flags = %{public}d", code, option.GetFlags());
    std::u16string descriptor = MotionSrvStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "MotionSrvStub::OnRemoteRequest failed, descriptor is not matched!");
        return E_MOTION_GET_SERVICE_FAILED;
    }

    switch (code) {
        case static_cast<int>(Imotion::MOTION_SUBSCRIBE): {
            return SubscribeStub(data);
        }
        case static_cast<int>(Imotion::MOTION_UNSUBSCRIBE): {
            return UnSubscribeStub(data);
        }
        case static_cast<int>(Imotion::MOTION_GETCACHE): {
            return GetLatestMotionDataStub(data, reply);
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return ERR_OK;
}

int32_t MotionSrvStub::SubscribeStub(MessageParcel& data)
{
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "Enter");
    int32_t type = -1;
    MOTION_READ_PARCEL_WITH_RET(data, Int32, type, E_MOTION_READ_PARCEL_ERROR);
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "Read type successfully");
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    MOTION_RETURN_IF_WITH_RET((obj == nullptr), E_MOTION_READ_PARCEL_ERROR);
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "Read remote obj successfully");
    sptr<ImotionCallback> callback = iface_cast<ImotionCallback>(obj);
    MOTION_RETURN_IF_WITH_RET((callback == nullptr), E_MOTION_READ_PARCEL_ERROR);
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "Read callback successfully");
    Subscribe(MotionDataUtils::MotionType(type), callback);
    return ERR_OK;
}

int32_t MotionSrvStub::UnSubscribeStub(MessageParcel& data)
{
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "Enter");
    int32_t type = -1;
    MOTION_READ_PARCEL_WITH_RET(data, Int32, type, E_MOTION_READ_PARCEL_ERROR);
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    MOTION_RETURN_IF_WITH_RET((obj == nullptr), E_MOTION_READ_PARCEL_ERROR);
    sptr<ImotionCallback> callback = iface_cast<ImotionCallback>(obj);
    MOTION_RETURN_IF_WITH_RET((callback == nullptr), E_MOTION_READ_PARCEL_ERROR);
    UnSubscribe(MotionDataUtils::MotionType(type), callback);
    return ERR_OK;
}

int32_t MotionSrvStub::GetLatestMotionDataStub(MessageParcel& data, MessageParcel& reply)
{
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "Enter");
    int32_t type = -1;
    MOTION_READ_PARCEL_WITH_RET(data, Int32, type, E_MOTION_READ_PARCEL_ERROR);
    MotionDataUtils::MotionData motionData = GetCache(MotionDataUtils::MotionType(type));
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "motionData.type: %{public}d", motionData.type);
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "motionData.value: %{public}d", motionData.value);
    MOTION_WRITE_PARCEL_WITH_RET(reply, Int32, motionData.type, E_MOTION_WRITE_PARCEL_ERROR);
    MOTION_WRITE_PARCEL_WITH_RET(reply, Int32, motionData.value, E_MOTION_WRITE_PARCEL_ERROR);
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "Exit");
    return ERR_OK;
}
} // Msdp
} // OHOS
