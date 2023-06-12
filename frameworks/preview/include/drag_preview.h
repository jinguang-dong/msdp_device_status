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
#ifndef DRAG_PREVIEW_H
#define DRAG_PREVIEW_H
#include <cstdint>
#include <functional>
#include <mutex>

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace Preview {
enum class DragState {
    ERROR = 0,
    START = 1,
    STOP = 2,
    CANCEL = 3
};

enum class DragResult {
    DRAG_SUCCESS = 0,
    DRAG_FAIL = 1,
    DRAG_CANCEL = 2,
    DRAG_EXCEPTION = 3
};

struct DragNotifyMsg {
    int32_t displayX { -1 };
    int32_t displayY { -1 };
    int32_t targetPid { -1 };
    DragResult result { DragResult::DRAG_FAIL };
};

class DragPreview {
public:
    DragPreview() = default;
    DragPreview(const DragPreview&) = delete;
    DragPreview(DragPreview&&) = delete;
    DragPreview& operator=(const DragPreview&) = delete;
    DragPreview& operator=(DragPreview&&) = delete;
    ~DragPreview() = default;

    int32_t StartDrag(std::function<void(const DragNotifyMsg&)> callback);
    int32_t StopDrag(DragResult result);

private:
    DragState dragState_ { DragState::STOP };
    std::mutex mtx_;
    std::function<void(const DragNotifyMsg&)> stopCallback_ { nullptr };
};
} // namespace Preview
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DRAG_PREVIEW_H
