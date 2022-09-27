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

#include "motion_srv_proxy.h"

#include <ipc_types.h>
#include <message_parcel.h>
#include <message_option.h>

#include "imotion_callback.h"
#include "motion_common.h"

namespace OHOS {
namespace Msdp {
void MotionSrvProxy::Subscribe(const MotionDataUtils::MotionType& type, const sptr<ImotionCallback>& callback)
{
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Enter");
    sptr<IRemoteObject> remote = Remote();
    MOTION_RETURN_IF((remote == nullptr) || (callback == nullptr));

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(MotionSrvProxy::GetDescriptor())) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "Write descriptor failed");
        return;
    }

    MOTION_WRITE_PARCEL_NO_RET(data, Int32, type);
    MOTION_WRITE_PARCEL_NO_RET(data, RemoteObject, callback->AsObject());


    int ret = remote->SendRequest(static_cast<int>(Imotion::MOTION_SUBSCRIBE), data, reply, option);
    if (ret != ERR_OK) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return;
    }
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Exit");
}

void MotionSrvProxy::UnSubscribe(const MotionDataUtils::MotionType& type,
    const sptr<ImotionCallback>& callback)
{
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Enter");
    sptr<IRemoteObject> remote = Remote();
    MOTION_RETURN_IF((remote == nullptr) || (callback == nullptr));

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(MotionSrvProxy::GetDescriptor())) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "Write descriptor failed!");
        return;
    }

    MOTION_WRITE_PARCEL_NO_RET(data, Int32, type);
    MOTION_WRITE_PARCEL_NO_RET(data, RemoteObject, callback->AsObject());

    int ret = remote->SendRequest(static_cast<int>(Imotion::MOTION_UNSUBSCRIBE), data, reply, option);
    if (ret != ERR_OK) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return;
    }
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Exit");
}

MotionDataUtils::MotionData MotionSrvProxy::GetCache(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Enter");
    MotionDataUtils::MotionData motionData;
    motionData.type = MotionDataUtils::MotionType::TYPE_INVALID;
    motionData.value = MotionDataUtils::MotionValue::VALUE_INVALID;

    sptr<IRemoteObject> remote = Remote();
    MOTION_RETURN_IF_WITH_RET((remote == nullptr), motionData);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(MotionSrvProxy::GetDescriptor())) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "Write descriptor failed!");
        return motionData;
    }

    MOTION_WRITE_PARCEL_WITH_RET(data, Int32, type, motionData);

    int ret = remote->SendRequest(static_cast<int>(Imotion::MOTION_GETCACHE), data, reply, option);
    if (ret != ERR_OK) {
        MOTION_HILOGE(MOTION_MODULE_INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return motionData;
    }

    int32_t motionType = -1;
    int32_t motionValue = -1;

    MOTION_READ_PARCEL_WITH_RET(reply, Int32, motionType, motionData);
    MOTION_READ_PARCEL_WITH_RET(reply, Int32, motionValue, motionData);

    motionData.type = MotionDataUtils::MotionType(motionType);
    motionData.value = MotionDataUtils::MotionValue(motionValue);

    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "type: %{public}d", motionData.type);
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "value: %{public}d", motionData.value);
    MOTION_HILOGD(MOTION_MODULE_INNERKIT, "Exit");
    return motionData;
}
} // Msdp
} // OHOS