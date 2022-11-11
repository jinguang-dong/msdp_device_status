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

#include "devicestatus_service.h"

#include <ipc_skeleton.h>

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "hisysevent.h"

#include "devicestatus_common.h"
#include "devicestatus_dumper.h"
#include "devicestatus_permission.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace OHOS::HiviewDFX;
namespace {
const bool REGISTER_RESULT = 
    SystemAbility::MakeAndRegisterAbility(DelayedSpSingleton<DeviceStatusService>::GetInstance().GetRefPtr());
} // namespace
DeviceStatusService::DeviceStatusService() : SystemAbility(MSDP_DEVICESTATUS_SERVICE_ID, true)
{
    DEV_HILOGD(SERVICE, "Add SystemAbility");
}

DeviceStatusService::~DeviceStatusService() {}

void DeviceStatusService::OnDump()
{
    DEV_HILOGI(SERVICE, "OnDump");
}

void DeviceStatusService::OnStart()
{
    DEV_HILOGD(SERVICE, "Enter");
    if (ready_) {
        DEV_HILOGE(SERVICE, "OnStart is ready, nothing to do");
        return;
    }

    if (!Init()) {
        DEV_HILOGE(SERVICE, "OnStart call init fail");
        return;
    }
    if (!Publish(DelayedSpSingleton<DeviceStatusService>::GetInstance())) {
        DEV_HILOGE(SERVICE, "OnStart register to system ability manager failed");
        return;
    }
    ready_ = true;
    DEV_HILOGI(SERVICE, "OnStart and add system ability success");
}

void DeviceStatusService::OnStop()
{
    DEV_HILOGD(SERVICE, "Enter");
    if (!ready_) {
        return;
    }
    ready_ = false;

    if (devicestatusManager_ == nullptr) {
        DEV_HILOGE(SERVICE, "devicestatusManager_ is null");
        return;
    }
    DEV_HILOGD(SERVICE, "unload algorithm library exit");
}

int DeviceStatusService::Dump(int fd, const std::vector<std::u16string>& args)
{
    DEV_HILOGI(SERVICE, "dump DeviceStatusServiceInfo");
    if (fd < 0) {
        DEV_HILOGE(SERVICE, "fd is invalid");
        return RET_NG;
    }
    DeviceStatusDumper &deviceStatusDumper = DeviceStatusDumper::GetInstance();
    if (args.empty()) {
        DEV_HILOGE(SERVICE, "param cannot be empty");
        dprintf(fd, "param cannot be empty\n");
        deviceStatusDumper.DumpHelpInfo(fd);
        return RET_NG;
    }
    std::vector<std::string> argList = { "" };
    std::transform(args.begin(), args.end(), std::back_inserter(argList),
        [](const std::u16string &arg) {
        return Str16ToStr8(arg);
    });

    std::vector<Data> datas;
    for (auto type = TYPE_STILL;type <= TYPE_LID_OPEN;
        type = (Type)(type+1)) {
        Data data = GetCache(type);
        if (data.value != OnChangedValue::VALUE_INVALID) {
            datas.emplace_back(data);
        }
    }
    deviceStatusDumper.ParseCommand(fd, argList, datas);
    return RET_OK;
}


bool DeviceStatusService::Init()
{
    DEV_HILOGD(SERVICE, "Enter");
    if (devicestatusManager_ == nullptr) {
        DEV_HILOGE(SERVICE, "devicestatusManager_ is null");
        auto ms = DelayedSpSingleton<DeviceStatusService>::GetInstance();
        devicestatusManager_ = std::make_shared<DeviceStatusManager>(ms);
    }
    if (!devicestatusManager_->Init()) {
        DEV_HILOGE(SERVICE, "OnStart init fail");
        return false;
    }

    return true;
}

bool DeviceStatusService::IsServiceReady()
{
    DEV_HILOGD(SERVICE, "Enter");
    return ready_;
}

std::shared_ptr<DeviceStatusManager> DeviceStatusService::GetDeviceStatusManager()
{
    DEV_HILOGD(SERVICE, "Enter");
    return devicestatusManager_;
}

void DeviceStatusService::Subscribe(Type type,
    ActivityEvent event,
    ReportLatencyNs latency,
    sptr<IdevicestatusCallback> callback)
{
    DEV_HILOGI(SERVICE, "Enter event:%{public}d,latency:%{public}d", event, latency);
    if (devicestatusManager_ == nullptr) {
        DEV_HILOGE(SERVICE, "UnSubscribe func is nullptr");
        return;
    }
    DeviceStatusDumper::GetInstance().SaveAppInfo(type,callback);
    devicestatusManager_->Subscribe(type,event,latency, callback);
    DEV_HILOGD(SERVICE, "Exit");
    ReportSensorSysEvent(type, true);
}

void DeviceStatusService::UnSubscribe(Type type,
    ActivityEvent event,
    sptr<IdevicestatusCallback> callback)
{
    DEV_HILOGE(SERVICE, "EnterUNevent: %{public}d", event);
    if (devicestatusManager_ == nullptr) {
        DEV_HILOGE(SERVICE, "UnSubscribe func is nullptr");
        return;
    }

    auto appInfo = std::make_shared<AppInfo>();
    if (appInfo == nullptr) {
        DEV_HILOGE(SERVICE, "appInfo is null");
        return;
    }
    appInfo->uid = IPCSkeleton::GetCallingUid();
    appInfo->pid = IPCSkeleton::GetCallingPid();
    appInfo->tokenId = IPCSkeleton::GetCallingTokenID();
    appInfo->packageName = DeviceStatusDumper::GetInstance().GetPackageName(appInfo->tokenId);
    appInfo->type = type;
    appInfo->callback = callback;
    DeviceStatusDumper::GetInstance().RemoveAppInfo(appInfo);
    devicestatusManager_->UnSubscribe(type, event, callback);
    ReportSensorSysEvent(type, false);
}

Data DeviceStatusService::GetCache(const Type& type)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (devicestatusManager_ == nullptr) {
        Data data = {type, OnChangedValue::VALUE_EXIT};
        data.value = OnChangedValue::VALUE_INVALID;
        DEV_HILOGI(SERVICE, "GetLatestDeviceStatusData func is nullptr,return default!");
        return data;
    }
    return devicestatusManager_->GetLatestDeviceStatusData(type);
}

void DeviceStatusService::ReportSensorSysEvent(int32_t type, bool enable)
{
    auto callerToken = GetCallingTokenID();
    std::string packageName;
    devicestatusManager_->GetPackageName(callerToken, packageName);
    auto uid = GetCallingUid();
    std::string str = enable ? "Subscribe" : "UnSubscribe";
    HiSysEvent::Write(HiSysEvent::Domain::MSDP, str, HiSysEvent::EventType::STATISTIC,
        "UID", uid, "PKGNAME", packageName, "TYPE", type);
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
