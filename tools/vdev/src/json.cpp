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
#include "json.h"

#include "devicestatus_define.h"
#include "fi_log.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "Json" };
} // namespace

Json::Json(cJSON* json, std::function<void(cJSON*)> deleter)
    : json_(json), deleter_(deleter)
{}

Json::Json(std::shared_ptr<Json> parent, cJSON* json)
    : parent_(parent), json_(json)
{}

Json::~Json()
{
    if (deleter_ != nullptr) {
        deleter_(json_);
    }
}

bool Json::IsObject() const
{
    CHKPF(json_);
    return static_cast<bool>(cJSON_IsObject(json_));
}

bool Json::IsString() const
{
    CHKPF(json_);
    return static_cast<bool>(cJSON_IsString(json_));
}

bool Json::IsNumber() const
{
    CHKPF(json_);
    return static_cast<bool>(cJSON_IsNumber(json_));
}

bool Json::IsArray() const
{
    CHKPF(json_);
    return static_cast<bool>(cJSON_IsArray(json_));
}

bool Json::IsNull() const
{
    CHKPF(json_);
    return static_cast<bool>(cJSON_IsNull(json_));
}

std::string Json::StringValue() const
{
    if (!IsString()) {
        return "";
    }
    return cJSON_GetStringValue(json_);
}

int32_t Json::IntValue() const
{
    if (!IsNumber()) {
        return RET_ERR;
    }
    return static_cast<int32_t>(cJSON_GetNumberValue(json_));
}

bool Json::HasItem(const std::string &name) const
{
    CHKPF(json_);
    return static_cast<bool>(cJSON_HasObjectItem(json_, name.c_str()));
}

std::shared_ptr<Json> Json::GetItem(const std::string &name)
{
    cJSON* element = nullptr;
    if (!HasItem(name)) {
        element = cJSON_CreateNull();
    } else {
        element = cJSON_GetObjectItem(json_, name.c_str());
    }
    return std::make_shared<Json>(shared_from_this(), element);
}

std::shared_ptr<Json> Json::Load(const char* path)
{
    CALL_DEBUG_ENTER;
    std::string buff;
    int32_t ret = VirtualDeviceBuilder::ReadFile(path, buff);
    if (ret != RET_OK) {
        FI_HILOGE("Load parse file \'%{public}s\' is null", path);
        return nullptr;
    }

    cJSON* json = cJSON_Parse(buff.c_str());
    return std::make_shared<Json>(json,
        [](cJSON* json) {
            if (json != nullptr) {
                cJSON_Delete(json);
            }
        }
    );
}

std::vector<std::shared_ptr<Json>> Json::GetArrayItems()
{
    std::vector<std::shared_ptr<Json>> items;
    if (!IsArray()) {
        return items;
    }
    cJSON* element = nullptr;
    cJSON_ArrayForEach(element, json_) {
        items.push_back(std::make_shared<Json>(shared_from_this(), element));
    }
    return items;
}

std::vector<std::shared_ptr<Json>> Json::GetArrayItems(const std::string &name)
{
    std::vector<std::shared_ptr<Json>> items;
    if (name.empty()) {
        return items;
    }
    cJSON* element = nullptr;
    cJSON* actions = cJSON_GetObjectItem(json_, name.c_str());
    cJSON_ArrayForEach(element, actions) {
        items.push_back(std::make_shared<Json>(shared_from_this(), element));
    }
    return items;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS