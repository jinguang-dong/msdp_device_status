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

#include "across_device_drag.h"

// #include<string>

#ifdef OHOS_BUILD_ENABLE_COORDINATION
#include "coordination_softbus_adapter.h"
#include "coordination_device_manager.h"
// #include "coordination_sm.h"
// #include "coordination_util.h"
#endif // OHOS_BUILD_ENABLE_COORDINATION
#include "drag_data_adapter.h"
#include "devicestatus_define.h"
#include "fi_log.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "AcrossDeviceDrag" };
// inline constexpr int64_t DEVICE_ID_SIZE_MAX = 1000;
inline constexpr int64_t MAX_SIZE = 512;
} // namespace

AcrossDeviceDrag::AcrossDeviceDrag()
{
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    CooSoftbusAdapter->RegisterRecvFunc(CoordinationSoftbusAdapter::DRAGING_DATA,
        std::bind(&AcrossDeviceDrag::RecvDragingData, this, std::placeholders::_1, std::placeholders::_2));
    CooSoftbusAdapter->RegisterRecvFunc(CoordinationSoftbusAdapter::STOPDRAG_DATA,
        std::bind(&AcrossDeviceDrag::RecvStopDragData, this, std::placeholders::_1, std::placeholders::_2));
#endif // OHOS_BUILD_ENABLE_COORDINATION
}

int32_t AcrossDeviceDrag::Init(IContext *context)
{
    CALL_DEBUG_ENTER;
    CHKPR(context, RET_ERR);
    remotedeviceId_ = "";
    context_ = context;
    context_->GetDragManager().RegisterStateChange(std::bind(&AcrossDeviceDrag::DragStateChanged, this, std::placeholders::_1));
    return RET_OK;
}

void AcrossDeviceDrag::RecvDragingData(const void* data, uint32_t dataLen)
{
    CALL_DEBUG_ENTER;
    CHKPV(data);
    if (dataLen == 0) {
        FI_HILOGE("Recv data len is 0");
        return;
    }
    std::for_each(remoteDataMap_.begin(), remoteDataMap_.end(), [](auto item) {
        FI_HILOGI("remoteDataMap_ deviceId::%{public}s, data::%{public}d", (item.first).c_str(), item.second);
    });

    const DragInfo* dragInfo = static_cast<const DragInfo *>(data);
    if(dragInfo->dragState != static_cast<int32_t>(DragMessage::MSG_DRAG_STATE_START)) {
        FI_HILOGE("Drag state is not draging");
        //需要让未穿越的隐藏鼠标显示
        return;
    }
    // dragState_ = static_cast<DragMessage>(dragInfo->dragState);
    DragData dragData;
    DecodeDragData(dragInfo, dragData);
    //绘制
    context_->GetDragManager().SetAcross(true);
    if (context_->GetDragManager().StartDrag(dragData, nullptr) !=RET_OK) {
        FI_HILOGI("StartDrag failed");
        //需要让未穿越的隐藏鼠标显示
        return;
    }
    FI_HILOGI(" 66 remotedeviceId_:%{public}s", remotedeviceId_.c_str());
    if (remoteDataMap_.find(remotedeviceId_) == remoteDataMap_.end()) {
        FI_HILOGI("remoteDataMap not find");
        remoteDataMap_.insert(std::make_pair(remotedeviceId_, dragInfo->hasData));
        std::for_each(remoteDataMap_.begin(), remoteDataMap_.end(), [](auto item) {
            FI_HILOGI("222 remoteDataMap_ deviceId::%{public}s, data::%{public}d", (item.first).c_str(), item.second);
        });
    }
    remoteDataMap_[remotedeviceId_] = dragInfo->hasData;
    std::for_each(remoteDataMap_.begin(), remoteDataMap_.end(), [](auto item) {
        FI_HILOGI("33 remoteDataMap_ deviceId::%{public}s, data::%{public}d", (item.first).c_str(), item.second);
    });
    if (OHOS::MMI::InputManager::GetInstance()->IsPointerVisible()) {
        OHOS::MMI::InputManager::GetInstance()->SetPointerVisible(false);
    }
    context_->GetDragManager().OnSetDragWindowVisible(true);
    return;
}

