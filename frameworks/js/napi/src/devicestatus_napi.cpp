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

#include "devicestatus_napi.h"
#include "devicestatus_common.h"
#include "devicestatus_client.h"

#include <js_native_api.h>
#include <new>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

using namespace OHOS;
using namespace OHOS::Msdp;
using namespace OHOS::Msdp::DeviceStatus;
namespace {
auto &g_DeviceStatusClient = DevicestatusClient::GetInstance();
static constexpr size_t ARG_0 = 0;
static constexpr size_t ARG_1 = 1;
static constexpr size_t ARG_2 = 2;
static constexpr size_t ARG_3 = 3;
static constexpr size_t ARG_4 = 4;
constexpr int32_t NAPI_BUF_LENGTH  = 256;
std::mutex mutex_;
static const std::vector<std::string> vecDeviceStatusValue {
    "VALUE_ENTER", "VALUE_EXIT"
};
thread_local DeviceStatusNapi *g_obj = nullptr;
Data g_data;
} // namespace
std::map<int32_t, sptr<IdevicestatusCallback>> DeviceStatusNapi::callbackMap_;
napi_ref DeviceStatusNapi::devicestatusValueRef_;

struct ResponseEntity {
    OnChangedValue value;
};

void DeviceStatusCallback::OnDevicestatusChanged(const Data& devicestatusData)
{
    DEV_HILOGD(JS_NAPI, "OnDevicestatusChanged enter");
    std::lock_guard<std::mutex> guard(mutex_);
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        DEV_HILOGE(JS_NAPI, "loop is nullptr");
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        DEV_HILOGE(JS_NAPI, "work is nullptr");
        return;
    }
    DEV_HILOGD(JS_NAPI, "devicestatusData.type:%{public}d, devicestatusData.value:%{public}d",
        devicestatusData.type, devicestatusData.value);
    g_data = devicestatusData;
    DEV_HILOGD(JS_NAPI, "g_data.type:%{public}d, g_data.value:%{public}d", g_data.type, g_data.value);
    work->data = static_cast<void *>(&g_data);
    int ret = uv_queue_work(loop, work, [] (uv_work_t *work) {}, EmitOnEvent);
    if (ret != 0) {
        DEV_HILOGE(JS_NAPI, "Failed to execute work queue");
        if (work != nullptr) {
            delete work;
        }
    }
}

void DeviceStatusCallback::EmitOnEvent(uv_work_t *work, int status)
{
    if (work == nullptr ) {
        DEV_HILOGE(JS_NAPI, "work is nullptr");
        return;
    }
    if (work->data == nullptr) {
        delete work;
        DEV_HILOGE(JS_NAPI, "work->data is nullptr");
        return;
    }

    Data* data = static_cast<Data*>(work->data);
    DeviceStatusNapi* deviceStatusNapi = DeviceStatusNapi::GetDeviceStatusNapi();
    if (deviceStatusNapi == nullptr) {
        DEV_HILOGE(JS_NAPI, "device status is nullptr");
        delete work;
        return;
    }

    int32_t type = static_cast<int32_t>(data->type);
    int32_t value = static_cast<int32_t>(data->value);
    DEV_HILOGE(JS_NAPI, "type:%{public}d, value:%{public}d", type, value);
    deviceStatusNapi->OnDeviceStatusChangedDone(type, value, false);
    if (work != nullptr) {
        delete work;
    }
}

DeviceStatusNapi* DeviceStatusNapi::GetDeviceStatusNapi()
{
    DEV_HILOGD(JS_NAPI, "Enter");
    return g_obj;
}

DeviceStatusNapi::DeviceStatusNapi(napi_env env) : DeviceStatusEvent(env)
{
    env_ = env;
    callbackRef_ = nullptr;
    devicestatusValueRef_ = nullptr;
}

DeviceStatusNapi::~DeviceStatusNapi()
{
    if (callbackRef_ != nullptr) {
        napi_delete_reference(env_, callbackRef_);
    }
    if (devicestatusValueRef_ != nullptr) {
        napi_delete_reference(env_, devicestatusValueRef_);
    }
    if (g_obj != nullptr) {
        delete g_obj;
    }
}

