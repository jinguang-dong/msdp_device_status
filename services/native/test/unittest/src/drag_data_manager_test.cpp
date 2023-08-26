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

#include "drag_data_manager_test.h"

#include "securec.h"

#include "fi_log.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace testing::ext;
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DragDataManagerTest" };
constexpr int32_t MAX_PIXEL_MAP_WIDTH { 600 };
constexpr int32_t MAX_PIXEL_MAP_HEIGHT { 600 };
constexpr int32_t PIXEL_MAP_WIDTH { 300 };
constexpr int32_t PIXEL_MAP_HEIGHT { 300 };
constexpr int32_t SHADOWINFO_X { 10 };
constexpr int32_t SHADOWINFO_Y { 10 };
constexpr int32_t DISPLAY_X { 50 };
constexpr int32_t DISPLAY_Y { 50 };
constexpr int32_t INT32_BYTE { 4 };
constexpr uint32_t DEFAULT_ICON_COLOR { 0xFF };
}
void DragDataManagerTest::SetUpTestCase() {}

void DragDataManagerTest::TearDownTestCase() {}

void DragDataManagerTest::SetUp() {}

void DragDataManagerTest::TearDown() {}

std::shared_ptr<Media::PixelMap> DragDataManagerTest::CreatePixelMap(int32_t width, int32_t height)
{
    CALL_DEBUG_ENTER;
    if (width <= 0 || width > MAX_PIXEL_MAP_WIDTH || height <= 0 || height > MAX_PIXEL_MAP_HEIGHT) {
        FI_HILOGE("Invalid size, width:%{public}d, height:%{public}d", width, height);
        return nullptr;
    }
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.pixelFormat = Media::PixelFormat::BGRA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opts.scaleMode = Media::ScaleMode::FIT_TARGET_SIZE;

    int32_t colorLen = width * height;
    uint32_t *colorPixels = new (std::nothrow) uint32_t[colorLen];
    if (colorPixels == nullptr) {
        return nullptr;
    }
    int32_t colorByteCount = colorLen * INT32_BYTE;
    auto ret = memset_s(colorPixels, colorByteCount, DEFAULT_ICON_COLOR, colorByteCount);
    if (ret != EOK) {
        FI_HILOGE("memset_s failed");
        delete[] colorPixels;
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(colorPixels, colorLen, opts);
    if (pixelMap == nullptr) {
        FI_HILOGE("Create pixelMap failed");
        delete[] colorPixels;
        return nullptr;
    }
    delete[] colorPixels;
    return pixelMap;
}

namespace {
/**
 * @tc.name: DragDataManagerTest
 * @tc.desc: test devicestatus callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DragDataManagerTest, DragDataManagerTest001, TestSize.Level0)
{
    CALL_TEST_DEBUG;
    DRAG_DATA_MGR.SetDragStyle(DragCursorStyle::DEFAULT);
    EXPECT_TRUE(DRAG_DATA_MGR.GetDragStyle() == DragCursorStyle::DEFAULT);

    DRAG_DATA_MGR.SetDragStyle(DragCursorStyle::FORBIDDEN);
    EXPECT_TRUE(DRAG_DATA_MGR.GetDragStyle() == DragCursorStyle::FORBIDDEN);

    DRAG_DATA_MGR.SetDragStyle(DragCursorStyle::COPY);
    EXPECT_TRUE(DRAG_DATA_MGR.GetDragStyle() == DragCursorStyle::COPY);

    DRAG_DATA_MGR.SetDragStyle(DragCursorStyle::MOVE);
    EXPECT_TRUE(DRAG_DATA_MGR.GetDragStyle() == DragCursorStyle::MOVE);
}

/**
 * @tc.name: DragDataManagerTest004
 * @tc.desc: test get devicestatus drag data
 * @tc.type: FUNC
 */
HWTEST_F(DragDataManagerTest, DragDataManagerTest004, TestSize.Level0)
{
    CALL_TEST_DEBUG;
    std::shared_ptr<Media::PixelMap> pixelMap = CreatePixelMap(PIXEL_MAP_WIDTH, PIXEL_MAP_HEIGHT);
    EXPECT_FALSE(pixelMap == nullptr);
    DragData dragData;
    dragData.shadowInfo.pixelMap = pixelMap;
    dragData.shadowInfo.x = SHADOWINFO_X;
    dragData.shadowInfo.y = SHADOWINFO_Y;
    dragData.displayX = DISPLAY_X;
    dragData.displayY = DISPLAY_Y;
    MMI::PointerStyle pointerStyle;
    DRAG_DATA_MGR.Init(dragData, pointerStyle);
    DragData dragDataFirst = DRAG_DATA_MGR.GetDragData();
    EXPECT_TRUE(dragDataFirst.displayX == DISPLAY_X);
    EXPECT_TRUE(dragDataFirst.displayY == DISPLAY_Y);
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    int32_t width = 0;
    int32_t height = 0;
    DRAG_DATA_MGR.GetShadowOffset(offsetX, offsetY, width, height);
    EXPECT_TRUE(offsetX == SHADOWINFO_X);
    EXPECT_TRUE(offsetY == SHADOWINFO_Y);
    EXPECT_TRUE(width == PIXEL_MAP_WIDTH);
    EXPECT_TRUE(height == PIXEL_MAP_HEIGHT);
    DRAG_DATA_MGR.ResetDragData();
    DragData dragDataSecond = DRAG_DATA_MGR.GetDragData();
    EXPECT_TRUE(dragDataSecond.displayX == -1);
    EXPECT_TRUE(dragDataSecond.displayY == -1);
}

/**
 * @tc.name: DragDataManagerTest005
 * @tc.desc: test pixelMap is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DragDataManagerTest, DragDataManagerTest005, TestSize.Level0)
{
    DragData dragData;
    dragData.shadowInfo.pixelMap = nullptr;
    dragData.shadowInfo.x = SHADOWINFO_X;
    dragData.shadowInfo.y = SHADOWINFO_Y;
    dragData.displayX = DISPLAY_X;
    dragData.displayY = DISPLAY_Y;
    MMI::PointerStyle pointerStyle;
    DRAG_DATA_MGR.Init(dragData, pointerStyle);
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    int32_t width = 0;
    int32_t height = 0;
    auto ret = DRAG_DATA_MGR.GetShadowOffset(offsetX, offsetY, width, height);
    EXPECT_TRUE(ret == -1);
}
} // namespace
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OH