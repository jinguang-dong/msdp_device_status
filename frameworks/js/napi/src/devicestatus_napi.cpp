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
#include <uv.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include <js_native_api.h>

using namespace OHOS::Msdp;
using namespace OHOS::Msdp::DeviceStatus;
using namespace OHOS;
namespace {
auto &g_DevicestatusClient = DevicestatusClient::GetInstance();
static constexpr uint8_t ARG_0 = 0;
static constexpr uint8_t ARG_1 = 1;
static constexpr uint8_t ARG_2 = 2;
static constexpr uint8_t ARG_3 = 3;
static constexpr uint8_t ARG_4 = 4;
constexpr int32_t NAPI_BUF_LENGTH  = 256;
static const std::vector<std::string> vecDevicestatusValue {
    "VALUE_ENTER", "VALUE_EXIT"
};
thread_local DevicestatusNapi *g_obj = nullptr;
}
std::map<int32_t, sptr<IdevicestatusCallback>> DevicestatusNapi::callbackMap_;
napi_ref DevicestatusNapi::devicestatusValueRef_;

struct ResponseEntity {
    DataUtils::Value value;
};

void DevicestatusCallback::OnDevicestatusChanged(const DataUtils::Data& devicestatusData)
{
    DEV_HILOGI(JS_NAPI, "Callback enter");
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (loop == nullptr) {
        DEV_HILOGE(JS_NAPI, "loop is nullptr");
        return;
    }

    static DataUtils::Data data;
    data = devicestatusData;
    int ret = uv_queue_work(loop, work, [] (uv_work_t *work) {}, [] (uv_work_t *work, int status) {
        DevicestatusNapi* deviceStatusNapi = DevicestatusNapi::GetDevicestatusNapi();
        if (deviceStatusNapi == nullptr) {
            DEV_HILOGE(JS_NAPI, "device status is nullptr");
            return;
        }
        deviceStatusNapi->OnDevicestatusChangedDone(static_cast<int32_t> (data.type),
            static_cast<int32_t> (data.value), false);
        delete work;
    });
    if (ret != 0) {
        DEV_HILOGE(JS_NAPI, "Failed to execute work queue");
        delete work;
    }
}

DevicestatusNapi* DevicestatusNapi::GetDevicestatusNapi()
{
    DEV_HILOGI(JS_NAPI, "enter");
    return g_obj;
}

DevicestatusNapi::DevicestatusNapi(napi_env env, napi_value thisVar) : DevicestatusEvent(env, thisVar)
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
    delete g_obj;
}

void DevicestatusNapi::OnDevicestatusChangedDone(const int32_t& type, const int32_t& value, bool isOnce)
{
    DEV_HILOGI(JS_NAPI, "Enter, value = %{public}d", value);
    OnEvent(type, ARG_1, value, isOnce);
    DEV_HILOGI(JS_NAPI, "Exit");
}

void DevicestatusNapi::InvokeCallBack(napi_env env, napi_value *args, int32_t value)
{
    DEV_HILOGI(JS_NAPI, "Enter");

    napi_value callback = nullptr;
    napi_value indexObj = nullptr;
    napi_create_object(env, &indexObj);
    napi_value successIndex = nullptr;

    napi_ref callbackSuccess = nullptr;
    napi_value ret;

    bool flag = value == 0 ? false : true;
    napi_get_boolean(env, flag, &successIndex);
    napi_set_named_property(env, indexObj, "deviceStatusValue", successIndex);
    napi_create_reference(env, args[ARG_1], 1, &callbackSuccess);
    napi_get_reference_value(env, callbackSuccess, &callback);
    napi_call_function(env, nullptr, callback, 1, &indexObj, &ret);
    napi_delete_reference(env, callbackSuccess);
    DEV_HILOGI(JS_NAPI, "Exit");
}

int32_t DevicestatusNapi::ConvertTypeToInt(std::string type)
{
    if(type == "still") {
        return DataUtils::Type::TYPE_STILL;
    } else if(type == "horizontalPosition") {
        return DataUtils::Type::TYPE_HORIZONTAL_POSITION;
    } else if(type == "verticalPosition") {
        return DataUtils::Type::TYPE_VERTICAL_POSITION;
    }else{
        return DataUtils::Type::TYPE_INVALID;
    }
}

