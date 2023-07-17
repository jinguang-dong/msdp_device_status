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

#include "fusion_distributed_input.h"
#include <algorithm>
#include <map>
#include <mutex>
#include <functional>
#include <string>
#include <vector>

#include "fusion_distributed_input_internal.h"

using namespace OHOS;
using namespace DistributedHardware::DistributedInput;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, Msdp::MSDP_DOMAIN_ID, "fusion_distributed_input" };

bool IsNeedFilterOut(const char* ptr, const BusinessEvent *business) {
    CALL_DEBUG_ENTER;
    CHKPR(ptr, ERROR_NULL_POINTER);
    string deviceId = ptr;
    return DistributedInputKit::IsNeedFilterOut(deviceId, event);
}