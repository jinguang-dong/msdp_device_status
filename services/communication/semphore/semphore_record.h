/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef SEMPHORE_RECORD_H
#define SEMPHORE_RECORD_H
#include <string>

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {

static std::string SEM_DRAG_NAME { "SEM_DRAG" };
constexpr int32_t SEM_DRAG_TIMEOUT { 1000 };
constexpr uint32_t SEM_DRAG_CREATE_FLAG { O_CREAT | O_EXCL };
constexpr uint32_t SEM_DRAG_CREATE_MODE { 0666 };
constexpr uint32_t SEM_DRAG_CREATE_VALUE { 1 };
constexpr uint32_t SEM_DRAG_OPEN_FLAG { O_RDWR };

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // SEMPHORE_RECORD_H
