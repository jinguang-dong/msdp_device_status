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

#ifndef MOTION_EVENT_H
#define MOTION_EVENT_H

#include <string>
#include <list>
#include <memory>
#include <set>

#include <map>
#include "napi/native_api.h"
#include "motion_data_utils.h"

namespace OHOS {
namespace Msdp {
struct MotionEventListener {
    std::set<napi_ref> onHandlerRefSet;
};

class MotionEvent {
public:
    MotionEvent(napi_env env, napi_value thisVar);
    MotionEvent() {};
    virtual ~MotionEvent();

    virtual bool On(const int32_t& eventType, napi_value handler);
    virtual bool Off(const int32_t& eventType, napi_value handler);
    bool IsSameValue(const napi_env &env, const napi_value &lhs, const napi_value &rhs);

    virtual void OnEvent(const int32_t& eventType, size_t argc, const int32_t& value);
    virtual void OnEventGesture(const int32_t& eventType, size_t argc, MotionDataUtils::MotionData motionData);
    virtual void OnEventPickUp(const int32_t& eventType, size_t argc, MotionDataUtils::MotionData motionData);
    virtual void OnEventRotate(const int32_t& eventType, size_t argc, MotionDataUtils::MotionData motionData);
protected:
    napi_env env_;
    napi_ref thisVarRef_;

    std::map<int32_t, std::shared_ptr<MotionEventListener>> eventMap_;
};

class JsResponse {
public:
    int32_t motionValue_ = -1;
};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_EVENT_H
