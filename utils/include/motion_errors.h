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

#ifndef MOTION_ERRORS_H
#define MOTION_ERRORS_H

#include <errors.h>

namespace OHOS {
namespace Msdp {
enum {
    /**
     *  Module type: Motion Service
     */
    MOTION_MODULE_TYPE_SERVICE = 0,
    /**
     *  Module type: Motion Kit
     */
    MOTION_MODULE_TYPE_KIT = 1
};

// offset of motion error, only be used in this file.
constexpr ErrCode MOTION_SERVICE_ERR_OFFSET = ErrCodeOffset(SUBSYS_MSDP, MOTION_MODULE_TYPE_SERVICE);

enum {
    E_MOTION_WRITE_PARCEL_ERROR = MOTION_SERVICE_ERR_OFFSET,
    E_MOTION_READ_PARCEL_ERROR,
    E_MOTION_GET_SYSTEM_ABILITY_MANAGER_FAILED,
    E_MOTION_GET_SERVICE_FAILED,
    E_MOTION_ADD_DEATH_RECIPIENT_FAILED,
    E_MOTION_INNER_ERR
};
enum {
    MOTION_OK = 0,
    MOTION_FAILED = -1,
    MOTION_INVALID_FD = -2,
    MOTION_NOT_FIND_JSON_ITEM = -3,
};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_ERRORS_H