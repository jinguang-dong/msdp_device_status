/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include "i_context.h"
#include "i_plugin_manager.h"
#include "stream_server.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
typedef IDeviceManager* DeviceManagerPlugin(IContext* context);
typedef ICoordination* CoordinationPlugin(IContext* context);
class PluginManager : public IPluginManager {
public:
    DISALLOW_COPY_AND_MOVE(PluginManager);

    PluginManager() = default;
    ~PluginManager() = default;
    int32_t Init(IContext* context, StreamServer* streamServer);
    IDeviceManager* GetDeviceManager() override;
    ICoordination* GetCoordination() override;
    int32_t UninstallDeviceManager();
    int32_t UninstallCoordination();
    int32_t LoadDeviceManager();
    int32_t LoadCoordination();
    void OnSessionLost(SessionPtr session);
    void SetSessionID(int32_t pid)
    {
        sessionID_ = pid;
    }
    
private:
    IContext *context_ { nullptr };
    void *deviceManagerHandle_ { nullptr };
    void *coordinationHandle_ { nullptr };
    IDeviceManager* deviceManager_ { nullptr };
    ICoordination* coordination_ { nullptr };
    StreamServer* streamServer_ { nullptr };
    int32_t sessionID_ { -1 };
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // PLUGIN_MANAGER_H
