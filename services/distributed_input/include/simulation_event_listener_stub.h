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
#ifndef SIMULATION_EVENT_LISTENER_STUB_H
#define SIMULATION_EVENT_LISTENER_STUB_H
#include "refbase.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class SimulationEventListenerStub : public RefBase {
public:
    SimulationEventListenerStub() = default;
    ~SimulationEventListenerStub() = default;
    virtual int32_t OnSimulationEvent(uint32_t type, uint32_t code, int32_t value);
};
}
}
}
#endif