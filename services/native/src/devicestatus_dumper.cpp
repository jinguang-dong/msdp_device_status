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

#include "devicestatus_dumper.h"

#include <cinttypes>
#include <csignal>
#include <iomanip>
#include <map>
#include <sstream>

#include "string_ex.h"
#include "unique_fd.h"

#include "devicestatus_common.h"

namespace OHOS {
namespace Msdp {
namespace {
    constexpr uint32_t MS_NS = 1000000;
}
void DevicestatusDumper::DumpDevicestatusSubscriber(int32_t fd,
    const std::map<DevicestatusDataUtils::DevicestatusType, int32_t> &listenerMap) const
{
    DEV_HILOGI(SERVICE, "DumpDevicestatusSubscriber start");
    DumpCurrentTime(fd);
    dprintf(fd, "There are currently %d subscribers.\n", listenerMap.size());
    for (auto it = listenerMap.begin(); it != listenerMap.end(); ++it) {
        dprintf(fd, "Device status Type is %s , current type callback number is %d .\n",
            GetStatusType(it->first).c_str(), it->second);
    }
}

void DevicestatusDumper::DumpDevicestatusCurrentStatus(int32_t fd,
    const std::vector<DevicestatusDataUtils::DevicestatusData> &datas) const
{
    DEV_HILOGI(SERVICE, "DumpDevicestatusCurrentStatus start");
    DumpCurrentTime(fd);
    dprintf(fd, "Current device status: \n");
    if (datas.size() == 0) {
        dprintf(fd, "No device status available\n");
    }
    int32_t num = 0;
    for (auto it = datas.begin(); it != datas.end(); ++it) {
        if (it->value == DevicestatusDataUtils::VALUE_INVALID) {
            continue;
        }
        num ++;
        dprintf(fd, "Device status Type is %s , current type state is %s .\n",
            GetStatusType(it->type).c_str(), GetDeviceState(it->value).c_str());
    }
    if (num == 0) {
        dprintf(fd, "No device status available\n");
    }
}

std::string DevicestatusDumper::GetDeviceState(const DevicestatusDataUtils::DevicestatusValue &value) const
{
    std::string valueString;
    switch (value) {
        case DevicestatusDataUtils::VALUE_ENTER: {
            valueString = "enter";
            break;
        }
        case DevicestatusDataUtils::VALUE_EXIT: {
            valueString = "exit";
            break;
        }
        case DevicestatusDataUtils::VALUE_INVALID: {
            valueString = "invalid";
            break;
        }
        default: {
            valueString = "unknown";
            break;
        }
    }
    return valueString;
}

std::string DevicestatusDumper::GetStatusType(const DevicestatusDataUtils::DevicestatusType &type) const
{
    std::string typeString;
    switch (type) {
        case DevicestatusDataUtils::TYPE_HIGH_STILL: {
            typeString = "high still";
            break;
        }
        case DevicestatusDataUtils::TYPE_FINE_STILL: {
            typeString = "fine still";
            break;
        }
        case DevicestatusDataUtils::TYPE_CAR_BLUETOOTH: {
            typeString = "car bluetooth";
            break;
        }
        case DevicestatusDataUtils::TYPE_LID_OPEN: {
            typeString = "lid open";
            break;
        }
        default: {
            typeString = "unknown";
            break;
        }
    }
    return typeString;
}

void DevicestatusDumper::DumpCurrentTime(int32_t fd) const
{
    timespec curTime = { 0, 0 };
    clock_gettime(CLOCK_REALTIME, &curTime);
    struct tm *timeinfo = localtime(&(curTime.tv_sec));
    if (timeinfo == nullptr) {
        DEV_HILOGI(SERVICE, "DumpCurrentTime get localtime failed");
        return;
    }
    dprintf(fd, "Current time: %02d:%02d:%02d.%03d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
            int32_t { (curTime.tv_nsec / MS_NS) });
}

void DevicestatusDumper::DumpIllegalArgsInfo(int32_t fd) const
{
    dprintf(fd, "The arguments are illegal and you can enter '-h' for help.");
}

void DevicestatusDumper::DumpHelpInfo(int32_t fd) const
{
    dprintf(fd, "Usage:\n");
    dprintf(fd, "      -h: dump help\n");
    dprintf(fd, "      -s: dump the device_status subscribers\n");
    dprintf(fd, "      -c: dump the device_status current device status\n");
}
} // namespace Msdp
} // namespace OHOS