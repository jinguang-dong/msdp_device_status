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

#include "devicestatus_agent.h"

#include "devicestatus_common.h"
#include "devicestatus_client.h"
#include "devicestatus_define.h"

#include "idevicestatus_callback.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
void DeviceStatusAgent::DeviceStatusAgentCallback::OnDeviceStatusChanged(
    const Data& devicestatusData)
{
    DEV_HILOGI(INNERKIT, "type:%{public}d, value:%{public}d", static_cast<Type>(devicestatusData.type),
        static_cast<OnChangedValue>(devicestatusData.value));
    std::shared_ptr<DeviceStatusAgent> agent = agent_.lock();
    if (agent == nullptr) {
        DEV_HILOGE(SERVICE, "agent is nullptr");
        return;
    }
    auto iter = agent->agentEvents_.find(devicestatusData.type);
    if (iter == agent->agentEvents_.end()) {
        DEV_HILOGE(INNERKIT, "Failed to find type");
        return;
    }
    iter->second->OnEventResult(devicestatusData);
}

int32_t DeviceStatusAgent::SubscribeAgentEvent(const Type& type,
    const ActivityEvent& event, const ReportLatencyNs& latency,
    std::shared_ptr<DeviceStatusAgent::DeviceStatusAgentEvent> agentEvent)
{
    DEV_HILOGD(INNERKIT, "Enter");
    if (agentEvent == nullptr) {
        DEV_HILOGE(INNERKIT, "agentEvent is nullptr");
        return ERR_INVALID_VALUE;
    }
    if (!IsSupportType(type)) {
        DEV_HILOGE(INNERKIT, "Failed to support type");
        return ERR_INVALID_VALUE;
    }
    if (!IsSupportEvent(event)) {
        DEV_HILOGE(INNERKIT, "Failed to support event");
        return ERR_INVALID_VALUE;
    }
    RegisterServiceEvent(type, event, latency);
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = agentEvents_.find(type);
    if (iter == agentEvents_.end()) {
        agentEvents_.emplace(type, agentEvent);
    } else {
        iter->second = agentEvent;
    }
    return RET_OK;
}

int32_t DeviceStatusAgent::UnsubscribeAgentEvent(const Type& type, const ActivityEvent& event)
{
    DEV_HILOGD(INNERKIT, "Enter");
    if (!IsSupportType(type)) {
        DEV_HILOGE(INNERKIT, "Failed to support type");
        return ERR_INVALID_VALUE;
    }
    if (!IsSupportEvent(event)) {
        DEV_HILOGE(INNERKIT, "Failed to support event");
        return ERR_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = agentEvents_.find(type);
    if (iter == agentEvents_.end()) {
        DEV_HILOGE(INNERKIT, "Failed to find type");
        return ERR_INVALID_VALUE;
    }
    agentEvents_.erase(iter);
    UnRegisterServiceEvent(type, event);
    if (agentEvents_.empty()) {
        DeviceStatusClient::GetInstance().DestoryDataChannel(callback_);
        connected_ = false;
    }
    return RET_OK;
}

void DeviceStatusAgent::RegisterServiceEvent(const Type& type, const ActivityEvent& event,
    const ReportLatencyNs& latency)
{
    DEV_HILOGD(INNERKIT, "Enter");
    if (!connected_) {
        callback_ = new (std::nothrow) DeviceStatusAgentCallback(shared_from_this());
        DeviceStatusClient::GetInstance().CreateDataChannel(callback_);
        connected_ = true;
    }
    int32_t ret = DeviceStatusClient::GetInstance().SubscribeCallback(type, event, latency);
    if (ret != RET_OK) {
        DEV_HILOGE(INNERKIT, "Subscribe failed");
        return;
    }
}

void DeviceStatusAgent::UnRegisterServiceEvent(const Type& type,
    const ActivityEvent& event)
{
    DEV_HILOGD(INNERKIT, "Enter");
    int32_t ret = DeviceStatusClient::GetInstance().UnsubscribeCallback(type, event);
    if (ret != RET_OK) {
        DEV_HILOGE(INNERKIT, "Unsubscribe failed");
        return;
    }
}

Data DeviceStatusAgent::GetDeviceStatusData(const Type type)
{
    return DeviceStatusClient::GetInstance().GetDeviceStatusData(type);
}

void DeviceStatusAgent::RegisterDeathListener(std::function<void()> deathListener)
{
    DeviceStatusClient::GetInstance().RegisterDeathListener(deathListener);
}

bool DeviceStatusAgent::IsSupportType(const Type& type)
{
    return (type > Type::TYPE_INVALID && type < Type::TYPE_MAX);
}

bool DeviceStatusAgent::IsSupportEvent(const ActivityEvent& event)
{
    return (event > ActivityEvent::EVENT_INVALID && event <= ActivityEvent::ENTER_EXIT);
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
