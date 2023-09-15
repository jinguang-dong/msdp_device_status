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

#ifndef I_DRAG_CONTEXT_H
#define I_DRAG_CONTEXT_H

#include "i_drag_manager.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class IDragContext {
public:
    IDragContext() = default;
    virtual ~IDragContext() = default;

    virtual IDragManager& GetDragManager() = 0;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // I_DRAG_CONTEXT_H