void DeviceStatusNapi::OnDeviceStatusChangedDone(int32_t type, int32_t value, bool isOnce)
{
    DEV_HILOGD(JS_NAPI, "Enter, value:%{public}d", value);
    OnEvent(type, ARG_1, value, isOnce);
    DEV_HILOGD(JS_NAPI, "Exit");
}

int32_t DeviceStatusNapi::ConvertTypeToInt(const std::string &type)
{
    if (type == "still") {
        return Type::TYPE_STILL;
    } else if (type == "horizontalPosition") {
        return Type::TYPE_HORIZONTAL_POSITION;
    } else if (type == "verticalPosition") {
        return Type::TYPE_VERTICAL_POSITION;
    } else {
        return Type::TYPE_INVALID;
    }
}

bool DeviceStatusNapi::CheckArguments(napi_env env, napi_callback_info info)
{
    DEV_HILOGD(JS_NAPI, "Enter");
    int arr[ARG_4] = {};
    size_t argc = ARG_4;
    napi_value args[ARG_4] = {};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get_cb_info");
        return false;
    }
    for (size_t arg = 0; arg < ARG_4; arg++) {
        napi_valuetype valueType = napi_undefined;
        status = napi_typeof(env, args[arg], &valueType);
        if (status != napi_ok) {
            DEV_HILOGE(JS_NAPI, "Failed to get arguments");
            return false;
        }
        DEV_HILOGD(JS_NAPI, "valueType:%{public}d", valueType);
        arr[arg] = valueType;
    }
    if (arr[ARG_0] != napi_string || arr[ARG_1] != napi_number || arr[ARG_2] != napi_number ||
        arr[ARG_3] != napi_function) {
        DEV_HILOGE(JS_NAPI, "fail to get arguements");
        return false;
    }
    DEV_HILOGD(JS_NAPI, "Exit");
    return true;
}

bool DeviceStatusNapi::CheckUnsubArguments(napi_env env, napi_callback_info info)
{
    DEV_HILOGD(JS_NAPI, "Enter");
    int arr[ARG_3] = {};
    size_t argc = ARG_3;
    napi_value args[ARG_3] = {};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get_cb_info");
        return false;
    }
    for (size_t arg = 0; arg < ARG_3; arg++) {
        napi_valuetype valueType = napi_undefined;
        status = napi_typeof(env, args[arg], &valueType);
        if (status != napi_ok) {
            DEV_HILOGE(JS_NAPI, "Failed to get arguments");
            return false;
        }
        DEV_HILOGD(JS_NAPI, "valueType:%{public}d", valueType);
        arr[arg] = valueType;
    }
    if (arr[ARG_0] != napi_string || arr[ARG_1] != napi_number || arr[ARG_2] != napi_function) {
        DEV_HILOGE(JS_NAPI, "fail to get arguements");
        return false;
    }
    DEV_HILOGD(JS_NAPI, "Exit");
    return true;
}

bool DeviceStatusNapi::CheckGetArguments(napi_env env, napi_callback_info info)
{
    DEV_HILOGD(JS_NAPI, "Enter");
    int arr[ARG_2] = {};
    size_t argc = ARG_2;
    napi_value args[ARG_2] = {};
    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get_cb_info");
        return false;
    }
    for (size_t arg = 0; arg < ARG_2; arg++) {
        napi_valuetype valueType = napi_undefined;
        status = napi_typeof(env, args[arg], &valueType);
        if (status != napi_ok) {
            DEV_HILOGE(JS_NAPI, "Failed to get arguments");
            return false;
        }
        DEV_HILOGD(JS_NAPI, "valueType:%{public}d", valueType);
        arr[arg] = valueType;
    }
    if (arr[ARG_0] != napi_string || arr[ARG_1] != napi_function) {
        DEV_HILOGE(JS_NAPI, "fail to get arguements");
        return false;
    }
    DEV_HILOGD(JS_NAPI, "Exit");
    return true;
}

