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

#ifndef MOTION_NAPI_H
#define MOTION_NAPI_H

#include <map>
#include <uv.h>
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "motion_callback_stub.h"
#include "motion_event.h"

namespace OHOS {
namespace Msdp {
class MotionCallback : public MotionCallbackStub {
public:
    explicit MotionCallback(napi_env env) :env_(env) {}
    virtual ~MotionCallback() {};
    void OnMotionChanged(const MotionDataUtils::MotionData& motionData) override;
    static void EmitOnEvent(uv_work_t *work, int status) ;

private:
    napi_env env_;
};

class MotionNapi : public MotionEvent {
public:
    explicit MotionNapi(napi_env env, napi_value thisVar);
    virtual ~MotionNapi();

    static napi_value Init(napi_env env, napi_value exports);
    static napi_value SubscribeMotion(napi_env env, napi_callback_info info);
    static napi_value UnSubscribeMotion(napi_env env, napi_callback_info info);
    static napi_value EnumMotionTypeConstructor(napi_env env, napi_callback_info info);
    static napi_value CreateEnumMotionType(napi_env env, napi_value exports);
    static napi_value EnumMotionValueConstructor(napi_env env, napi_callback_info info);
    static napi_value CreateMotionValueType(napi_env env, napi_value exports);
    static napi_value ResponseConstructor(napi_env env, napi_callback_info info);
    static napi_status AddProperty(napi_env env, napi_value object, const std::string name, int32_t enumValue);
    static napi_value CreateMotionValueObject(napi_env env);
    static void InvokeCallBack(napi_env env, napi_value *args, int32_t value);
    static void RegisterCallback(const int32_t& eventType);
    static int32_t ConvertTypeToInt(std::string type);
    static void OnMotionChangedDone(const int32_t& type, const int32_t& value);//
    static MotionNapi* GetMotionNapi();
    static std::map<int32_t, sptr<ImotionCallback>> callbackMap_;
    static std::map<int32_t, MotionNapi*> objectMap_;

private:
    napi_ref callbackRef_;
    static napi_ref motionValueRef_;
    napi_env env_;
};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_NAPI_H
