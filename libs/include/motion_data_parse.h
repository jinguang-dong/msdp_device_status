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

#ifndef MOTION_DATA_PARSE_H
#define MOTION_DATA_PARSE_H

#include <fcntl.h>
#include <poll.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>

#include "cJSON.h"

#include "securec.h"
#include "motion_data_utils.h"
#include "data_define.h"

namespace OHOS {
namespace Msdp {
// const std::string MSDP_MOTION_DATA_PATH = "/data/msdp/motion_data.json";
// const std::string MSDP_DATA_DIR = "/data/msdp";

// struct JsonParser {
//     JsonParser() = default;
//     ~JsonParser()
//     {
//         if (json_ != nullptr) {
//             cJSON_Delete(json_);
//         }
//     }
//     operator cJSON *()
//     {
//         return json_;
//     }
//     cJSON *json_ = nullptr;
// };

class MotionDataParse {
public:
    MotionDataParse() = default;
    ~MotionDataParse() = default;
    bool ParseMotionData(MotionDataUtils::MotionData& data, MotionDataUtils::MotionType& type);
    bool DisableCount(const  MotionDataUtils::MotionType& type);
    bool MotionDataInit(const std::string& fileData, bool logStatus, MotionDataUtils::MotionType& type,
        MotionDataUtils::MotionData& motionData);
    int32_t CreateJsonFile();
};
}
}
#endif // MOTION_DATA_PARSE_H