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

#ifndef DINPUT_H
#define DINPUT_H

#include <cstddef>
#include <cstdint>
#include "distributed_input_kit.h"
#include "constants_dinput.h"
#include "i_start_stop_d_inputs_call_back.h"
#include "prepare_d_input_call_back_stub.h"
#include "simulation_event_listener_stub.h"
#include "start_d_input_call_back_stub.h"
#include "start_stop_d_inputs_call_back_stub.h"
#include "start_stop_result_call_back_stub.h"
#include "stop_d_input_call_back_stub.h"
#include "unprepare_d_input_call_back_stub.h"


using DInputCallback = std::function<void(bool)>;
struct Callbacks {
    DInputCallback statuscb;
};
using CallbackInput = void (*)(bool status, Callbacks* callback);
using DInputCb = void (*)(const char* devId, int32_t status, size_t id, void* userData);

class StopDInputCallbackDHIds final :
    public OHOS::DistributedHardware::DistributedInput::StartStopDInputsCallbackStub {
public:
    void OnResultDhids(const std::string &devId, const int32_t &status) override;
    void SetData(const DInputCb &callback, const size_t &id, void* userData);
private:
    DInputCb callback_ { nullptr };
    size_t id_ { -1 };
    void* userData_ { nullptr };
};

class StartDInputCallbackSink final :
    public OHOS::DistributedHardware::DistributedInput::StartStopDInputsCallbackStub {
public:
    void OnResultDhids(const std::string &devId, const int32_t &status) override;
    void SetData(const DInputCb &callback, const size_t &id, void* userData);
private:
    DInputCb callback_ { nullptr };
    size_t id_ { -1 };
    void* userData_ { nullptr };
};

class PrepareStartDInputCallbackSink final :
    public OHOS::DistributedHardware::DistributedInput::PrepareDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const int32_t &status) override;
    void SetData(const DInputCb &callback, const size_t &id, void* userData);
private:
    DInputCb callback_ { nullptr };
    size_t id_ { -1 };
    void* userData_ { nullptr };
};

class UnPrepareStopDInputCallbackSink final :
    public OHOS::DistributedHardware::DistributedInput::UnprepareDInputCallbackStub {
public:
    void OnResult(const std::string &devId, const int32_t &status) override;
    void SetData(const DInputCb &callback, const size_t &id, void* userData);
private:
    DInputCb callback_ { nullptr };
    size_t id_ { -1 };
    void* userData_ { nullptr };
};

struct CBusinessEvent {
    size_t pressed_keys_len;
    int32_t* pressed_keys;
    int32_t key_code;
    int32_t key_action;
};

int32_t CBusinessEventFrom(const OHOS::DistributedHardware::DistributedInput::BusinessEvent *event, CBusinessEvent *cEvent);

#ifdef __cplusplus
extern "C" {
#endif

int32_t PrepareRemoteInput(const char* srcId, const char* sinkId,
    DInputCb callback, size_t id, void* userData);
int32_t UnPrepareRemoteInput(const char* srcId, const char* sinkId,
    DInputCb callback, size_t id, void* userData);
int32_t StartRemoteInput(const char* srcId, const char* sinkId, const char** dhIds, size_t nDhIds,
    DInputCb callback, size_t id, void* userData);
int32_t StopRemoteInput(const char* srcId, const char* sinkId, const char** dhIds, size_t nDhIds,
    DInputCb callback, size_t id, void* userData);
int32_t IsNeedFilterOut(const char* sinkId, const CBusinessEvent* event);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // DINPUT_H