napi_value DeviceStatusNapi::SubscribeDeviceStatus(napi_env env, napi_callback_info info)
{
    DEV_HILOGD(JS_NAPI, "Enter");
    size_t argc = ARG_4;
    napi_value args[ARG_4] = {};

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, (status == napi_ok) && (argc >= ARG_4), "Bad parameters");

    if (!CheckArguments(env, info)) {
        return nullptr;
    }
    size_t modeLen = 0;
    status = napi_get_value_string_utf8(env, args[ARG_0], nullptr, 0, &modeLen);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get string item");
        return nullptr;
    }

    char mode[NAPI_BUF_LENGTH] = {};
    status = napi_get_value_string_utf8(env, args[ARG_0], mode, modeLen + 1, &modeLen);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get string item");
        return nullptr;
    }
    std::string typeMode = mode;

    int32_t eventMode = 0;
    status = napi_get_value_int32(env, args[ARG_1], &eventMode);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get value item");
        return nullptr;
    }

    int32_t latencyMode = 0;
    status = napi_get_value_int32(env, args[ARG_2], &latencyMode);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get value item");
        return nullptr;
    }
    int32_t type = ConvertTypeToInt(typeMode);
    int32_t event = eventMode;
    int32_t latency = latencyMode;
    DEV_HILOGD(JS_NAPI, "type:%{public}d, event:%{public}d, latency:%{public}d", type, event, latency);

    NAPI_ASSERT(env, (type >= Type::TYPE_STILL) &&
        (type <= Type::TYPE_LID_OPEN), "type is illegal");
    NAPI_ASSERT(env, (event >= ActivityEvent::ENTER) &&
        (event <= ActivityEvent::ENTER_EXIT), "event is illegal");
    NAPI_ASSERT(env, (latency >= ReportLatencyNs::SHORT) &&
        (latency <= ReportLatencyNs::LONG), "latency is illegal");
    if (g_obj == nullptr) {
        g_obj = new (std::nothrow) DeviceStatusNapi(env);
        DEV_HILOGD(JS_NAPI, "Didn't find object, so created it");
    }
    napi_wrap(env, nullptr, reinterpret_cast<void *>(g_obj),
        [](napi_env env, void *data, void *hint) {
            (void)env;
            (void)hint;
            DeviceStatusNapi *devicestatus = (DeviceStatusNapi *)data;
            delete devicestatus;
        },
        nullptr, &(g_obj->callbackRef_));

    if (!g_obj->On(type, args[ARG_3], false)) {
        DEV_HILOGE(JS_NAPI, "type:%{public}d already exists", type);
        return nullptr;
    }

    sptr<IdevicestatusCallback> callback;

    auto callbackIter = callbackMap_.find(type);
    if (callbackIter != callbackMap_.end()) {
        DEV_HILOGD(JS_NAPI, "Callback exists");
        return nullptr;
    }
    callback = new DeviceStatusCallback(env);
    if (callback == nullptr) {
        return nullptr;
    }
    g_DeviceStatusClient.SubscribeCallback(Type(type), callback);
    auto ret = callbackMap_.insert(std::pair<int32_t, sptr<IdevicestatusCallback>>(type, callback));
    if (!ret.second) {
        return nullptr;
    }

    DEV_HILOGD(JS_NAPI, "Exit");
    return nullptr;
}

