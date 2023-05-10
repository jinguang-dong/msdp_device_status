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

#include "continue_mission_callback.h"

#include <message_parcel.h>

#include "devicestatus_common.h"
#include "devicestatus_define.h"
#include "fi_log.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "ContinueMissionCallback" };
} // namespace

ContinueMissionCallback::ContinueMissionCallback(
    std::function<void(const ContinueMissionParam&)> callback) : callback_(callback) {}

int32_t ContinueMissionCallback::OnContinueMission(const ContinueMissionParam& continueMissionParam)
{
    CHKPR(callback_, RET_ERR);
    callback_(continueMissionParam);
    return RET_OK;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS