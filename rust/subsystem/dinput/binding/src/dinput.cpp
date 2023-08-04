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

#include "dinput.h"
#include "devicestatus_define.h"
#include "constants_dinput.h"

using namespace OHOS;
using namespace OHOS::DistributedHardware::DistributedInput;
using namespace OHOS::Msdp::DeviceStatus;

namespace {
constexpr HiviewDFX::HiLogLabel LABEL { LOG_CORE, Msdp::MSDP_DOMAIN_ID, "FusionDInput" };
}

void StartDInputCallbackSink::OnResultDhids(const std::string &devId, const int32_t &status)
{
    CALL_DEBUG_ENTER;
    callback_(devId.c_str(), status, id_, userData_);
}

void StopDInputCallbackDHIds::OnResultDhids(const std::string &devId, const int32_t &status)
{
    CALL_DEBUG_ENTER;
    callback_(devId.c_str(), status, id_, userData_);
}

void PrepareStartDInputCallbackSink::OnResult(const std::string &devId, const int32_t &status)
{
    CALL_DEBUG_ENTER;
    callback_(devId.c_str(), status, id_, userData_);
}

void UnPrepareStopDInputCallbackSink::OnResult(const std::string &devId, const int32_t &status)
{
    CALL_DEBUG_ENTER;
    callback_(devId.c_str(), status, id_, userData_);
}

void StartDInputCallbackSink::SetData(const DInputCb &callback, const size_t &id, void* userData)
{
    CALL_DEBUG_ENTER;
    callback_ = callback;
    id_ = id;
    userData_ = userData;
}

void StopDInputCallbackDHIds::SetData(const DInputCb &callback, const size_t &id, void* userData)
{
    CALL_DEBUG_ENTER;
    callback_ = callback;
    id_ = id;
    userData_ = userData;
}

void PrepareStartDInputCallbackSink::SetData(const DInputCb &callback, const size_t &id, void* userData)
{
    CALL_DEBUG_ENTER;
    callback_ = callback;
    id_ = id;
    userData_ = userData;
}

void UnPrepareStopDInputCallbackSink::SetData(const DInputCb &callback, const size_t &id, void* userData)
{
    CALL_DEBUG_ENTER;
    callback_ = callback;
    id_ = id;
    userData_ = userData;
}

int32_t PrepareRemoteInput(const char* srcId, const char* sinkId, DInputCb callback, size_t id, void* userData)
{
    CALL_DEBUG_ENTER;
    sptr<PrepareStartDInputCallbackSink> prepareStartDinputCb = new (std::nothrow) PrepareStartDInputCallbackSink();
    prepareStartDinputCb->SetData(callback, id, userData);
    sptr<IPrepareDInputCallback> prepareStartDInputCbSink = prepareStartDinputCb;
    std::string remote = srcId;
    std::string origin = sinkId;
    return DistributedInputKit::PrepareRemoteInput(remote, origin, prepareStartDInputCbSink);
}

int32_t UnPrepareRemoteInput(const char* srcId, const char* sinkId, DInputCb callback, size_t id, void* userData)
{
    CALL_DEBUG_ENTER;
    sptr<UnPrepareStopDInputCallbackSink> unPrepareStartDinputCb = new (std::nothrow) UnPrepareStopDInputCallbackSink();
    unPrepareStartDinputCb->SetData(callback, id, userData);
    sptr<IUnprepareDInputCallback> unPrepareStopDInputCbSink = unPrepareStartDinputCb;
    std::string remote = srcId;
    std::string origin = sinkId;
    return DistributedInputKit::UnprepareRemoteInput(remote, origin, unPrepareStopDInputCbSink);
}

int32_t StartRemoteInput(const char* srcId, const char* sinkId, const char** dhIds, size_t nDhIds,
    DInputCb callback, size_t id, void* userData)
{
    CALL_DEBUG_ENTER;
    std::vector<std::string> inputdhids;
    int j = nDhIds;
    for (int i = 0; i < j; ++i) {
        std::string str;
        const char* id = dhIds[i];
        str = id;
        inputdhids.push_back(str);
    }
    OHOS::sptr<StartDInputCallbackSink> startDinputCb = new (std::nothrow) StartDInputCallbackSink();
    startDinputCb->SetData(callback, id, userData);
    OHOS::sptr<IStartStopDInputsCallback> startDInputCbSink = startDinputCb;
    std::string remote = srcId;
    std::string origin = sinkId;
    return DistributedInputKit::StartRemoteInput(remote, origin, inputdhids, startDInputCbSink);
}

int32_t StopRemoteInput(const char* srcId, const char* sinkId, const char** dhIds, size_t nDhIds,
    DInputCb callback, size_t id, void* userData)
{
    CALL_DEBUG_ENTER;
    std::vector<std::string> inputdhids;
    int j = nDhIds;
    for (int i = 0; i < j; ++i) {
        std::string str;
        const char* id = dhIds[i];
        str = id;
        inputdhids.push_back(str);
    }
    sptr<StopDInputCallbackDHIds> stopDinputCb = new (std::nothrow) StopDInputCallbackDHIds();
    stopDinputCb->SetData(callback, id, userData);
    sptr<IStartStopDInputsCallback> stopDInputCbDhids = stopDinputCb;
    std::string remote = srcId;
    std::string origin = sinkId;
    return DistributedInputKit::StopRemoteInput(remote, origin, inputdhids, stopDInputCbDhids);
}

int32_t IsNeedFilterOut(const char* sinkId, const CBusinessEvent* cbevent) {
    CALL_DEBUG_ENTER;
    BusinessEvent event;
    event.keyCode = cbevent->key_code;
    event.keyAction = cbevent->key_action;
    int size = cbevent->n_pressed_keys;
    for (int i = 0; i < size; ++i) {
        event.pressedKeys.push_back(cbevent->pressed_keys[i]);
    }
    std::string deviceId = sinkId;
    return DistributedInputKit::IsNeedFilterOut(deviceId, event);
}
