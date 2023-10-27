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

#include "permission_util.h"

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "privacy_kit.h"

#include "fi_log.h"

namespace OHOS {
namespace Msdp {
using namespace OHOS::HiviewDFX;
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "PermissionUtil" };
}  // namespace

bool PermissionUtil::IsNativeToken(AccessTokenID callerToken)
{
    int32_t tokenType = AccessTokenKit::GetTokenTypeFlag(callerToken);
    if (tokenType != ATokenTypeEnum::TOKEN_NATIVE) {
        FI_HILOGE("TokenType is not TOKEN_NATIVE, tokenType:%{public}d", tokenType);
        return false;
    }
    return true;
}
}  // namespace Msdp
}  // namespace OHOS
