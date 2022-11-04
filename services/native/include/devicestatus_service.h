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

#ifndef DEVICESTATUS_SERVICE_H
#define DEVICESTATUS_SERVICE_H

#include <memory>
#include <iremote_object.h>
#include <system_ability.h>

#include "devicestatus_srv_stub.h"
#include "idevicestatus_callback.h"
#include "devicestatus_data_utils.h"
#include "devicestatus_dumper.h"
#include "devicestatus_manager.h"
#include "devicestatus_delayed_sp_singleton.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class DeviceStatusService final : public SystemAbility, public DeviceStatusServiceStub {
    DECLARE_SYSTEM_ABILITY(DeviceStatusService)
    DECLARE_DELAYED_SP_SINGLETON(DeviceStatusService);
public:
    virtual void OnDump() override;
    virtual void OnStart() override;
    virtual void OnStop() override;

    void Subscribe(const Type& type,
        const ActivityEvent& event,
        const ReportLatencyNs& latency,
        const sptr<IRemoteDevStaCallbck>& callback) override;
    void UnSubscribe(const Type& type,
        const ActivityEvent& event,
        const sptr<IRemoteDevStaCallbck>& callback) override;
    Data GetCache(const Type& type) override;
    bool IsServiceReady();
    std::shared_ptr<DeviceStatusManager> GetDeviceStatusManager();
    int Dump(int fd, const std::vector<std::u16string>& args) override;
    void ReportSensorSysEvent(int32_t type, bool enable);
private:
    bool Init();
    std::atomic<bool> ready_ = false;
    std::shared_ptr<DeviceStatusManager> devicestatusManager_;
    std::shared_ptr<DeviceStatusMsdpClientImpl> msdpImpl_;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_SERVICE_H