napi_value DeviceStatusNapi::UnsubscribeDeviceStatus(napi_env env, napi_callback_info info)
{
    DEV_HILOGD(JS_NAPI, "Enter");
    size_t argc = ARG_3;
    napi_value args[ARG_3] = {};

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, (status == napi_ok) && (argc >= ARG_3), "Bad parameters");

    if (!CheckUnsubArguments(env, info)) {
        return nullptr;
    }

    size_t len;
    status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &len);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get string item");
        return nullptr;
    }
    std::vector<char> typeBuf(len + 1);
    status = napi_get_value_string_utf8(env, args[0], typeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get string item");
        return nullptr;
    }

    int32_t type = ConvertTypeToInt(typeBuf.data());
    NAPI_ASSERT(env, (type >= Type::TYPE_STILL) &&
        (type <= Type::TYPE_LID_OPEN), "type is illegal");

    int32_t eventMode = 0;
    status = napi_get_value_int32(env, args[ARG_1], &eventMode);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get int32 item");
        return nullptr;
    }
    int32_t event = eventMode;
    NAPI_ASSERT(env, (event >= ActivityEvent::ENTER) &&
        (event <= ActivityEvent::ENTER_EXIT), "event is illegal");

    DEV_HILOGD(JS_NAPI, "type:%{public}d, event:%{public}d", type, event);

    if (!g_obj->Off(type, args[ARG_2], false)) {
        DEV_HILOGE(JS_NAPI, "Not ready to Unsubscribe for type:%{public}d", type);
        return nullptr;
    }
    auto callbackIter = callbackMap_.find(type);
    if (callbackIter != callbackMap_.end()) {
        DEV_HILOGD(JS_NAPI, "unsub event");
        g_DeviceStatusClient.UnSubscribeCallback(Type(type), callbackIter->second);
        callbackMap_.erase(type);
    } else {
        NAPI_ASSERT(env, false, "No existed callback");
        return nullptr;
    }
    DEV_HILOGD(JS_NAPI, "Exit");
    return nullptr;
}

napi_value DeviceStatusNapi::GetDeviceStatus(napi_env env, napi_callback_info info)
{
    DEV_HILOGD(JS_NAPI, "Enter");
    size_t argc = ARG_2;
    napi_value args[ARG_2] = {};

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, (status == napi_ok) && (argc >= ARG_2), "Bad parameters");

    if (!CheckGetArguments(env, info)) {
        return nullptr;
    }
    size_t len;
    status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &len);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get string item");
        return nullptr;
    }
    std::vector<char> typeBuf(len + 1);
    status = napi_get_value_string_utf8(env, args[0], typeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get string item");
        return nullptr;
    }

    int32_t type = ConvertTypeToInt(typeBuf.data());
    NAPI_ASSERT(env, (type >= Type::TYPE_STILL) &&
        (type <= Type::TYPE_LID_OPEN), "type is illegal");

    if (g_obj == nullptr) {
        g_obj = new (std::nothrow) DeviceStatusNapi(env);
        napi_wrap(env, nullptr, reinterpret_cast<void *>(g_obj),
            [](napi_env env, void *data, void *hint) {
                (void)env;
                (void)hint;
                DeviceStatusNapi *devicestatus = (DeviceStatusNapi *)data;
                delete devicestatus;
            },
            nullptr, &(g_obj->callbackRef_));
    }

    if (!g_obj->On(type, args[ARG_1], true)) {
        DEV_HILOGE(JS_NAPI, "type:%{public}d already exists", type);
        return nullptr;
    }

    Data devicestatusData = g_DeviceStatusClient.GetDevicestatusData(Type(type));

    g_obj->OnDeviceStatusChangedDone(devicestatusData.type, devicestatusData.value, true);
    g_obj->Off(devicestatusData.type, args[ARG_1], true);

    DEV_HILOGD(JS_NAPI, "Exit");
    return nullptr;
}

napi_value DeviceStatusNapi::Init(napi_env env, napi_value exports)
{
    DEV_HILOGD(JS_NAPI, "Enter");
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("on", SubscribeDeviceStatus),
        DECLARE_NAPI_FUNCTION("off", UnsubscribeDeviceStatus),
        DECLARE_NAPI_FUNCTION("once", GetDeviceStatus),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    DEV_HILOGD(JS_NAPI, "Exit");
    return exports;
}

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value DeviceStatusInit(napi_env env, napi_value exports)
{
    DEV_HILOGD(JS_NAPI, "Enter");

    napi_value ret = DeviceStatusNapi::Init(env, exports);

    DEV_HILOGD(JS_NAPI, "Exit");

    return ret;
}
EXTERN_C_END

/*
 * Module definition
 */
static napi_module g_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = "devicestatus",
    .nm_register_func = DeviceStatusInit,
    .nm_modname = "devicestatus",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

/*
 * Module registration
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&g_module);
}
