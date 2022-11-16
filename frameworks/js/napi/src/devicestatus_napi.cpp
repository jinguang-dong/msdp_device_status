/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
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

using namespace OHOS::Msdp::DeviceStatus;
using namespace OHOS;
namespace {
auto &g_DevicestatusClient = DevicestatusClient::GetInstance();
static constexpr size_t ARG_0 = 0;
static constexpr size_t ARG_1 = 1;
static constexpr size_t ARG_2 = 2;
static constexpr size_t ARG_3 = 3;
static constexpr size_t ARG_4 = 4;
static const std::vector<std::string> vecDevicestatusValue {
    "VALUE_ENTER", "VALUE_EXIT"
};
}
std::map<int32_t, sptr<IdevicestatusCallback>> DevicestatusNapi::callbackMap_;
std::map<int32_t, DevicestatusNapi*> DevicestatusNapi::objectMap_;
napi_ref DevicestatusNapi::devicestatusValueRef_;

struct ResponseEntity {
    OnChangedValue value;
};

void DevicestatusCallback::OnDevicestatusChanged(const Data& devicestatusData)
{
    DEV_HILOGD(JS_NAPI, "Callback enter");
    DevicestatusNapi* devicestatusNapi = DevicestatusNapi::GetDevicestatusNapi(devicestatusData.type);
    if (devicestatusNapi == nullptr) {
        DEV_HILOGD(JS_NAPI, "devicestatus is nullptr");
        return;
    }
    devicestatusNapi->OnDevicestatusChangedDone(static_cast<int32_t> (devicestatusData.type),
        static_cast<int32_t> (devicestatusData.value), false);
    DEV_HILOGD(JS_NAPI, "Callback exit");
}

DevicestatusNapi* DevicestatusNapi::GetDevicestatusNapi(int32_t type)
{
    DEV_HILOGD(JS_NAPI, "Enter, type: %{public}d", type);

    DevicestatusNapi* obj = nullptr;
    bool isExists = false;
    for (auto it = objectMap_.begin(); it != objectMap_.end(); ++it) {
        if (it->first == type) {
            isExists = true;
            obj = static_cast<DevicestatusNapi*>(it->second);
            DEV_HILOGD(JS_NAPI, "Found object");
            break;
        }
    }
    if (!isExists) {
        DEV_HILOGE(JS_NAPI, "Didn't find object");
    }
    return obj;
}

DevicestatusNapi::DevicestatusNapi(napi_env env) : DeviceStatusEvent(env)
{
    env_ = env;
    callbackRef_ = nullptr;
    devicestatusValueRef_ = nullptr;
}

DevicestatusNapi::~DevicestatusNapi()
{
    if (callbackRef_ != nullptr) {
        napi_delete_reference(env_, callbackRef_);
    }

    if (devicestatusValueRef_ != nullptr) {
        napi_delete_reference(env_, devicestatusValueRef_);
    }
}
bool DevicestatusNapi::CheckArguments(napi_env env, napi_callback_info info)
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

bool DevicestatusNapi::CheckUnsubArguments(napi_env env, napi_callback_info info)
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

void DevicestatusNapi::OnDevicestatusChangedDone(const int32_t& type, const int32_t& value, bool isOnce)
{
    DEV_HILOGD(JS_NAPI, "Enter, value: %{public}d", value);
    OnEvent(type, ARG_1, value, isOnce);
    DEV_HILOGD(JS_NAPI, "Exit");
}

bool DevicestatusNapi::CheckGetArguments(napi_env env, napi_callback_info info)
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

