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

#ifndef DEVICESTATUS_MANAGER_H
#define DEVICESTATUS_MANAGER_H

#include <set>
#include <map>

#include "sensor_if.h"
#include "devicestatus_data_utils.h"
#include "idevicestatus_algorithm.h"
#include "idevicestatus_callback.h"
#include "devicestatus_common.h"
#include "devicestatus_msdp_client_impl.h"
#include "accesstoken_kit.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace Security::AccessToken;
static constexpr uint8_t ARG_4 = 4;
class DeviceStatusService;
class DeviceStatusManager {
public:
    explicit DeviceStatusManager(const wptr<DeviceStatusService>& ms) : ms_(ms)
    {
        DEV_HILOGI(SERVICE, "DeviceStatusManager instance is created.");
    }
    ~DeviceStatusManager() = default;

    class DeviceStatusCallbackDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        DeviceStatusCallbackDeathRecipient() = default;
        virtual void OnRemoteDied(const wptr<IRemoteObject> &remote);
        virtual ~DeviceStatusCallbackDeathRecipient() = default;
    };

    bool Init();
    bool EnableRdb() { return true; }
    bool InitInterface() { return true; }
    bool DisableRdb() { return true; }
    void NotifyDevicestatusChange(const Data& devicestatusData) { return; }
    void Subscribe(const Type& type, const sptr<IdevicestatusCallback>& callback) { return; }
    void UnSubscribe(const Type& type, const sptr<IdevicestatusCallback>& callback) { return; }
    bool Enable(Type type);
    bool InitAlgoMngrInterface(Type type);
    bool Disable(Type type);
    int32_t InitDataCallback();
    int32_t NotifyDeviceStatusChange(const Data& devicestatusData);
    void Subscribe(Type type,
        ActivityEvent event,
        ReportLatencyNs latency,
        sptr<IdevicestatusCallback> callback);
    void UnSubscribe(Type type,
        ActivityEvent event, const sptr<IdevicestatusCallback> callback);
    Data GetLatestDeviceStatusData(Type type);
    int32_t SensorDataCallback(const struct SensorEvents *event);
    int32_t MsdpDataCallback(const Data& data);
    int32_t LoadAlgorithm(bool bCreate) { return 0; }
    int32_t UnloadAlgorithm(bool bCreate) { return 0; }
    int32_t LoadAlgorithm();
    int32_t UnloadAlgorithm();
    int32_t GetPackageName(AccessTokenID tokenId, std::string &packageName);

private:
    struct classcomp {
        bool operator()(const sptr<IdevicestatusCallback> &l, const sptr<IdevicestatusCallback> &r) const
        {
            return l->AsObject() < r->AsObject();
        }
    };
    const wptr<DeviceStatusService> ms_;
    std::mutex mutex_;
    sptr<IRemoteObject::DeathRecipient> devicestatusCBDeathRecipient_;
    std::unique_ptr<DeviceStatusMsdpClientImpl> msdpImpl_;
    std::map<Type, OnChangedValue> msdpData_;
    std::map<Type, std::set<const sptr<IdevicestatusCallback>, classcomp>> listenerMap_;
    int32_t type_;
    int32_t event_;
    int arrs_ [ARG_4] = {};
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_MANAGER_H
