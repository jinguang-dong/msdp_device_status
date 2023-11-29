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

#include "drag_data_packer.h"

#include "devicestatus_common.h"
#include "devicestatus_define.h"
#include "devicestatus_errors.h"

namespace OHOS {
namespace Msdp {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DragDataPacker" };
} // namespace

namespace DeviceStatus {

int32_t DragDataPacker::Marshalling(const DragData &dragData, Parcel &data, bool isCross)
{
    CALL_DEBUG_ENTER;
    if (ShadowPacker::Marshalling(dragData.shadowInfos, data, isCross) != RET_OK) {
        FI_HILOGE("Failed to marshalling shadowInfos");
        return RET_ERR;
    }
    WRITEUINT8VECTOR(data, dragData.buffer, ERR_INVALID_VALUE);
    WRITESTRING(data, dragData.udKey, ERR_INVALID_VALUE);
    WRITESTRING(data, dragData.extraInfo, ERR_INVALID_VALUE);
    WRITESTRING(data, dragData.filterInfo, ERR_INVALID_VALUE);
    WRITEINT32(data, dragData.sourceType, ERR_INVALID_VALUE);
    WRITEINT32(data, dragData.dragNum, ERR_INVALID_VALUE);
    WRITEINT32(data, dragData.pointerId, ERR_INVALID_VALUE);
    WRITEINT32(data, dragData.displayX, ERR_INVALID_VALUE);
    WRITEINT32(data, dragData.displayY, ERR_INVALID_VALUE);
    WRITEINT32(data, dragData.displayId, ERR_INVALID_VALUE);
    WRITEBOOL(data, dragData.hasCanceledAnimation, ERR_INVALID_VALUE);
    if (SummaryPacker::Marshalling(dragData.summarys, data) != RET_OK) {
        FI_HILOGE("Failed to summarys marshalling");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t DragDataPacker::UnMarshalling(Parcel &data, DragData &dragData, bool isCross)
{
    CALL_DEBUG_ENTER;
    if (ShadowPacker::UnMarshalling(data, dragData.shadowInfos, isCross) != RET_OK) {
        FI_HILOGE("UnMarshallingShadowInfos failed");
        return RET_ERR;
    }
    READUINT8VECTOR(data, dragData.buffer, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READSTRING(data, dragData.udKey, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READSTRING(data, dragData.extraInfo, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READSTRING(data, dragData.filterInfo, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READINT32(data, dragData.sourceType, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READINT32(data, dragData.dragNum, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READINT32(data, dragData.pointerId, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READINT32(data, dragData.displayX, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READINT32(data, dragData.displayY, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READINT32(data, dragData.displayId, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READBOOL(data, dragData.hasCanceledAnimation, E_DEVICESTATUS_READ_PARCEL_ERROR);
    if (SummaryPacker::UnMarshalling(data, dragData.summarys) != RET_OK) {
        FI_HILOGE("Failed to summarys unmarshalling");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t ShadowPacker::Marshalling(const std::vector<ShadowInfo> &shadowInfos, Parcel &data, bool isCross)
{
    CALL_DEBUG_ENTER;
    if (shadowInfos.empty()) {
        FI_HILOGE("Invalid parameter shadowInfos");
        return ERR_INVALID_VALUE;
    }
    int32_t shadowNum = static_cast<int32_t>(shadowInfos.size());
    if (shadowNum > SHADOW_NUM_LIMIT) {
        FI_HILOGW("Only %{public}d shadowInfos are allowed to be packaged at most, now %{public}d exceeding the limit",
            SHADOW_NUM_LIMIT, shadowNum);
        shadowNum  = SHADOW_NUM_LIMIT;
    }
    WRITEINT32(data, shadowNum, ERR_INVALID_VALUE);
    for (int32_t i = 0; i < shadowNum; i++) {
        if (PackUpShadowInfo(shadowInfos[i], data, isCross) != RET_OK) {
            FI_HILOGE("PackUpShadowInfo failed");
            return RET_ERR;
        }
    }
    return RET_OK;
}

int32_t ShadowPacker::UnMarshalling(Parcel &data, std::vector<ShadowInfo> &shadowInfos, bool isCross)
{
    CALL_DEBUG_ENTER;
    int32_t shadowNum { 0 };
    READINT32(data, shadowNum, E_DEVICESTATUS_READ_PARCEL_ERROR);
    if (shadowNum <= 0 || shadowNum > SHADOW_NUM_LIMIT) {
        FI_HILOGE("Invalid shadowNum:%{public}d", shadowNum);
        return RET_ERR;
    }
    for (int32_t i = 0; i < shadowNum; i++) {
        FI_HILOGI("ShadowNum:%{public}d", shadowNum);
        ShadowInfo shadowInfo;
        if (UnPackShadowInfo(data, shadowInfo, isCross) != RET_OK) {
            FI_HILOGE("UnPackShadowInfo failed");
            return RET_ERR;
        }
        CHKPR(shadowInfo.pixelMap, RET_ERR);
        shadowInfos.push_back(shadowInfo);
    }
    return RET_OK;
}

int32_t ShadowPacker::PackUpShadowInfo(const ShadowInfo &shadowInfo, Parcel &data, bool isCross)
{
    CALL_DEBUG_ENTER;
    CHKPR(shadowInfo.pixelMap, RET_ERR);
    if (isCross) {
        FI_HILOGI("EncodeTlv");
        std::vector<uint8_t> pixelBuffer;
        if (!shadowInfo.pixelMap->EncodeTlv(pixelBuffer)) {
            FI_HILOGE("EncodeTlv pixelMap failed");
            return ERR_INVALID_VALUE;
        }
        WRITEUINT8VECTOR(data, pixelBuffer, ERR_INVALID_VALUE);
    } else {
        FI_HILOGI("Marshalling");
        if (!shadowInfo.pixelMap->Marshalling(data)) {
            FI_HILOGE("Marshalling pixelMap failed");
            return ERR_INVALID_VALUE;
        }
    }
    WRITEINT32(data, shadowInfo.x, ERR_INVALID_VALUE);
    WRITEINT32(data, shadowInfo.y, ERR_INVALID_VALUE);
    return RET_OK;
}

int32_t ShadowPacker::UnPackShadowInfo(Parcel &data, ShadowInfo &shadowInfo, bool isCross)
{
    CALL_DEBUG_ENTER;
    Media::PixelMap *rawPixelMap = nullptr;
    if (isCross) {
        FI_HILOGI("DecodeTlv");
        std::vector<uint8_t> pixelBuffer;
        READUINT8VECTOR(data, pixelBuffer, ERR_INVALID_VALUE);
        rawPixelMap = Media::PixelMap::DecodeTlv(pixelBuffer);
    } else {
        FI_HILOGI("UnMarshalling");
        rawPixelMap = OHOS::Media::PixelMap::Unmarshalling(data);
    }
    FI_HILOGI("rawPixelMap->GetWidth():%{public}d, rawPixelMap->GetHeight:%{public}d",rawPixelMap->GetWidth(), rawPixelMap->GetHeight());
    CHKPR(rawPixelMap, RET_ERR);
    shadowInfo.pixelMap = std::shared_ptr<Media::PixelMap>(rawPixelMap);
    CHKPR(shadowInfo.pixelMap, RET_ERR);
    READINT32(data, shadowInfo.x, E_DEVICESTATUS_READ_PARCEL_ERROR);
    READINT32(data, shadowInfo.y, E_DEVICESTATUS_READ_PARCEL_ERROR);
    return RET_OK;
}

int32_t SummaryPacker::Marshalling(const SummaryMap &val, Parcel &parcel)
{
    CALL_DEBUG_ENTER;
    WRITEINT32(parcel, static_cast<int32_t>(val.size()), ERR_INVALID_VALUE);
    for (auto const &[k, v] : val) {
        WRITESTRING(parcel, k, ERR_INVALID_VALUE);
        WRITEINT64(parcel, v, ERR_INVALID_VALUE);
    }
    return RET_OK;
}

int32_t SummaryPacker::UnMarshalling(Parcel &parcel, SummaryMap &val)
{
    CALL_DEBUG_ENTER;
    int32_t size = 0;
    READINT32(parcel, size, E_DEVICESTATUS_READ_PARCEL_ERROR);
    if (size < 0) {
        FI_HILOGE("Invalid size:%{public}d", size);
        return RET_ERR;
    }
    size_t readAbleSize = parcel.GetReadableBytes();
    if ((static_cast<size_t>(size) > readAbleSize) || static_cast<size_t>(size) > val.max_size()) {
        return RET_ERR;
    }
    for (int32_t i = 0; i < size; ++i) {
        std::string key;
        READSTRING(parcel, key, E_DEVICESTATUS_READ_PARCEL_ERROR);
        READINT64(parcel, val[key], E_DEVICESTATUS_READ_PARCEL_ERROR);
    }
    return RET_OK;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
