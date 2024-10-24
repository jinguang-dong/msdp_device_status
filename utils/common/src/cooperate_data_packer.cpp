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

#include "cooperate_data_packer.h"

#include "devicestatus_common.h"
#include "devicestatus_define.h"
#include "devicestatus_errors.h"

#undef LOG_TAG
#define LOG_TAG "CooperateDataPacker"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {

int32_t CoefficientPacker::Marshalling(const CoefficientMap &val, Parcel &parcel)
{
    WRITEINT32(parcel, static_cast<int32_t>(val.size()), ERR_INVALID_VALUE);
    for (auto const &[k, v] : val) {
        WRITEINT32(parcel, k, ERR_INVALID_VALUE);
        WRITEDOUBLE(parcel, v, ERR_INVALID_VALUE);
    }
    return RET_OK;
}

int32_t CoefficientPacker::UnMarshalling(Parcel &parcel, CoefficientMap &val)
{
    size_t readAbleSize = parcel.GetReadableBytes();
    int32_t size = 0;
    READINT32(parcel, size, E_DEVICESTATUS_READ_PARCEL_ERROR);
    if (size < 0 || (static_cast<size_t>(size) > readAbleSize) || static_cast<size_t>(size) > val.max_size()) {
        FI_HILOGE("Invalid size:%{public}d", size);
        return RET_ERR;
    }
    for (int32_t i = 0; i < size; ++i) {
        int32_t key;
        READINT32(parcel, key, E_DEVICESTATUS_READ_PARCEL_ERROR);
        READDOUBLE(parcel, val[key], E_DEVICESTATUS_READ_PARCEL_ERROR);
    }
    return RET_OK;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
