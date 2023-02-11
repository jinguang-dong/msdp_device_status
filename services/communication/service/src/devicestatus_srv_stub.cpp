/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "devicestatus_srv_stub.h"

#include "message_parcel.h"
#include "pixel_map.h"

#include "devicestatus_callback_proxy.h"
#include "devicestatus_common.h"
#include "devicestatus_data_utils.h"
#include "devicestatus_define.h"
#include "devicestatus_service.h"
#include "devicestatus_srv_proxy.h"
#include "fi_log.h"
#include "idevicestatus_callback.h"
#include "util.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "DeviceStatusSrvStub" };
} // namespace

int32_t DeviceStatusSrvStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DEV_HILOGD(SERVICE, "cmd = %{public}d, flags = %{public}d", code, option.GetFlags());
    std::u16string descriptor = DeviceStatusSrvStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        DEV_HILOGE(SERVICE, "DeviceStatusSrvStub::OnRemoteRequest failed, descriptor is not matched");
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
        case REGISTER_COORDINATION_MONITOR: {
            return RegisterCoordinationMonitorStub(data, reply);
        }
        case UNREGISTER_COORDINATION_MONITOR: {
            return UnregisterCoordinationMonitorStub(data, reply);
        }
        case ENABLE_COORDINATION: {
            return EnableCoordinationStub(data, reply);
        }
        case START_COORDINATION: {
            return StartCoordinationStub(data, reply);
        }
        case STOP_COORDINATION: {
            return StopCoordinationStub(data, reply);
        }
        case GET_COORDINATION_STATE: {
            return GetCoordinationStateStub(data, reply);
        }
        case ALLOC_SOCKET_FD: {
            return HandleAllocSocketFdStub(data, reply);
        }
        case START_DRAG : {
            return StartDragStub(data, reply);
        }
        case STOP_DRAG : {
            return StopDragStub(data, reply);
        }
        case UPDATED_DRAG_STYLE: {
            return UpdateDragStyleStub(data, reply);
        }
        case UPDATED_DRAG_MESSAGE: {
            return UpdateDragMessageStub(data, reply);
        }
        case GET_DRAG_TARGET_PID: {
            return GetDragTargetPidStub(data, reply);
        }
        case REGISTER_THUMBNAIL_DRAW: {
            return RegisterThumbnailDrawStub(data, reply);
        }
        case UNREGISTER_THUMBNAIL_DRAW: {
            return UnregisterThumbnailDrawStub(data, reply);
        }
        case REGISTER_DRAG_MONITOR: {
            return AddDraglistenerStub(data, reply);
        }
        case UNREGISTER_DRAG_MONITOR: {
            return RemoveDraglistenerStub(data, reply);
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return RET_OK;
}

int32_t DeviceStatusSrvStub::SubscribeStub(MessageParcel& data)
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
    return RET_OK;
}

int32_t DeviceStatusSrvStub::UnsubscribeStub(MessageParcel& data)
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
    return RET_OK;
}

int32_t DeviceStatusSrvStub::GetLatestDeviceStatusDataStub(MessageParcel& data, MessageParcel& reply)
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
    return RET_OK;
}