void AcrossDeviceDrag::DecodeDragData(const DragInfo* dragInfo, DragData &dragData)
{
    CALL_DEBUG_ENTER;
    CHKPV(dragInfo);
    std::vector<uint8_t> buff(dragInfo->data, dragInfo->data + dragInfo->dataLen);
    Media::PixelMap *pixelMap = Media::PixelMap::DecodeTlv(buff);
    UpdateDragStateChanged(false);
    // 解析drag数据
    dragData.shadowInfo.pixelMap = std::shared_ptr<Media::PixelMap>(pixelMap);
    dragData.shadowInfo.x = dragInfo->offsetX;
    dragData.shadowInfo.y = dragInfo->offsetY;
    dragData.buffer.insert(dragData.buffer.begin(), dragInfo->buffer, dragInfo->buffer + MAX_SIZE);
    dragData.sourceType = dragInfo->sourceType;
    dragData.pointerId = dragInfo->pointerId;
    dragData.displayId = dragInfo->displayId;
    dragData.dragNum = dragInfo->dragNum;
    dragData.udKey = dragInfo->udKey;
    // [dragData.displayX, dragData.displayY](std::shared_ptr<MMI::PointerEvent> pointerEvent) {
    //     MMI::PointerEvent::PointerItem pointerItem;
    //     pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem);
    //     dragData.displayX = pointerItem.GetDisplayX();
    //     dragData.displayY = pointerItem.GetDisplayY();
    // };
    dragData.hasCanceledAnimation = dragInfo->hasCanceledAnimation;
    hasCanceledAnimation_ = dragInfo->hasCanceledAnimation;
    dragStyle_ = static_cast<const DragCursorStyle>(dragInfo->dragStyle);
    PrintDragData(dragData);
}

void AcrossDeviceDrag::PrintDragData(const DragData &dragData)
{
    FI_HILOGI("PixelFormat:%{public}d, PixelAlphaType:%{public}d, PixelAllocatorType:%{public}d, PixelWidth:%{public}d,"
        "PixelHeight:%{public}d, udKey:%{public}s, sourceType:%{public}d, pointerId:%{public}d, shadowX:%{public}d,"
        "shadowY:%{public}d, displayId:%{public}d, displayX:%{public}d, displayY:%{public}d, dragNum:%{public}d,"
        "hasCanceledAnimation:%{public}d", static_cast<int32_t>(dragData.shadowInfo.pixelMap->GetPixelFormat()),
        static_cast<int32_t>(dragData.shadowInfo.pixelMap->GetAlphaType()),
        static_cast<int32_t>(dragData.shadowInfo.pixelMap->GetAllocatorType()),
        dragData.shadowInfo.pixelMap->GetWidth(), dragData.shadowInfo.pixelMap->GetHeight(), 
        dragData.udKey.c_str(), dragData.sourceType, dragData.pointerId, dragData.shadowInfo.x, dragData.shadowInfo.y,
        dragData.displayId, dragData.displayX, dragData.displayY, dragData.dragNum, dragData.hasCanceledAnimation);
}

void AcrossDeviceDrag::RecvStopDragData(const void* data, uint32_t dataLen)
{
    CALL_DEBUG_ENTER;
    CHKPV(data);
    if (dataLen == 0) {
        FI_HILOGE("Recv data len is 0");
        return;
    }
    auto recvData = static_cast<const DragMessage *>(data);
    FI_HILOGI("recvData:%{public}d, dataLen: %{public}d", *recvData, dataLen);

    // 解析result数据
    auto result = DragResult::DRAG_SUCCESS;
    if (*recvData != DragMessage::MSG_DRAG_STATE_STOP) {
        result = DragResult::DRAG_FAIL;
    }
    if (remoteDataMap_.find(remotedeviceId_) != remoteDataMap_.end() && remoteDataMap_[remotedeviceId_]) {
        FI_HILOGI("Data on the peer device already exists");
        remoteDataMap_[remotedeviceId_] = false;
    }
    //
    context_->GetDragManager().StopDrag(result, hasCanceledAnimation_, true);//怎么添加判断本端还是对端
    DragReset();
    // ResetDragData();
    remotedeviceId_ = "";
    return;
}

