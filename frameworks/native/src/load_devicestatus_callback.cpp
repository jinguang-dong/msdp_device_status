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

#include "load_devicestatus_callback.h"

#include "devicestatus_common.h"
#include "devicestatus_client.h"

namespace OHOS {
namespace Msdp {
void LoadDeviceStatusCallback::OnLoadSystemAbilitySucess(
    int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject)
{  
    DEV_HILOGI(INNERKIT, "load devicestatus SA sucess, systemAbilityId %{public}d, remoteObject result:%{public}s",
    systemAbilityId, (remoteObject != nullptr) ? "true" : "false"));
    if (remoteObject == nullptr) {
        DEV_HILOGI(INNERKIT, "remoteObject is nullptr");
        return;
    }
    DevicestatusClient::GetInstance().LodServiceSuccess();
}    

void LoadDeviceStatusCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    DEV_HILOGI(INNERKIT, "load devicestatus failed, systemAbilityId %{public}d", systemAbilityId);
    DevicestatusClient::GetInstance().LodServiceFail();
}
} //namespace Msdp
} //namespace OHOS