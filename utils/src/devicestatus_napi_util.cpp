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

#include "devicestatus_napi_util.h"

namespace OHOS {
namespace Msdp {
bool TypeOf(napi_env env, napi_value value, napi_valuetype type)
{
    napi_valuetype valueType = napi_undefined;
    if ((napi_typeof(env, value, &valueType)) != napi_ok) {
        DEV_HILOGE(COMMON, "%{public}s failed", std::string("napi_typeof").c_str());
        auto infoTemp = std::string(__FUNCTION__)+ ": " + std::string("napi_typeof") + " failed";
        return false;
    }

    return (valueType == type);
}
} // namespace Msdp
} // namespace OHOS
