/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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


#include <message_option.h>
#include <message_parcel.h>

#include "hitrace_meter.h"
#include "idevicestatus_callback.h"
#include "iremote_object.h"
#include "devicestatus_common.h"
#include "devicestatus_data_utils.h"
#include "devicestatus_srv_proxy.h"
#include "bytrace_adapter.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
void DeviceStatusSrvProxy::Subscribe(const Type& type,
    const ActivityEvent& event,
    const ReportLatencyNs& latency,
    const sptr<IRemoteDevStaCallbck>& callback)
{
    DEV_HILOGI(INNERKIT, "Enter event: %{public}d", event);
    DEV_HILOGI(INNERKIT, "Enter event: %{public}d", latency);
    sptr<IRemoteObject> remote = Remote();
    DEV_RET_IF_NULL((remote == nullptr) || (callback == nullptr));

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DeviceStatusSrvProxy::GetDescriptor())) {
        DEV_HILOGE(INNERKIT, "Write descriptor failed");
        return;
    }

    DEVICESTATUS_WRITE_PARCEL_NO_RET(data, Int32, type);
    DEVICESTATUS_WRITE_PARCEL_NO_RET(data, Int32, event);
    DEVICESTATUS_WRITE_PARCEL_NO_RET(data, Int32, latency);
    DEVICESTATUS_WRITE_PARCEL_NO_RET(data, RemoteObject, callback->AsObject());

    int32_t ret = remote->SendRequest(static_cast<int32_t>(Idevicestatus::DEVICESTATUS_SUBSCRIBE), data, reply, option);
    if (ret != ERR_OK) {
        DEV_HILOGE(INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return;
    }
    DEV_HILOGD(INNERKIT, "Exit");
}

void DeviceStatusSrvProxy::UnSubscribe(const Type& type,
    const ActivityEvent& event,
    const sptr<IRemoteDevStaCallbck>& callback)
{
    DEV_HILOGD(INNERKIT, "Enter");
    DEV_HILOGI(INNERKIT, "UNevent: %{public}d", event);
    sptr<IRemoteObject> remote = Remote();
    DEV_RET_IF_NULL((remote == nullptr) || (callback == nullptr));

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DeviceStatusSrvProxy::GetDescriptor())) {
        DEV_HILOGE(INNERKIT, "Write descriptor failed!");
        return;
    }

    DEVICESTATUS_WRITE_PARCEL_NO_RET(data, Int32, type);
    DEVICESTATUS_WRITE_PARCEL_NO_RET(data, Int32, event);
    DEVICESTATUS_WRITE_PARCEL_NO_RET(data, RemoteObject, callback->AsObject());

    int32_t ret = remote->SendRequest(static_cast<int32_t>(Idevicestatus::DEVICESTATUS_UNSUBSCRIBE),
        data, reply, option);
    if (ret != ERR_OK) {
        DEV_HILOGE(INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return;
    }
    DEV_HILOGD(INNERKIT, "Exit");
}

Data DeviceStatusSrvProxy::GetCache(const Type& type)
{
    DEV_HILOGD(INNERKIT, "Enter");
    Data devicestatusData;
    devicestatusData.type = Type::TYPE_INVALID;
    devicestatusData.value = OnChangedValue::VALUE_INVALID;

    sptr<IRemoteObject> remote = Remote();
    DEV_RET_IF_NULL_WITH_RET((remote == nullptr), devicestatusData);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DeviceStatusSrvProxy::GetDescriptor())) {
        DEV_HILOGE(INNERKIT, "Write descriptor failed!");
        return devicestatusData;
    }

    DEVICESTATUS_WRITE_PARCEL_WITH_RET(data, Int32, type, devicestatusData);

    int32_t ret = remote->SendRequest(static_cast<int32_t>(Idevicestatus::DEVICESTATUS_GETCACHE), data, reply, option);
    if (ret != ERR_OK) {
        DEV_HILOGE(INNERKIT, "SendRequest is failed, error code: %{public}d", ret);
        return devicestatusData;
    }

    int32_t devicestatusType = -1;
    int32_t devicestatusValue = -1;
    DEVICESTATUS_READ_PARCEL_WITH_RET(reply, Int32, devicestatusType, devicestatusData);
    DEVICESTATUS_READ_PARCEL_WITH_RET(reply, Int32, devicestatusValue, devicestatusData);
    devicestatusData.type = Type(devicestatusType);
    devicestatusData.value = OnChangedValue(devicestatusValue);
    DEV_HILOGD(INNERKIT, "type: %{public}d, value: %{public}d", devicestatusData.type, devicestatusData.value);
    DEV_HILOGD(INNERKIT, "Exit");
    return devicestatusData;
}
} // namespace DeviceStatus
} // Msdp
} // OHOS
