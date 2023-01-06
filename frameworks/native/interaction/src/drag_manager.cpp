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

#include "devicestatus_define.h"
#include "drag_manager.h"

namespace OHOS {
namespace Msdp {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "DragManager" };
} // namespace
using namespace DeviceStatus;

DragManager *DragManager::instance_ = new (std::nothrow) DragManager();

DragManager *DragManager::GetInstance()
{
    return instance_;
}

/**
 * buffer 里边是啥在整个框架中是不需要关注的，我们只需要将buffer往下传传到服务端
*/
int32_t DragManager::StartDrag(ShadowThumbnail shadowThumbnail, const std::vector<uint8_t>& buffer, 
    std::function<void(int32_t)> callback, int32_t sourceType)
{

}

int32_t DragManager::StopDrag(int32_t &dragResult)
{

}

} // namespace Msdp
} // namespace OHOS
