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

#include "coordination_softbus_adapter.h"

#include <chrono>
#include <thread>

#include "softbus_bus_center.h"
#include "softbus_common.h"

#include "device_coordination_softbus_define.h"
#include "devicestatus_define.h"
#include "coordination_sm.h"
#include "coordination_util.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "CoordinationSoftbusAdapter" };
std::shared_ptr<CoordinationSoftbusAdapter> g_instance = nullptr;
constexpr int32_t DINPUT_LINK_TYPE_MAX = 4;
const SessionAttribute g_sessionAttr = {
    .dataType = SessionType::TYPE_BYTES,
    .linkTypeNum = DINPUT_LINK_TYPE_MAX,
    .linkType = {
        LINK_TYPE_WIFI_WLAN_2G,
        LINK_TYPE_WIFI_WLAN_5G,
        LINK_TYPE_WIFI_P2P,
        LINK_TYPE_BR
    }
};

void ResponseStartRemoteCoordination(int32_t sessionId, const JsonParser& parser)
{
    CALL_DEBUG_ENTER;
    cJSON* deviceId = cJSON_GetObjectItemCaseSensitive(parser.json_, FI_SOFTBUS_KEY_LOCAL_DEVICE_ID);
    cJSON* buttonIsPressed = cJSON_GetObjectItemCaseSensitive(parser.json_, FI_SOFTBUS_POINTER_BUTTON_IS_PRESS);
    if (!cJSON_IsString(deviceId) || !cJSON_IsBool(buttonIsPressed)) {
        FI_HILOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ONPREPARE, data type is error");
        return;
    }
    CooSM->StartRemoteCoordination(deviceId->valuestring, cJSON_IsTrue(buttonIsPressed));
}

void ResponseStartRemoteCoordinationResult(int32_t sessionId, const JsonParser& parser)
{
    CALL_DEBUG_ENTER;
    cJSON* result = cJSON_GetObjectItemCaseSensitive(parser.json_, FI_SOFTBUS_KEY_RESULT);
    cJSON* dhid = cJSON_GetObjectItemCaseSensitive(parser.json_, FI_SOFTBUS_KEY_START_DHID);
    cJSON* x = cJSON_GetObjectItemCaseSensitive(parser.json_, FI_SOFTBUS_KEY_POINTER_X);
    cJSON* y = cJSON_GetObjectItemCaseSensitive(parser.json_, FI_SOFTBUS_KEY_POINTER_Y);
    if (!cJSON_IsBool(result) || !cJSON_IsString(dhid) || !cJSON_IsNumber(x) || !cJSON_IsNumber(y)) {
        FI_HILOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ONPREPARE, data type is error");
        return;
    }
    CooSM->StartRemoteCoordinationResult(cJSON_IsTrue(result), dhid->valuestring, x->valueint, y->valueint);
}

void ResponseStopRemoteCoordination(int32_t sessionId, const JsonParser& parser)
{
    CooSM->StopRemoteCoordination();
}

void ResponseStopRemoteCoordinationResult(int32_t sessionId, const JsonParser& parser)
{
    CALL_DEBUG_ENTER;
    cJSON* result = cJSON_GetObjectItemCaseSensitive(parser.json_, FI_SOFTBUS_KEY_RESULT);

    if (!cJSON_IsBool(result)) {
        FI_HILOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ONPREPARE, data type is error");
        return;
    }
    CooSM->StopRemoteCoordinationResult(cJSON_IsTrue(result));
}

void ResponseStartCoordinationOtherResult(int32_t sessionId, const JsonParser& parser)
{
    CALL_DEBUG_ENTER;
    cJSON* deviceId = cJSON_GetObjectItemCaseSensitive(parser.json_, FI_SOFTBUS_KEY_OTHER_DEVICE_ID);

    if (!cJSON_IsString(deviceId)) {
        FI_HILOGE("OnBytesReceived cmdType is TRANS_SINK_MSG_ONPREPARE, data type is error");
        return;
    }
    CooSM->StartCoordinationOtherResult(deviceId->valuestring);
}
} // namespace

static int32_t SessionOpened(int32_t sessionId, int32_t result)
{
    return CooSoftbusAdapter->OnSessionOpened(sessionId, result);
}