int32_t AcrossDeviceDrag::SendDragingData()//角标变化的数据发送
{
    CALL_DEBUG_ENTER;
    if (context_->GetDragManager().GetDragState() != DragMessage::MSG_DRAG_STATE_START) {//dragState_改为dragdata.dragState_
        FI_HILOGE("Drag state is not draging");
        // ResetDragData();
        return RET_ERR;
    }
	auto dragData = DataAdapter.GetDragData();
    PrintDragData(dragData);
    if (dragData.sourceType != OHOS::MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        FI_HILOGE("Source type is not mouse");
        // ResetDragData();
        return RET_ERR;
    }
    auto pixelMap = dragData.shadowInfo.pixelMap;
    std::vector<uint8_t> buff;
    if (!pixelMap->EncodeTlv(buff)) {
        FI_HILOGE("Pixel map tlv encode fail");
        return RET_ERR;
    }
    auto size = buff.size();
    DragInfo* dragInfo = (DragInfo*)malloc(sizeof(DragInfo) + size);
    CHKPR(dragInfo, RET_ERR);
    FI_HILOGI("sizeof(DragInfo):%{public}d, size: %{public}d, sizeof(dragInfo)%{public}d", sizeof(DragInfo), size, sizeof(dragInfo));
    if (EncodeDragData(dragData, buff, dragInfo) != RET_OK) {
        FI_HILOGE("EncodeDragData fail");
        return RET_ERR;
    }
    CHKPR(dragInfo, RET_ERR);

    std::string stardeviceId;
    CooSM->GetStartId(stardeviceId);
    std::string remotedeviceId;
    CooSM->GetRemoteId(remotedeviceId);
    auto messageId = CoordinationSoftbusAdapter::DRAGING_DATA;
    FI_HILOGI("stardeviceId:%{public}s, remotedeviceId:%{public}s, messageId: %{public}d", stardeviceId.c_str(), remotedeviceId.c_str(), messageId);
    if (CooSoftbusAdapter->OpenInputSoftbus(remotedeviceId_) != RET_OK) {
        FI_HILOGE("Open input softbus fail");
        return static_cast<int32_t>(CoordinationMessage::COORDINATION_FAIL);
    }
    FI_HILOGI("remotedeviceId_:%{public}s", remotedeviceId_.c_str());
    std::for_each(remoteDataMap_.begin(), remoteDataMap_.end(), [](auto item) {
        FI_HILOGI("remoteDataMap_ deviceId::%{public}s, data::%{public}d", (item.first).c_str(), item.second);
    });
    if (CooSoftbusAdapter->SendData(remotedeviceId_, messageId, dragInfo, sizeof(DragInfo) + size) != RET_OK) {
        FI_HILOGE("SendData failed");
        return RET_ERR;
    }
    free(dragInfo);
    return RET_OK;
}

