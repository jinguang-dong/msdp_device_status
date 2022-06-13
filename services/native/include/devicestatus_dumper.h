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

#ifndef DEVICESTATUS_DUMPER_H
#define DEVICESTATUS_DUMPER_H

#include <refbase.h>
#include <map>
#include <singleton.h>
#include <string>
#include <vector>

#include "devicestatus_data_utils.h"

namespace OHOS {
namespace Msdp {
const std::string ARG_DUMP_HELP = "-h";
const std::string ARG_DUMP_DEVICESTATUS_SUBSCRIBER = "-s";
const std::string ARG_DUMP_DEVICESTATUS_CURRENT_STATE = "-c";
constexpr int32_t ERR_OK = 0;
constexpr int32_t ERR_NG = -1;

class DevicestatusDumper final : public RefBase,
    public Singleton<DevicestatusDumper> {
public:
    DevicestatusDumper() = default;
    virtual ~DevicestatusDumper() = default;
    void DumpHelpInfo(int32_t fd) const;
    void DumpIllegalArgsInfo(int32_t fd) const;
    void DumpDevicestatusSubscriber(int32_t fd, const std::map<DevicestatusDataUtils::DevicestatusType,
        int32_t> &listenerMap) const;
    void DumpDevicestatusCurrentStatus(int32_t fd,
        const std::vector<DevicestatusDataUtils::DevicestatusData> &datas) const;
private:
    DISALLOW_COPY_AND_MOVE(DevicestatusDumper);
    void DumpCurrentTime(int32_t fd) const;
    std::string GetStatusType(const DevicestatusDataUtils::DevicestatusType &type) const;
    std::string GetDeviceState(const DevicestatusDataUtils::DevicestatusValue &type) const;
};
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_DUMPER_H