napi_value DevicestatusNapi::SubscribeDevicestatus(napi_env env, napi_callback_info info)
{
    DEV_HILOGD(JS_NAPI, "Enter");
    napi_value result = nullptr;
    size_t argc = ARG_2;
    napi_value args[ARG_2] = {0};
    void *data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, &data);
    NAPI_ASSERT(env, status == napi_ok, "Bad parameters");

    napi_valuetype valueType1 = napi_undefined;
    napi_typeof(env, args[ARG_0], &valueType1);
    DEV_HILOGD(JS_NAPI, "valueType1: %{public}d", valueType1);
    NAPI_ASSERT(env, valueType1 == napi_number, "type mismatch for parameter 1");

    napi_valuetype valueType2 = napi_undefined;
    napi_typeof(env, args[ARG_1], &valueType2);
    DEV_HILOGD(JS_NAPI, "valueType2: %{public}d", valueType2);
    NAPI_ASSERT(env, valueType2 == napi_function, "type mismatch for parameter 2");

    int32_t type;
    status = napi_get_value_int32(env, args[ARG_0], &type);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get type");
        return result;
    }

    if (type < 0 || type > Type::TYPE_LID_OPEN) {
        return result;
    }

    bool isObjExists = false;
    for (auto it = objectMap_.begin(); it != objectMap_.end(); ++it) {
        if (it->first == type) {
            isObjExists = true;
            DEV_HILOGE(JS_NAPI, "Object already exists");
            return result;
        }
    }
    DevicestatusNapi* obj = nullptr;
    if (!isObjExists) {
        DEV_HILOGD(JS_NAPI, "Didn't find object, so created it");
        obj = new (std::nothrow) DevicestatusNapi(env);
        if (obj == nullptr) {
            DEV_HILOGE(JS_NAPI, "obj is nullptr");
            return result;
        }
        napi_wrap(env, nullptr, reinterpret_cast<void *>(obj),
            [](napi_env env, void *data, void *hint) {
                (void)env;
                (void)hint;
                DevicestatusNapi *devicestatus = static_cast<DevicestatusNapi *>(data);
                delete devicestatus;
            },
            nullptr, &(obj->callbackRef_));
        objectMap_.insert(std::pair<int32_t, DevicestatusNapi*>(type, obj));
    }

    if (!obj->On(type, args[ARG_1], false)) {
        DEV_HILOGE(JS_NAPI, "type: %{public}d already exists", type);
        return result;
    }

    bool isCallbackExists = false;
    for (auto it = callbackMap_.begin(); it != callbackMap_.end(); ++it) {
        if (it->first == type) {
            isCallbackExists = true;
            break;
        }
    }
    if (isCallbackExists) {
        DEV_HILOGE(JS_NAPI, "Callback exists.");
        return result;
    }
    DEV_HILOGD(JS_NAPI, "Didn't find callback, so created it");
    sptr<IdevicestatusCallback> callback = new (std::nothrow) DevicestatusCallback();
    if (callback == nullptr) {
        DEV_HILOGE(JS_NAPI, "Callback is nullptr.");
        return result;
    }
    g_DevicestatusClient.SubscribeCallback(Type(type), callback);
    callbackMap_.insert(std::pair<int32_t, sptr<IdevicestatusCallback>>(type, callback));

    napi_get_undefined(env, &result);
    DEV_HILOGD(JS_NAPI, "Exit");
    return result;
}

napi_value DevicestatusNapi::UnSubscribeDevicestatus(napi_env env, napi_callback_info info)
{
    DEV_HILOGD(JS_NAPI, "Enter");
    napi_value result = nullptr;
    size_t argc = ARG_2;
    napi_value args[ARG_2] = { 0 };
    void *data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, &data);
    NAPI_ASSERT(env, status == napi_ok, "Bad parameters");

    napi_valuetype valueType1 = napi_undefined;
    napi_typeof(env, args[ARG_0], &valueType1);
    DEV_HILOGD(JS_NAPI, "valueType1: %{public}d", valueType1);
    NAPI_ASSERT(env, valueType1 == napi_number, "type mismatch for parameter 1");

    int32_t type;
    status = napi_get_value_int32(env, args[ARG_0], &type);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get type");
        return result;
    }

    if (type < 0 || type > Type::TYPE_LID_OPEN) {
        return result;
    }

    DevicestatusNapi* obj = nullptr;
    bool isObjExists = false;
    for (auto it = objectMap_.begin(); it != objectMap_.end(); ++it) {
        if (it->first == type) {
            isObjExists = true;
            obj = static_cast<DevicestatusNapi*>(it->second);
            DEV_HILOGD(JS_NAPI, "Found object");
        }
    }
    if (!isObjExists) {
        DEV_HILOGE(JS_NAPI, "Didn't find object, so created it");
        return result;
    }

    if (obj == nullptr) {
        DEV_HILOGE(JS_NAPI, "obj is nullptr");
        return result;
    }
    if (!obj->Off(type, args[ARG_1],false)) {
        DEV_HILOGE(JS_NAPI, "Failed to get callback for type: %{public}d", type);
        return result;
    }
    DEV_HILOGW(JS_NAPI, "erase objectMap_");
    objectMap_.erase(type);

    sptr<IdevicestatusCallback> callback;
    bool isCallbackExists = false;
    for (auto it = callbackMap_.begin(); it != callbackMap_.end(); ++it) {
        if (it->first == type) {
            isCallbackExists = true;
            callback = (sptr<IdevicestatusCallback>)(it->second);
            break;
        }
    }
    if (!isCallbackExists) {
        DEV_HILOGE(JS_NAPI, "No existed callback");
        return result;
    } else if (callback != nullptr) {
        g_DevicestatusClient.UnSubscribeCallback(Type(type), callback);
        callbackMap_.erase(type);
    }
    napi_get_undefined(env, &result);
    DEV_HILOGD(JS_NAPI, "Exit");
    return result;
}