static void SessionClosed(int32_t sessionId)
{
    CooSoftbusAdapter->OnSessionClosed(sessionId);
}

static void BytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    CooSoftbusAdapter->OnBytesReceived(sessionId, data, dataLen);
}

static void MessageReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    (void)sessionId;
    (void)data;
    (void)dataLen;
}

static void StreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
    const StreamFrameInfo *param)
{
    (void)sessionId;
    (void)data;
    (void)ext;
    (void)param;
}

int32_t CoordinationSoftbusAdapter::Init()
{
    CALL_INFO_TRACE;
    sessListener_ = {
        .OnSessionOpened = SessionOpened,
        .OnSessionClosed = SessionClosed,
        .OnBytesReceived = BytesReceived,
        .OnMessageReceived = MessageReceived,
        .OnStreamReceived = StreamReceived
    };
    std::string networkId = COORDINATION::GetLocalDeviceId();
    if (networkId.empty()) {
        FI_HILOGE("Local networkid is empty");
        return RET_ERR;
    }
    localSessionName_ = SESSION_NAME + networkId.substr(0, INTERCEPT_STRING_LENGTH);
    int32_t ret = CreateSessionServer(FI_PKG_NAME, localSessionName_.c_str(), &sessListener_);
    if (ret != RET_OK) {
        FI_HILOGE("Create session server failed, error code:%{public}d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

CoordinationSoftbusAdapter::~CoordinationSoftbusAdapter()
{
    Release();
}

void CoordinationSoftbusAdapter::Release()
{
    CALL_INFO_TRACE;
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    std::for_each(sessionDevMap_.begin(), sessionDevMap_.end(), [](auto item) {
        CloseSession(item.second);
        FI_HILOGD("Close session success");
    });
    int32_t ret = RemoveSessionServer(FI_PKG_NAME, localSessionName_.c_str());
    FI_HILOGD("RemoveSessionServer ret:%{public}d", ret);
    sessionDevMap_.clear();
    channelStatusMap_.clear();
}

bool CoordinationSoftbusAdapter::CheckDeviceSessionState(const std::string &devId)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    if (sessionDevMap_.find(devId) == sessionDevMap_.end()) {
        FI_HILOGE("Check session state error");
        return false;
    }
    return true;
}

int32_t CoordinationSoftbusAdapter::OpenInputSoftbus(const std::string &remoteDevId)
{
    CALL_INFO_TRACE;
    if (CheckDeviceSessionState(remoteDevId)) {
        FI_HILOGD("Softbus session has already  opened");
        return RET_OK;
    }

    int32_t ret = Init();
    if (ret != RET_OK) {
        FI_HILOGE("Init failed");
        return RET_ERR;
    }

    std::string peerSessionName = SESSION_NAME + remoteDevId.substr(0, INTERCEPT_STRING_LENGTH);
    int32_t sessionId = OpenSession(localSessionName_.c_str(), peerSessionName.c_str(), remoteDevId.c_str(),
        GROUP_ID.c_str(), &g_sessionAttr);
    if (sessionId < 0) {
        FI_HILOGE("OpenSession failed");
        return RET_ERR;
    }
    return WaitSessionOpend(remoteDevId, sessionId);
}

int32_t CoordinationSoftbusAdapter::WaitSessionOpend(const std::string &remoteDevId, int32_t sessionId)
{
    CALL_INFO_TRACE;
    std::unique_lock<std::mutex> waitLock(operationMutex_);
    sessionDevMap_[remoteDevId] = sessionId;
    auto status = openSessionWaitCond_.wait_for(waitLock, std::chrono::seconds(SESSION_WAIT_TIMEOUT_SECOND),
        [this, remoteDevId] () { return channelStatusMap_[remoteDevId]; });
    if (!status) {
        FI_HILOGE("OpenSession timeout");
        return RET_ERR;
    }
    channelStatusMap_[remoteDevId] = false;
    return RET_OK;
}

void CoordinationSoftbusAdapter::CloseInputSoftbus(const std::string &remoteDevId)
{
    CALL_INFO_TRACE;
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    if (sessionDevMap_.find(remoteDevId) == sessionDevMap_.end()) {
        FI_HILOGI("SessionDevIdMap not find");
        return;
    }
    int32_t sessionId = sessionDevMap_[remoteDevId];

    CloseSession(sessionId);
    sessionDevMap_.erase(remoteDevId);
    channelStatusMap_.erase(remoteDevId);
}

std::shared_ptr<CoordinationSoftbusAdapter> CoordinationSoftbusAdapter::GetInstance()
{
    static std::once_flag flag;
    std::call_once(flag, [&]() {
        g_instance.reset(new (std::nothrow) CoordinationSoftbusAdapter());
    });
    return g_instance;
}

int32_t CoordinationSoftbusAdapter::StartRemoteCoordination(const std::string &localDeviceId,
    const std::string &remoteDeviceId)
{
    CALL_DEBUG_ENTER;
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    if (sessionDevMap_.find(remoteDeviceId) == sessionDevMap_.end()) {
        FI_HILOGE("Start remote coordination error, not find this device");
        return RET_ERR;
    }
    int32_t sessionId = sessionDevMap_[remoteDeviceId];
    auto pointerEvent = CooSM->GetLastPointerEvent();
    CHKPR(pointerEvent, RET_ERR);
    bool isPointerButtonPressed =
        (pointerEvent->GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN) ? true : false;
    cJSON *jsonStr = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_CMD_TYPE, cJSON_CreateNumber(REMOTE_COORDINATION_START));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_LOCAL_DEVICE_ID, cJSON_CreateString(localDeviceId.c_str()));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_SESSION_ID, cJSON_CreateNumber(sessionId));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_POINTER_BUTTON_IS_PRESS, cJSON_CreateBool(isPointerButtonPressed));
    char *smsg = cJSON_Print(jsonStr);
    cJSON_Delete(jsonStr);
    int32_t ret = SendDataPacket(sessionId, DataType::DATA_TYPE_COORDINATION, smsg, strlen(smsg));
    cJSON_free(smsg);
    return ret;
}

