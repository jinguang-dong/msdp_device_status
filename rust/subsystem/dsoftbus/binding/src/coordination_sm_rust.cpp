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

#include "coordination_sm_rust.h"

#include "input_manager.h"
#include "coordination_sm.h"
#include "coordination_sm_rust_internal.h"

#define COOR_SM_INSTANCE OHOS::DelayedSingleton<OHOS::Msdp::DeviceStatus::CoordinationSM>::GetInstance()

namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, ::OHOS::Msdp::MSDP_DOMAIN_ID, "CoordinationSMRust" };
} // namespace

CPointerEvent* CGetLastPointerEvent() {
    CALL_DEBUG_ENTER;
    auto cPointerEvent = new (std::nothrow) CPointerEvent(COOR_SM_INSTANCE->GetLastPointerEvent());
    return cPointerEvent;
}

void CStartRemoteCoordinationResult(bool isSuccess, const char* startDeviceDhid, int32_t xPercent, int32_t yPercent) {
    CALL_DEBUG_ENTER;
    COOR_SM_INSTANCE->StartRemoteCoordinationResult(isSuccess, startDeviceDhid, xPercent, yPercent);
}

void CStartRemoteCoordination(const char* remoteNetworkId, bool buttonIsPressed) {
    CALL_DEBUG_ENTER;
    COOR_SM_INSTANCE->StartRemoteCoordination(remoteNetworkId, buttonIsPressed);
}

bool CGetPressedButtons(CPointerEvent* cPointerEvent)
{
    CALL_DEBUG_ENTER;
    if (cPointerEvent == nullptr) {
        return false;
    }

    for (const auto &item : cPointerEvent->event->GetPressedButtons()) {
        if (item == OHOS::MMI::PointerEvent::MOUSE_BUTTON_LEFT) {
            return true;
        }
    }
    return false;
}

