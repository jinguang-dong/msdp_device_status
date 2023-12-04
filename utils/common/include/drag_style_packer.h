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

#ifndef DRAG_STYLE_PACKER_H
#define DRAG_STYLE_PACKER_H

#include "parcel.h"

#include "drag_data.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {

class DragStylePacker {
public:
    static int32_t Marshalling(const DragStyle &dragStyle, Parcel &data);
    static int32_t UnMarshalling(Parcel &data, DragStyle &dragStyle);
};

class DragAnimationPacker {
public:
    static int32_t Marshalling(const DragAnimation &dragAnimation, Parcel &data);
    static int32_t UnMarshalling(Parcel &data, DragAnimation &dragAnimation);
};

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DRAG_STYLE_PACKER_H