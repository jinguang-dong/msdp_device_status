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

#include "drag_preview.h"

#include "fi_log.h"

#ifndef RET_OK
#define RET_OK (0)
#endif

#ifndef RET_ERR
#define RET_ERR (-1)
#endif

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace Preview {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DragPreview" };
} // namespace

int32_t DragPreview::StartDrag(std::function<void(const DragNotifyMsg&)> callback)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (dragState_ == DragState::START) {
        FI_HILOGE("Drag instance is running, can not start drag again");
        return RET_ERR;
    }
    stopCallback_ = callback;
    dragState_ = DragState::START;
    return RET_OK;
}

int32_t DragPreview::StopDrag(DragResult result)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    if (dragState_ == DragState::STOP) {
        FI_HILOGE("No drag instance running, can not stop drag");
        return RET_ERR;
    }
    dragState_ = DragState::STOP;
    if (stopCallback_) {
        DragNotifyMsg msg {
            .result = result
        };
        stopCallback_(msg);
    }
    return RET_OK;
}
} // namespace Preview
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
