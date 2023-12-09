/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef I_COOPERATE_H
#define I_COOPERATE_H

#include "cJSON.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
struct JsonParser {
    JsonParser() = default;
    ~JsonParser()
    {
        if (json != nullptr) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
    operator cJSON *()
    {
        return json;
    }
    cJSON *json { nullptr };
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // I_COOPERATE_H