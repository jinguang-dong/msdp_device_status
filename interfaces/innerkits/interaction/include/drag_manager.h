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

#ifndef DRAG_MANAGER_H
#define DRAG_MANAGER_H

#include <memory>
#include <vector>
#include "nocopyable.h"
#include "pixel_map.h"

/**
*TODO 考虑之后将DragManager合并到InteractionManager中, 
*TODO 因为Drag本质上仍然是交互的一种方式，在下一层实现一个 DragManagerImpl 使其与键鼠穿越分离开来

*/
namespace OHOS {
namespace Msdp {
class DragManager {
public:

struct ShadowThumbnail {
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap;
    int32_t x;
    int32_t y;
};

enum class DragState {
    DRAG_START = 0,
    DRAG_IN_PROGRESS = 1,
    DRAG_END = 2
};

public:
    static DragManager *GetInstance();
    virtual ~DragManager() = default;

public:
    /**
     * @brief 开始拖拽接口。
     * @param shadowThumbnail 拖放阴影缩略图。
     * @param buffer 拖放自定义数据
     * @param callback 回调,这个好像是在StopDrag的时候才调用的，在这里传入只是给存起来
     * @param sourceType 源类型
     * @return 返回值如果是0表示接口调用成功，返回其他值表示接口调用失败。
     * @since 9
     */
    int32_t StartDrag(ShadowThumbnail shadowThumbnail, const std::vector<uint8_t> buffer, 
        std::function<void(int32_t)> callback, int32_t sourceType);

    /**
     * @brief 结束拖拽接口。
     * @return 返回值如果是0表示接口调用成功，返回其他值表示接口调用失败。
     * @since 9
     */
    int32_t StopDrag(int32_t &dragResult);


private:
    DragManager() = default;
    DISALLOW_COPY_AND_MOVE(DragManager);
    static DragManager *instance_;
};
}; // namespace Msdp
} // namespace OHOS
#define DragMgr OHOS::Msdp::DragManager::GetInstance()
#endif