napi_value DevicestatusNapi::SubscribeDevicestatus(napi_env env, napi_callback_info info)
{
    DEV_HILOGI(JS_NAPI, "Enter");
    napi_value result = nullptr;
    size_t argc = ARG_4;
    napi_value args[ARG_4] = {0};
    napi_value jsthis;
    void *data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, &jsthis, &data);
    NAPI_ASSERT(env, status == napi_ok, "Bad parameters");

    napi_valuetype valueType1 = napi_undefined;
    napi_typeof(env, args[ARG_0], &valueType1);
    DEV_HILOGI(JS_NAPI, "valueType1: %{public}d", valueType1);
    NAPI_ASSERT(env, valueType1 == napi_string, "type mismatch for parameter 1");

    napi_valuetype valueType2 = napi_undefined;
    napi_typeof(env, args[ARG_1], &valueType2);
    DEV_HILOGI(JS_NAPI, "valueType2: %{public}d", valueType2);
    NAPI_ASSERT(env, valueType2 == napi_number, "type mismatch for parameter 2");

    napi_valuetype valueType3 = napi_undefined;
    napi_typeof(env, args[ARG_2], &valueType3);
    DEV_HILOGI(JS_NAPI, "valueType3: %{public}d", valueType3);
    NAPI_ASSERT(env, valueType3 == napi_number, "type mismatch for parameter 3");

    napi_valuetype valueType4 = napi_undefined;
    napi_typeof(env, args[ARG_3], &valueType4);
    DEV_HILOGI(JS_NAPI, "valueType4: %{public}d", valueType4);
    NAPI_ASSERT(env, valueType4 == napi_function, "type mismatch for parameter 4");

    size_t modeLen = 0;
    napi_get_value_string_utf8(env, args[ARG_0], nullptr, 0, &modeLen);
    NAPI_ASSERT(env, modeLen > 0, "modeLen == 0");
    NAPI_ASSERT(env, modeLen < NAPI_BUF_LENGTH, "modeLen >= MAXLEN");
    char mode[NAPI_BUF_LENGTH] = {0};
    napi_get_value_string_utf8(env, args[ARG_0], mode, modeLen + 1, &modeLen);
    std::string eventMode = mode;

    int32_t e_mode = 0;
    napi_get_value_int32(env,args[ARG_1], &e_mode);

    int32_t t_mode = 0;
    napi_get_value_int32(env,args[ARG_2], &t_mode);

    int32_t type = ConvertTypeToInt(eventMode);
    int32_t event = e_mode;
    int32_t latency = t_mode;

    DEV_HILOGI(JS_NAPI, "type: %{public}d",type);
    DEV_HILOGI(JS_NAPI, "event: %{public}d",event);
    DEV_HILOGI(JS_NAPI, "latency: %{public}d",latency);

    NAPI_ASSERT(env, type >= DataUtils::Type::TYPE_STILL && type <=
        DataUtils::Type::TYPE_LID_OPEN, "type is illegal");

    NAPI_ASSERT(env, event >= DataUtils::ActivityEvent::ENTER && event <=
        DataUtils::ActivityEvent::ENTER_EXIT, "event is illegal");
    NAPI_ASSERT(env, latency >= DataUtils::ReportLatencyNs::SHORT && latency <=
        DataUtils::ReportLatencyNs::LONG, "event is illegal");
    DEV_HILOGI(JS_NAPI, "Didn't find object, so created it");
    g_obj = new DevicestatusNapi(env, jsthis);
    napi_wrap(env, jsthis, reinterpret_cast<void *>(g_obj),
        [](napi_env env, void *data, void *hint) {
            (void)env;
            (void)hint;
            DevicestatusNapi *devicestatus = (DevicestatusNapi *)data;
            delete devicestatus;
        },
        nullptr, &(g_obj->callbackRef_));

    if (!g_obj->On(type,args[ARG_3], false)) {
        DEV_HILOGE(JS_NAPI, "type: %{public}d already exists", type);
        return result;
    }

    sptr<IdevicestatusCallback> callback;

    auto callbackIter = callbackMap_.find(type);
    if (callbackIter != callbackMap_.end()) {
        DEV_HILOGI(JS_NAPI, "Callback exists.");
        callback = callbackIter->second;
    } else {
        callback = new DevicestatusCallback(env);
        g_DevicestatusClient.SubscribeCallback(DataUtils::Type(type), 
        DataUtils::ActivityEvent(event),DataUtils::ReportLatencyNs(latency),callback);
        callbackMap_.insert(std::pair<int32_t, sptr<IdevicestatusCallback>>(type, callback));
    }

    InvokeCallBack(env, args, 0);

    napi_get_undefined(env, &result);
    DEV_HILOGI(JS_NAPI, "Exit");
    return result;
}