int32_t AcrossDeviceDrag::EncodeDragData(const DragData dragData, std::vector<uint8_t> buff, DragInfo* dragInfo)
{
    CALL_DEBUG_ENTER;
    CHKPR(dragInfo, RET_ERR);
    dragInfo->dragState = static_cast<int32_t>(context_->GetDragManager().GetDragState());//DragMessage::MSG_DRAG_STATE_START
    dragInfo->sourceType = static_cast<int32_t>(dragData.sourceType);//鼠标？触摸屏？
    dragInfo->pointerId = dragData.pointerId;
    dragInfo->displayId = dragData.displayId;
    dragInfo->dragNum = dragData.dragNum;
    dragInfo->dragStyle = static_cast<int32_t>(DataAdapter.GetDragStyle());
    dragInfo->offsetX = dragData.shadowInfo.x;
    dragInfo->offsetY = dragData.shadowInfo.y;
    // dragInfo->displayX = dragData.displayX;
    // dragInfo->displayY = dragData.displayY;
    dragInfo->hasCanceledAnimation = dragData.hasCanceledAnimation;
    hasCanceledAnimation_ = dragData.hasCanceledAnimation;
    dragInfo->udKey = dragData.udKey;
    dragInfo->hasData = true;
    if (context_->GetDragManager().GetUdKey(dragInfo->udKey) != RET_OK) {
        FI_HILOGE("memcpy_s failed");
    }
    errno_t retBuffer = memcpy_s(dragInfo->buffer, MAX_SIZE, &dragData.buffer[0], dragData.buffer.size());//拖拽元数据
    if (retBuffer != EOK) {
        FI_HILOGE("memcpy_s failed");
        free(dragInfo);//需要发送stop？
        return RET_ERR;
    }
    dragInfo->dataLen = buff.size();
    errno_t ret = memcpy_s(dragInfo->data, dragInfo->dataLen, &buff[0], buff.size());
    if (ret != EOK) {
        FI_HILOGE("memcpy_s failed");
        free(dragInfo);//需要发送stop？
        return RET_ERR;
    }
    FI_HILOGE("*** %{public}d, %{public}d, %{public}d, %{public}d, %{public}d, %{public}d, %{public}d, %{public}d,"
        "%{public}d, %{public}d, %{public}d, %{public}d, %{public}s, %{public}d, %{public}d, %{public}d, %{public}zu***",
        dragInfo->dragState, dragInfo->sourceType, dragInfo->pointerId, dragInfo->offsetX, dragInfo->offsetX,
        dragInfo->displayId, dragInfo->dragNum, dragInfo->dragStyle, dragInfo->hasCanceledAnimation, dragInfo->buffer[1],
        dragInfo->buffer[255], dragInfo->buffer[511], (dragInfo->udKey).c_str(), dragInfo->data[0], dragInfo->data[1],
        dragInfo->data[32300], dragInfo->dataLen);
    return RET_OK;
}

int32_t AcrossDeviceDrag::SendStopDragData()
{
    CALL_DEBUG_ENTER;
    auto dragState = context_->GetDragManager().GetDragState();
    if (context_->GetDragManager().GetDragState() != DragMessage::MSG_DRAG_STATE_STOP) {
        FI_HILOGE("State:%{public}d, is not drag start", dragState);
        return RET_ERR;
    }
    std::string stardeviceId;
    CooSM->GetStartId(stardeviceId);
    std::string remotedeviceId;
    CooSM->GetRemoteId(remotedeviceId);
    auto messageId = CoordinationSoftbusAdapter::STOPDRAG_DATA;
    FI_HILOGI("stardeviceId:%{public}s, remotedeviceId:%{public}s, messageId: %{public}d", stardeviceId.c_str(), remotedeviceId.c_str(), messageId);

    FI_HILOGI("remotedeviceId_:%{public}s", remotedeviceId_.c_str());
    if (CooSoftbusAdapter->OpenInputSoftbus(remotedeviceId_) != RET_OK) {
        FI_HILOGE("Open input softbus fail");
        return static_cast<int32_t>(CoordinationMessage::COORDINATION_FAIL);
    }
    if (CooSoftbusAdapter->SendData(remotedeviceId_, messageId, &dragState, sizeof(dragState)) != RET_OK) {
        FI_HILOGE("SendData failed");
        return RET_ERR;
    }
    if (remoteDataMap_.find(remotedeviceId_) != remoteDataMap_.end() && remoteDataMap_[remotedeviceId_]) {
        FI_HILOGI("Data on the peer device already exists");
        remoteDataMap_[remotedeviceId_] = false;
    }
    DragReset();
    // ResetDragData();
    return RET_OK;
}

void AcrossDeviceDrag::DragReset()
{
    CALL_DEBUG_ENTER;
    remotedeviceId_ = "";
    cooStateChangeType_ = CooStateChangeType::STATE_NONE;
    hasCanceledAnimation_ = false;
    dragStyle_ = DragCursorStyle::DEFAULT;
    CooSM->UnRegisterStateChange();
    CooSoftbusAdapter->UnRegisterRecvFunc();
    context_->GetDragManager().UnRegisterStateChange();
}

