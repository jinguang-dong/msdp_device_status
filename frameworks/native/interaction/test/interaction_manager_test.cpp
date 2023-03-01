/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <iostream>
#include <vector>
#include <utility>

#include <unistd.h>

#include <gtest/gtest.h>
#include "image_source.h"
#include "input_manager.h"
#include "pointer_event.h"

#include "coordination_message.h"
#include "devicestatus_define.h"
#include "devicestatus_errors.h"
#include "interaction_manager.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace testing::ext;
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "InteractionManagerTest" };
constexpr int32_t TIME_WAIT_FOR_OP = 100;
static const std::string IMAGE_INPUT_JPG_PATH_600 = "/data/local/tmp/image/test600.jpg";
#define INPUT_MANAGER  MMI::InputManager::GetInstance()
} // namespace
class InteractionManagerTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
};

void InteractionManagerTest::SetUpTestCase()
{
}

void InteractionManagerTest::SetUp()
{
}

void InteractionManagerTest::TearDown()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
}

std::shared_ptr<Media::PixelMap> CreatePixelMap()
{
    Media::SourceOptions opts;
    opts.formatHint = "image/jpg";
    uint32_t errorCode = 0;
    std::unique_ptr<Media::ImageSource> imageSource =
        Media::ImageSource::CreateImageSource(IMAGE_INPUT_JPG_PATH_600, opts, errorCode);
    Media::ImageInfo imageInfo;
    imageSource->GetImageInfo(0, imageInfo);
    FI_HILOGD("imageInfo.size.width:%{public}d, imageInfo.size.height:%{public}d",
        imageInfo.size.width, imageInfo.size.width);
    if (errorCode != 0 || imageSource.get() == nullptr) {
        FI_HILOGE("CreateImageSource failed");
        return nullptr;
    }
    Media::DecodeOptions decodeOpts;
    decodeOpts.allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC;
    std::unique_ptr<Media::PixelMap> uniquePixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    std::shared_ptr<Media::PixelMap> pixelMap = std::move(uniquePixelMap);
    return pixelMap;
}

int32_t CreatePixelMap(int32_t pixelMapWidth, int32_t pixelMapHeight, std::shared_ptr<OHOS::Media::PixelMap> pixelMap)
{
    if (pixelMapWidth <= 0 || pixelMapWidth > MAX_PIXEL_MAP_WIDTH ||
        pixelMapHeight <= 0 || pixelMapHeight > MAX_PIXEL_MAP_HEIGHT) {
        FI_HILOGE("Invalid size, pixelMapWidth:%{public}d, pixelMapHeight:%{public}d", pixelMapWidth, pixelMapHeight);
        return RET_ERR;
    }
    OHOS::Media::ImageInfo info;
    info.size.width = pixelMapWidth;
    info.size.height = pixelMapHeight;
    info.pixelFormat = Media::PixelFormat::RGB_888;
    info.colorSpace = OHOS::Media::ColorSpace::SRGB;
    pixelMap->SetImageInfo(info);
    int32_t bufferSize = pixelMap->GetByteCount();
    char *buffer = static_cast<char *>(malloc(bufferSize));
    if (buffer == nullptr) {
        FI_HILOGE("Malloc buffer failed");
        return RET_ERR;
    }
    char *character = buffer;
    for (int32_t i = 0; i < bufferSize; i++) {
        *(character++) = static_cast<char>(i);
    }
    pixelMap->SetPixelsAddr(buffer, nullptr, bufferSize, OHOS::Media::AllocatorType::HEAP_ALLOC, nullptr);
    return RET_OK;
}

int32_t SetParamShare(DragData& dragData)
{
    auto pixelMap = CreatePixelMap();
    if (pixelMap == nullptr) {
        FI_HILOGE("CreatePixelMap failed");
        return RET_ERR;
    }
    dragData.pictureResourse.pixelMap = pixelMap;
    dragData.pictureResourse.x = 0;
    dragData.pictureResourse.y = 0;
    dragData.buffer = std::vector<uint8_t>(MAX_BUFFER_SIZE, 0);
    dragData.sourceType = MMI::PointerEvent::SOURCE_TYPE_MOUSE;
    dragData.pointerId = 0;
    dragData.dragNum = 1;
    return RET_OK;
}

int32_t SetParamHeap(DragData& dragData)
{
    auto pixelMap = std::make_shared<OHOS::Media::PixelMap>();
    if (CreatePixelMap(MAX_PIXEL_MAP_WIDTH, MAX_PIXEL_MAP_HEIGHT, pixelMap) != RET_OK) {
        FI_HILOGE("CreatePixelMap failed");
        return RET_ERR;
    }
    dragData.pictureResourse.pixelMap = pixelMap;
    dragData.pictureResourse.x = 0;
    dragData.pictureResourse.y = 0;
    dragData.buffer = std::vector<uint8_t>(MAX_BUFFER_SIZE, 0);
    dragData.sourceType = MMI::PointerEvent::SOURCE_TYPE_MOUSE;
    dragData.pointerId = 0;
    dragData.dragNum = 1;
    return RET_OK;
}