int32_t DeviceStatusSrvStub::RegisterCoordinationMonitorStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t ret = RegisterCoordinationListener();
    if (ret != RET_OK) {
        FI_HILOGE("Call RegisterCoordinationEvent failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t DeviceStatusSrvStub::UnregisterCoordinationMonitorStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t ret = UnregisterCoordinationListener();
    if (ret != RET_OK) {
        FI_HILOGE("Call RegisterCoordinationEvent failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t DeviceStatusSrvStub::EnableCoordinationStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t userData;
    bool enabled;
    READINT32(data, userData, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READBOOL(data, enabled, E_DEVICESTATUS_READ_PARCEL_ERROR);
    int32_t ret = EnableCoordination(userData, enabled);
    if (ret != RET_OK) {
        FI_HILOGE("Call RegisterCoordinationEvent failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t DeviceStatusSrvStub::StartCoordinationStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t userData;
    READINT32(data, userData, E_DEVICESTATUS_READ_PARCEL_ERROR);
    std::string sinkDeviceId;
    READSTRING(data, sinkDeviceId, E_DEVICESTATUS_READ_PARCEL_ERROR);
    int32_t srcDeviceId;
    READINT32(data, srcDeviceId, E_DEVICESTATUS_READ_PARCEL_ERROR);
    int32_t ret = StartCoordination(userData, sinkDeviceId, srcDeviceId);
    if (ret != RET_OK) {
        FI_HILOGE("Call StartCoordination failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t DeviceStatusSrvStub::StopCoordinationStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t userData;
    READINT32(data, userData, E_DEVICESTATUS_READ_PARCEL_ERROR);
    int32_t ret = StopCoordination(userData);
    if (ret != RET_OK) {
        FI_HILOGE("Call RegisterCoordinationEvent failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t DeviceStatusSrvStub::GetCoordinationStateStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t userData;
    READINT32(data, userData, E_DEVICESTATUS_READ_PARCEL_ERROR);
    std::string deviceId;
    READSTRING(data, deviceId, E_DEVICESTATUS_READ_PARCEL_ERROR);
    int32_t ret = GetCoordinationState(userData, deviceId);
    if (ret != RET_OK) {
        FI_HILOGE("Call RegisterCoordinationEvent failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t DeviceStatusSrvStub::UpdateDragStyleStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t style;
    READINT32(data, style, E_DEVICESTATUS_READ_PARCEL_ERROR);
    int32_t ret = UpdateDragStyle(style);
    if (ret != RET_OK) {
        FI_HILOGE("Call UpdateDragStyle failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t DeviceStatusSrvStub::UpdateDragMessageStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    std::u16string message;
    READSTRING16(data, message, E_DEVICESTATUS_READ_PARCEL_ERROR);
    int32_t ret = UpdateDragMessage(message);
    if (ret != RET_OK) {
        FI_HILOGE("Call UpdateDragMessage failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t DeviceStatusSrvStub::GetDragTargetPidStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t pid = GetDragTargetPid();
    WRITEINT32(reply, pid, IPC_STUB_WRITE_PARCEL_ERR);
    return RET_OK;
}

int32_t DeviceStatusSrvStub::RegisterThumbnailDrawStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t ret = RegisterThumbnailDraw();
    if (ret != RET_OK) {
        FI_HILOGE("Call RegisterThumbnailDraw failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t DeviceStatusSrvStub::UnregisterThumbnailDrawStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t ret = UnregisterThumbnailDraw();
    if (ret != RET_OK) {
        FI_HILOGE("Call UnregisterThumbnailDraw failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t DeviceStatusSrvStub::HandleAllocSocketFdStub(MessageParcel& data, MessageParcel& reply)
{
    int32_t pid = GetCallingPid();
    if (!IsRunning()) {
        FI_HILOGE("Service is not running. pid:%{public}d, go switch default", pid);
        return SERVICE_NOT_RUNNING;
    }
    int32_t moduleId;
    READINT32(data, moduleId, E_DEVICESTATUS_READ_PARCEL_ERROR);
    std::string clientName;
    READSTRING(data, clientName, E_DEVICESTATUS_READ_PARCEL_ERROR);

    int32_t clientFd = -1;
    uint32_t tokenId = GetCallingTokenID();
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(tokenId);
    int32_t ret = AllocSocketFd(clientName, moduleId, clientFd, tokenType);
    if (ret != RET_OK) {
        FI_HILOGE("AllocSocketFd failed pid:%{public}d, go switch default", pid);
        if (clientFd >= 0) {
            close(clientFd);
        }
        return ret;
    }

    if (!reply.WriteFileDescriptor(clientFd)) {
        FI_HILOGE("Write file descriptor failed");
        close(clientFd);
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    WRITEINT32(reply, tokenType, IPC_STUB_WRITE_PARCEL_ERR);
    FI_HILOGD("Send clientFd to client, clientFd:%{public}d, tokenType:%{public}d", clientFd, tokenType);
    close(clientFd);
    return RET_OK;
}

int32_t DeviceStatusSrvStub::StartDragStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    auto pixelMap = OHOS::Media::PixelMap::Unmarshalling(data);
    CHKPR(pixelMap, RET_ERR);
    DragData dragData;
    dragData.pixelMap = std::shared_ptr<OHOS::Media::PixelMap> (pixelMap);
    if (dragData.pixelMap->GetWidth() > MAX_PIXEL_MAP_WIDTH ||
        dragData.pixelMap->GetHeight() > MAX_PIXEL_MAP_HEIGHT) {
        FI_HILOGE("Too big pixelMap, width:%{public}d, height:%{public}d",
            dragData.pixelMap->GetWidth(), dragData.pixelMap->GetHeight());
        return RET_ERR;
    }
    READINT32(data, dragData.x, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READINT32(data, dragData.y, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READUINT8VECTOR(data, dragData.buffer, E_DEVICESTATUS_READ_PARCEL_ERROR);
    if (dragData.buffer.size() > MAX_BUFFER_SIZE) {
        FI_HILOGE("Invalid buffer, bufferSize:%{public}zu", dragData.buffer.size());
        return RET_ERR;
    }
    READINT32(data, dragData.sourceType, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READINT32(data, dragData.dragNum, E_DEVICESTATUS_READ_PARCEL_ERROR);
    int32_t ret = StartDrag(dragData);
    if (ret != RET_OK) {
        FI_HILOGE("Call StartDrag failed ret:%{public}d", ret);
    }
    WRITEINT32(reply, ret, IPC_STUB_WRITE_PARCEL_ERR);
    return ret;
}

int32_t DeviceStatusSrvStub::StopDragStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t result;
    READINT32(data, result, E_DEVICESTATUS_READ_PARCEL_ERROR);
    int32_t ret = StopDrag(result);
    if (ret != RET_OK) {
        FI_HILOGE("Call StopDrag failed ret:%{public}d", ret);
    }
    WRITEINT32(reply, ret, IPC_STUB_WRITE_PARCEL_ERR);
    return ret;
}

int32_t DeviceStatusSrvStub::AddDraglistenerStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t ret = AddDraglistener();
    if (ret != RET_OK) {
        FI_HILOGE("Call AddDraglistener failed ret:%{public}d", ret);
    }
    return ret;
}

int32_t DeviceStatusSrvStub::RemoveDraglistenerStub(MessageParcel& data, MessageParcel& reply)
{
    CALL_DEBUG_ENTER;
    int32_t ret = RemoveDraglistener();
    if (ret != RET_OK) {
        FI_HILOGE("Call RemoveDraglistener failed ret:%{public}d", ret);
    }
    return ret;
}
} // namespace DeviceStatus
} // Msdp
} // OHOS