napi_value DevicestatusNapi::GetDevicestatus(napi_env env, napi_callback_info info)
{
    DEV_HILOGD(JS_NAPI, "Enter");
    napi_value result = nullptr;
    size_t argc = ARG_2;
    napi_value args[ARG_2] = {0};
    void *data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, &data);
    NAPI_ASSERT(env, status == napi_ok, "Bad parameters");

    napi_valuetype valueType1 = napi_undefined;
    napi_typeof(env, args[ARG_0], &valueType1);
    DEV_HILOGD(JS_NAPI, "valueType1: %{public}d", valueType1);
    NAPI_ASSERT(env, valueType1 == napi_number, "type mismatch for parameter 1");

    napi_valuetype valueType2 = napi_undefined;
    napi_typeof(env, args[ARG_1], &valueType2);
    DEV_HILOGD(JS_NAPI, "valueType2: %{public}d", valueType2);
    NAPI_ASSERT(env, valueType2 == napi_function, "type mismatch for parameter 2");

    int32_t type;
    status = napi_get_value_int32(env, args[ARG_0], &type);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get type");
        return result;
    }

    DevicestatusNapi* obj = new (std::nothrow) DevicestatusNapi(env);
    if (obj == nullptr) {
        DEV_HILOGE(JS_NAPI, "obj is nullptr");
        return result;
    }
    napi_wrap(env, nullptr, reinterpret_cast<void *>(obj),
        [](napi_env env, void *data, void *hint) {
            (void)env;
            (void)hint;
            DevicestatusNapi *devicestatus = static_cast<DevicestatusNapi *>(data);
            delete devicestatus;
        },
        nullptr, &(obj->callbackRef_));

    if (!obj->On(type, args[ARG_1], true)) {
        DEV_HILOGE(JS_NAPI, "type: %{public}d already exists", type);
        return result;
    }

    Data devicestatusData = \
        g_DevicestatusClient.GetDevicestatusData(Type(type));

    obj->OnDevicestatusChangedDone(devicestatusData.type, devicestatusData.value, true);
    obj->Off(devicestatusData.type, args[ARG_1], true);

    napi_get_undefined(env, &result);
    DEV_HILOGD(JS_NAPI, "Exit");
    return result;
}

napi_value DevicestatusNapi::Init(napi_env env, napi_value exports)
{
    DEV_HILOGD(JS_NAPI, "Enter");
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("on", SubscribeDevicestatus),
        DECLARE_NAPI_FUNCTION("off", UnSubscribeDevicestatus),
        DECLARE_NAPI_FUNCTION("once", GetDevicestatus),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    DEV_HILOGD(JS_NAPI, "Exit");
    return exports;
}

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value DevicestatusInit(napi_env env, napi_value exports)
{
    DEV_HILOGD(JS_NAPI, "Enter");

    napi_value ret = DevicestatusNapi::Init(env, exports);

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
    .nm_register_func = DevicestatusInit,
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
