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

#ifndef FUSION_DISTRIBUTED_INPUT_INTERNAL_H
#define FUSION_DISTRIBUTED_INPUT_INTERNAL_H

#include "fusion_distributed_input.h"
#include "distributed_input_kit.h"
#include "i_start_stop_d_inputs_call_back.h"
#include "prepare_d_input_call_back_stub.h"
#include "simulation_event_listener_stub.h"
#include "start_d_input_call_back_stub.h"
#include "start_stop_d_inputs_call_back_stub.h"
#include "start_stop_result_call_back_stub.h"
#include "stop_d_input_call_back_stub.h"
#include "unprepare_d_input_call_back_stub.h"

struct DistributedInputKit {
    std::shared_ptr<DistributedHardware::DistributedInput::DistributedInputKit> distributedinputkit;
}

struct IStartStopDInputsCallback {
    sptr<IStartStopDInputsCallback> cb;
}

struct IPrepareDInputCallback {
    sptr<IPrepareDInputCallback> cb;
}

struct IUnprepareDInputCallback {
    sptr<IUnprepareDInputCallback> cb;
}

struct BusinessEvent {
    std::vector<int32_t> pressedKeys;
    int32_t keyCode { -1 };
    int32_t keyAction { -1 };
}

class StartDInputCallback final : public DistributedHardware::DistributedInput::StartDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const uint32_t &inputTypes, const int32_t &status) override;
};

class StopDInputCallback final : public DistributedHardware::DistributedInput::StopDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const uint32_t &inputTypes, const int32_t &status) override;
};

class StartDInputCallbackDHIds final :
    public DistributedHardware::DistributedInput::StartStopDInputsCallbackStub {
public:
    void OnResultDhids(const std::string &devId, const int32_t &status) override;
};

class StopDInputCallbackDHIds final :
    public DistributedHardware::DistributedInput::StartStopDInputsCallbackStub {
public:
    void OnResultDhids(const std::string &devId, const int32_t &status) override;
};

class StartDInputCallbackSink final :
    public DistributedHardware::DistributedInput::StartStopDInputsCallbackStub {
public:
    void OnResultDhids(const std::string &devId, const int32_t &status) override;
};

class StopDInputCallbackSink final :
    public DistributedHardware::DistributedInput::StartStopDInputsCallbackStub {
public:
    void OnResultDhids(const std::string &devId, const int32_t &status) override;
};

class PrepareStartDInputCallback final :
    public DistributedHardware::DistributedInput::PrepareDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const int32_t &status) override;
};

class UnPrepareStopDInputCallback final :
    public DistributedHardware::DistributedInput::UnprepareDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const int32_t &status) override;
};

class PrepareStartDInputCallbackSink final :
    public DistributedHardware::DistributedInput::PrepareDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const int32_t &status) override;
};

class UnPrepareStopDInputCallbackSink final :
    public DistributedHardware::DistributedInput::UnprepareDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const int32_t &status) override;
};
#endif // FUSION_DISTRIBUTED_INPUT_INTERNAL_H