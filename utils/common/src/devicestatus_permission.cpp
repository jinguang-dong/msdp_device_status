/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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

#include "devicestatus_permission.h"
#include "fi_log.h"

namespace OHOS {
namespace Msdp {
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DeviceStatusPermission" };
} // namespace

bool DeviceStatusPermission::CheckDeviceStatusPermission(AccessTokenID callerToken)
{
    CALL_INFO_TRACE;
    int32_t auth = AccessTokenKit::VerifyAccessToken(callerToken, ACTIVITY_MOTION_PERMISSION);
    if (auth != PERMISSION_GRANTED) {
        FI_HILOGE("permission verify fail");
        return false;
    }
    return true;
}
} // namespace Msdp
} // namespace OHOS
