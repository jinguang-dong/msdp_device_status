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

#include "motion_event.h"
#include "motion_common.h"

using namespace OHOS::Msdp;

MotionEvent::MotionEvent(napi_env env, napi_value thisVar)
{
    env_ = env;
    thisVarRef_ = nullptr;
    napi_create_reference(env, thisVar, 1, &thisVarRef_);
}

MotionEvent::~MotionEvent()
{
    eventMap_.clear();
    napi_delete_reference(env_, thisVarRef_);
}

bool MotionEvent::On(const int32_t& eventType, napi_value handler)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, \
        "MotionEvent On in for event: %{public}d", eventType);
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "scope is nullptr");
        return false;
    }
    std::map<int32_t, std::shared_ptr<MotionEventListener>>::iterator typeIter;
    napi_ref onHandlerRef;
    napi_value onHandler;
    typeIter = eventMap_.find(eventType);
    if (typeIter != eventMap_.end()) {
        MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "eventType: %{public}d already exists", eventType);
        napi_create_reference(env_, handler, 1, &(onHandlerRef));
        napi_get_reference_value(env_, onHandlerRef, &onHandler);
        for (auto iter: typeIter->second->onHandlerRefSet)  {
            napi_get_reference_value(env_, iter, &handler);
            if (!IsSameValue(env_, handler, onHandler)) {
                typeIter->second->onHandlerRefSet.insert(onHandlerRef);
            }
        }
    } else {
        auto listener = std::make_shared<MotionEventListener>();
        std::set<napi_ref>  OnRefSet;
        napi_create_reference(env_, handler, 1, &onHandlerRef);
        OnRefSet.insert(onHandlerRef);
        listener->onHandlerRefSet = OnRefSet;
        eventMap_[eventType] = listener;
    }
    napi_close_handle_scope(env_, scope);
    return true;
}

bool MotionEvent::IsSameValue(const napi_env &env, const napi_value &lhs, const napi_value &rhs)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "IsSameValue enter");
    bool result = false;
    napi_strict_equals(env, lhs, rhs, &result);
    return result;
}

bool MotionEvent::Off(const int32_t& eventType, napi_value handler)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, \
        "MotionEvent off in for event: %{public}d", eventType);
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "scope is nullptr");
        return false;
    }
    std::map<int32_t, std::shared_ptr<MotionEventListener>>::iterator iter;
    iter = eventMap_.find(eventType);
    if (iter == eventMap_.end()) {
        MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "eventType %{public}d not find", eventType);
        return false;
    }
    std::set<napi_ref>  OnRefSet;
    auto reSet  = iter->second->onHandlerRefSet;
    if (handler == nullptr) {
        for (auto it = reSet.begin(); it != reSet.end(); ++it)
        {
                napi_delete_reference(env_, *it);

        }
        eventMap_.erase(eventType);
        return true;
    }

    napi_value  deletehandler;
    for (auto it = reSet.begin(); it != reSet.end(); ++it)
    {
        napi_get_reference_value(env_, *it, &deletehandler);
        if (IsSameValue(env_, handler, deletehandler)) {
            napi_delete_reference(env_, *it);
        }       
    }
    if (iter->second->onHandlerRefSet.size() == 0) {
        eventMap_.erase(eventType);
    }
    napi_close_handle_scope(env_, scope);
    return true;
}

void MotionEvent::OnEvent(const int32_t& eventType, size_t argc, const int32_t& value)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "OnEvent for %{public}d: %{public}d",
        eventType, value);
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "scope is nullptr");
        return;
    }

    std::map<int32_t, std::shared_ptr<MotionEventListener>>::iterator typeIter;
    typeIter = eventMap_.find(eventType);
    if (typeIter == eventMap_.end()) {
        MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "OnEvent: eventType %{public}d not find", eventType);
        return;
    }
    
    for (auto iter: typeIter->second->onHandlerRefSet) {
        napi_value handler = nullptr;
        napi_value thisVar = nullptr;
        napi_status status = napi_ok;
        status = napi_get_reference_value(env_, iter, &handler);
        if (status != napi_ok) {
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, \
                "OnEvent napi_get_reference_value handler for %{public}d failed, status=%{public}d", eventType, status);
            return;
        }

        napi_value callResult = nullptr;
        bool flag = (value == 0 ? false : true);
        napi_value tmpValue = nullptr;
        napi_value result;
        napi_create_object(env_, &result);

        napi_get_boolean(env_, flag, &tmpValue);
        napi_set_named_property(env_, result, "motionValue", tmpValue);

        status = napi_call_function(env_, thisVar, handler, argc, &result, &callResult);
        if (status != napi_ok) {
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, \
                "OnEvent: napi_call_function for %{public}d failed, status=%{public}d", eventType, status);
            return;
        }
    }

    napi_close_handle_scope(env_, scope);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
}

