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
#include <vector>

#include "devicestatus_common.h"
#include "devicestatus_define.h"
#include "devicestatus_errors.h"
#include "drag_style_packer.h"

namespace OHOS {
namespace Msdp {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DragStylePacker" };
} // namespace

namespace DeviceStatus {

int32_t DragStylePacker::Marshalling(const DragStyle &dragStyle, Parcel &data)
{
    std::vector<int32_t> types;
    for (const auto &elem : dragStyle.types) {
        types.push_back(static_cast<int32_t>(elem));
    }
    WRITEINT32VECTOR(data, types, ERR_INVALID_VALUE);
    WRITEUINT32(data, dragStyle.foregroundColor, ERR_INVALID_VALUE);
    WRITEINT32(data, dragStyle.opacity, ERR_INVALID_VALUE);
    WRITEINT32(data, dragStyle.radius, ERR_INVALID_VALUE);
    WRITEFLOAT(data, dragStyle.scale, ERR_INVALID_VALUE);
    return RET_OK;
}

int32_t DragStylePacker::UnMarshalling(Parcel &data, DragStyle &dragStyle)
{
    std::vector<int32_t> types;
    READINT32VECTOR(data, types, ERR_INVALID_VALUE);
    for (const auto &elem : types) {
        dragStyle.types.push_back(static_cast<StyleType>(elem));
    }
    READUINT32(data, dragStyle.foregroundColor, ERR_INVALID_VALUE);
    READINT32(data, dragStyle.opacity, ERR_INVALID_VALUE);
    READINT32(data, dragStyle.radius, ERR_INVALID_VALUE);
    READFLOAT(data, dragStyle.scale, ERR_INVALID_VALUE);
    return RET_OK;
}

int32_t DragAnimationPacker::Marshalling(const DragAnimation &dragAnimation, Parcel &data)
{
    WRITEINT32(data, dragAnimation.duration, ERR_INVALID_VALUE);
    WRITESTRING(data, dragAnimation.curveName, ERR_INVALID_VALUE);
    WRITEFLOATVECTOR(data, dragAnimation.curve, ERR_INVALID_VALUE);
    return RET_OK;
}

int32_t DragAnimationPacker::UnMarshalling(Parcel &data, DragAnimation &dragAnimation)
{
    READINT32(data, dragAnimation.duration, ERR_INVALID_VALUE);
    READSTRING(data,  dragAnimation.curveName, ERR_INVALID_VALUE);
    READFLOATVECTOR(data, dragAnimation.curve, ERR_INVALID_VALUE);
    return RET_OK;
}

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS