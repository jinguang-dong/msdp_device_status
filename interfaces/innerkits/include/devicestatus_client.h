/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef DEVICESTATUS_CLIENT_H
#define DEVICESTATUS_CLIENT_H

#include <singleton.h>
#include <map>

#include "idevicestatus.h"
#include "idevicestatus_callback.h"
#include "devicestatus_data_utils.h"
#include "devicestatus_common.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class DeviceStatusClient final : public DelayedRefSingleton<DeviceStatusClient> {
    DECLARE_DELAYED_REF_SINGLETON(DeviceStatusClient)

public:
std::map<Type, int32_t> GetTypeMap()
    {
        return typeMap_;
    }
    DISALLOW_COPY_AND_MOVE(DeviceStatusClient);

    void SubscribeCallback(Type type, ActivityEvent event, ReportLatencyNs latency,
    sptr<IRemoteDevStaCallbck> callback);
    void UnSubscribeCallback(Type type, ActivityEvent event, sptr<IRemoteDevStaCallbck> callback);
    Data GetDeviceStatusData(const Type type);

private:
    class DeviceStatusDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        DeviceStatusDeathRecipient() = default;
        ~DeviceStatusDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remote);
    private:
        DISALLOW_COPY_AND_MOVE(DeviceStatusDeathRecipient);
    };

    ErrCode Connect();
    sptr<Idevicestatus> devicestatusProxy_ {nullptr};
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ {nullptr};
    void ResetProxy(const wptr<IRemoteObject>& remote);
    std::mutex mutex_;
    std::map<Type, int32_t> typeMap_ = {};
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_CLIENT_H
