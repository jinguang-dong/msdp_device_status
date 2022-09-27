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

#ifndef MOTION_DUMP_H
#define MOTION_DUMP_H

#include <vector>
#include <cinttypes>
#include <ctime>
#include <queue>

#include "singleton.h"
#include "motion_hilog_wrapper.h"

namespace OHOS {
namespace Msdp {
class MotionDump : public Singleton<MotionDump> {
public:
    MotionDump() = default;
    virtual ~MotionDump() = default;
    bool DumpMotionHelp(int32_t fd, const std::vector<std::u16string> &args);
    void DumpHelp(int32_t fd);

private:
    void DumpCurrentTime(int32_t fd);
};
}  // namespace Msdp
}  // namespace OHOS
#endif  // MOTION_DUMP_H
