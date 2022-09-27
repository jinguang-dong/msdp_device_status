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

#include "motion_napi.h"

#include "motion_common.h"
#include "motion_client.h"


using namespace OHOS::Msdp;
using namespace OHOS;
namespace {
auto &g_MotionClient = MotionClient::GetInstance();
// static napi_ref g_responseConstructor;
static const uint8_t ARG_0 = 0;
static const uint8_t ARG_1 = 1;
static const uint8_t ARG_2 = 2;
static const std::vector<std::string> vecMotionValue {
    "VALUE_ENTER", "VALUE_EXIT"
};
}
std::map<int32_t, sptr<ImotionCallback>> MotionNapi::callbackMap_;
std::map<int32_t, MotionNapi*> MotionNapi::objectMap_;
napi_ref MotionNapi::motionValueRef_;
MotionNapi *g_motionObj = nullptr;
std::mutex mutex_;
 MotionDataUtils::MotionData g_motionData;


struct ResponseEntity {
    MotionDataUtils::MotionValue value;
};

void MotionCallback::OnMotionChanged(const MotionDataUtils::MotionData& motionData)
{
    MOTION_HILOGD(MOTION_MODULE_JS_NAPI, "Callback enter");
    std::lock_guard<std::mutex> guard(mutex_);
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(env_,  &loop);
    if (loop == nullptr) {
        MOTION_HILOGD(MOTION_MODULE_JS_NAPI, "loop is nullptr");
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    g_motionData = motionData;
    work->data = static_cast<void*>(&g_motionData);
    int32_t type = static_cast<int32_t> (g_motionData.type);
    int32_t value = static_cast<int32_t> (g_motionData.value);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter, value = %{public}d, type = %{public}d", value, type);
    int ret = uv_queue_work(loop, work, [] (uv_work_t *work) {},  EmitOnEvent);
    if (ret != 0) {
        MOTION_HILOGD(MOTION_MODULE_JS_NAPI, "Failed to execute work queue");
        delete work;
    }
}

void MotionCallback::EmitOnEvent(uv_work_t *work, int status) {
        MotionDataUtils::MotionData*  data = static_cast<MotionDataUtils::MotionData*>(work->data);
        MotionNapi* motionNapi = MotionNapi::GetMotionNapi();
        if (motionNapi == nullptr) {
            MOTION_HILOGD(MOTION_MODULE_JS_NAPI, "motionNapi is nullptr");
            return;
        }
        if (data->type == MotionDataUtils::MotionType::TYPE_PICKUP || data->type == 
            MotionDataUtils::MotionType::TYPE_CLOSE_TO_EAR || data->type ==
            MotionDataUtils::MotionType::TYPE_FLIP || data->type == 
            MotionDataUtils::MotionType::TYPE_SHAKE || data->type ==
            MotionDataUtils::MotionType::TYPE_POCKET ){
            g_motionObj->OnEventPickUp(static_cast<int32_t> (data->type), ARG_1, *data);
        }
        else if (data->type == MotionDataUtils::MotionType::TYPE_ROTATE){
            g_motionObj->OnEventRotate(static_cast<int32_t> (data->type), ARG_1, *data);
        }
        else if (data->type == MotionDataUtils::MotionType::TYPE_LEFT_SIDE_SWIPE ||data->type == 
            MotionDataUtils::MotionType::TYPE_RIGHT_SIDE_SWIPE ||data->type == 
            MotionDataUtils::MotionType::TYPE_UP_SIDE_SWIPE ||data->type == 
            MotionDataUtils::MotionType::TYPE_TWO_FINGERS_PINCH || data->type == 
            MotionDataUtils::TYPE_THREE_FINGERS_SLIDE){
            g_motionObj->OnEventGesture(static_cast<int32_t> (data->type), ARG_1, *data);
        }
        else {
           motionNapi->OnMotionChangedDone(static_cast<int32_t> (data->type), static_cast<int32_t> (data->value));
            // g_motionObj->OnEventGesture(static_cast<int32_t> (data->type), ARG_1, *data);
        }
        delete work;
}

MotionNapi* MotionNapi::GetMotionNapi()
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter,");
    return g_motionObj;
}

MotionNapi::MotionNapi(napi_env env, napi_value thisVar) : MotionEvent(env, thisVar)
{
    env_ = env;
    callbackRef_ = nullptr;
    motionValueRef_ = nullptr;
}

MotionNapi::~MotionNapi()
{
    if (callbackRef_ != nullptr) {
        napi_delete_reference(env_, callbackRef_);
    }

    if (motionValueRef_ != nullptr) {
        napi_delete_reference(env_, motionValueRef_);
    }
    if (g_motionObj != nullptr) {
        delete  g_motionObj;
    }
    g_motionObj = nullptr;
}

