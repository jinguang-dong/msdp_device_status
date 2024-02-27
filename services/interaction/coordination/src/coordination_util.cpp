/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <string>
#include <unordered_set>
#include <unistd.h>
 
#include "coordination_util.h"
 
#include "coordination_sm.h"
#include "device_manager.h"
#include "devicestatus_define.h"
 #include "softbus_bus_center.h"
namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace COORDINATION {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "CoordinationUtil" };
} // namespace
std::string GetLocalNetworkId()
{
    CALL_DEBUG_ENTER;
    auto localNode = std::make_unique<NodeBasicInfo>();
    int32_t ret = GetLocalNodeDeviceInfo(FI_PKG_NAME, localNode.get());
    if (ret != RET_OK) {
        FI_HILOGE("Get local node device info, ret:%{public}d", ret);
        return {};
    }
    std::string networkId(localNode->networkId, sizeof(localNode->networkId));
    FI_HILOGD("Get local node device info, networkId:%{public}s", AnonyNetworkId(networkId).c_str());
    return localNode->networkId;
}
std::string GetLocalUdid()
{
    OHOS::DistributedHardware::DmDeviceInfo dmDeviceInfo;
    const std::string PKG_NAME = "DBinderBus_Dms_" + std::to_string(getpid());
    int32_t errCode = DistributedHardware::DeviceManager::GetInstance().GetLocalDeviceInfo(PKG_NAME, dmDeviceInfo);
    if (errCode != 0) {
        FI_HILOGE("GetLocalBasicInfo errCode:%{public}d", errCode);
        return "";
    }
    std::string udid = "";
    OHOS::DistributedHardware::DeviceManager::GetInstance().GetUuidByNetworkId(PKG_NAME, dmDeviceInfo.networkId,
        udid);
    return udid;
}
 
std::string GetUdidByNetworkId(std::string networkId)
{
    const std::string PKG_NAME = "DBinderBus_Dms_" + std::to_string(getpid());
    std::string udid = "";
    OHOS::DistributedHardware::DeviceManager::GetInstance().GetUdidByNetworkId(PKG_NAME, networkId, udid);
    return udid;
}
} // namespace COORDINATION
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