int32_t CoordinationSoftbusAdapter::StartRemoteCoordinationResult(const std::string &remoteDeviceId,
    bool isSuccess, const std::string &startDhid, int32_t xPercent, int32_t yPercent)
{
    CALL_DEBUG_ENTER;
    FI_HILOGE("*** %{public}d, %{public}s, %{public}d, %{public}d ***",
        (int)isSuccess, startDhid.c_str(), xPercent, yPercent);
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    if (sessionDevMap_.find(remoteDeviceId) == sessionDevMap_.end()) {
        FI_HILOGE("Stop remote coordination error, not find this device");
        return RET_ERR;
    }
    int32_t sessionId = sessionDevMap_[remoteDeviceId];
    cJSON *jsonStr = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_CMD_TYPE, cJSON_CreateNumber(REMOTE_COORDINATION_START_RES));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_RESULT, cJSON_CreateBool(isSuccess));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_START_DHID, cJSON_CreateString(startDhid.c_str()));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_POINTER_X, cJSON_CreateNumber(xPercent));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_POINTER_Y, cJSON_CreateNumber(yPercent));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_SESSION_ID, cJSON_CreateNumber(sessionId));
    char *smsg = cJSON_Print(jsonStr);
    cJSON_Delete(jsonStr);
    int32_t ret = SendDataPacket(sessionId, DataType::DATA_TYPE_COORDINATION, smsg, strlen(smsg));
    cJSON_free(smsg);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    if (isSuccess && DragAdpt->IsDragging()) {
        return StartDrag(sessionId);
    }
    return RET_OK;
}