void MotionNapi::OnMotionChangedDone(const int32_t& type, const int32_t& value)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter, value = %{public}d", value);
    g_motionObj->OnEvent(type, ARG_1, value);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
}


int32_t MotionNapi::ConvertTypeToInt(std::string type)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter");
    if (type == "pickUp") {
        return MotionDataUtils::MotionType::TYPE_PICKUP;
    } else if (type == "takeOff") {
        return MotionDataUtils::MotionType::TYPE_TAKE_OFF;
    } else if (type == "closeToEar") {
        return MotionDataUtils::MotionType::TYPE_CLOSE_TO_EAR;
    } else if (type == "flip") {
        return MotionDataUtils::MotionType::TYPE_FLIP;
    } else if (type == "wave") {
        return MotionDataUtils::MotionType::TYPE_WAVE;
    } else if (type == "shake") {
        return MotionDataUtils::MotionType::TYPE_SHAKE;
    } else if (type == "rotate") {
        return MotionDataUtils::MotionType::TYPE_ROTATE;
    } else if (type == "pocket") {
        return MotionDataUtils::MotionType::TYPE_POCKET;
    } else if (type == "wristTitle") {
        return MotionDataUtils::MotionType::TYPE_WRIST_TILT;
    } else if (type == "leftSideSwipe"){
        return MotionDataUtils::MotionType::TYPE_LEFT_SIDE_SWIPE;
    } else if (type == "rightSideSwipe"){
        return MotionDataUtils::MotionType::TYPE_RIGHT_SIDE_SWIPE;
    } else if (type == "upSideSwipe"){
        return MotionDataUtils::MotionType::TYPE_UP_SIDE_SWIPE;
    } else if (type == "twoFingersPinch"){
        return MotionDataUtils::MotionType::TYPE_TWO_FINGERS_PINCH;
    } else if (type == "threeFingersSlide"){
        return MotionDataUtils::MotionType::TYPE_THREE_FINGERS_SLIDE;
    } else {
        return MotionDataUtils::MotionType::TYPE_INVALID;
    }
}

napi_value MotionNapi::SubscribeMotion(napi_env env, napi_callback_info info)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter");
    napi_value result = nullptr;
    size_t argc = ARG_2;
    napi_value args[ARG_2] = {0};
    napi_value jsthis;
    void *data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, &jsthis, &data);
    NAPI_ASSERT(env, (status == napi_ok) && (argc >= 0), "Bad parameters");

    napi_valuetype valueType1 = napi_undefined;
    napi_typeof(env, args[ARG_0], &valueType1);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "valueType1: %{public}d", valueType1);
    NAPI_ASSERT(env, valueType1 == napi_string, "type mismatch for parameter 1");

    napi_valuetype valueType2 = napi_undefined;
    napi_typeof(env, args[ARG_1], &valueType2);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "valueType2: %{public}d", valueType2);
    NAPI_ASSERT(env, valueType2 == napi_function, "type mismatch for parameter 2");

    size_t len;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &len);
    std::vector<char> typeBuf(len + 1);
    status = napi_get_value_string_utf8(env, args[0], typeBuf.data(), len + 1, &len);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "napi_get_value_string_utf8");

    if (status != napi_ok) {
        MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Failed to get string item");
        return result;
    }

    int32_t type = ConvertTypeToInt(typeBuf.data());
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "type is %{public}d", type);

    if (g_motionObj == nullptr) {
        g_motionObj = new MotionNapi(env, jsthis);
    }
    napi_wrap(env, jsthis, reinterpret_cast<void *>(g_motionObj),
    [](napi_env env, void *data, void *hint) {
        (void)env;
        (void)hint;
        MotionNapi *motion = (MotionNapi *)data;
        delete motion;
    },
    nullptr, &(g_motionObj->callbackRef_));

    if (!g_motionObj->On(type, args[ARG_1])) {
        MOTION_HILOGE(MOTION_MODULE_JS_NAPI, "type: %{public}d already exists", type);
        return result;
    }

    sptr<ImotionCallback> callback;
    bool isCallbackExists = false;
    for (auto it = callbackMap_.begin(); it != callbackMap_.end(); ++it) {
        if (it->first == type) {
            isCallbackExists = true;
            break;
        }
    }
    if (!isCallbackExists) {
        callback = new MotionCallback(env);
        callbackMap_.insert(std::pair<int32_t, sptr<ImotionCallback>>(type, callback));
        g_MotionClient.SubscribeCallback(MotionDataUtils::MotionType(type), callback);  
    } else {
        MOTION_HILOGE(MOTION_MODULE_JS_NAPI, "Callback exists.");
    }
    g_motionObj->OnMotionChangedDone(type, 0);
    napi_get_undefined(env, &result);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
    return result;
}

