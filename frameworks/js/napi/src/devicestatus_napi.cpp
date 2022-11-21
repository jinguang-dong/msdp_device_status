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

#include "device_status_napi.h"

#include <js_native_api.h>
#include <new>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "devicestatus_client.h"
#include "devicestatus_common.h"

using namespace OHOS;
using namespace OHOS::Msdp;
using namespace OHOS::Msdp::DeviceStatus;
namespace {
constexpr size_t ARG_0 = 0;
constexpr size_t ARG_1 = 1;
constexpr size_t ARG_2 = 2;
constexpr size_t ARG_3 = 3;
constexpr size_t ARG_4 = 4;
constexpr int32_t NAPI_BUF_LENGTH  = 256;
static const std::vector<std::string> vecDeviceStatusValue {
    "VALUE_ENTER", "VALUE_EXIT"
};
thread_local DeviceStatusNapi *g_obj = nullptr;
} // namespace
std::map<int32_t, sptr<IRemoteDevStaCallback>> DeviceStatusNapi::callbackMap_;
napi_ref DeviceStatusNapi::devicestatusValueRef_ = nullptr;

struct ResponseEntity {
    OnChangedValue value;
};

void DeviceStatusCallback::OnDeviceStatusChanged(const Data& devicestatusData)
{
    DEV_HILOGD(JS_NAPI, "OnDeviceStatusChanged enter");
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
    data_ = devicestatusData;
    work->data = static_cast<void *>(&data_);
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
    if (work == nullptr) {
        DEV_HILOGE(JS_NAPI, "work is nullptr");
        return;
    }
    Data* data = static_cast<Data*>(work->data);
    delete work;
    if (work->data == nullptr) {
        DEV_HILOGE(JS_NAPI, "work->data is nullptr");
        return;
    }

    DeviceStatusNapi* deviceStatusNapi = DeviceStatusNapi::GetDeviceStatusNapi();
    if (deviceStatusNapi == nullptr) {
        DEV_HILOGE(JS_NAPI, "deviceStatusNapi is nullptr");
        return;
    }

    int32_t type = static_cast<int32_t>(data->type);
    int32_t value = static_cast<int32_t>(data->value);
    DEV_HILOGE(JS_NAPI, "type:%{public}d, value:%{public}d", type, value);
    deviceStatusNapi->OnDeviceStatusChangedDone(type, value, false);
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
    DeviceStatusClient::GetInstance().RegisterDeathListener([this] {
        DEV_HILOGI(JS_NAPI, "Receive death notification");
        callbackMap_.clear();
        ClearEventMap();
    });
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
        g_obj = nullptr;
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
        DEV_HILOGE(JS_NAPI, "Failed to get arguements");
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
        DEV_HILOGE(JS_NAPI, "Failed to get arguements");
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
        DEV_HILOGE(JS_NAPI, "Failed to get arguements");
        return false;
    }
    DEV_HILOGD(JS_NAPI, "Exit");
    return true;
}

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
    .nm_priv = (static_cast<void *>(0)),
    .reserved = {0}
};

/*
 * Module registration
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&g_module);
}
