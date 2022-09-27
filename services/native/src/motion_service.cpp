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

#include "motion_service.h"

#include <ipc_skeleton.h>
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "motion_permission.h"
#include "motion_common.h"

namespace OHOS {
namespace Msdp {
namespace {
auto ms = MotionDelayedSpSingleton<MotionService>::GetInstance();
const bool G_REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(ms.GetRefPtr());
constexpr int32_t MAX_DMUP_PARAM = 2;
}
MotionService::MotionService() : SystemAbility(MSDP_MOTION_SERVICE_ID, true)
{
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "Add SystemAbility");
}

MotionService::~MotionService() {}

void MotionService::OnDump()
{
    MOTION_HILOGD(MOTION_MODULE_SERVICE, "OnDump");
}

void MotionService::OnStart()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    if (ready_) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "OnStart is ready, nothing to do.");
        return;
    }

    if (!Init()) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "OnStart call init fail");
        return;
    }
    if (!Publish(MotionDelayedSpSingleton<MotionService>::GetInstance())) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "OnStart register to system ability manager failed");
        return;
    }
    ready_ = true;
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "OnStart and add system ability success");
}

void MotionService::OnStop()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    if (!ready_) {
        return;
    }
    ready_ = false;

    if (!motionManager_) {
        motionManager_->UnloadAlgorithm();
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "unload algorithm library");
    }

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Exit");
}

bool MotionService::Init()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");

    if (!motionManager_) {
        motionManager_ = std::make_shared<MotionManager>(ms);
    }
    if (!motionManager_->Init()) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "OnStart init fail");
        return false;
    }

    return true;
}

bool MotionService::IsServiceReady()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    return ready_;
}

std::shared_ptr<MotionManager> MotionService::GetMotionManager()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    return motionManager_;
}

void MotionService::Subscribe(const MotionDataUtils::MotionType& type,
    const sptr<ImotionCallback>& callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    motionManager_->Subscribe(type, callback);
}

void MotionService::UnSubscribe(const MotionDataUtils::MotionType& type,
    const sptr<ImotionCallback>& callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    motionManager_->UnSubscribe(type, callback);
}

MotionDataUtils::MotionData MotionService::GetCache(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    return motionManager_->GetLatestMotionData(type);
}

int32_t MotionService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    MotionDump &motionDump = MotionDump::GetInstance();
    if ((args.empty()) || (args[0].size() != MAX_DMUP_PARAM)) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "param cannot be empty or the length is not 2");
        dprintf(fd, "cmd param number is not equal to 2\n");
        motionDump.DumpHelp(fd);
        return -1;
    }
    bool helpRet = motionDump.DumpMotionHelp(fd, args);
    bool total = helpRet;
    if (!total) {
        dprintf(fd, "cmd param is error\n");
        motionDump.DumpHelp(fd);
        return -1;
    }
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
    return 0;
}
} // namespace Msdp
} // namespace OHOS