int32_t CoordinationSoftbusAdapter::StartDrag(int32_t sessionId)
{
    uint8_t dstPixels[14400] = { 0 };
    dstPixels[1] = 1;
    dstPixels[1111] = 2;
    dstPixels[14399] = 3;
    DragInfo dragInfo;
    dragInfo.pixelFormat = 1;
    dragInfo.alphaType = 2;
    dragInfo.width = 3;
    dragInfo.height = 4;
    dragInfo.allocatorType = 5;
    dragInfo.dragState = 6;
    dragInfo.num = 7;
    dragInfo.buffer[1] = 8;
    dragInfo.buffer[255] = 9;
    dragInfo.buffer[511] = 10;

    DataPacket* dataPacket = (DataPacket*)malloc(sizeof(DataPacket) + 14400);
    if (dataPacket == nullptr) {
        FI_HILOGE("Malloc failed");
        return RET_ERR;
    }
    dataPacket->dragInfo = dragInfo;
    dataPacket->dataType = DataType::DATA_TYPE_DRAG;
    dataPacket->dataLen = 14400;
    errno_t ret = memcpy_s(dataPacket->data, dataPacket->dataLen, dstPixels, dataPacket->dataLen);
    if (ret != EOK) {
        FI_HILOGE("memcpy_s failed");
        free(dataPacket);
        return RET_ERR;
    }
    int32_t result = SendMsg(sessionId, dataPacket, sizeof(DataPacket) + 14400);
    free(dataPacket);
    if (result != RET_OK) {
        FI_HILOGE("Start remote coordination result send session msg failed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t CoordinationSoftbusAdapter::StopRemoteCoordination(const std::string &remoteDeviceId)
{
    CALL_DEBUG_ENTER;
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    if (sessionDevMap_.find(remoteDeviceId) == sessionDevMap_.end()) {
        FI_HILOGE("Stop remote coordination error, not find this device");
        return RET_ERR;
    }
    int32_t sessionId = sessionDevMap_[remoteDeviceId];
    cJSON *jsonStr = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_CMD_TYPE, cJSON_CreateNumber(REMOTE_COORDINATION_STOP));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_SESSION_ID, cJSON_CreateNumber(sessionId));
    char *smsg = cJSON_Print(jsonStr);
    cJSON_Delete(jsonStr);
    int32_t ret = SendDataPacket(sessionId, DataType::DATA_TYPE_COORDINATION, smsg, strlen(smsg));
    cJSON_free(smsg);
    return ret;
}

int32_t CoordinationSoftbusAdapter::StopRemoteCoordinationResult(const std::string &remoteDeviceId,
    bool isSuccess)
{
    CALL_DEBUG_ENTER;
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    if (sessionDevMap_.find(remoteDeviceId) == sessionDevMap_.end()) {
        FI_HILOGE("Stop remote coordination result error, not find this device");
        return RET_ERR;
    }
    int32_t sessionId = sessionDevMap_[remoteDeviceId];
    cJSON *jsonStr = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_CMD_TYPE, cJSON_CreateNumber(REMOTE_COORDINATION_STOP_RES));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_RESULT, cJSON_CreateBool(isSuccess));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_SESSION_ID, cJSON_CreateNumber(sessionId));
    char *smsg = cJSON_Print(jsonStr);
    cJSON_Delete(jsonStr);
    int32_t ret = SendDataPacket(sessionId, DataType::DATA_TYPE_COORDINATION, smsg, strlen(smsg));
    cJSON_free(smsg);
    return ret;
}

int32_t CoordinationSoftbusAdapter::StartCoordinationOtherResult(const std::string &remoteDeviceId,
    const std::string &srcNetworkId)
{
    CALL_DEBUG_ENTER;
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    if (sessionDevMap_.find(remoteDeviceId) == sessionDevMap_.end()) {
        FI_HILOGE("Start coordination other result error, not find this device");
        return RET_ERR;
    }
    int32_t sessionId = sessionDevMap_[remoteDeviceId];
    cJSON *jsonStr = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_CMD_TYPE, cJSON_CreateNumber(REMOTE_COORDINATION_STOP_OTHER_RES));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_OTHER_DEVICE_ID, cJSON_CreateString(srcNetworkId.c_str()));
    cJSON_AddItemToObject(jsonStr, FI_SOFTBUS_KEY_SESSION_ID, cJSON_CreateNumber(sessionId));
    char *smsg = cJSON_Print(jsonStr);
    cJSON_Delete(jsonStr);
    int32_t ret = SendDataPacket(sessionId, DataType::DATA_TYPE_COORDINATION, smsg, strlen(smsg));
    cJSON_free(smsg);
    return ret;
}

