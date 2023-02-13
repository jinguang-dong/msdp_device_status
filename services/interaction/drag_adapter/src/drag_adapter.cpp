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

#include "drag_adapter.h"

#include "devicestatus_define.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "DragAdapter" };
} // namespace

DragAdapter::DragAdapter()
{
    // 注册消息id和回调函数
    CooSoftbusAdapter->Registerfun(messageid, std::bind(&DragAdapter::Dragfun, this, std::placeholders::_1, std::placeholders::_2));
}

DragAdapter::~DragAdapter() {}

int32_t DragAdapter::Dragfun(const void* data, uint32_t dataLen)    // 解析数据
{
    const DragInfo* dragInfo = reinterpret_cast<const DragInfo *>(data);

    const uint8_t *dstPixels = reinterpret_cast<const uint8_t *>(dragInfo->data);
    int num1 = *(dstPixels + 1);
    int num2 = *(dstPixels + 1111);
    int num3 = *(dstPixels + 14399);
    FI_HILOGE("*** %{public}zu, %{public}d, %{public}d, %{public}d, %{public}d, %{public}d, %{public}d, "
        "%{public}d, %{public}d, %{public}d, %{public}d, %{public}d, %{public}d, %{public}d ***",
        dataPacket->dataLen, num1, num2, num3, dataPacket->dragInfo.pixelFormat, dataPacket->dragInfo.alphaType,
        dataPacket->dragInfo.width, dataPacket->dragInfo.height, dataPacket->dragInfo.allocatorType,
        dataPacket->dragInfo.dragState, dataPacket->dragInfo.num, dataPacket->dragInfo.buffer[1],
        dataPacket->dragInfo.buffer[255], dataPacket->dragInfo.buffer[511]);
}

int32_t DragAdapter::DragfunTmp()   // 组织数据，调用softbus的SendMsg
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

    DragInfo* dragInfo = (DragInfo*)malloc(sizeof(DragInfo) + 14400);
    if (dragInfo == nullptr) {
        FI_HILOGE("Malloc failed");
        return RET_ERR;
    }
    dragInfo->dataLen = 14400;
    errno_t ret = memcpy_s(dragInfo->data, dragInfo->dataLen, dstPixels, dragInfo->dataLen);
    CooSoftbusAdapter->SendMsg(deviceId, messageId, dragInfo, sizeof(DragInfo) + 14400);
}

int32_t DragAdapter::RegisterCallback(GetDragStateCallback callback)
{
    CHKPR(callback, RET_ERR);
    getDragStateCallback_ = callback;
    return RET_OK;
}

bool DragAdapter::IsDragging()
{
    int32_t dragState;
    getDragStateCallback_(dragState);
    if (dragState == draging) {
        return true;
    }
    return false;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
