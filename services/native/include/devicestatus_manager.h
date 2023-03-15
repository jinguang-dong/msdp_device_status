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
#include <list>

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
        ClientInfo(ActivityEvent event, ReportLatencyNs latency)
            : event_(event), latency_(latency) {}
        ~ClientInfo() = default;
        ActivityEvent GetEvent() const;
        ReportLatencyNs GetLatency() const;
        void SetEvent(ActivityEvent event);
        void SetLatency(ReportLatencyNs latency);
        void SetEnabled(bool enabled);
        bool GetEnabled() const;
    private:
        ActivityEvent event_ { ActivityEvent::EVENT_INVALID };
        ReportLatencyNs latency_ { ReportLatencyNs::Latency_INVALID };
        bool enabled_ { false };
    };

    bool Init();
    int32_t UpdateDataChannel(int32_t pid, sptr<IRemoteDevStaCallback> callback);
    int32_t DestoryDataChannel(int32_t pid, sptr<IRemoteDevStaCallback> callback);
    bool UpdateClientInfo(int32_t pid, Type type, std::shared_ptr<ClientInfo> clientInfo);
    bool DestoryInfo(int32_t pid, Type type, std::shared_ptr<ClientInfo> clientInfo);
    Data GetLatestDeviceStatusData(Type type);
    bool OnlyCurPidSubscribe(Type type, int32_t pid);
    bool ClearClientInfo(Type type);
    bool ClearCurPidClientInfo(Type type, int32_t pid);
    bool IsOnlyCurPidSubscribe(Type type, int32_t pid);
    std::list<sptr<IRemoteDevStaCallback>> GetChannels(Type type);
    std::shared_ptr<ClientInfo> GetClientInfo(Type type, int32_t pid);
    int32_t LoadAlgorithm();
    int32_t UnloadAlgorithm();
    int32_t GetPackageName(AccessTokenID tokenId, std::string &packageName);
    bool Enable(Type type);
    bool Disable(Type type);
    bool CheckEnable(Type type, int32_t pid);
private:
    bool InitAlgoMngrInterface(Type type);
    int32_t InitDataCallback();
    int32_t NotifyDeviceStatusChange(const Data &devicestatusData);
    int32_t SensorDataCallback(struct SensorEvents *event);
    int32_t MsdpDataCallback(const Data &data);

    const wptr<DeviceStatusService> ms_;
    std::mutex mutex_;
    std::mutex channelMutex_;
    std::mutex clientInfoMutex_;
    sptr<IRemoteObject::DeathRecipient> devicestatusCBDeathRecipient_;
    std::unique_ptr<DeviceStatusMsdpClientImpl> msdpImpl_;
    std::map<Type, OnChangedValue> msdpData_;
    std::map<int32_t, sptr<IRemoteDevStaCallback>> dataChannels_;
    std::map<int32_t, std::map<int32_t, std::shared_ptr<ClientInfo>>> clientInfos_;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_MANAGER_H