std::shared_ptr<MMI::PointerEvent> SetupPointerEvent(
    int32_t displayX, int32_t displayY, int32_t action, int32_t sourceType, int32_t pointerId)
{
    auto pointerEvent = MMI::PointerEvent::Create();
    CHKPP(pointerEvent);
    MMI::PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDisplayX(displayX);
    item.SetDisplayY(displayY);
    item.SetPressure(5);
    item.SetDeviceId(1);
    pointerEvent->AddPointerItem(item);

    item.SetPointerId(1);
    item.SetDisplayX(623);
    item.SetDisplayY(823); 
    item.SetPressure(0);
    item.SetDeviceId(1);
    pointerEvent->AddPointerItem(item);

    pointerEvent->SetPointerAction(action);
    pointerEvent->SetPointerId(pointerId);
    pointerEvent->SetSourceType(sourceType);
    return pointerEvent;
}

void SimulateDown(std::pair<int, int> loc, int32_t sourceType, int32_t pointerId)
{
    int32_t x = loc.first;
    int32_t y = loc.second;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = 
        SetupPointerEvent(x, y, MMI::PointerEvent::POINTER_ACTION_DOWN, sourceType, pointerId);
        INPUT_MANAGER->SimulateInputEvent(pointerEvent);
}

void SimulateMove(std::pair<int, int> srcLoc, std::pair<int, int> dstLoc, int32_t sourceType, int32_t pointerId)
{
    int32_t srcX = srcLoc.first;
    int32_t srcY = srcLoc.second;
    int32_t dstX = dstLoc.first;
    int32_t dstY = dstLoc.second;
    std::vector<std::pair<int32_t, int32_t>> pointers;
    if (dstX - srcX == 0) {
        for (int32_t y = srcY; y <= dstY; y++) {
            pointers.push_back({srcX, y});
        }
    } else if (dstY - srcY == 0) {
        for (int32_t x = srcX; x <= dstX; x++) {
            pointers.push_back({x, srcY});
        }
    } else {
        int32_t slope = (dstY - srcY) / (dstX - srcX);
        for (int32_t x = srcX; x < dstX; x++) {
            pointers.push_back({x, srcY + slope * (x - srcX)});
        }
        pointers.push_back({dstX, dstY});
    }
    for (const auto& pointer : pointers) {
        std::shared_ptr<MMI::PointerEvent> pointerEvent = 
        SetupPointerEvent(pointer.first , pointer.second, MMI::PointerEvent::POINTER_ACTION_MOVE,sourceType, pointerId);
        INPUT_MANAGER->SimulateInputEvent(pointerEvent);
        usleep(10);
    }
}

void SimulateUp(std::pair<int, int> loc, int32_t sourceType, int32_t pointerId)
{
    int32_t x = loc.first;
    int32_t y = loc.second;
    std::shared_ptr<MMI::PointerEvent> pointerEvent = 
    SetupPointerEvent(x, y, MMI::PointerEvent::POINTER_ACTION_UP, sourceType, pointerId);
    INPUT_MANAGER->SimulateInputEvent(pointerEvent);
}

