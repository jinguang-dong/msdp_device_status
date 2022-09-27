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

#ifndef MOTION_COMMON_H
#define MOTION_COMMON_H

#include <cstdint>
#include <type_traits>

#include "motion_hilog_wrapper.h"
#include "motion_errors.h"

namespace OHOS {
namespace Msdp {
#define MOTION_RETURN_IF_WITH_RET(cond, retval) if (cond) {return (retval);}
#define MOTION_RETURN_IF(cond) if (cond) {return;}
#define MOTION_RETURN_IF_WITH_LOG(cond, loginfo)                                     \
    do {                                                                             \
        if (cond) {                                                                  \
            MOTION_HILOGE(MOTION_MODULE_COMMON, "%{public}s "#loginfo" ", __func__); \
            return;                                                                     \
        }                                                                               \
    } while (0)                                                                         \

#define MOTION_READ_PARCEL_NO_RET(parcel, type, out)                                           \
    do {                                                                                \
        if (!(parcel).Read##type(out)) {                                                \
            MOTION_HILOGE(MOTION_MODULE_COMMON, "%{public}s read "#out" failed", __func__);                          \
            return;                                                                     \
        }                                                                               \
    } while (0)                                                                         \

#define MOTION_WRITE_PARCEL_NO_RET(parcel, type, data)                                         \
    do {                                                                                \
        if (!(parcel).Write##type(data)) {                                              \
            MOTION_HILOGE(MOTION_MODULE_COMMON, "%{public}s write "#data" failed", __func__);                        \
            return;                                                                     \
        }                                                                               \
    } while (0)                                                                         \

#define MOTION_READ_PARCEL_WITH_RET(parcel, type, out, retval)                                \
    do {                                                                               \
        if (!(parcel).Read##type(out)) {                                               \
            MOTION_HILOGE(MOTION_MODULE_COMMON, "%{public}s read "#out" failed", __func__);                         \
            return (retval);                                                           \
        }                                                                              \
    } while (0)                                                                        \

#define MOTION_WRITE_PARCEL_WITH_RET(parcel, type, data, retval)                              \
    do {                                                                               \
        if (!(parcel).Write##type(data)) {                                             \
            MOTION_HILOGE(MOTION_MODULE_COMMON, "%{public}s write "#data" failed", __func__);                       \
            return (retval);                                                           \
        }                                                                              \
    } while (0)

template<typename E>
constexpr auto MotionToUnderlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}
} // namespace Msdp
} // namespace OHOS

#endif // MOTION_COMMON_H
