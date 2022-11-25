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

#include "message_parcel.h"

#include "devicestatus_common.h"
#include "devicestatus_data_utils.h"
#include "devicestatus_service.h"
#include "devicestatus_srv_stub.h"
#include "devicestatus_srv_proxy.h"
#include "idevicestatus_callback.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
int32_t DeviceStatusServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DEV_HILOGD(SERVICE, "cmd = %{public}d, flags = %{public}d", code, option.GetFlags());
    std::u16string descriptor = DeviceStatusServiceStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        DEV_HILOGE(SERVICE, "DeviceStatusServiceStub::OnRemoteRequest failed, descriptor is not matched");
        return E_DEVICESTATUS_GET_SERVICE_FAILED;
    }

    switch (code) {
        case static_cast<int32_t>(Idevicestatus::DEVICESTATUS_SUBSCRIBE): {
            return SubscribeStub(data);
        }
        case static_cast<int32_t>(Idevicestatus::DEVICESTATUS_UNSUBSCRIBE): {
            return UnsubscribeStub(data);
        }
        case static_cast<int32_t>(Idevicestatus::DEVICESTATUS_GETCACHE): {
            return GetLatestDeviceStatusDataStub(data, reply);
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return ERR_OK;
}

int32_t DeviceStatusServiceStub::SubscribeStub(MessageParcel& data)
{
    DEV_HILOGD(SERVICE, "Enter");
    int32_t type = -1;
    READINT32(data, type, E_DEVICESTATUS_READ_PARCEL_ERROR);
    DEV_HILOGD(SERVICE, "Read type successfully");
    int32_t event = -1;
    READINT32(data, event, E_DEVICESTATUS_READ_PARCEL_ERROR);
    DEV_HILOGD(SERVICE, "Read event successfully");
    DEV_HILOGD(SERVICE, "event:%{public}d", event);
    int32_t latency = -1;
    READINT32(data, latency, E_DEVICESTATUS_READ_PARCEL_ERROR);
    DEV_HILOGD(SERVICE, "Read latency successfully");
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    DEV_RET_IF_NULL_WITH_RET((obj == nullptr), E_DEVICESTATUS_READ_PARCEL_ERROR);
    DEV_HILOGI(SERVICE, "Read remote obj successfully");
    sptr<IRemoteDevStaCallback> callback = iface_cast<IRemoteDevStaCallback>(obj);
    DEV_RET_IF_NULL_WITH_RET((callback == nullptr), E_DEVICESTATUS_READ_PARCEL_ERROR);
    DEV_HILOGI(SERVICE, "Read callback successfully");
    Subscribe(Type(type), ActivityEvent(event), ReportLatencyNs(latency), callback);
    return ERR_OK;
}

int32_t DeviceStatusServiceStub::UnsubscribeStub(MessageParcel& data)
{
    DEV_HILOGD(SERVICE, "Enter");
    int32_t type = -1;
    READINT32(data, type, E_DEVICESTATUS_READ_PARCEL_ERROR);
    int32_t event = -1;
    READINT32(data, event, E_DEVICESTATUS_READ_PARCEL_ERROR);
    DEV_HILOGE(SERVICE, "UNevent: %{public}d", event);
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    DEV_RET_IF_NULL_WITH_RET((obj == nullptr), E_DEVICESTATUS_READ_PARCEL_ERROR);
    sptr<IRemoteDevStaCallback> callback = iface_cast<IRemoteDevStaCallback>(obj);
    DEV_RET_IF_NULL_WITH_RET((callback == nullptr), E_DEVICESTATUS_READ_PARCEL_ERROR);
    Unsubscribe(Type(type), ActivityEvent(event), callback);
    return ERR_OK;
}

int32_t DeviceStatusServiceStub::GetLatestDeviceStatusDataStub(MessageParcel& data, MessageParcel& reply)
{
    DEV_HILOGD(SERVICE, "Enter");
    int32_t type = -1;
    READINT32(data, type, E_DEVICESTATUS_READ_PARCEL_ERROR);
    Data devicestatusData = GetCache(Type(type));
    DEV_HILOGD(SERVICE, "devicestatusData.type: %{public}d", devicestatusData.type);
    DEV_HILOGD(SERVICE, "devicestatusData.value: %{public}d", devicestatusData.value);
    WRITEINT32(reply, devicestatusData.type, E_DEVICESTATUS_WRITE_PARCEL_ERROR);
    WRITEINT32(reply, devicestatusData.value, E_DEVICESTATUS_WRITE_PARCEL_ERROR);
    DEV_HILOGD(SERVICE, "Exit");
return ERR_OK;
    return ERR_OK;
}
} // DeviceStatus
} // Msdp
} // OHOS
