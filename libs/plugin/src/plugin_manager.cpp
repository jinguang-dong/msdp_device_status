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
#include "plugin_manager.h"

#include <string>

#include <dlfcn.h>

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MSDP_DOMAIN_ID, "PluginManager"};

#ifdef __aarch64__
const std::string deviceLib = "/system/lib64/libmsdp_device_manager.z.so";
const std::string coordinationLib = "/system/lib64/libmsdp_coordination.z.so";
#else
const std::string deviceLib = "/system/lib/libmsdp_device_manager.z.so";
const std::string coordinationLib = "/system/lib/libmsdp_coordination.z.so";
#endif // #ifdef __aarch64__
}

int32_t PluginManager::Init(IContext* context, StreamServer* streamServer)
{
    CALL_DEBUG_ENTER;
    CHKPR(context, RET_ERR);
    CHKPR(streamServer, RET_ERR);
    context_ = context;
    streamServer_ = streamServer;
    return RET_OK;
}

int32_t PluginManager::LoadDeviceManager()
{
    CALL_DEBUG_ENTER;
    if (deviceManager_ != nullptr) {
        FI_HILOGW("deviceManager_ has been loaded");
        return RET_OK;
    }
    deviceManagerHandle_ = dlopen(deviceLib.data(), RTLD_NOW);
    if (deviceManagerHandle_ == nullptr) {
        FI_HILOGE("Open plugin failed, plugin name:%{public}s, msg:%{public}s", deviceLib.data(), dlerror());
        return RET_ERR;
    }
    auto func = (DeviceManagerPlugin*)dlsym(deviceManagerHandle_, "CreateDeviceManager");
    if (func == nullptr) {
        FI_HILOGE("Dlsym msg:%{public}s", dlerror());
        return RET_ERR;
    }
    CHKPR(context_, RET_ERR);
    deviceManager_ = func(context_);
    CHKPR(deviceManager_, RET_ERR);
    context_->EnableDeviceMananger();

    return RET_OK;
}

int32_t PluginManager::UninstallDeviceManager()
{
    CALL_DEBUG_ENTER;
    if (deviceManagerHandle_ == nullptr) {
        FI_HILOGE("Open plugin failed, so name:%{public}s, msg:%{public}s", deviceLib.data(), dlerror());
        return RET_ERR;
    }
    auto releasePlugin = (void(*)(IDeviceManager*, IContext *context))dlsym(deviceManagerHandle_,
        "ReleaseDeviceManager");
    if (releasePlugin == nullptr) {
        FI_HILOGE("Dlsym msg:%{public}s", dlerror());
        return RET_ERR;
    }
    releasePlugin(deviceManager_, context_);
    deviceManager_ = nullptr;
    FI_HILOGE("start DeviceManager dlclose");
    dlclose(deviceManagerHandle_);
    FI_HILOGE("DeviceManager dlclose msg:%{public}s", dlerror());
    deviceManagerHandle_ = nullptr;
    return RET_OK;
}

IDeviceManager* PluginManager::GetDeviceManager()
{
    CALL_DEBUG_ENTER;
    return deviceManager_;
}

int32_t PluginManager::LoadCoordination()
{
    CALL_DEBUG_ENTER;
    if (coordination_ != nullptr) {
        FI_HILOGW("coordination_ has been loaded");
        return RET_OK;
    }
    coordinationHandle_ = dlopen(coordinationLib.data(), RTLD_NOW);
    if (coordinationHandle_ == nullptr) {
        FI_HILOGE("Open plugin failed, plugin name:%{public}s, msg:%{public}s", coordinationLib.data(), dlerror());
        return RET_ERR;
    }
    FI_HILOGE("Open coordination"); // lzc
    auto func = (CoordinationPlugin*)dlsym(coordinationHandle_, "CreateCoordination");
    if (func == nullptr) {
        FI_HILOGE("Dlsym msg:%{public}s", dlerror());
        return RET_ERR;
    }
    coordination_ = func(context_);
    CHKPR(coordination_, RET_ERR);
    CHKPR(streamServer_, RET_ERR);
    streamServer_->AddSessionDeletedCallback(sessionID_,
        std::bind(&PluginManager::OnSessionLost, this, std::placeholders::_1));
    // streamServer_->AddSessionDeletedCallback(sessionID_, [this](SessionPtr session) {
    //     coordination_->OnSessionLost(session);
    //     UninstallCoordination();
    //     UninstallDeviceManager();
    // });
    return RET_OK;
}

int32_t PluginManager::UninstallCoordination()
{
    CALL_DEBUG_ENTER;
    if (coordinationHandle_ == nullptr) {
        FI_HILOGE("Open plugin failed, plugin name:%{public}s, msg:%{public}s", coordinationLib.data(), dlerror());
        return RET_ERR;
    }
    auto releasePlugin = (void(*)(ICoordination*))dlsym(coordinationHandle_, "ReleaseCoordination");
    if (releasePlugin == nullptr) {
        FI_HILOGE("Dlsym msg:%{public}s", dlerror());
        return RET_ERR;
    }
    // CHKPR(streamServer_, RET_ERR);
    // streamServer_->RemoveSessionDeletedCallback(MSDP_COORDINATION);
    CHKPR(coordination_, RET_ERR);
    releasePlugin(coordination_);
    coordination_ = nullptr;
    FI_HILOGE("start Coordination dlclose"); // lzc
    CHKPR(coordinationHandle_, RET_ERR);
    dlclose(coordinationHandle_);
    FI_HILOGE("Coordination dlclose msg:%{public}s", dlerror()); // lzc
    coordinationHandle_ = nullptr;
    return RET_OK;
}

void PluginManager::OnSessionLost(SessionPtr session)
{
    CALL_DEBUG_ENTER;
    CHKPV(coordination_);
    coordination_->OnSessionLost(session);
    UninstallCoordination();
    UninstallDeviceManager();
}

ICoordination* PluginManager::GetCoordination()
{
    CALL_DEBUG_ENTER;
    return coordination_;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
