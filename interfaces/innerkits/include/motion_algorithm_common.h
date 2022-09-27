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

#ifndef MOTION_ALGORITHM_COMMON_H
#define MOTION_ALGORITHM_COMMON_H

#include <string>
#include <map>

namespace OHOS {
namespace Msdp {
enum class TransmissionMedium : int32_t {
    /** Bluetooth */
    BLUETOOTH = 0,
    /** Wi-Fi */
    WIFI = 1,
    /** USB */
    USB = 2,
    /** Ethernet */
    ETHERNET = 3,
    /* Ultrasound */
    ULTRASOUND = 4,
    DM_MEDIUM_BUTT
};

enum class MediumPriority : uint32_t {
    HIGH = 0,
    MIDDLE,
    LOW,
};

/**
 * @brief Structure of algorithm library capabilities
 *
 */
struct MsdpAlgorithmCapability {
    std::string vendor;
    std::string version;
    std::string description;
    std::map<TransmissionMedium, MediumPriority> mediumMap;
    MotionDataUtils::MotionType type;
};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_ALGORITHM_COMMON_H