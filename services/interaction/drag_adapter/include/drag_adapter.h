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

#ifndef DRAG_ADAPTER_H
#define DRAG_ADAPTER_H

#include <cstdint>

#include "nocopyable.h"
#include "singleton.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
struct DragInfo {
    int32_t pixelFormat { 0 };
    int32_t alphaType { 0 };
    int32_t width { 0 };
    int32_t height { 0 };
    int32_t allocatorType { 0 };
    int32_t dragState { 0 };
    int32_t num { 0 };
    int32_t offsetX { 0 };
    int32_t offsetY { 0 };
    int32_t dragCount { 0 };
    uint8_t buffer[512] { 0 };
    uint32_t dataLen;
    int8_t data[0];
};
class DragAdapter {
    DECLARE_DELAYED_SINGLETON(DragAdapter);

public:
    using GetDragStateCallback = std::function<void(int32_t &dragState)>;
    using SetDrapDataCallback = std::function<void(DragInfo &dragInfo, const uint8_t* dstPixels, int32_t pixelDataSize)>;
    using GetDrapDataCallback = std::function<void(DragInfo &dragInfo, const uint8_t** dstPixels, int32_t &pixelDataSize)>;
    using StartDragCallback = std::function<void()>;
    using EndDragCallback = std::function<void(int32_t dragState)>;
    using SendDragResCallback = std::function<void(int32_t dragState)>;
    int32_t RegisterCallback(GetDragStateCallback callback);
    int32_t RegisterCallback(SetDrapDataCallback callback) { return 0; }
    int32_t RegisterCallback(GetDrapDataCallback callback) { return 0; }
    int32_t RegisterCallback(StartDragCallback callback) { return 0; }
    int32_t RegisterCallback(EndDragCallback callback) { return 0; }
    int32_t RegisterSoftbusCallback(SendDragResCallback callback) { return 0; }

    bool IsDragging();
    void SetDrapData(DragInfo& dragInfo, const uint8_t* pixelsData, int32_t pixelsDataSize) {}
    void GetDrapData(DragInfo& dragInfo, const uint8_t** pixelsData, int32_t& pixelsDataSize) {}
    void StartDrag() {}
    void EndDrag(int32_t dragState) {}
    void SendDragRes(int32_t dragState) {}

private:
    DISALLOW_COPY_AND_MOVE(DragAdapter);

private:
    GetDragStateCallback getDragStateCallback_ = { nullptr };
    SetDrapDataCallback setDrapDataCallback_ = { nullptr };
    GetDrapDataCallback getDrapDataCallback_ = { nullptr };
    StartDragCallback startDragCallback_ = { nullptr };
    EndDragCallback endDragCallback_ = { nullptr };
    SendDragResCallback sendDragResCallback_ = { nullptr };
};

#define DragAdpt ::OHOS::DelayedSingleton<DragAdapter>::GetInstance()
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DRAG_ADAPTER_H