void CoordinationSoftbusAdapter::HandleSessionData(int32_t sessionId, const std::string& message)
{
    JsonParser parser;
    parser.json_ = cJSON_Parse(message.c_str());
    if (!cJSON_IsObject(parser.json_)) {
        FI_HILOGE("Parser.json_ is not object");
        return;
    }
    cJSON* comType = cJSON_GetObjectItemCaseSensitive(parser.json_, FI_SOFTBUS_KEY_CMD_TYPE);
    if (!cJSON_IsNumber(comType)) {
        FI_HILOGE("OnBytesReceived cmdType is not number type");
        return;
    }
    FI_HILOGD("valueint: %{public}d", comType->valueint);
    switch (comType->valueint) {
        case REMOTE_COORDINATION_START: {
            ResponseStartRemoteCoordination(sessionId, parser);
            break;
        }
        case REMOTE_COORDINATION_START_RES: {
            ResponseStartRemoteCoordinationResult(sessionId, parser);
            break;
        }
        case REMOTE_COORDINATION_STOP: {
            ResponseStopRemoteCoordination(sessionId, parser);
            break;
        }
        case REMOTE_COORDINATION_STOP_RES: {
            ResponseStopRemoteCoordinationResult(sessionId, parser);
            break;
        }
        case REMOTE_COORDINATION_STOP_OTHER_RES: {
            ResponseStartCoordinationOtherResult(sessionId, parser);
            break;
        }
        default: {
            FI_HILOGE("OnBytesReceived cmdType is undefined");
            break;
        }
    }
}

void CoordinationSoftbusAdapter::OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    FI_HILOGD("*** sessionId:%{public}d, dataLen:%{public}d", sessionId, dataLen);
    if (sessionId < 0 || data == nullptr || dataLen <= 0) {
        FI_HILOGE("Param check failed");
        return;
    }
    const DataPacket* dataPacket = static_cast<const DataPacket *>(data);
    dataPacket->msessageID;
    dataPacket->data;
    fun(dataPacket->data, dataPacket->datelen);

    FI_HILOGD("dataType:%{public}d", dataPacket->dataType);
    if (dataPacket->dataType == DataType::DATA_TYPE_COORDINATION) {
        std::string message = std::string(static_cast<const char *>(dataPacket->data), dataPacket->dataLen);
        HandleSessionData(sessionId, message);
    } else if (dataPacket->dataType == DataType::DATA_TYPE_DRAG) {
        const uint8_t *dstPixels = reinterpret_cast<const uint8_t *>(dataPacket->data);
        int num1 = *(dstPixels + 1);
        int num2 = *(dstPixels + 1111);
        int num3 = *(dstPixels + 14399);
        FI_HILOGE("*** %{public}zu, %{public}d, %{public}d, %{public}d, %{public}d, %{public}d, %{public}d, "
            "%{public}d, %{public}d, %{public}d, %{public}d, %{public}d, %{public}d, %{public}d ***",
            dataPacket->dataLen, num1, num2, num3, dataPacket->dragInfo.pixelFormat, dataPacket->dragInfo.alphaType,
            dataPacket->dragInfo.width, dataPacket->dragInfo.height, dataPacket->dragInfo.allocatorType,
            dataPacket->dragInfo.dragState, dataPacket->dragInfo.num, dataPacket->dragInfo.buffer[1],
            dataPacket->dragInfo.buffer[255], dataPacket->dragInfo.buffer[511]);
        setDrapData();
    }
}

