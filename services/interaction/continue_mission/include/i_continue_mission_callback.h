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

#ifndef I_CONTINUE_MISSION_CALLBACK_H
#define I_CONTINUE_MISSION_CALLBACK_H

#include <iremote_broker.h>
#include <iremote_object.h>

#include "continue_mission_param.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class IContinueMissionCallback : public IRemoteBroker {
public:
    enum  {
        CONTINUE_MISSION = 0,
    };
    virtual int32_t OnContinueMission(const ContinueMissionParam& continueMissionParam) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.msdp.IContinueMissionCallback");
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // I_CONTINUE_MISSION_CALLBACK_H