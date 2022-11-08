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

#include "devicestatus_event.h"
#include "devicestatus_common.h"

#include <js_native_api.h>
#include <map>
#include <uv.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

using namespace OHOS::Msdp;

DevicestatusEvent::DevicestatusEvent(napi_env env)
{
    env_ = env;
}

DevicestatusEvent::~DevicestatusEvent()
{
    eventOnceMap_.clear();
    eventMap_.clear();
}

bool DevicestatusEvent::On(int32_t eventType, napi_value handler, bool isOnce)
{
    DEV_HILOGI(JS_NAPI, "DevicestatusEvent On in for event:%{public}d, isOnce:%{public}d", eventType, isOnce);
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        DEV_HILOGE(JS_NAPI, "scope is nullptr");
        return false;
    }
    napi_ref onHandlerRef;
    napi_status status = napi_ok;
    if (isOnce) {
        auto iter = eventOnceMap_.find(eventType);
        if (iter != eventOnceMap_.end()) {
            DEV_HILOGI(JS_NAPI, "eventType:%{public}d already exists", eventType);
        } else {
            auto listener = std::make_shared<DevicestatusEventListener>();
            status = napi_create_reference(env_, handler, 1, &onHandlerRef);
            if (status != napi_ok) {
                DEV_HILOGE(JS_NAPI, "create reference fail");
                napi_close_handle_scope(env_, scope);
                return false;
            }
            listener->onHandlerRef = onHandlerRef;
            eventOnceMap_[eventType] = listener;
        }
    } else {
        auto iter = eventMap_.find(eventType);
        if (iter != eventMap_.end()) {
            DEV_HILOGE(JS_NAPI, "eventType:%{public}d already exists", eventType);
        } else {
            auto listener = std::make_shared<DevicestatusEventListener>();
            status = napi_create_reference(env_, handler, 1, &onHandlerRef);
            if (status != napi_ok) {
                DEV_HILOGE(JS_NAPI, "create reference fail");
                napi_close_handle_scope(env_, scope);
                return false;
            }
            listener->onHandlerRef = onHandlerRef;
            eventMap_[eventType] = listener;
        }
    }
    napi_close_handle_scope(env_, scope);
    return true;
}

bool DevicestatusEvent::Off(int32_t eventType, bool isOnce)
{
    DEV_HILOGI(JS_NAPI, "DevicestatusEvent off in for event:%{public}d, isOnce:%{public}d", eventType, isOnce);
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        DEV_HILOGE(JS_NAPI, "scope is nullptr");
        return false;
    }
    if (isOnce) {
        auto iter = eventOnceMap_.find(eventType);
        if (iter == eventOnceMap_.end()) {
            DEV_HILOGE(JS_NAPI, "eventType %{public}d not find", eventType);
            napi_close_handle_scope(env_, scope);
            return false;
        }
    } else {
        auto iter = eventMap_.find(eventType);
        if (iter == eventMap_.end()) {
            DEV_HILOGE(JS_NAPI, "eventType %{public}d not find", eventType);
            napi_close_handle_scope(env_, scope);
            return false;
        }
    }
    if (isOnce) {
        eventOnceMap_.erase(eventType);
    } else {
        eventMap_.erase(eventType);
    }
    napi_close_handle_scope(env_, scope);
    return true;
}

void DevicestatusEvent::CheckRet(int32_t eventType, size_t argc, int32_t value,
    std::shared_ptr<DevicestatusEventListener>& typeHandler)
{
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        DEV_HILOGE(JS_NAPI, "scope is nullptr");
        return;
    }
    napi_value handler = nullptr;
    napi_status status = napi_ok;
    status = napi_get_reference_value(env_, typeHandler->onHandlerRef, &handler);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "OnEvent handler for %{public}d failed, status:%{public}d", eventType, status);
        napi_close_handle_scope(env_, scope);
        return;
    }
    napi_value result;
    status = napi_create_object(env_, &result);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "create fail");
        napi_close_handle_scope(env_, scope);
        return;
    }
    napi_value tmpValue = nullptr;
    status = napi_create_int32(env_, eventType, &tmpValue);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "create fail");
        napi_close_handle_scope(env_, scope);
        return;
    }
    status = napi_set_named_property(env_, result, "type", tmpValue);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "set_name fail");
        napi_close_handle_scope(env_, scope);
        return;
    }
    bool flag = (value == 0) ? false : true;
    status = napi_get_boolean(env_, flag, &tmpValue);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "get_boolean fail");
        napi_close_handle_scope(env_, scope);
        return;
    }
    status = napi_set_named_property(env_, result, "value", tmpValue);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "set_named fail");
        napi_close_handle_scope(env_, scope);
        return;
    }
    napi_value callResult = nullptr;
    status = napi_call_function(env_, nullptr, handler, argc, &result, &callResult);
    if (status != napi_ok) {
        DEV_HILOGE(JS_NAPI, "OnEvent:napi_call_function for %{public}d failed, status:%{public}d", eventType, status);
        napi_close_handle_scope(env_, scope);
        return;
    }
}

void DevicestatusEvent::OnEvent(int32_t eventType, size_t argc, int32_t value, bool isOnce)
{
    DEV_HILOGI(JS_NAPI, "OnEvent for %{public}d, isOnce:%{public}d", eventType, isOnce);
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (scope == nullptr) {
        DEV_HILOGE(JS_NAPI, "scope is nullptr");
        return;
    }
    std::map<int32_t, std::shared_ptr<DevicestatusEventListener>>::iterator typeHandler;
    if (isOnce) {
        typeHandler = eventOnceMap_.find(eventType);
        if (typeHandler == eventOnceMap_.end()) {
            DEV_HILOGE(JS_NAPI, "OnEvent:eventType %{public}d not find", eventType);
            napi_close_handle_scope(env_, scope);
            return;
        }
    } else {
        typeHandler = eventMap_.find(eventType);
        if (typeHandler == eventMap_.end()) {
            DEV_HILOGE(JS_NAPI, "OnEvent:eventType %{public}d not find", eventType);
            napi_close_handle_scope(env_, scope);
            return;
        }
    }
    CheckRet(eventType, argc, value, typeHandler->second);
    napi_close_handle_scope(env_, scope);
    DEV_HILOGI(JS_NAPI, "Exit");
}