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

#ifndef SHADOW_PACKER_H
#define SHADOW_PACKER_H

#include <message_parcel.h>

#include "drag_data.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class ShadowPacker {
public:
    static int32_t Marshalling(const std::vector<ShadowInfo> &shadowInfos, Parcel &data);
    static int32_t UnMarshalling(Parcel &data, std::vector<ShadowInfo> &shadowInfos);
};

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // SHADOW_PACKER_H