/**
 * @tc.name: InteractionManagerTest_RegisterCoordinationListener_001
 * @tc.desc: Register coordination listener
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InteractionManagerTest, InteractionManagerTest_RegisterCoordinationListener_001, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::shared_ptr<ICoordinationListener> consumer = nullptr;
    int32_t ret = InteractionManager::GetInstance()->RegisterCoordinationListener(consumer);
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    ASSERT_EQ(ret, RET_ERR);
#else
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
#endif // OHOS_BUILD_ENABLE_COORDINATION
}

/**
 * @tc.name: InteractionManagerTest_RegisterCoordinationListener_002
 * @tc.desc: Register coordination listener
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InteractionManagerTest, InteractionManagerTest_RegisterCoordinationListener_002, TestSize.Level1)
{
    CALL_DEBUG_ENTER;
    class CoordinationListenerTest : public ICoordinationListener {
    public:
        CoordinationListenerTest() : ICoordinationListener() {}
        void OnCoordinationMessage(const std::string &deviceId, CoordinationMessage msg) override
        {
            FI_HILOGD("RegisterCoordinationListenerTest");
        };
    };
    std::shared_ptr<CoordinationListenerTest> consumer =
        std::make_shared<CoordinationListenerTest>();
    int32_t ret = InteractionManager::GetInstance()->RegisterCoordinationListener(consumer);
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    ASSERT_EQ(ret, RET_OK);
#else
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
#endif // OHOS_BUILD_ENABLE_COORDINATION
    ret = InteractionManager::GetInstance()->UnregisterCoordinationListener(consumer);
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    ASSERT_EQ(ret, RET_OK);
#else
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
#endif // OHOS_BUILD_ENABLE_COORDINATION
}

/**
 * @tc.name: InteractionManagerTest_UnregisterCoordinationListener
 * @tc.desc: Unregister coordination listener
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InteractionManagerTest, InteractionManagerTest_UnregisterCoordinationListener, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::shared_ptr<ICoordinationListener> consumer = nullptr;
    int32_t ret = InteractionManager::GetInstance()->UnregisterCoordinationListener(consumer);
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    ASSERT_EQ(ret, RET_OK);
#else
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
#endif // OHOS_BUILD_ENABLE_COORDINATION
}

/**
 * @tc.name: InteractionManagerTest_EnableCoordination
 * @tc.desc: Enable coordination
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InteractionManagerTest, InteractionManagerTest_EnableCoordination, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    bool enabled = false;
    auto fun = [](std::string listener, CoordinationMessage coordinationMessages) {
        FI_HILOGD("Enable coordination success");
    };
    int32_t ret = InteractionManager::GetInstance()->EnableCoordination(enabled, fun);
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    ASSERT_EQ(ret, RET_OK);
#else
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
#endif // OHOS_BUILD_ENABLE_COORDINATION
}

/**
 * @tc.name: InteractionManagerTest_StartCoordination
 * @tc.desc: Start coordination
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InteractionManagerTest, InteractionManagerTest_StartCoordination, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    std::string sinkDeviceId("");
    int32_t srcDeviceId = -1;
    auto fun = [](std::string listener, CoordinationMessage coordinationMessages) {
        FI_HILOGD("Start coordination success");
    };
    int32_t ret = InteractionManager::GetInstance()->StartCoordination(sinkDeviceId, srcDeviceId, fun);
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    ASSERT_NE(ret, RET_OK);
#else
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
#endif // OHOS_BUILD_ENABLE_COORDINATION
}

/**
 * @tc.name: InteractionManagerTest_StopCoordination
 * @tc.desc: Stop coordination
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InteractionManagerTest, InteractionManagerTest_StopCoordination, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    auto fun = [](std::string listener, CoordinationMessage coordinationMessages) {
        FI_HILOGD("Stop coordination success");
    };
    int32_t ret = InteractionManager::GetInstance()->StopCoordination(fun);
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    ASSERT_NE(ret, ERROR_UNSUPPORT);
#else
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
#endif // OHOS_BUILD_ENABLE_COORDINATION
}

/**
 * @tc.name: InteractionManagerTest_GetCoordinationState
 * @tc.desc: Get coordination state
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InteractionManagerTest, InteractionManagerTest_GetCoordinationState, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    const std::string deviceId("");
    auto fun = [](bool state) {
        FI_HILOGD("Get coordination state success");
    };
    int32_t ret = InteractionManager::GetInstance()->GetCoordinationState(deviceId, fun);
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    ASSERT_EQ(ret, RET_OK);
#else
    ASSERT_EQ(ret, ERROR_UNSUPPORT);
#endif // OHOS_BUILD_ENABLE_COORDINATION
}

/**
 * @tc.name: InteractionManagerTest_StartDrag
 * @tc.desc: Start Drag
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InteractionManagerTest, InteractionManagerTest_StartDrag_Share, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    DragData dragData;
    int32_t ret = SetParamShare(dragData);
    ASSERT_EQ(ret, RET_OK);
    std::function<void(int32_t)> callback = [](int32_t result) {
        FI_HILOGD("StartDrag success");
    };
    ret = InteractionManager::GetInstance()->StartDrag(dragData, callback);
    ASSERT_EQ(ret, RET_OK);
}

/**
 * @tc.name: InteractionManagerTest_StopDrag
 * @tc.desc: Stop drag
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InteractionManagerTest, InteractionManagerTest_StopDrag_Share, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t result = 0;
    int32_t ret = InteractionManager::GetInstance()->StopDrag(result);
    ASSERT_EQ(ret, RET_OK);
}

/**
 * @tc.name: InteractionManagerTest_StartDrag
 * @tc.desc: Start Drag
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InteractionManagerTest, InteractionManagerTest_StartDrag_Heap, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    DragData dragData;
    int32_t ret = SetParamHeap(dragData);
    ASSERT_EQ(ret, RET_OK);
    std::function<void(int32_t)> callback = [](int32_t result) {
        FI_HILOGD("StartDrag success");
    };
    ret = InteractionManager::GetInstance()->StartDrag(dragData, callback);
    ASSERT_EQ(ret, RET_OK);
}

/**
 * @tc.name: InteractionManagerTest_StopDrag
 * @tc.desc: Stop drag
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InteractionManagerTest, InteractionManagerTest_StopDrag_Heap, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t result = 0;
    int32_t ret = InteractionManager::GetInstance()->StopDrag(result);
    ASSERT_EQ(ret, RET_OK);
}

 /**
*  @tc.name: InteractionManagerTest_GetDragTargetPid
 * @tc.desc: Get Drag Target Pid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(InteractionManagerTest, InteractionManagerTest_GetDragTargetPid, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    int32_t pid = InteractionManager::GetInstance()->GetDragTargetPid();
    ASSERT_TRUE(pid >= -1);
}

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