napi_value MotionNapi::UnSubscribeMotion(napi_env env, napi_callback_info info)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter");
    napi_value result = nullptr;
    size_t argc = ARG_2;
    napi_value args[ARG_2] = { 0 };
    napi_value jsthis;
    void *data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, &jsthis, &data);
    NAPI_ASSERT(env, (status == napi_ok) && (argc >= 0), "Bad parameters");

    napi_valuetype valueType1 = napi_undefined;
    napi_typeof(env, args[ARG_0], &valueType1);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "valueType1: %{public}d", valueType1);
    NAPI_ASSERT(env, valueType1 == napi_string, "type mismatch for parameter 1");

    size_t len;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &len);
    std::vector<char> typeBuf(len + 1);
    status = napi_get_value_string_utf8(env, args[0], typeBuf.data(), len + 1, &len);
    if (status != napi_ok) {
        MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Failed to get string item");
        return result;
    }

    int32_t type = ConvertTypeToInt(typeBuf.data());

    if (argc != 1) {
        napi_valuetype valueType2 = napi_undefined;
        napi_typeof(env, args[ARG_1], &valueType2);
        MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "valueType2: %{public}d", valueType2);
        NAPI_ASSERT(env, valueType2 == napi_function, "type mismatch for parameter 2");
        if (!g_motionObj->Off(type, args[1])) {
            NAPI_ASSERT(env, false, "Off failed, failed to get callback for type");
            return result;
       } 
    } else {
        if (!g_motionObj->Off(type, nullptr)) {
            NAPI_ASSERT(env, false, "Off failed, failed to get callback for type");
            return result;
       } 
    }

    sptr<ImotionCallback> callback;
    bool isCallbackExists = false;
    for (auto it = callbackMap_.begin(); it != callbackMap_.end(); ++it) {
        if (it->first == type) {
            isCallbackExists = true;
            callback = (sptr<ImotionCallback>)(it->second);
            break;
        }
    }

    if (!isCallbackExists) {
        NAPI_ASSERT(env, false, "No existed callback");
        return result;
    } else if (callback != nullptr) {
        g_MotionClient.UnSubscribeCallback(MotionDataUtils::MotionType(type), callback);
        callbackMap_.erase(type);
        if (argc == ARG_2) {
            g_motionObj->OnMotionChangedDone(type, 0);
        }
    }

    napi_get_undefined(env, &result);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
    return result;
}

napi_value MotionNapi::EnumMotionTypeConstructor(napi_env env, napi_callback_info info)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter");
    napi_value thisArg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
    return thisArg;
}

napi_value MotionNapi::CreateEnumMotionType(napi_env env, napi_value exports)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter");
    napi_value pickup = nullptr;
    napi_value takeOff = nullptr;
    napi_value closeToEar = nullptr;
    napi_value flip = nullptr;
    napi_value wave = nullptr;
    napi_value shake = nullptr;
    napi_value rotate = nullptr;
    napi_value pocket = nullptr;
    napi_value wristTilt = nullptr;
    napi_value leftSideSwipe = nullptr;
    napi_value rightSideSwipe = nullptr;
    napi_value upSideSwipe = nullptr;
    napi_value twoFingersPinch = nullptr;
    napi_value threeFingersSlide = nullptr;

    napi_create_string_utf8(env, "pickUp", NAPI_AUTO_LENGTH, &pickup);
    napi_create_string_utf8(env, "takeOff", NAPI_AUTO_LENGTH, &takeOff);
    napi_create_string_utf8(env, "closeToEar", NAPI_AUTO_LENGTH, &closeToEar);
    napi_create_string_utf8(env, "flip", NAPI_AUTO_LENGTH, &flip);
    napi_create_string_utf8(env, "wave", NAPI_AUTO_LENGTH, &wave);
    napi_create_string_utf8(env, "shake", NAPI_AUTO_LENGTH, &shake);
    napi_create_string_utf8(env, "rotate", NAPI_AUTO_LENGTH, &rotate);
    napi_create_string_utf8(env, "pocket", NAPI_AUTO_LENGTH, &pocket);
    napi_create_string_utf8(env, "wristTitle", NAPI_AUTO_LENGTH, &wristTilt);
    napi_create_string_utf8(env, "leftSideSwipe", NAPI_AUTO_LENGTH, &leftSideSwipe);
    napi_create_string_utf8(env, "rightSideSwipe", NAPI_AUTO_LENGTH, &rightSideSwipe);
    napi_create_string_utf8(env, "upSideSwipe", NAPI_AUTO_LENGTH, &upSideSwipe);
    napi_create_string_utf8(env, "twoFingersPinch", NAPI_AUTO_LENGTH, &twoFingersPinch);
    napi_create_string_utf8(env, "threeFingersSlide", NAPI_AUTO_LENGTH, &threeFingersSlide);


    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_PICKUP", pickup),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_TAKE_OFF", takeOff),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_CLOSE_TO_EAR", closeToEar),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_FLIP", flip),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_WAVE", wave),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_SHAKE", shake),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_ROTATE", rotate),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_POCKET", pocket),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_WRIST_TILT", wristTilt),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_LEFT_SIDE_SWIPE", leftSideSwipe ),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_RIGHT_SIDE_SWIPE", rightSideSwipe),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_UP_SIDE_SWIPE", upSideSwipe),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_TWO_FINGERS_PINCH", twoFingersPinch),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_THREE_FINGERS_SLIDE", threeFingersSlide),
    };
    napi_value result = nullptr;
    napi_define_class(env, "MotionType", NAPI_AUTO_LENGTH, EnumMotionTypeConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);

    napi_set_named_property(env, exports, "MotionType", result);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
    return exports;
}