void AcrossDeviceDrag::DragStateChanged(DragMessage state)
{
    CALL_DEBUG_ENTER;
    FI_HILOGI("dragState:%{public}d", state);
    switch (state) {
        case DragMessage::MSG_DRAG_STATE_START: {
            UpdateDragStateChanged(true);
            ProcessDragingState();
            break;
        }
        case DragMessage::MSG_DRAG_STATE_STOP: {
            ProcessStopDragState();
            break;
        }
        default: {
            FI_HILOGI("State:%{public}d", state);
            context_->GetDragManager().UnRegisterStateChange();
            break;
        }
    }
}

void AcrossDeviceDrag::UpdateDragStateChanged(bool coor)
{
    CoordinationState coordinationState { CoordinationState::STATE_FREE };
    CooSM->GetCoordinationState(coordinationState);
    FI_HILOGI("coordinationState: %{public}d, cooStateChangeType_: %{public}d", coordinationState, cooStateChangeType_);
    if (cooStateChangeType_ == CooStateChangeType::STATE_NONE) {
        if (coordinationState == CoordinationState::STATE_IN && coor) {
            FI_HILOGI("DragStateChanged CooStateChangeType::STATE_FREE_TO_IN");
            ProcessFreeToIn(CoordinationState::STATE_FREE, CoordinationState::STATE_IN);//考虑下如果不是FREE_TO_IN
        } else if (coordinationState == CoordinationState::STATE_FREE && !coor) {
            FI_HILOGI("CooStateChangeType::STATE_OUT_TO_FREE");
            ProcessOutToFree(CoordinationState::STATE_OUT, CoordinationState::STATE_FREE);//考虑下如果不是STATE_OUT_TO_FREE
        }
    }
}

void AcrossDeviceDrag::ProcessDragingState()
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    CooSM->RegisterStateChange(CooStateChangeType::STATE_FREE_TO_IN,
        std::bind(&AcrossDeviceDrag::ProcessFreeToIn, this, std::placeholders::_1, std::placeholders::_2));
    CooSM->RegisterStateChange(CooStateChangeType::STATE_FREE_TO_OUT,
        std::bind(&AcrossDeviceDrag::ProcessFreeToOut, this, std::placeholders::_1, std::placeholders::_2));
    CooSM->RegisterStateChange(CooStateChangeType::STATE_IN_TO_FREE,
        std::bind(&AcrossDeviceDrag::ProcessInToFree, this, std::placeholders::_1, std::placeholders::_2));
    CooSM->RegisterStateChange(CooStateChangeType::STATE_OUT_TO_FREE,
        std::bind(&AcrossDeviceDrag::ProcessOutToFree, this, std::placeholders::_1, std::placeholders::_2));
#endif // OHOS_BUILD_ENABLE_COORDINATION
}

#ifdef OHOS_BUILD_ENABLE_COORDINATION
void AcrossDeviceDrag::ProcessFreeToIn(CoordinationState oldState, CoordinationState newState)//会不会前面设置好了，这里又“”了
{
    CALL_INFO_TRACE;
    cooStateChangeType_ = CooStateChangeType::STATE_FREE_TO_IN;
    std::string stardeviceId;
    CooSM->GetStartId(stardeviceId);
    FI_HILOGI("stardeviceId:%{public}s", stardeviceId.c_str());
    remotedeviceId_ = CooDevMgr->GetOriginNetworkId(stardeviceId);
    FI_HILOGI("remotedeviceId_:%{public}s", remotedeviceId_.c_str());
    if (remoteDataMap_.find(remotedeviceId_) != remoteDataMap_.end() && remoteDataMap_[remotedeviceId_]) {
        FI_HILOGI("Data on the peer device already exists");
        if (OHOS::MMI::InputManager::GetInstance()->IsPointerVisible()) {
            OHOS::MMI::InputManager::GetInstance()->SetPointerVisible(false);
        }
        context_->GetDragManager().OnSetDragWindowVisible(true);
    }
    // across_ = true;
}

