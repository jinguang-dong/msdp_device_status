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

#include "devicestatus_data_parse.h"

#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>

#include "devicestatus_data_define.h"
#include "devicestatus_errors.h"
#include "fi_log.h"
#include "utility.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DeviceStatusDataParse" };
constexpr int32_t FILE_SIZE_MAX { 0x5000 };
constexpr int32_t READ_DATA_BUFF_SIZE { 256 };
constexpr int32_t INVALID_FILE_SIZE { -1 };
const std::string MSDP_DATA_PATH { "/data/msdp/device_status_data.json" };
const std::string MSDP_DATA_DIR { "/data/msdp" };
} // namespace

std::vector<int32_t> DeviceStatusDataParse::tempcount_ =
    std::vector<int32_t> (static_cast<int32_t>(Type::TYPE_MAX), static_cast<int32_t>(TypeValue::INVALID));

int32_t DeviceStatusDataParse::CreateJsonFile()
{
    int32_t fd = open(MSDP_DATA_PATH.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
    if (fd < 0) {
        FI_HILOGE("open failed");
        return DEVICESTATUS_FAILED;
    }
    if (close(fd) < 0) {
        FI_HILOGE("Close fd failed, error:%{public}s, fd:%{public}d", strerror(errno), fd);
    }

    struct stat buf;
    if (stat(MSDP_DATA_DIR.c_str(), &buf) != 0) {
        FI_HILOGE("stat folder path is invalid %{public}d", errno);
        return DEVICESTATUS_FAILED;
    }
    if (chown(MSDP_DATA_PATH.c_str(), buf.st_uid, buf.st_gid) != 0) {
        FI_HILOGE("chown failed, errno is %{public}d", errno);
        return DEVICESTATUS_FAILED;
    }

    return DEVICESTATUS_OK;
}

bool DeviceStatusDataParse::ParseDeviceStatusData(Type type, Data& data)
{
    std::string jsonBuf = ReadJsonFile(MSDP_DATA_PATH.c_str());
    if (jsonBuf.empty()) {
        FI_HILOGE("read json failed, errno is %{public}d", errno);
        data.type = type;
        data.value = OnChangedValue::VALUE_INVALID;
        return false;
    }
    return DeviceStatusDataInit(jsonBuf, true, type, data);
}

bool DeviceStatusDataParse::DeviceStatusDataInit(const std::string& fileData, bool logStatus, Type& type,
    Data& data)
{
    CALL_DEBUG_ENTER;
    JsonParser parser;
    parser.json = cJSON_Parse(fileData.c_str());
    data.type = type;
    data.value = OnChangedValue::VALUE_INVALID;
    if (cJSON_IsArray(parser.json)) {
        FI_HILOGE("parser is array");
        return false;
    }

    if (type < Type::TYPE_ABSOLUTE_STILL || type >= Type::TYPE_MAX) {
        FI_HILOGE("Type error");
        return false;
    }

    cJSON* mockarray = cJSON_GetObjectItem(parser.json, DeviceStatusJson[type].json.c_str());
    int32_t jsonsize = cJSON_GetArraySize(mockarray);
    if (jsonsize == 0) {
        FI_HILOGE("json size is zero");
        return false;
    }
    tempcount_[type] = tempcount_[type] % jsonsize;
    cJSON* mockvalue = cJSON_GetArrayItem(mockarray, tempcount_[type]);
    tempcount_[type]++;
    data.type = type;
    if (mockvalue == nullptr || !cJSON_IsNumber(mockvalue)) {
        FI_HILOGE("json parser number is failed");
        return false;
    } 
    data.value = static_cast<OnChangedValue>(mockvalue->valueint);
    FI_HILOGD("type:%{public}d, status:%{public}d", data.type, data.value);
    return true;
}

bool DeviceStatusDataParse::DisableCount(const Type type)
{
    CALL_DEBUG_ENTER;
    tempcount_[static_cast<int32_t>(type)] = static_cast<int32_t>(TypeValue::INVALID);
    return true;
}

std::string DeviceStatusDataParse::ReadJsonFile(const std::string &filePath)
{
    if (filePath.empty()) {
        FI_HILOGE("Path is empty");
        return "";
    }
    char realPath[PATH_MAX] = {};
    if (realpath(filePath.c_str(), realPath) == nullptr) {
        FI_HILOGE("Path is error, %{public}d", errno);
        return "";
    }
    if (!CheckFileDir(realPath, MSDP_DATA_DIR)) {
        FI_HILOGE("File dir is invalid");
        return "";
    }
    if (!CheckFileExtendName(realPath, "json")) {
        FI_HILOGE("Unable to parse files other than json format");
        return "";
    }
    if (!Utility::DoesFileExist(filePath.c_str())) {
        FI_HILOGE("File not exist");
        return "";
    }
    if (!CheckFileSize(filePath)) {
        FI_HILOGE("File size out of read range");
        return "";
    }
    return ReadFile(realPath);
}

int32_t DeviceStatusDataParse::GetFileSize(const std::string& filePath)
{
    struct stat statbuf = { 0 };
    if (stat(filePath.c_str(), &statbuf) != 0) {
        FI_HILOGE("Get file size error");
        return INVALID_FILE_SIZE;
    }
    return statbuf.st_size;
}

bool DeviceStatusDataParse::CheckFileDir(const std::string& filePath, const std::string& dir)
{
    if (filePath.compare(0, MSDP_DATA_DIR.size(), MSDP_DATA_DIR) != 0) {
        FI_HILOGE("FilePath dir is invalid");
        return false;
    }
    return true;
}

bool DeviceStatusDataParse::CheckFileSize(const std::string& filePath)
{
    int32_t fileSize = GetFileSize(filePath);
    if ((fileSize <= 0) || (fileSize > FILE_SIZE_MAX)) {
        FI_HILOGE("File size out of read range");
        return false;
    }
    return true;
}

bool DeviceStatusDataParse::CheckFileExtendName(const std::string& filePath, const std::string& checkExtension)
{
    std::string::size_type pos = filePath.find_last_of('.');
    if (pos == std::string::npos) {
        FI_HILOGE("File is not find extension");
        return false;
    }
    return (filePath.substr(pos + 1, filePath.npos) == checkExtension);
}

std::string DeviceStatusDataParse::ReadFile(const std::string &filePath)
{
    CALL_DEBUG_ENTER;
    FILE* fp = fopen(filePath.c_str(), "r");
    if (fp == nullptr) {
        FI_HILOGE("Open failed");
        return "";
    }
    std::string dataStr;
    char buf[READ_DATA_BUFF_SIZE] = {};
    while (fgets(buf, sizeof(buf), fp) != nullptr) {
        dataStr += buf;
    }
    if (fclose(fp) != 0) {
        FI_HILOGW("Close file failed");
    }
    return dataStr;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
