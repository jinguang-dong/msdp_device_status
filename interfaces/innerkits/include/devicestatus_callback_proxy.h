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

#ifndef DEVICESTATUS_CALLBACK_PROXY_H
#define DEVICESTATUS_CALLBACK_PROXY_H

#include <iremote_proxy.h>
#include <nocopyable.h>

#include "idevicestatus_callback.h"
#include "devicestatus_data_utils.h"
#include "devicestatus_client.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class DeviceStatusCallbackProxy : public IRemoteProxy<IRemoteDevStaCallback> {
public:
    explicit DeviceStatusCallbackProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IRemoteDevStaCallback>(impl) {}
    ~DeviceStatusCallbackProxy() = default;
    DISALLOW_COPY_AND_MOVE(DeviceStatusCallbackProxy);
    virtual void OnDeviceStatusChanged(const Data& devicestatusData) override;

private:
    static inline BrokerDelegator<DeviceStatusCallbackProxy> delegator_;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_CALLBACK_PROXY_H