napi_value DevicestatusNapi::UnSubscribeDevicestatus(napi_env env, napi_callback_info info)
{
    DEV_HILOGI(JS_NAPI, "Enter");
    napi_value result = nullptr;
    size_t argc = ARG_3;
    napi_value args[ARG_3] = { 0 };
    napi_value jsthis;
    void *data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, &jsthis, &data);
    NAPI_ASSERT(env, (status == napi_ok) && (argc >= 0), "Bad parameters");

    napi_valuetype valueType1 = napi_undefined;
    napi_typeof(env, args[ARG_0], &valueType1);
    DEV_HILOGE(JS_NAPI, "valueType1: %{public}d", valueType1);
    NAPI_ASSERT(env, valueType1 == napi_string, "type mismatch for parameter 1");

    size_t len;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &len);
    std::vector<char> typeBuf(len + 1);
    status = napi_get_value_string_utf8(env, args[0], typeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get string item");
        return result;
    }

    int32_t type = ConvertTypeToInt(typeBuf.data());
    NAPI_ASSERT(env, type >= DataUtils::Type::TYPE_STILL && type <=
        DataUtils::Type::TYPE_LID_OPEN, "type is illegal");

    int32_t e_mode = 0;
    napi_get_value_int32(env,args[ARG_1], &e_mode);
    int32_t event = e_mode;
    NAPI_ASSERT(env, event >= DataUtils::ActivityEvent::ENTER && event <=
        DataUtils::ActivityEvent::ENTER_EXIT, "event is illegal");

    DEV_HILOGI(JS_NAPI, "UNtype: %{public}d",type);
    DEV_HILOGI(JS_NAPI, "UNevent: %{public}d",event);
    if (argc != 1) {
        napi_valuetype valueType2 = napi_undefined;
        napi_typeof(env, args[ARG_1], &valueType2);
        DEV_HILOGI(JS_NAPI, "valueType2: %{public}d", valueType2);
        NAPI_ASSERT(env, valueType2 == napi_number, "type mismatch for parameter 2");
    }

    if (!g_obj->Off(type, false)) {
        NAPI_ASSERT(env, false, "Failed to get callback for type");
        DEV_HILOGE(JS_NAPI, "Failed to get callback for type: %{public}d", type);
        return result;
    }
    auto callbackIter = callbackMap_.find(type);
    if (callbackIter != callbackMap_.end()) {
        DEV_HILOGI(JS_NAPI,"unsub event");
        g_DevicestatusClient.UnSubscribeCallback(DataUtils::Type(type), DataUtils::ActivityEvent(event),callbackIter->second);
        callbackMap_.erase(type);
        if (argc == ARG_2) {
            InvokeCallBack(env, args, 0);
        }
    } else {
        NAPI_ASSERT(env, false, "No existed callback");
        return result;
    }
    napi_get_undefined(env, &result);
    DEV_HILOGI(JS_NAPI, "Exit");
    return result;
}

napi_value DevicestatusNapi::GetDevicestatus(napi_env env, napi_callback_info info)
{
    DEV_HILOGI(JS_NAPI, "Enter");
    napi_value result = nullptr;
    size_t argc = ARG_2;
    napi_value args[ARG_2] = {0};
    napi_value jsthis;
    void *data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, &jsthis, &data);
    NAPI_ASSERT(env, status == napi_ok, "Bad parameters");

    napi_valuetype valueType1 = napi_undefined;//
    napi_typeof(env, args[ARG_0], &valueType1);
    DEV_HILOGI(JS_NAPI, "valueType1: %{public}d", valueType1);
    NAPI_ASSERT(env, valueType1 == napi_string, "type mismatch for parameter 1");

    napi_valuetype valueType2 = napi_undefined;
    napi_typeof(env, args[ARG_1], &valueType2);
    DEV_HILOGI(JS_NAPI, "valueType2: %{public}d", valueType2);
    NAPI_ASSERT(env, valueType2 == napi_function, "type mismatch for parameter 2");

    size_t len;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &len);
    std::vector<char> typeBuf(len + 1);
    status = napi_get_value_string_utf8(env, args[0], typeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "Failed to get string item");
        return result;
    }

    int32_t type = ConvertTypeToInt(typeBuf.data());

    NAPI_ASSERT(env, type >= 0 && type <=
        DataUtils::Type::TYPE_LID_OPEN, "type is illegal");

    if (g_obj == nullptr) {
        g_obj = new DevicestatusNapi(env, jsthis);
        napi_wrap(env, jsthis, reinterpret_cast<void *>(g_obj),
            [](napi_env env, void *data, void *hint) {
                (void)env;
                (void)hint;
                DevicestatusNapi *devicestatus = (DevicestatusNapi *)data;
                delete devicestatus;
            },
            nullptr, &(g_obj->callbackRef_));
    }

    if (!g_obj->On(type, args[ARG_1], true)) {
        DEV_HILOGE(JS_NAPI, "type: %{public}d already exists", type);
        return result;
    }

    DataUtils::Data devicestatusData = \
        g_DevicestatusClient.GetDevicestatusData(DataUtils::Type(type));

    g_obj->OnDevicestatusChangedDone(devicestatusData.type, devicestatusData.value, true);
    g_obj->Off(devicestatusData.type, true);

    napi_get_undefined(env, &result);
    DEV_HILOGI(JS_NAPI, "Exit");
    return result;
}

