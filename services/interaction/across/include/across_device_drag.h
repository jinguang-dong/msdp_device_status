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

#ifndef ACROSS_DEVICE_DRAG
#define ACROSS_DEVICE_DRAG

#ifdef OHOS_BUILD_ENABLE_COORDINATION
#include "coordination_sm.h"
#endif // OHOS_BUILD_ENABLE_COORDINATION
#include "drag_message.h"
#include "i_context.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
struct DragInfo {
    int32_t dragState { 0 };//拖拽状态
    int32_t sourceType { 0 };//鼠标或者触摸屏
    int32_t pointerId { 0 };
    int32_t displayId { 0 };
    int32_t offsetX { 0 };
    int32_t offsetY { 0 };
    int32_t dragNum { 0 };//拖拽数量
    int32_t dragStyle { 0 };
    bool hasCanceledAnimation { false };
    bool hasData { false };
    uint8_t buffer[512] { 0 };//拖拽元数据：开始拖拽的接口有传入
    std::string udKey;
    uint32_t dataLen;
    int8_t data[0];//动态存储pixelmap->GetPixels()
};

class AcrossDeviceDrag final {
public:
    AcrossDeviceDrag();
    ~AcrossDeviceDrag() = default;

    int32_t Init(IContext *context);

private:
    void RecvDragingData(const void* data, uint32_t dataLen);
    void DecodeDragData(const DragInfo* dragInfo, DragData &dragData);
    void RecvStopDragData(const void* data, uint32_t dataLen);
    void DragStateChanged(DragMessage state);
    void UpdateDragStateChanged(bool coor);
    void PrintDragData(const DragData &dragData);
    int32_t SendDragingData();
    int32_t EncodeDragData(const DragData dragData, const std::vector<uint8_t> buff, DragInfo* dragInfo);
    int32_t SendStopDragData();
    void DragReset();
    void ProcessDragingState();
    void ProcessStopDragState();
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    void ProcessFreeToIn(CoordinationState oldState, CoordinationState newState);
    void ProcessFreeToOut(CoordinationState oldState, CoordinationState newState);
    void ProcessInToFree(CoordinationState oldState, CoordinationState newState);
    void ProcessOutToFree(CoordinationState oldState, CoordinationState newState);
#endif // OHOS_BUILD_ENABLE_COORDINATION

private:
    IContext *context_ { nullptr };
    CooStateChangeType cooStateChangeType_ {CooStateChangeType::STATE_NONE};
    std::string remotedeviceId_;
    std::map<std::string, bool> remoteDataMap_;
    bool hasCanceledAnimation_ { false };
    DragCursorStyle dragStyle_ { DragCursorStyle::DEFAULT };
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // ACROSS_DEVICE_DRAG