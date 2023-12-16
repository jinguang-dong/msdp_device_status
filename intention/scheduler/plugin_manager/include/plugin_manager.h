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

#include <map>

#include "nocopyable.h"

#include "i_context.h"
#include "i_plugin_manager.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {

// Loading、unloading and bookkeeping of modules.
class PluginManager final : public IPluginManager {
public:
    class Plugin {
    public:
        Plugin(IContext *context, void *handle);
        Plugin(Plugin &&other);
        DISALLOW_COPY(Plugin);
        ~Plugin();
        Plugin& operator=(Plugin &&other);

        IPlugin* GetInstance();

    private:
        IContext *context_ { nullptr };
        void *handle_ { nullptr };
        IPlugin *instance_ { nullptr };
    };

    PluginManager(IContext *context);
    ~PluginManager() = default;
    DISALLOW_COPY_AND_MOVE(PluginManager);

    // Load module identified by [`intention`].
    IPlugin* LoadPlugin(Intention intention);

    // Unload module identified by [`intention`].
    void UnloadPlugin(Intention intention);

private:
    void LoadLibrary(Intention intention);
    IPlugin* DoLoadPlugin(Intention intention);

private:
    IContext *context_ { nullptr };
    std::map<Intention, Plugin> libs_;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // PLUGIN_MANAGER_H