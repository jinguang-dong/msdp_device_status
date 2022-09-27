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

#include "motion_data_parse.h"

#include "motion_common.h"
#include "util.h"
namespace OHOS {
namespace Msdp {
int32_t MotionDataParse::CreateJsonFile()
{
    int32_t fd = open(MSDP_MOTION_DATA_PATH.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
    if (fd < 0) {
        MOTION_HILOGE(MOTION_MODULE_ALGORITHM, "open failed.");
        return MOTION_FAILED;
    }
    close(fd);
    fd = -1;

    struct stat buf;
    if (stat(MSDP_DATA_DIR.c_str(), &buf) != 0) {
        MOTION_HILOGE(MOTION_MODULE_ALGORITHM, "stat folder path is invalid %{public}d.", errno);
        return MOTION_FAILED;
    }
    if (chown(MSDP_MOTION_DATA_PATH.c_str(), buf.st_uid, buf.st_gid) != 0) {
        MOTION_HILOGE(MOTION_MODULE_ALGORITHM, "chown failed, errno is %{public}d.", errno);
        return MOTION_FAILED;
    }

    return MOTION_OK;
}

bool MotionDataParse::ParseMotionData(MotionDataUtils::MotionData& data, MotionDataUtils::MotionType& type)
{
    std::string jsonBuf = MMI::ReadJsonFile(MSDP_MOTION_DATA_PATH.c_str());
    if (jsonBuf.empty()) {
        MOTION_HILOGE(MOTION_MODULE_ALGORITHM, "read json failed, errno is %{public}d.", errno);
        return false;
    }
    return MotionDataInit(jsonBuf, true, type, data);
}


/** Json definition
{
    "motionData" : {
        "type" : 8,
        "value" : 1
    },
}
*/
bool MotionDataParse::MotionDataInit(const std::string& fileData, bool logStatus, MotionDataUtils::MotionType& type,
    MotionDataUtils::MotionData& motionData)
{
    MOTION_HILOGD(MOTION_MODULE_ALGORITHM, "enter");
    JsonParser parser;
    parser.json_ = cJSON_Parse(fileData.c_str());
    if (cJSON_IsArray(parser.json_)) {
        MOTION_HILOGE(MOTION_MODULE_ALGORITHM, "parser is array");
        return {};
    }

    cJSON* cJsonMotionData = cJSON_GetObjectItem(parser.json_, "motionData");
    if (cJSON_IsObject(cJsonMotionData)) {
        cJSON* cJsonType = cJSON_GetObjectItem(cJsonMotionData, "type");
        cJSON* cJsonValue = cJSON_GetObjectItem(cJsonMotionData, "value");
        if (cJSON_IsNumber(cJsonType)) {
            motionData.type = static_cast<MotionDataUtils::MotionType>(cJsonType->valueint);
        }
        if (cJSON_IsNumber(cJsonValue)) {
            motionData.value = static_cast<MotionDataUtils::MotionValue>(cJsonValue->valueint);
        }
    }

    if (logStatus) {
        MOTION_HILOGW(MOTION_MODULE_ALGORITHM, "type: %{public}d. status: %{public}d", motionData.type,
        motionData.value);
    }

    if (type >= MotionDataUtils::MotionType::TYPE_PICKUP && type <= MotionDataUtils::MotionType::TYPE_WRIST_TILT) {
        cJSON* in_vector = cJSON_GetObjectItem(parser.json_, MotionJson[type].Json);
        int32_t in_vector_size = cJSON_GetArraySize(in_vector);
        in_vector_count[type] = in_vector_count[type] % in_vector_size;
        cJSON * cJsonValue = cJSON_GetArrayItem(in_vector, in_vector_count[type]);
        in_vector_count[type]++;
        if (cJSON_IsNumber(cJsonValue)) {
            motionData.type  = type;
            motionData.value = static_cast<MotionDataUtils::MotionValue>(cJsonValue->valueint);
            MOTION_HILOGW(MOTION_MODULE_ALGORITHM, "type: %{public}d. status: %{public}d",
                motionData.type, motionData.value);
            return true;
        }
    }
    return false;
}

bool MotionDataParse::DisableCount(const MotionDataUtils::MotionType& type)
{
    int motionItem = int(type);
    MOTION_HILOGW(MOTION_MODULE_ALGORITHM, "before: in_vector_count[%{public}d] is in_vector_count[motionItem] \
    = %{public}d", motionItem, in_vector_count[motionItem] );
    in_vector_count[motionItem] = int(MotionDataUtils::MotionTypeValue::INVALID);
    MOTION_HILOGW(MOTION_MODULE_ALGORITHM, "in_vector_count[%{public}d] is in_vector_count[motionItem] = %{public}d",
        motionItem ,in_vector_count[motionItem] );
    return true;
}
} // namespace MSDP
} // namespace OHOS