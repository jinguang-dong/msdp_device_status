/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef COOPERATE_DATA_PACKER_H
#define COOPERATE_DATA_PACKER_H

#include <map>
#include <string>

#include "parcel.h"


namespace OHOS {
namespace Msdp {
namespace DeviceStatus {

using CoefficientMap = std::map<int32_t, double>;

class CoefficientPacker {
public:
    static int32_t Marshalling(const CoefficientMap &val, Parcel &parcel);
    static int32_t UnMarshalling(Parcel &parcel, CoefficientMap &val);
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // COOPERATE_DATA_PACKER_H