void MotionEvent::OnEventPickUp(const int32_t& eventType, size_t argc,MotionDataUtils::MotionData motionData)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "OnEventPickUp for %{public}d",
        eventType);
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "scope is nullptr");
        return;
    }
    std::map<int32_t, std::shared_ptr<MotionEventListener>>::iterator typeIter;
    {
        typeIter = eventMap_.find(eventType);
        if (typeIter == eventMap_.end()) {
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "OnEventPickUp: eventType %{public}d not find", eventType);
            return;
        }
    }

    for (auto iter: typeIter->second->onHandlerRefSet) {
        napi_value handler = nullptr;
        napi_value thisVar = nullptr;
        napi_status status = napi_ok;
        status = napi_get_reference_value(env_, iter, &handler);
        if (status != napi_ok) {
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, \
                "OnEventPickUp napi_get_reference_value handler for %{public}d failed, status=%{public}d", eventType, status);
            return;
        }

        napi_value callResult = nullptr;
        int32_t type = static_cast<int32_t>(motionData.type);
        int32_t value = static_cast<int32_t>(motionData.value);
        bool flag = (value == 0 ? false : true);
        napi_value tmpValue = nullptr;
        napi_value result;
        napi_create_object(env_, &result);
        
        napi_create_int32(env_, type, &tmpValue);
        napi_set_named_property(env_, result, "type", tmpValue);
        napi_get_boolean(env_, flag, &tmpValue);
        napi_set_named_property(env_, result, "motionValue", tmpValue);

        status = napi_call_function(env_, thisVar, handler, argc, &result, &callResult);
        if (status != napi_ok) {
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, \
                "OnEventPickUp: napi_call_function for %{public}d failed, status=%{public}d", eventType, status);
            return;
        }
    }

    napi_close_handle_scope(env_, scope);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
}

void MotionEvent::OnEventRotate(const int32_t& eventType, size_t argc,MotionDataUtils::MotionData motionData)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "OnEventRotate for %{public}d",
        eventType);
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "scope is nullptr");
        return;
    }
    std::map<int32_t, std::shared_ptr<MotionEventListener>>::iterator typeIter;
    {
        typeIter = eventMap_.find(eventType);
        if (typeIter == eventMap_.end()) {
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "OnEventRotate: eventType %{public}d not find", eventType);
            return;
        }
    }

    for (auto iter: typeIter->second->onHandlerRefSet) {
        napi_value handler = nullptr;
        napi_value thisVar = nullptr;
        napi_status status = napi_ok;
        status = napi_get_reference_value(env_, iter, &handler);
        if (status != napi_ok) {
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, \
                "OnEventRotate napi_get_reference_value handler for %{public}d failed, status=%{public}d", eventType, status);
            return;
        }

        napi_value callResult = nullptr;
        int32_t type = static_cast<int32_t>(motionData.type);
        int32_t rotateAction = static_cast<int32_t>(motionData.rotateAction);
        
        // bool flag = (value == 0 ? false : true);
        napi_value tmpValue = nullptr;
        napi_value result;
        napi_create_object(env_, &result);

        // napi_get_boolean(env_, flag, &tmpValue);
        napi_create_int32(env_, type, &tmpValue);
        napi_set_named_property(env_, result, "type", tmpValue);
        napi_create_int32(env_, rotateAction, &tmpValue);
        napi_set_named_property(env_, result, "rotateAction", tmpValue);

        status = napi_call_function(env_, thisVar, handler, argc, &result, &callResult);
        if (status != napi_ok) {
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, \
                "OnEventRotate: napi_call_function for %{public}d failed, status=%{public}d", eventType, status);
            return;
        }
    }

    napi_close_handle_scope(env_, scope);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
}

void MotionEvent::OnEventGesture(const int32_t& eventType, size_t argc,MotionDataUtils::MotionData motionData)
{
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "OnEventGesture for %{public}d",
        eventType);
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "scope is nullptr");
        return;
    }

    std::map<int32_t, std::shared_ptr<MotionEventListener>>::iterator typeIter;
    {
        typeIter = eventMap_.find(eventType);
        if (typeIter == eventMap_.end()) {
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "OnEventGesture: eventType %{public}d not find", eventType);
            return;
        }
    }

    for (auto iter: typeIter->second->onHandlerRefSet) {
        napi_value handler = nullptr;
        napi_value thisVar = nullptr;
        napi_status status = napi_ok;
        status = napi_get_reference_value(env_, iter, &handler);
        if (status != napi_ok) {
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, \
                "OnEventGesture napi_get_reference_value handler for %{public}d failed, status=%{public}d", eventType, status);
            return;
        }

        napi_value callResult = nullptr;
        int32_t type = static_cast<int32_t>(motionData.type);
        int32_t value = static_cast<int32_t>(motionData.value);
        int32_t statusInt = static_cast<int32_t>(motionData.status);
        int32_t action =  static_cast<int32_t>(motionData.action);
        double move = motionData.move;
        bool flag = (value == 0 ? false : true);
        napi_value tmpValue = nullptr;
        napi_value result;
        napi_create_object(env_, &result);

        napi_create_int32(env_, type, &tmpValue);
        napi_set_named_property(env_, result, "type", tmpValue);
        napi_get_boolean(env_, flag, &tmpValue);
        napi_set_named_property(env_, result, "motionValue", tmpValue);
        napi_create_int32(env_, statusInt, &tmpValue);
        napi_set_named_property(env_, result, "status", tmpValue);
        napi_create_int32(env_, action, &tmpValue);
        napi_set_named_property(env_, result, "action", tmpValue);
        napi_create_double(env_, move, &tmpValue);
        napi_set_named_property(env_, result, "move", tmpValue);

        status = napi_call_function(env_, thisVar, handler, argc, &result, &callResult);
        if (status != napi_ok) {
            MOTION_HILOGI(MOTION_MODULE_JS_NAPI, \
                "OnEventGesture: napi_call_function for %{public}d failed, status=%{public}d", eventType, status);
            return;
        }
    }

    napi_close_handle_scope(env_, scope);
    MOTION_HILOGI(MOTION_MODULE_JS_NAPI, "Exit");
  
}