napi_value DevicestatusNapi::EnumDevicestatusEventConstructor(napi_env env, napi_callback_info info)
{
    DEV_HILOGI(JS_NAPI, "Enter");
    napi_value thisArg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);
    DEV_HILOGI(JS_NAPI, "Exit");
    return thisArg;
}

napi_value DevicestatusNapi::CreateDevicestatusEvent(napi_env env, napi_value exports)
{
    DEV_HILOGI(JS_NAPI, "Enter");
    napi_value enter = nullptr;
    napi_value exit = nullptr;
    napi_value enter_exit = nullptr;

    napi_create_int32(env,1,&enter);
    napi_create_int32(env,2,&exit);
    napi_create_int32(env,3,&enter_exit);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("ENTER", enter),
        DECLARE_NAPI_STATIC_PROPERTY("EXIT", exit),
        DECLARE_NAPI_STATIC_PROPERTY("ENTER_EXIT", enter_exit),
    };

    napi_value result = nullptr;
    napi_define_class(env, "ActivityEvent", NAPI_AUTO_LENGTH, EnumDevicestatusEventConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);

    napi_set_named_property(env, exports, "ActivityEvent", result);
    DEV_HILOGI(JS_NAPI, "Exit");
    return exports;
}

napi_value DevicestatusNapi::EnumDevicestatusValueConstructor(napi_env env, napi_callback_info info)
{
    DEV_HILOGI(JS_NAPI, "Enter");
    napi_value thisArg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);
    DEV_HILOGI(JS_NAPI, "Exit");
    return thisArg;
}

napi_value DevicestatusNapi::CreateDevicestatusValueType(napi_env env, napi_value exports)
{
    DEV_HILOGI(JS_NAPI, "Enter");
    napi_value enter = nullptr;
    napi_value exit = nullptr;

    napi_create_int32(env, (int32_t)DataUtils::Value::VALUE_ENTER, &enter);
    napi_create_int32(env, (int32_t)DataUtils::Value::VALUE_EXIT, &exit);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("VALUE_ENTER", enter),
        DECLARE_NAPI_STATIC_PROPERTY("VALUE_EXIT", exit),
    };
    napi_value result = nullptr;
    napi_define_class(env, "Value", NAPI_AUTO_LENGTH, EnumDevicestatusValueConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);

    napi_set_named_property(env, exports, "Value", result);
    DEV_HILOGI(JS_NAPI, "Exit");
    return exports;
}

napi_value DevicestatusNapi::Init(napi_env env, napi_value exports)
{
    DEV_HILOGI(JS_NAPI, "Enter");
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("on", SubscribeDevicestatus),
        DECLARE_NAPI_FUNCTION("off", UnSubscribeDevicestatus),
        DECLARE_NAPI_FUNCTION("once", GetDevicestatus),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    CreateDevicestatusEvent(env, exports);
    DEV_HILOGI(JS_NAPI, "Exit");
    return exports;
}

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value DevicestatusInit(napi_env env, napi_value exports)
{
    DEV_HILOGI(JS_NAPI, "Enter");

    napi_value ret = DevicestatusNapi::Init(env, exports);

    DEV_HILOGI(JS_NAPI, "Exit");

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