void AcrossDeviceDrag::ProcessFreeToOut(CoordinationState oldState, CoordinationState newState)
{
    CALL_INFO_TRACE;
    cooStateChangeType_ = CooStateChangeType::STATE_FREE_TO_OUT;
    CooSM->GetRemoteId(remotedeviceId_);
    FI_HILOGI("remotedeviceId_:%{public}s", remotedeviceId_.c_str());
    // 通知udmf同步数据接口
    auto state = context_->GetDragManager().GetDragState();
    if (context_->GetDragManager().GetDragState() != DragMessage::MSG_DRAG_STATE_START) {
        FI_HILOGI("State:%{public}d, is not drag start", state);
        if (SendStopDragData() != RET_OK) {
            FI_HILOGI("SendStopDragData failed");
        }
        return;
    }

    std::for_each(remoteDataMap_.begin(), remoteDataMap_.end(), [](auto item) {
        FI_HILOGI("remoteDataMap_ deviceId::%{public}s, data::%{public}d", (item.first).c_str(), item.second);
    });
    if (remoteDataMap_.find(remotedeviceId_) == remoteDataMap_.end()) {
        FI_HILOGI("remoteDataMap_.find(remotedeviceId_) == remoteDataMap_.end()");
    } else if (!remoteDataMap_[remotedeviceId_]) {
        FI_HILOGI("remotedeviceId_::%{public}s, !remoteDataMap_[remotedeviceId_]:%{public}d", remotedeviceId_.c_str(), !remoteDataMap_[remotedeviceId_]);
    }
    if (dragStyle_ != DataAdapter.GetDragStyle() || remoteDataMap_.find(remotedeviceId_) == remoteDataMap_.end() ||
        !remoteDataMap_[remotedeviceId_]) {
        if (OHOS::MMI::InputManager::GetInstance()->IsPointerVisible()) {
            OHOS::MMI::InputManager::GetInstance()->SetPointerVisible(false);
        }
        context_->GetDragManager().OnSetDragWindowVisible(false);
        FI_HILOGI("Data on the peer device does not exist");
        if (SendDragingData() != RET_OK) {
            FI_HILOGI("SendDragingData failed");
            return;
        }
    }
    //隐藏绘制的窗口
    if (OHOS::MMI::InputManager::GetInstance()->IsPointerVisible()) {
        OHOS::MMI::InputManager::GetInstance()->SetPointerVisible(false);
    }
    context_->GetDragManager().OnSetDragWindowVisible(false);
}

void AcrossDeviceDrag::ProcessInToFree(CoordinationState oldState, CoordinationState newState)
{
    CALL_INFO_TRACE;
    cooStateChangeType_ = CooStateChangeType::STATE_IN_TO_FREE;
    std::string stardeviceId;
    CooSM->GetStartId(stardeviceId);
    FI_HILOGI("stardeviceId:%{public}s", stardeviceId.c_str());
    FI_HILOGI("remotedeviceId_:%{public}s", remotedeviceId_.c_str());
    auto state = context_->GetDragManager().GetDragState();
    if (state != DragMessage::MSG_DRAG_STATE_START) {
        FI_HILOGI("State:%{public}d, is not drag start", state);
        if (SendStopDragData() != RET_OK) {
            FI_HILOGI("SendStopDragData failed");
        }
        return;
    }

    std::for_each(remoteDataMap_.begin(), remoteDataMap_.end(), [](auto item) {
        FI_HILOGI("remoteDataMap_ deviceId::%{public}s, data::%{public}d", (item.first).c_str(), item.second);
    });
    if (remoteDataMap_.find(remotedeviceId_) == remoteDataMap_.end()) {
        FI_HILOGI("remoteDataMap_.find(remotedeviceId_) == remoteDataMap_.end()");
    } else if (!remoteDataMap_[remotedeviceId_]) {
        FI_HILOGI("remotedeviceId_::%{public}s, !remoteDataMap_[remotedeviceId_]:%{public}d", remotedeviceId_.c_str(), !remoteDataMap_[remotedeviceId_]);
    }
    //如果是拖拽中需要发送数据
    if (dragStyle_ != DataAdapter.GetDragStyle() || remoteDataMap_.find(remotedeviceId_) == remoteDataMap_.end() ||
        !remoteDataMap_[remotedeviceId_]) {
        if (OHOS::MMI::InputManager::GetInstance()->IsPointerVisible()) {
            OHOS::MMI::InputManager::GetInstance()->SetPointerVisible(false);
        }
        context_->GetDragManager().OnSetDragWindowVisible(false);
        FI_HILOGI("Data on the peer device does not exist");
        if (SendDragingData() != RET_OK) {
            FI_HILOGI("SendDragingData failed");
            return;
        }
    }
    // 销毁绘制的窗口
    if (OHOS::MMI::InputManager::GetInstance()->IsPointerVisible()) {
        OHOS::MMI::InputManager::GetInstance()->SetPointerVisible(false);
    }
    context_->GetDragManager().OnSetDragWindowVisible(false);
}

