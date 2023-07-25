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
namespace DeviceStatus {
namespace {
static sptr<IBundleMgr> g_bundleMgr = nullptr;
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DeviceStatusPermission" };
} // namespace

sptr<IBundleMgr> DeviceStatusPermission::GetBundleMgr()
{
    FI_HILOGD("Enter");
    if (g_bundleMgr != nullptr) {
        FI_HILOGD("g_bundleMgr is not nullptr");
        return g_bundleMgr;
    }
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        FI_HILOGE("GetSystemAbilityManager is nullptr");
        return nullptr;
    }

    auto bundleMgrSa = sam->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleMgrSa == nullptr) {
        FI_HILOGE("GetSystemAbility is nullptr");
        return nullptr;
    }
    auto bundleMgr = iface_cast<IBundleMgr>(bundleMgrSa);
    if (bundleMgr == nullptr) {
        FI_HILOGE("bundleMgr is nullptr");
        return nullptr;
    }

    g_bundleMgr = bundleMgr;
    FI_HILOGD("Exit");
    return g_bundleMgr;
}

bool DeviceStatusPermission::IsTokenAplEquals(ATokenAplEnum  apl)
{
    FI_HILOGD("Enter");
    AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    ATokenTypeEnum type = AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (type != ATokenTypeEnum::TOKEN_HAP) {
        FI_HILOGE("type is error");
        return false;
    }
    HapTokenInfo info;
    if (AccessTokenKit::GetHapTokenInfo(tokenId, info) != 0) {
        FI_HILOGE("GetHapTokenInfo failed!");
        return false;
    }
    if (info.apl != apl) {
        FI_HILOGE("Ability privilege level not match");
        return false;
    }
    FI_HILOGD("Ability privilege level match ");
    return true;
}

bool DeviceStatusPermission::IsSystemCoreTokenType()
{
    FI_HILOGD("Enter");
    bool isMatch = IsTokenAplEquals(ATokenAplEnum::APL_SYSTEM_CORE);
    if (!isMatch) {
        FI_HILOGE("APL_SYSTEM_CORE accsess token denied");
    }
    FI_HILOGD("Exit");
    return isMatch;
}

bool DeviceStatusPermission::IsSystemBasicTokenType()
{
    FI_HILOGD("Enter");
    bool isMatch = IsTokenAplEquals(ATokenAplEnum::APL_SYSTEM_BASIC);
    if (!isMatch) {
        FI_HILOGE("APL_SYSTEM_BASIC accsess token denied");
    }
    FI_HILOGD("Exit");
    return isMatch;
}

bool DeviceStatusPermission::IsSystemAplType()
{
    return IsSystemBasicTokenType() || IsSystemCoreTokenType();
}

bool DeviceStatusPermission::IsSystemHap()
{
    FI_HILOGD("Enter");
    auto bundleMgr = GetBundleMgr();
    if (bundleMgr == nullptr) {
        FI_HILOGE("bundleMgr is nullptr");
        return false;
    }
    auto isSystemApp = bundleMgr->CheckIsSystemAppByUid(IPCSkeleton::GetCallingUid());
    if (!isSystemApp) {
        FI_HILOGE("hap is not system app");
        return false;
    }
    FI_HILOGE("hap is system app");
    return true;
}

bool DeviceStatusPermission::HasSystemPermission()
{
    return IsSystemAplType() || IsSystemHap();
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
