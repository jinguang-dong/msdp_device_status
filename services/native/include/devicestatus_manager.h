/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
class DeviceStatusService;
class DeviceStatusManager {
public:
    explicit DeviceStatusManager(const wptr<DeviceStatusService> &ms) : ms_(ms) {}
    ~DeviceStatusManager() = default;

    class DeviceStatusCallbackDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        DeviceStatusCallbackDeathRecipient() = default;
        virtual void OnRemoteDied(const wptr<IRemoteObject> &remote);
        virtual ~DeviceStatusCallbackDeathRecipient() = default;
    };

    class ClientInfo {
    public:
        ClientInfo(int32_t pid, Type type, ActivityEvent event,
        sptr<IRemoteDevStaCallback> callback, ReportLatencyNs latency)
            : pid_(pid), type_(type), event_(event), callback_(callback), latency_(latency) {}
        ~ClientInfo() = default;

        int32_t GetPid() const
        {
            return pid_;
        }
        Type GetType() const
        {
            return type_;
        }
        ActivityEvent GetEvent() const
        {
            return event_;
        }
        sptr<IRemoteDevStaCallback> GetCallback() const
        {
            return callback_;
        }
        ReportLatencyNs GetLatency() const
        {
            return latency_;
        }
    private:
        int32_t pid_ { -1 };
        Type type_ { Type::TYPE_INVALID };
        ActivityEvent event_ { ActivityEvent::EVENT_INVALID };
        sptr<IRemoteDevStaCallback> callback_ { nullptr };
        ReportLatencyNs latency_ { ReportLatencyNs::Latency_INVALID };
    };

    bool Init();
    bool Enable(Type type);
    bool InitAlgoMngrInterface(Type type);
    bool Disable(Type type);
    int32_t InitDataCallback();
    int32_t NotifyDeviceStatusChange(const Data &devicestatusData);
    void Subscribe(std::shared_ptr<ClientInfo> clientInfo);
    void Unsubscribe(std::shared_ptr<ClientInfo> clientInfo);
    Data GetLatestDeviceStatusData(Type type);
    int32_t SensorDataCallback(struct SensorEvents *event);
    int32_t MsdpDataCallback(const Data &data);
    int32_t LoadAlgorithm();
    int32_t UnloadAlgorithm();
    int32_t GetPackageName(AccessTokenID tokenId, std::string &packageName);

private:
    const wptr<DeviceStatusService> ms_;
    std::mutex mutex_;
    sptr<IRemoteObject::DeathRecipient> devicestatusCBDeathRecipient_;
    std::unique_ptr<DeviceStatusMsdpClientImpl> msdpImpl_;
    std::map<Type, OnChangedValue> msdpData_;
    std::map<int32_t, std::shared_ptr<ClientInfo>> listenerMap_;
    int32_t type_ {};
    int32_t event_ {};
    std::map<int32_t, std::vector<Type>> activeTypes_;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_MANAGER_H