int32_t CoordinationSoftbusAdapter::SendDataPacket(int32_t sessionId, DataType dataType, void* src, size_t count)
{
    DataPacket* dataPacket = (DataPacket*)malloc(sizeof(DataPacket) + count);
    if (dataPacket == nullptr) {
        FI_HILOGE("Malloc failed");
        return RET_ERR;
    }
    dataPacket->dataType = dataType;
    dataPacket->dataLen = count;
    errno_t ret = memcpy_s(dataPacket->data, dataPacket->dataLen, src, dataPacket->dataLen);
    if (ret != EOK) {
        FI_HILOGE("memcpy_s failed");
        free(dataPacket);
        return RET_ERR;
    }
    int32_t result = SendMsg(sessionId, dataPacket, sizeof(DataPacket) + count);
    free(dataPacket);
    if (result != RET_OK) {
        FI_HILOGE("Start remote coordination result send session msg failed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t CoordinationSoftbusAdapter::SendMsg(int32_t sessionId, const DataPacket* dataPacket, size_t size)
{
    CALL_DEBUG_ENTER;
    FI_HILOGD("*** sessionId:%{public}d, dataLen:%{public}d", sessionId, dataPacket->dataLen);
    if (dataPacket->dataType == DataType::DATA_TYPE_COORDINATION && dataPacket->dataLen > MSG_MAX_SIZE) {
        FI_HILOGW("error:message.size() > MSG_MAX_SIZE message size:%{public}zu", dataPacket->dataLen);
        return RET_ERR;
    }
    FI_HILOGD("dataType:%{public}d", dataPacket->dataType);
    if (dataPacket->dataType == DataType::DATA_TYPE_DRAG) {
        char *pData = const_cast<char *>(dataPacket->data);
        uint8_t *dstPixels = reinterpret_cast<uint8_t *>(pData);
        int num1 = *(dstPixels + 1);
        int num2 = *(dstPixels + 1111);
        int num3 = *(dstPixels + 14399);
        FI_HILOGE("*** %{public}zu, %{public}d, %{public}d, %{public}d ***", dataPacket->dataLen,
            num1, num2, num3);
    }
    //return SendBytes(sessionId, message.c_str(), strlen(message.c_str()));
    return SendBytes(sessionId, dataPacket, size);
}

std::string CoordinationSoftbusAdapter::FindDevice(int32_t sessionId)
{
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    auto find_item = std::find_if(sessionDevMap_.begin(), sessionDevMap_.end(),
        [sessionId](const std::map<std::string, int>::value_type item) {
        return item.second == sessionId;
    });
    if (find_item == sessionDevMap_.end()) {
        FI_HILOGE("FindDevice error");
        return {};
    }
    return find_item->first;
}

int32_t CoordinationSoftbusAdapter::OnSessionOpened(int32_t sessionId, int32_t result)
{
    CALL_INFO_TRACE;
    char peerDevId[DEVICE_ID_SIZE_MAX] = {};
    int32_t getPeerDeviceIdResult = GetPeerDeviceId(sessionId, peerDevId, sizeof(peerDevId));
    FI_HILOGD("Get peer device id ret:%{public}d", getPeerDeviceIdResult);
    if (result != RET_OK) {
        std::string deviceId = FindDevice(sessionId);
        FI_HILOGE("Session open failed result:%{public}d", result);
        std::unique_lock<std::mutex> sessionLock(operationMutex_);
        if (sessionDevMap_.find(deviceId) != sessionDevMap_.end()) {
            sessionDevMap_.erase(deviceId);
        }
        if (getPeerDeviceIdResult == RET_OK) {
            channelStatusMap_[peerDevId] = true;
        }
        openSessionWaitCond_.notify_all();
        return RET_OK;
    }

    int32_t sessionSide = GetSessionSide(sessionId);
    FI_HILOGI("session open succeed, sessionId:%{public}d, sessionSide:%{public}d(1 is client side)",
        sessionId, sessionSide);
    std::lock_guard<std::mutex> notifyLock(operationMutex_);
    if (sessionSide == SESSION_SIDE_SERVER) {
        if (getPeerDeviceIdResult == RET_OK) {
            sessionDevMap_[peerDevId] = sessionId;
        }
    } else {
        if (getPeerDeviceIdResult == RET_OK) {
            channelStatusMap_[peerDevId] = true;
        }
        openSessionWaitCond_.notify_all();
    }
    return RET_OK;
}

void CoordinationSoftbusAdapter::OnSessionClosed(int32_t sessionId)
{
    CALL_DEBUG_ENTER;
    std::string deviceId = FindDevice(sessionId);
    std::unique_lock<std::mutex> sessionLock(operationMutex_);
    if (sessionDevMap_.find(deviceId) != sessionDevMap_.end()) {
        sessionDevMap_.erase(deviceId);
    }
    if (GetSessionSide(sessionId) != 0) {
        channelStatusMap_.erase(deviceId);
    }
    CooSM->Reset(deviceId);
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
