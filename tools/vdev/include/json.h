/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef VIRTUAL_JSON_H
#define VIRTUAL_JSON_H

#include <vector>

#include "cJSON.h"
#include "nocopyable.h"

#include "virtual_device_builder.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {

class Json : public std::enable_shared_from_this<Json> {
public:
    Json(cJSON* json, std::function<void(cJSON*)> deleter);
    Json(std::shared_ptr<Json> parent, cJSON* json);
    ~Json();
    DISALLOW_COPY_AND_MOVE(Json);

    bool IsObject() const;
    bool IsString() const;
    bool IsNumber() const;
    bool IsArray() const;
    bool IsNull() const;
    int32_t IntValue() const;
    std::string StringValue() const;
    bool HasItem(const std::string &name) const;
    std::shared_ptr<Json> GetItem(const std::string &name);
    std::vector<std::shared_ptr<Json>> GetArrayItems();
    std::vector<std::shared_ptr<Json>> GetArrayItems(const std::string &name);

public:
    static std::shared_ptr<Json> Load(const char* path);

private:
    std::shared_ptr<Json> parent_ { nullptr };
    cJSON* json_ { nullptr };
    std::function<void(cJSON*)> deleter_;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // VIRTUAL_JSON_H