void AcrossDeviceDrag::ProcessOutToFree(CoordinationState oldState, CoordinationState newState)//如果是对端开启拖拽，这里如何变化,remotedeviceId_为""
{//跨设备为2个？
    CALL_INFO_TRACE;
    cooStateChangeType_ = CooStateChangeType::STATE_OUT_TO_FREE;
    std::string stardeviceId;
    CooSM->GetStartId(stardeviceId);
    FI_HILOGI("stardeviceId:%{public}s", stardeviceId.c_str());
    // CooSM->GetRemoteId(remotedeviceId_);
    FI_HILOGI("remotedeviceId_:%{public}s", remotedeviceId_.c_str());
    CooSoftbusAdapter->GetRemoteId(remotedeviceId_);//对端发起拖拽，本端停止
    FI_HILOGI("remotedeviceId_222:%{public}s", remotedeviceId_.c_str());
    // 绘制窗口
    CoordinationState coordinationState { CoordinationState::STATE_FREE };
    CooSM->GetCoordinationState(coordinationState);
    FI_HILOGI("coordinationState: %{public}d, cooStateChangeType_: %{public}d", coordinationState, cooStateChangeType_);
    if (context_->GetDragManager().GetDragState() == DragMessage::MSG_DRAG_STATE_START && remotedeviceId_ != "")
    {
        if (remoteDataMap_.find(remotedeviceId_) == remoteDataMap_.end() || !remoteDataMap_[remotedeviceId_]) {
            FI_HILOGI("remoteDataMap not find");
            remoteDataMap_.insert(std::make_pair(remotedeviceId_, true));
            std::for_each(remoteDataMap_.begin(), remoteDataMap_.end(), [](auto item) {
                FI_HILOGI("222 remoteDataMap_ deviceId::%{public}s, data::%{public}d", (item.first).c_str(), item.second);
            });
        } else if (!remoteDataMap_[remotedeviceId_]) {
            FI_HILOGI("remotedeviceId_::%{public}s, !remoteDataMap_[remotedeviceId_]:%{public}d", remotedeviceId_.c_str(), !remoteDataMap_[remotedeviceId_]);
            remoteDataMap_[remotedeviceId_] = true;
        }
        FI_HILOGI("Data on the peer device already exists");
        if (OHOS::MMI::InputManager::GetInstance()->IsPointerVisible()) {
            OHOS::MMI::InputManager::GetInstance()->SetPointerVisible(false);
        }
        context_->GetDragManager().OnSetDragWindowVisible(true);
    }
    // if (remoteDataMap_.find(remotedeviceId_) != remoteDataMap_.end() && remoteDataMap_[remotedeviceId_]) {
    //     FI_HILOGI("Data on the peer device already exists");
    //     context_->GetDragManager().OnSetDragWindowVisible(true);
    // }
}
#endif // OHOS_BUILD_ENABLE_COORDINATION

void AcrossDeviceDrag::ProcessStopDragState()
{
    CALL_INFO_TRACE;
    if (cooStateChangeType_ != CooStateChangeType::STATE_FREE_TO_IN && cooStateChangeType_ != CooStateChangeType::STATE_OUT_TO_FREE) {
        FI_HILOGI("Coo state change type:%{public}d, is not free to in", cooStateChangeType_);
        return;
    }
    if (SendStopDragData() != RET_OK) {
        FI_HILOGI("SendStopDragData failed");
        return;
    }
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS