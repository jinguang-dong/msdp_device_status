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
#ifndef DISTRIBUTED_INPUT_KIT_H
#define DISTRIBUTED_INPUT_KIT_H
#include "simulation_event_listener_stub.h"
#include "i_start_stop_d_inputs_call_back.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {

struct BusinessEvent {
    int32_t keyCode { -1 };
    int32_t keyAction { 0 };
    std::vector<int32_t> pressedKeys;
};

class DistributedInputKit {
public:
    inline static bool IsNeedFilterOut(const std::string &deviceId, const BusinessEvent &event)
    {
        return true;
    }
    inline static void RegisterSimulationEventListener(sptr<SimulationEventListenerStub> simulationEventListener)
    {
    }
    inline static void UnregisterSimulationEventListener(sptr<SimulationEventListenerStub> simulationEventListener)
    {
    }
    inline static int32_t StartRemoteInput(const std::string &deviceId, const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> cb)
    {
        return 1;
    }
    inline static int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes, sptr<IStartDInputCallback> cb)
    {
        return 1;
    }
    inline static int32_t StartRemoteInput(const std::string &srcId, const std::string &sinkId, const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> cb)
    {
        return 1;
    }
    inline static int32_t StopRemoteInput(const std::string &deviceId, const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> cb)
    {
        return 1;
    }
    inline static int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId, const uint32_t &inputTypes, sptr<IStopDInputCallback> cb)
    {
        return 1;
    }
    inline static int32_t StopRemoteInput(const std::string &srcId, const std::string &sinkId, const std::vector<std::string> &dhIds, sptr<IStartStopDInputsCallback> cb)
    {
        return 1;
    }
    inline static int32_t PrepareRemoteInput(const std::string &srcId, const std::string &sinkId, sptr<IPrepareDInputCallback> cb)
    {
        return 1;
    }
    inline static int32_t UnprepareRemoteInput(const std::string &srcId, const std::string &sinkId, sptr<IUnprepareDInputCallback> cb)
    {
        return 1;
    }
    inline static int32_t PrepareRemoteInput(const std::string &deviceId, sptr<IPrepareDInputCallback> cb)
    {
        return 1;
    }
    inline static int32_t UnprepareRemoteInput(const std::string &deviceId, sptr<IUnprepareDInputCallback> cb)
    {
        return 1;
    }
};
}
}
}
#endif