napi_value MotionNapi::EnumMotionValueConstructor(napi_env env, napi_callback_info info)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter");
    napi_value thisArg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
    return thisArg;
}

napi_value MotionNapi::CreateMotionValueType(napi_env env, napi_value exports)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter");
    napi_value enter = nullptr;
    napi_value exit = nullptr;

    napi_create_int32(env, (int32_t)MotionDataUtils::MotionValue::VALUE_ENTER, &enter);
    napi_create_int32(env, (int32_t)MotionDataUtils::MotionValue::VALUE_EXIT, &exit);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("VALUE_ENTER", enter),
        DECLARE_NAPI_STATIC_PROPERTY("VALUE_EXIT", exit),
    };
    napi_value result = nullptr;
    napi_define_class(env, "MotionValue", NAPI_AUTO_LENGTH, EnumMotionValueConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);

    napi_set_named_property(env, exports, "MotionValue", result);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
    return exports;
}

napi_value MotionNapi::ResponseConstructor(napi_env env, napi_callback_info info)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter");
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);

    auto entity = new ResponseEntity();
    napi_wrap(
        env, thisVar, entity,
        [](napi_env env, void *data, void *hint) {
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Destructor");
            auto entity = (ResponseEntity*)data;
            delete entity;
        },
        nullptr, nullptr);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");

    return thisVar;
}

napi_status MotionNapi::AddProperty(napi_env env, napi_value object, const std::string name, int32_t enumValue)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter");
    napi_status status;
    napi_value enumNapiValue;

    status = napi_create_int32(env, enumValue, &enumNapiValue);
    if (status == napi_ok) {
        status = napi_set_named_property(env, object, name.c_str(), enumNapiValue);
    }
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
    return status;
}

napi_value MotionNapi::CreateMotionValueObject(napi_env env)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter");
    napi_value result = nullptr;
    napi_status status;
    std::string propName;

    status = napi_create_object(env, &result);
    if (status == napi_ok) {
        for (unsigned int i = MotionDataUtils::MotionValue::VALUE_ENTER; i < vecMotionValue.size(); i++) {
            propName = vecMotionValue[i];
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "propName: %{public}s", propName.c_str());
            status = AddProperty(env, result, propName, i);
            if (status != napi_ok) {
                MOTION_HILOGE(MOTION_MODULE_JS_NAPI, "Failed to add named prop!");
                break;
            }
            propName.clear();
        }
    }
    if (status == napi_ok) {
        // The reference count is for creation of motion value Object Reference
        status = napi_create_reference(env, result, 1, &motionValueRef_);
        if (status == napi_ok) {
            return result;
        }
    }
    MOTION_HILOGE(MOTION_MODULE_JS_NAPI, "CreateMotionValueObject is Failed!");
    napi_get_undefined(env, &result);

    return result;
}

napi_value MotionNapi::Init(napi_env env, napi_value exports)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter");
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("on", SubscribeMotion),
        DECLARE_NAPI_FUNCTION("off", UnSubscribeMotion),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    CreateEnumMotionType(env, exports);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
    return exports;
}

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value MotionInit(napi_env env, napi_value exports)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Enter");

    napi_value ret = MotionNapi::Init(env, exports);

    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");

    return ret;
}
EXTERN_C_END

/*
 * Module definition
 */
static napi_module g_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = "motion",
    .nm_register_func = MotionInit,
    .nm_modname = "motion",
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
