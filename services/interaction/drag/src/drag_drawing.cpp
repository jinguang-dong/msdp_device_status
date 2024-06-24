/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "drag_drawing.h"

#include <atomic>
#include <cstdint>
#include <fstream>
#include <limits>
#include <string>
#include <unistd.h>

#include <dlfcn.h>

#include "include/core/SkTextBlob.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "input_manager.h"
#include "parameters.h"
#include "pointer_event.h"
#include "pointer_style.h"
#include "render/rs_filter.h"
#include "screen_manager.h"
#include "string_ex.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

#include "animation_curve.h"
#include "devicestatus_define.h"
#include "drag_data_manager.h"
#include "drag_hisysevent.h"
#include "include/util.h"

#undef LOG_TAG
#define LOG_TAG "DragDrawing"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr int32_t BASELINE_DENSITY { 160 };
constexpr int32_t DEVICE_INDEPENDENT_PIXEL { 40 };
constexpr int32_t MAGIC_INDEPENDENT_PIXEL { 25 };
constexpr int32_t MAGIC_STYLE_OPT { 1 };
constexpr int32_t DRAG_NUM_ONE { 1 };
constexpr int32_t STRING_PX_LENGTH { 2 };
constexpr int32_t EIGHT_SIZE { 8 };
constexpr int32_t TWELVE_SIZE { 12 };
constexpr int64_t START_TIME { 181154000809 };
constexpr int64_t INTERVAL_TIME { 16666667 };
constexpr int32_t SVG_WIDTH { 40 };
constexpr float SCALE_THRESHOLD_EIGHT { 1.0F * INT32_MAX / (SVG_WIDTH + EIGHT_SIZE) };
constexpr float SCALE_THRESHOLD_TWELVE { 1.0F * INT32_MAX / (SVG_WIDTH + TWELVE_SIZE) };
constexpr int32_t SUCCESS_ANIMATION_DURATION { 300 };
constexpr int32_t ANIMATION_DURATION { 400 };
constexpr int32_t VIEW_BOX_POS { 2 };
constexpr int32_t BACKGROUND_FILTER_INDEX { 0 };
constexpr int32_t ASYNC_ROTATE_TIME { 150 };
constexpr int32_t PIXEL_MAP_INDEX { 1 };
constexpr int32_t DRAG_STYLE_INDEX { 2 };
constexpr int32_t MOUSE_ICON_INDEX { 3 };
constexpr int32_t SHORT_DURATION { 55 };
constexpr int32_t LONG_DURATION { 90 };
constexpr int32_t FIRST_PIXELMAP_INDEX { 0 };
constexpr int32_t SECOND_PIXELMAP_INDEX { 1 };
constexpr int32_t LAST_SECOND_PIXELMAP { 2 };
constexpr int32_t LAST_THIRD_PIXELMAP { 3 };
constexpr size_t TOUCH_NODE_MIN_COUNT { 3 };
constexpr size_t MOUSE_NODE_MIN_COUNT { 4 };
constexpr float DEFAULT_SCALING { 1.0f };
constexpr float BEGIN_ALPHA { 1.0f };
constexpr float END_ALPHA { 0.0f };
constexpr float START_STYLE_ALPHA { 1.0f };
constexpr float END_STYLE_ALPHA { 0.0f };
constexpr float BEGIN_SCALE { 1.0f };
constexpr float END_SCALE_FAIL { 1.2f };
constexpr float END_SCALE_SUCCESS { 0.0f };
constexpr float START_STYLE_SCALE { 1.0f };
constexpr float STYLE_CHANGE_SCALE { 1.1f };
constexpr float STYLE_MAX_SCALE { 1.2f };
constexpr float STYLE_END_SCALE { 1.0f };
constexpr float DEFAULT_PIVOT { 0.0f };
constexpr float SVG_ORIGINAL_SIZE { 40.0f };
constexpr float DEFAULT_POSITION_X { 0.0f };
constexpr float BLUR_SIGMA_SCALE { 0.57735f };
constexpr float RADIUS_VP { 23.0f };
constexpr float DEFAULT_SATURATION { 1.05f };
constexpr float DEFAULT_BRIGHTNESS { 1.05f };
constexpr float INCREASE_RATIO { 1.22f };
constexpr float DRAG_WINDOW_POSITION_Z { 6999.0f };
constexpr float DEFAULT_ANGLE { 0.0f };
constexpr float POSITIVE_ANGLE { 8.0f };
constexpr float NEGATIVE_ANGLE { -8.0f };
constexpr float DEFAULT_ALPHA { 1.0f };
constexpr float FIRST_PIXELMAP_ALPHA { 0.6f };
constexpr float SECOND_PIXELMAP_ALPHA { 0.3f };
constexpr float HALF_RATIO { 0.5f };
constexpr float ROTATION_0 { 0.0f };
constexpr float ROTATION_90 { 90.0f };
constexpr float ROTATION_360 { 360.0f };
constexpr float ROTATION_270 { 270.0f };
constexpr uint32_t TRANSPARENT_COLOR_ARGB { 0x00000000 };
constexpr int32_t DEFAULT_MOUSE_SIZE { 1 };
constexpr int32_t DEFAULT_COLOR_VALUE { 0 };
constexpr int32_t INVALID_COLOR_VALUE { -1 };
constexpr int32_t GLOBAL_WINDOW_ID { -1 };
constexpr int32_t MOUSE_DRAG_CURSOR_CIRCLE_STYLE { 41 };
constexpr int32_t CURSOR_CIRCLE_MIDDLE { 2 };
constexpr int32_t TWICE_SIZE { 2 };
constexpr int32_t HEX_FF { 0xFF };
const Rosen::RSAnimationTimingCurve SPRING = Rosen::RSAnimationTimingCurve::CreateSpring(0.347f, 0.99f, 0.0f);
constexpr float BEZIER_000 { 0.00f };
constexpr float BEZIER_020 { 0.20f };
constexpr float BEZIER_030 { 0.30f };
constexpr float BEZIER_033 { 0.33f };
constexpr float BEZIER_040 { 0.40f };
constexpr float BEZIER_060 { 0.60f };
constexpr float BEZIER_067 { 0.67f };
constexpr float BEZIER_100 { 1.00f };
constexpr float MIN_OPACITY { 0.0f };
constexpr float MAX_OPACITY { 1.0f };
constexpr int32_t TIME_DRAG_CHANGE_STYLE { 50 };
constexpr int32_t TIME_DRAG_STYLE { 100 };
constexpr int32_t TIME_STOP_FAIL_WINDOW { 125 };
constexpr int32_t TIME_STOP_SUCCESS_WINDOW { 250 };
constexpr int32_t TIME_STOP_SUCCESS_STYLE { 150 };
constexpr int32_t TIME_STOP { 0 };
constexpr int64_t TIME_SLEEP { 30000 };
constexpr int32_t INTERRUPT_SCALE { 15 };
constexpr int32_t TIMEOUT_MS { 500 };
constexpr float MAX_SCREEN_WIDTH_SM { 320.0f };
constexpr float MAX_SCREEN_WIDTH_MD { 600.0f };
constexpr float MAX_SCREEN_WIDTH_LG { 840.0f };
constexpr float MAX_SCREEN_WIDTH_XL { 1024.0f };
constexpr float SCALE_SM { 3.0f / 4 };
constexpr float SCALE_MD { 4.0f / 8 };
constexpr float SCALE_LG { 5.0f / 12 };
const std::string THREAD_NAME { "os_AnimationEventRunner" };
const std::string SUPER_HUB_THREAD_NAME { "os_SuperHubEventRunner" };
const std::string COPY_DRAG_PATH { "/system/etc/device_status/drag_icon/Copy_Drag.svg" };
const std::string COPY_ONE_DRAG_PATH { "/system/etc/device_status/drag_icon/Copy_One_Drag.svg" };
const std::string FORBID_DRAG_PATH { "/system/etc/device_status/drag_icon/Forbid_Drag.svg" };
const std::string FORBID_ONE_DRAG_PATH { "/system/etc/device_status/drag_icon/Forbid_One_Drag.svg" };
const std::string MOUSE_DRAG_DEFAULT_PATH { "/system/etc/device_status/drag_icon/Mouse_Drag_Default.svg" };
const std::string MOUSE_DRAG_MAGIC_DEFAULT_PATH { "/system/etc/device_status/drag_icon/Mouse_Drag_Magic_Default.svg" };
const std::string MOUSE_DRAG_CURSOR_CIRCLE_PATH { "/system/etc/device_status/drag_icon/Mouse_Drag_Cursor_Circle.png" };
const std::string MOVE_DRAG_PATH { "/system/etc/device_status/drag_icon/Move_Drag.svg" };
const std::string DRAG_DROP_EXTENSION_SO_PATH { "/system/lib64/drag_drop_ext/libdrag_drop_ext.z.so" };
const std::string BIG_FOLDER_LABEL { "scb_folder" };
struct DrawingInfo g_drawingInfo;
struct DragData g_dragData;

bool CheckNodesValid()
{
    FI_HILOGD("enter");
    if (g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return false;
    } else if (g_drawingInfo.nodes.empty() || g_drawingInfo.nodes[DRAG_STYLE_INDEX] == nullptr) {
        FI_HILOGE("Nodes invalid");
        return false;
    }
    if ((g_drawingInfo.sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE) &&
        (g_drawingInfo.nodes.size() < MOUSE_NODE_MIN_COUNT)) {
        FI_HILOGE("Nodes size invalid when mouse type, node size:%{public}zu", g_drawingInfo.nodes.size());
        return false;
    }
    if ((g_drawingInfo.sourceType == MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) &&
        (g_drawingInfo.nodes.size() < TOUCH_NODE_MIN_COUNT)) {
        FI_HILOGE("Nodes size invalid when touchscreen type, node size:%{public}zu", g_drawingInfo.nodes.size());
        return false;
    }
    return true;
}

float GetScaling()
{
    if (g_drawingInfo.isExistScalingValue) {
        return g_drawingInfo.scalingValue;
    }
    sptr<Rosen::Display> display = Rosen::DisplayManager::GetInstance().GetDisplayById(g_drawingInfo.displayId);
    if (display == nullptr) {
        FI_HILOGD("Get display info failed, display:%{public}d", g_drawingInfo.displayId);
        display = Rosen::DisplayManager::GetInstance().GetDisplayById(0);
        if (display == nullptr) {
            FI_HILOGE("Get display info failed, display is nullptr");
            return DEFAULT_SCALING;
        }
    }

    int32_t deviceDpi = display->GetDpi();
    FI_HILOGD("displayId:%{public}d, deviceDpi:%{public}d", g_drawingInfo.displayId, deviceDpi);
    if (deviceDpi < -std::numeric_limits<float>::epsilon()) {
        FI_HILOGE("Invalid deviceDpi:%{public}d", deviceDpi);
        return DEFAULT_SCALING;
    }
    g_drawingInfo.scalingValue = (1.0 * deviceDpi * DEVICE_INDEPENDENT_PIXEL / BASELINE_DENSITY) / SVG_ORIGINAL_SIZE;
    g_drawingInfo.isExistScalingValue = true;
    return g_drawingInfo.scalingValue;
}
} // namespace

int32_t DragDrawing::Init(const DragData &dragData, IContext* context)
{
    FI_HILOGI("enter");
    int32_t checkDragDataResult = CheckDragData(dragData);
    if (INIT_SUCCESS != checkDragDataResult) {
        return checkDragDataResult;
    }
    InitDrawingInfo(dragData);
    g_dragData = dragData;
    CreateWindow();
    CHKPR(g_drawingInfo.surfaceNode, INIT_FAIL);
    if (InitLayer() != RET_OK) {
        FI_HILOGE("Init layer failed");
        return INIT_FAIL;
    }
    DragAnimationData dragAnimationData;
    if (!CheckNodesValid() || InitDragAnimationData(dragAnimationData) != RET_OK) {
        FI_HILOGE("Init drag animation data or check nodes valid failed");
        return INIT_FAIL;
    }
    if (g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX || g_drawingInfo.nodes.size() <= PIXEL_MAP_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return INIT_FAIL;
    }
    std::shared_ptr<Rosen::RSCanvasNode> shadowNode = g_drawingInfo.nodes[PIXEL_MAP_INDEX];
    CHKPR(shadowNode, INIT_FAIL);
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPR(dragStyleNode, INIT_FAIL);
    FI_HILOGI("Begin to open drag drop extension library");
    dragExtHandler_ = dlopen(DRAG_DROP_EXTENSION_SO_PATH.c_str(), RTLD_LAZY);
    if (dragExtHandler_ == nullptr) {
        FI_HILOGE("Fail to open drag drop extension library");
    }
    FI_HILOGI("End to open drag drop extension library");
    OnStartDrag(dragAnimationData, shadowNode, dragStyleNode);
    if (!g_drawingInfo.multiSelectedNodes.empty()) {
        g_drawingInfo.isCurrentDefaultStyle = true;
        UpdateDragStyle(DragCursorStyle::MOVE);
    }
    context_ = context;
    CHKPR(rsUiDirector_, INIT_FAIL);
    if (g_drawingInfo.sourceType != MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        rsUiDirector_->SendMessages();
        return INIT_SUCCESS;
    }
    if (DrawMouseIcon() != RET_OK) {
        FI_HILOGE("Draw mouse icon failed");
        return INIT_FAIL;
    }
    rsUiDirector_->SendMessages();
    FI_HILOGI("leave");
    return INIT_SUCCESS;
}

int32_t DragDrawing::CheckDragData(const DragData &dragData)
{
    if (g_drawingInfo.isRunning) {
        FI_HILOGE("Drag drawing is running, can not init again");
        return INIT_CANCEL;
    }
    if (dragData.shadowInfos.empty()) {
        FI_HILOGE("ShadowInfos is empty");
        return INIT_FAIL;
    }
    for (const auto &shadowInfo : dragData.shadowInfos) {
        CHKPR(shadowInfo.pixelMap, INIT_FAIL);
    }
    if ((dragData.sourceType != MMI::PointerEvent::SOURCE_TYPE_MOUSE) &&
        (dragData.sourceType != MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN)) {
        FI_HILOGE("Invalid sourceType:%{public}d", dragData.sourceType);
        return INIT_FAIL;
    }
    if (dragData.dragNum < 0) {
        FI_HILOGE("Invalid dragNum:%{public}d", dragData.dragNum);
        return INIT_FAIL;
    }
    return INIT_SUCCESS;
}

void DragDrawing::Draw(int32_t displayId, int32_t displayX, int32_t displayY, bool isNeedAdjustDisplayXY)
{
    if (isRunningRotateAnimation_) {
        FI_HILOGD("Doing rotate drag window animate, ignore draw drag window");
        return;
    }
    if (displayId < 0) {
        FI_HILOGE("Invalid displayId:%{public}d", displayId);
        return;
    }
    if (isNeedAdjustDisplayXY) {
        RotateDisplayXY(displayX, displayY);
    }
    g_drawingInfo.displayId = displayId;
    g_drawingInfo.displayX = displayX;
    g_drawingInfo.displayY = displayY;
    if (displayX < 0) {
        g_drawingInfo.displayX = 0;
    }
    if (displayY < 0) {
        g_drawingInfo.displayY = 0;
    }
    int32_t adjustSize = TWELVE_SIZE * GetScaling();
    int32_t positionX = g_drawingInfo.displayX + g_drawingInfo.pixelMapX;
    int32_t positionY = g_drawingInfo.displayY + g_drawingInfo.pixelMapY - adjustSize;
    CHKPV(g_drawingInfo.parentNode);
    CHKPV(g_drawingInfo.pixelMap);
    g_drawingInfo.parentNode->SetBounds(positionX, positionY, g_drawingInfo.pixelMap->GetWidth(),
        g_drawingInfo.pixelMap->GetHeight());
    g_drawingInfo.parentNode->SetFrame(positionX, positionY, g_drawingInfo.pixelMap->GetWidth(),
        g_drawingInfo.pixelMap->GetHeight());
    if (g_drawingInfo.sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        DoDrawMouse();
    }
    if (!g_drawingInfo.multiSelectedNodes.empty() && !g_drawingInfo.multiSelectedPixelMaps.empty()) {
        MultiSelectedAnimation(positionX, positionY, adjustSize);
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void DragDrawing::UpdateDragPosition(int32_t displayId, float displayX, float displayY)
{
    if (displayId < 0) {
        FI_HILOGE("Invalid displayId:%{public}d", displayId);
        return;
    }
    RotatePosition(displayX, displayY);
    g_drawingInfo.displayId = displayId;
    g_drawingInfo.displayX = static_cast<int32_t>(displayX);
    g_drawingInfo.displayY = static_cast<int32_t>(displayY);
    g_drawingInfo.x = displayX;
    g_drawingInfo.y = displayY;
    if (displayX < 0) {
        g_drawingInfo.displayX = 0;
    }
    if (displayY < 0) {
        g_drawingInfo.displayY = 0;
    }
    float adjustSize = TWELVE_SIZE * GetScaling();
    float positionX = g_drawingInfo.x + g_drawingInfo.pixelMapX;
    float positionY = g_drawingInfo.y + g_drawingInfo.pixelMapY - adjustSize;
    auto parentNode = g_drawingInfo.parentNode;
    auto pixelMap  = g_drawingInfo.pixelMap;
    CHKPV(parentNode);
    CHKPV(pixelMap);
    parentNode->SetBounds(positionX, positionY, pixelMap->GetWidth(),
        pixelMap->GetHeight());
    parentNode->SetFrame(positionX, positionY, pixelMap->GetWidth(),
        pixelMap->GetHeight());
    if (g_drawingInfo.sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        UpdateMousePosition();
    }
    if (!g_drawingInfo.multiSelectedNodes.empty() && !g_drawingInfo.multiSelectedPixelMaps.empty()) {
        DoMultiSelectedAnimation(positionX, positionY, adjustSize);
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void DragDrawing::DoMultiSelectedAnimation(float positionX, float positionY, float adjustSize)
{
    size_t multiSelectedNodesSize = g_drawingInfo.multiSelectedNodes.size();
    size_t multiSelectedPixelMapsSize = g_drawingInfo.multiSelectedPixelMaps.size();
    for (size_t i = 0; (i < multiSelectedNodesSize) && (i < multiSelectedPixelMapsSize); ++i) {
        std::shared_ptr<Rosen::RSCanvasNode> multiSelectedNode = g_drawingInfo.multiSelectedNodes[i];
        std::shared_ptr<Media::PixelMap> multiSelectedPixelMap = g_drawingInfo.multiSelectedPixelMaps[i];
        Rosen::RSAnimationTimingProtocol protocol;
        if (i == FIRST_PIXELMAP_INDEX) {
            protocol.SetDuration(SHORT_DURATION);
        } else {
            protocol.SetDuration(LONG_DURATION);
        }
        auto pixelMap  = g_drawingInfo.pixelMap;
        CHKPV(pixelMap);
        CHKPV(multiSelectedNode);
        CHKPV(multiSelectedPixelMap);
        float multiSelectedPositionX = positionX + (static_cast<float>(pixelMap->GetWidth()) / TWICE_SIZE) -
            (static_cast<float>(multiSelectedPixelMap->GetWidth()) / TWICE_SIZE);
        float multiSelectedPositionY = positionY + (static_cast<float>(pixelMap->GetHeight()) / TWICE_SIZE) -
            (static_cast<float>(multiSelectedPixelMap->GetHeight()) / TWICE_SIZE);
        Rosen::RSNode::Animate(protocol, Rosen::RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
            multiSelectedNode->SetBounds(multiSelectedPositionX, multiSelectedPositionY + adjustSize,
                multiSelectedPixelMap->GetWidth(), multiSelectedPixelMap->GetHeight());
            multiSelectedNode->SetFrame(multiSelectedPositionX, multiSelectedPositionY + adjustSize,
                multiSelectedPixelMap->GetWidth(), multiSelectedPixelMap->GetHeight());
        });
    }
}

int32_t DragDrawing::UpdateDragStyle(DragCursorStyle style)
{
    FI_HILOGD("style:%{public}d", style);
    if ((style < DragCursorStyle::DEFAULT) || (style > DragCursorStyle::MOVE)) {
        DragDFX::WriteUpdateDragStyle(style, OHOS::HiviewDFX::HiSysEvent::EventType::FAULT);
        FI_HILOGE("Invalid style:%{public}d", style);
        return RET_ERR;
    }
    if ((style == DragCursorStyle::DEFAULT) ||
        ((style == DragCursorStyle::MOVE) && (g_drawingInfo.currentDragNum == DRAG_NUM_ONE))) {
        return UpdateDefaultDragStyle(style);
    }
    return UpdateValidDragStyle(style);
}

int32_t DragDrawing::UpdateShadowPic(const ShadowInfo &shadowInfo)
{
    FI_HILOGD("enter");
    CHKPR(shadowInfo.pixelMap, RET_ERR);
    g_drawingInfo.pixelMap = shadowInfo.pixelMap;
    g_drawingInfo.pixelMapX = shadowInfo.x;
    g_drawingInfo.pixelMapY = shadowInfo.y;
    g_drawingInfo.lastPixelMapX = g_drawingInfo.pixelMapX;
    g_drawingInfo.lastPixelMapY = g_drawingInfo.pixelMapY;
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return RET_ERR;
    }
    if (g_drawingInfo.nodes.size() <= PIXEL_MAP_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return RET_ERR;
    }
    std::shared_ptr<Rosen::RSCanvasNode> shadowNode = g_drawingInfo.nodes[PIXEL_MAP_INDEX];
    CHKPR(shadowNode, RET_ERR);
    DrawShadow(shadowNode);
    float scalingValue = GetScaling();
    if (SCALE_THRESHOLD_TWELVE < scalingValue || fabsf(SCALE_THRESHOLD_TWELVE - scalingValue) < EPSILON) {
        FI_HILOGE("Invalid scalingValue:%{public}f", scalingValue);
        return RET_ERR;
    }
    if (g_drawingInfo.sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        DrawMouseIcon();
    }
    ProcessFilter();
    Draw(g_drawingInfo.displayId, g_drawingInfo.displayX, g_drawingInfo.displayY, false);
    RotateDragWindow(rotation_);
    Rosen::RSTransaction::FlushImplicitTransaction();
    CHKPR(rsUiDirector_, RET_ERR);
    rsUiDirector_->SendMessages();
    FI_HILOGD("leave");
    return RET_OK;
}

int32_t DragDrawing::UpdatePixelMapsAngleAndAlpha()
{
    FI_HILOGD("enter");
    size_t mulNodesSize = g_drawingInfo.multiSelectedNodes.size();
    if (mulNodesSize <= 0) {
        FI_HILOGE("No pixelmap add");
        return RET_ERR;
    }
    if (mulNodesSize == 1) {
        g_drawingInfo.multiSelectedNodes.front()->SetRotation(POSITIVE_ANGLE);
        g_drawingInfo.multiSelectedNodes.front()->SetAlpha(FIRST_PIXELMAP_ALPHA);
    } else if (mulNodesSize == LAST_SECOND_PIXELMAP) {
        g_drawingInfo.multiSelectedNodes.back()->SetRotation(NEGATIVE_ANGLE);
        g_drawingInfo.multiSelectedNodes.back()->SetAlpha(SECOND_PIXELMAP_ALPHA);
    } else {
        g_drawingInfo.rootNode->RemoveChild(g_drawingInfo.multiSelectedNodes[mulNodesSize - LAST_THIRD_PIXELMAP]);
        g_drawingInfo.multiSelectedNodes[mulNodesSize - LAST_SECOND_PIXELMAP ]->SetRotation(POSITIVE_ANGLE);
        g_drawingInfo.multiSelectedNodes[mulNodesSize - LAST_SECOND_PIXELMAP ]->SetAlpha(FIRST_PIXELMAP_ALPHA);
        g_drawingInfo.multiSelectedNodes.back()->SetRotation(NEGATIVE_ANGLE);
        g_drawingInfo.multiSelectedNodes.back()->SetAlpha(SECOND_PIXELMAP_ALPHA);
    }
    FI_HILOGD("leave");
    return RET_OK;
}

int32_t DragDrawing::UpdatePixeMapDrawingOrder()
{
    FI_HILOGD("enter");
    std::shared_ptr<Rosen::RSCanvasNode> pixelMapNode = g_drawingInfo.nodes[PIXEL_MAP_INDEX];
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPR(pixelMapNode, RET_ERR);
    CHKPR(dragStyleNode, RET_ERR);
    CHKPR(g_drawingInfo.parentNode, RET_ERR);
    CHKPR(g_drawingInfo.rootNode, RET_ERR);
    g_drawingInfo.multiSelectedNodes.emplace_back(pixelMapNode);
    g_drawingInfo.parentNode->RemoveChild(dragStyleNode);
    g_drawingInfo.parentNode->RemoveChild(pixelMapNode);

    int32_t adjustSize = TWELVE_SIZE * GetScaling();
    int32_t positionX = g_drawingInfo.displayX + g_drawingInfo.pixelMapX;
    int32_t positionY = g_drawingInfo.displayY + g_drawingInfo.pixelMapY - adjustSize;
    int32_t pixelMapWidth = g_drawingInfo.pixelMap->GetWidth();
    int32_t pixelMapHeight = g_drawingInfo.pixelMap->GetHeight();
    pixelMapNode->SetBounds(positionX, positionY + adjustSize, pixelMapWidth, pixelMapHeight);
    pixelMapNode->SetFrame(positionX, positionY + adjustSize, pixelMapWidth, pixelMapHeight);

    std::shared_ptr<Rosen::RSCanvasNode> addSelectedNode = Rosen::RSCanvasNode::Create();
    CHKPR(addSelectedNode, RET_ERR);
    g_drawingInfo.nodes[PIXEL_MAP_INDEX] = addSelectedNode;
    g_drawingInfo.parentNode->AddChild(addSelectedNode);
    g_drawingInfo.parentNode->AddChild(dragStyleNode);
    g_drawingInfo.rootNode->AddChild(g_drawingInfo.multiSelectedNodes.back());
    g_drawingInfo.rootNode->RemoveChild(g_drawingInfo.parentNode);
    g_drawingInfo.rootNode->AddChild(g_drawingInfo.parentNode);

    if (g_drawingInfo.sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        std::shared_ptr<Rosen::RSCanvasNode> mouseIconNode = g_drawingInfo.nodes[MOUSE_ICON_INDEX];
        CHKPR(mouseIconNode, RET_ERR);
        g_drawingInfo.rootNode->RemoveChild(mouseIconNode);
        g_drawingInfo.rootNode->AddChild(mouseIconNode);
    }

    if (UpdatePixelMapsAngleAndAlpha() != RET_OK) {
        FI_HILOGE("setPixelMapsAngleAndAlpha failed");
        return RET_ERR;
    }
    DrawShadow(pixelMapNode);
    FI_HILOGD("leave");
    return RET_OK;
}

int32_t DragDrawing::AddSelectedPixelMap(std::shared_ptr<OHOS::Media::PixelMap> pixelMap)
{
    FI_HILOGD("enter");
    CHKPR(pixelMap, RET_ERR);
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return RET_ERR;
    }

    g_drawingInfo.multiSelectedPixelMaps.emplace_back(g_drawingInfo.pixelMap);
    g_drawingInfo.pixelMap = pixelMap;
    if (UpdatePixeMapDrawingOrder() != RET_OK) {
        FI_HILOGE("Update pixeMap drawing order failed");
        return RET_ERR;
    }
    Draw(g_drawingInfo.displayId, g_drawingInfo.displayX, g_drawingInfo.displayY, false);
    g_drawingInfo.currentDragNum = g_drawingInfo.multiSelectedPixelMaps.size() + 1;
    if (UpdateDragStyle(g_drawingInfo.currentStyle) != RET_OK) {
        FI_HILOGE("Update drag style failed");
        return RET_ERR;
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
    FI_HILOGD("leave");
    return RET_OK;
}

void DragDrawing::OnDragSuccess(IContext* context)
{
    FI_HILOGI("enter");
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return;
    }
    if (g_drawingInfo.nodes.size() <= PIXEL_MAP_INDEX || g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> shadowNode = g_drawingInfo.nodes[PIXEL_MAP_INDEX];
    CHKPV(shadowNode);
    std::shared_ptr<Rosen::RSCanvasNode> styleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPV(styleNode);
    g_drawingInfo.context = context;
    OnStopDragSuccess(shadowNode, styleNode);
    FI_HILOGI("leave");
}

void DragDrawing::OnDragFail(IContext* context)
{
    FI_HILOGI("enter");
    std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode = g_drawingInfo.surfaceNode;
    CHKPV(surfaceNode);
    std::shared_ptr<Rosen::RSNode> rootNode = g_drawingInfo.rootNode;
    CHKPV(rootNode);
    g_drawingInfo.context = context;
    OnStopDragFail(surfaceNode, rootNode);
    FI_HILOGI("leave");
}

void DragDrawing::EraseMouseIcon()
{
    FI_HILOGI("enter");
    if (g_drawingInfo.nodes.size() < MOUSE_NODE_MIN_COUNT) {
        FI_HILOGE("Nodes size invalid, node size:%{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    if (g_drawingInfo.nodes.size() <= MOUSE_ICON_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> mouseIconNode = g_drawingInfo.nodes[MOUSE_ICON_INDEX];
    CHKPV(mouseIconNode);
    if (drawMouseIconModifier_ != nullptr) {
        mouseIconNode->RemoveModifier(drawMouseIconModifier_);
        drawMouseIconModifier_ = nullptr;
    }
    CHKPV(g_drawingInfo.rootNode);
    g_drawingInfo.rootNode->RemoveChild(mouseIconNode);
    CHKPV(rsUiDirector_);
    rsUiDirector_->SendMessages();
    FI_HILOGI("leave");
}

void DragDrawing::DestroyDragWindow()
{
    FI_HILOGI("enter");
    ResetParameter();
    RemoveModifier();
    ClearMultiSelectedData();
    if (!g_drawingInfo.nodes.empty()) {
        g_drawingInfo.nodes.clear();
        g_drawingInfo.nodes.shrink_to_fit();
    }
    if (g_drawingInfo.parentNode != nullptr) {
        g_drawingInfo.parentNode->ClearChildren();
        g_drawingInfo.parentNode.reset();
        g_drawingInfo.parentNode = nullptr;
    }
    if (g_drawingInfo.rootNode != nullptr) {
        g_drawingInfo.rootNode->ClearChildren();
        g_drawingInfo.rootNode.reset();
        g_drawingInfo.rootNode = nullptr;
    }
    if (g_drawingInfo.surfaceNode != nullptr) {
        g_drawingInfo.surfaceNode->DetachToDisplay(screenId_);
        screenId_ = 0;
        g_drawingInfo.displayId = -1;
        g_drawingInfo.surfaceNode = nullptr;
        Rosen::RSTransaction::FlushImplicitTransaction();
    }
    CHKPV(rsUiDirector_);
    rsUiDirector_->SetRoot(-1);
    rsUiDirector_->SendMessages();
    FI_HILOGI("leave");
}

void DragDrawing::UpdateDrawingState()
{
    FI_HILOGD("enter");
    g_drawingInfo.isRunning = false;
    FI_HILOGD("leave");
}

void DragDrawing::UpdateDragWindowState(bool visible)
{
    CHKPV(g_drawingInfo.surfaceNode);
    if (visible) {
        g_drawingInfo.surfaceNode->SetVisible(true);
        FI_HILOGI("Drag surfaceNode show success");
    } else {
        g_drawingInfo.surfaceNode->SetVisible(false);
        FI_HILOGI("Drag surfaceNode hide success");
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void DragDrawing::OnStartDrag(const DragAnimationData &dragAnimationData,
    std::shared_ptr<Rosen::RSCanvasNode> shadowNode, std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode)
{
    FI_HILOGI("enter");
    CHKPV(shadowNode);
    if (DrawShadow(shadowNode) != RET_OK) {
        FI_HILOGE("Draw shadow failed");
        return;
    }
    g_drawingInfo.isCurrentDefaultStyle = true;
    if (dragExtHandler_ == nullptr) {
        FI_HILOGE("Fail to open drag drop extension library");
        return;
    }
    auto dragDropStartExtFunc = reinterpret_cast<DragStartExtFunc>(dlsym(dragExtHandler_, "OnStartDragExt"));
    if (dragDropStartExtFunc == nullptr) {
        FI_HILOGE("Fail to get drag drop extension function");
        dlclose(dragExtHandler_);
        dragExtHandler_ = nullptr;
        return;
    }
#ifdef OHOS_DRAG_ENABLE_ANIMATION
    if (!GetSuperHubHandler()->PostTask(std::bind(dragDropStartExtFunc, g_dragData))) {
        FI_HILOGE("Start style animation failed");
    }
#endif // OHOS_DRAG_ENABLE_ANIMATION
    FI_HILOGI("leave");
}

void DragDrawing::NotifyDragInfo(const std::string &sourceName, const std::string &targetName)
{
    FI_HILOGI("NotifyDragInfo");
    if (dragExtHandler_ == nullptr) {
        FI_HILOGE("Fail to open drag drop extension library");
        return;
    }
    auto dragDropExtFunc = reinterpret_cast<DragNotifyExtFunc>(dlsym(dragExtHandler_, "OnNotifyDragInfo"));
    if (dragDropExtFunc == nullptr) {
        FI_HILOGE("Fail to get drag drop extension function");
        dlclose(dragExtHandler_);
        dragExtHandler_ = nullptr;
        return;
    }
    struct DragEventInfo dragEventInfo;
    dragEventInfo.sourcePkgName = sourceName;
    dragEventInfo.targetPkgName = targetName;
    if (!GetSuperHubHandler()->PostTask(std::bind(dragDropExtFunc, dragEventInfo))) {
        FI_HILOGE("notify drag info failed");
    }
}

std::shared_ptr<AppExecFwk::EventHandler> DragDrawing::GetSuperHubHandler()
{
    if (superHubHandler_ == nullptr) {
        auto runner = AppExecFwk::EventRunner::Create(SUPER_HUB_THREAD_NAME);
        superHubHandler_ = std::make_shared<AppExecFwk::EventHandler>(std::move(runner));
    }
    return superHubHandler_;
}

void DragDrawing::ResetSuperHubHandler()
{
    if (superHubHandler_ != nullptr) {
        superHubHandler_->RemoveAllEvents();
        superHubHandler_ = nullptr;
    }
}

float DragDrawing::AdjustDoubleValue(double doubleValue)
{
    FI_HILOGI("doubleValue is %{public}f", doubleValue);
    float dragOriginDpi = DRAG_DATA_MGR.GetDragOriginDpi();
    if (dragOriginDpi > EPSILON) {
        float scalingValue = GetScaling() / dragOriginDpi;
        doubleValue = doubleValue * scalingValue;
        if (fabs(scalingValue - 1.0f) > EPSILON) {
            float widthScale = CalculateWidthScale();
            doubleValue = doubleValue * widthScale;
        }
    }
    float floatValue = static_cast<float>(doubleValue);
    FI_HILOGI("floatValue is %{public}f", floatValue);
    return floatValue;
}

void DragDrawing::CheckStyleNodeModifier(std::shared_ptr<Rosen::RSCanvasNode> styleNode)
{
    FI_HILOGD("enter");
    CHKPV(styleNode);
    if (drawStyleChangeModifier_ != nullptr) {
        styleNode->RemoveModifier(drawStyleChangeModifier_);
        drawStyleChangeModifier_ = nullptr;
    }
    if (drawStyleScaleModifier_ != nullptr && hasRunningScaleAnimation_) {
        needBreakStyleScaleAnimation_ = true;
    }
    styleNode->RemoveAllAnimations();
    FI_HILOGD("leave");
}

void DragDrawing::RemoveStyleNodeModifier(std::shared_ptr<Rosen::RSCanvasNode> styleNode)
{
    FI_HILOGD("enter");
    CHKPV(styleNode);
    if (drawStyleChangeModifier_ != nullptr) {
        styleNode->RemoveModifier(drawStyleChangeModifier_);
        drawStyleChangeModifier_ = nullptr;
    }
    if (drawStyleScaleModifier_ != nullptr) {
        styleNode->RemoveModifier(drawStyleScaleModifier_);
        drawStyleScaleModifier_ = nullptr;
    }
    FI_HILOGD("leave");
}

void DragDrawing::UpdateAnimationProtocol(Rosen::RSAnimationTimingProtocol protocol)
{
    FI_HILOGD("enter");
    g_drawingInfo.startNum = START_TIME;
    interruptNum_ = START_TIME * INTERRUPT_SCALE;
    hasRunningAnimation_ = true;
    bool stopSignal = true;
    CHKPV(rsUiDirector_);
    while (hasRunningAnimation_) {
        hasRunningAnimation_ = rsUiDirector_->FlushAnimation(g_drawingInfo.startNum);
        rsUiDirector_->FlushModifier();
        rsUiDirector_->SendMessages();
        if ((g_drawingInfo.startNum >= interruptNum_) && stopSignal) {
            protocol.SetDuration(TIME_STOP);
            stopSignal = false;
        }
        g_drawingInfo.startNum += INTERVAL_TIME;
        usleep(TIME_SLEEP);
    }
    FI_HILOGD("leave");
}

void DragDrawing::StartStyleAnimation(float startScale, float endScale, int32_t duration)
{
    FI_HILOGI("StartStyleAnimation, startScale is %{public}lf", startScale);
    if (!CheckNodesValid() || needBreakStyleScaleAnimation_ || hasRunningStopAnimation_) {
        FI_HILOGE("needBreakStyleScaleAnimation_ or hasRunningStopAnimation_, return");
        return;
    }
    if (g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPV(dragStyleNode);
    RemoveStyleNodeModifier(dragStyleNode);
    drawStyleScaleModifier_ = std::make_shared<DrawStyleScaleModifier>();
    dragStyleNode->AddModifier(drawStyleScaleModifier_);
    CHKPV(drawStyleScaleModifier_);
    drawStyleScaleModifier_->SetScale(startScale);
    Rosen::RSAnimationTimingProtocol protocol;
    protocol.SetDuration(duration);
    auto springCurveStyle = endScale == STYLE_END_SCALE
        ? Rosen::RSAnimationTimingCurve::CreateCubicCurve(BEZIER_030, BEZIER_000, BEZIER_040, BEZIER_100)
        : Rosen::RSAnimationTimingCurve::CreateCubicCurve(BEZIER_020, BEZIER_000, BEZIER_060, BEZIER_100);
    CHKPV(drawStyleScaleModifier_);
    Rosen::RSNode::Animate(protocol, springCurveStyle, [&]() {
        drawStyleScaleModifier_->SetScale(endScale);
    });
    UpdateAnimationProtocol(protocol);
    if (endScale == STYLE_CHANGE_SCALE) {
        if (drawStyleChangeModifier_ != nullptr) {
            dragStyleNode->RemoveModifier(drawStyleChangeModifier_);
            drawStyleChangeModifier_ = nullptr;
        }
        if (drawStyleScaleModifier_ != nullptr) {
            dragStyleNode->RemoveModifier(drawStyleScaleModifier_);
            drawStyleScaleModifier_ = nullptr;
        }
        drawStyleChangeModifier_ = std::make_shared<DrawStyleChangeModifier>(g_drawingInfo.stylePixelMap);
        dragStyleNode->AddModifier(drawStyleChangeModifier_);
    }
    if (endScale == STYLE_END_SCALE && drawStyleScaleModifier_ != nullptr) {
        dragStyleNode->RemoveModifier(drawStyleScaleModifier_);
        drawStyleScaleModifier_ = nullptr;
    }
}

void DragDrawing::ChangeStyleAnimation()
{
    FI_HILOGD("enter");
    hasRunningScaleAnimation_ = true;
    StartStyleAnimation(START_STYLE_SCALE, STYLE_CHANGE_SCALE, TIME_DRAG_CHANGE_STYLE);
    StartStyleAnimation(STYLE_CHANGE_SCALE, STYLE_MAX_SCALE, TIME_DRAG_CHANGE_STYLE);
    StartStyleAnimation(STYLE_MAX_SCALE, STYLE_END_SCALE, TIME_DRAG_STYLE);
    needBreakStyleScaleAnimation_ = false;
    hasRunningScaleAnimation_ = false;
    FI_HILOGD("leave");
}

void DragDrawing::OnDragStyleAnimation()
{
    FI_HILOGD("enter");
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return;
    }
    if (g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPV(dragStyleNode);
    needBreakStyleScaleAnimation_ = false;
    if (g_drawingInfo.isPreviousDefaultStyle == true || g_drawingInfo.isCurrentDefaultStyle == true) {
        FI_HILOGE("Has DefaultStyle, change style and return");
        CheckStyleNodeModifier(dragStyleNode);
        drawStyleChangeModifier_ = std::make_shared<DrawStyleChangeModifier>(g_drawingInfo.stylePixelMap);
        dragStyleNode->AddModifier(drawStyleChangeModifier_);
        return;
    }
    rsUiDirector_ = Rosen::RSUIDirector::Create();
    CHKPV(rsUiDirector_);
    rsUiDirector_->Init();
    rsUiDirector_->SetRSSurfaceNode(g_drawingInfo.surfaceNode);
    if (handler_ == nullptr) {
        auto runner = AppExecFwk::EventRunner::Create(THREAD_NAME);
        handler_ = std::make_shared<AppExecFwk::EventHandler>(std::move(runner));
    }
    CheckStyleNodeModifier(dragStyleNode);
    handler_->PostTask(std::bind(&DragDrawing::ChangeStyleAnimation, this));
    FI_HILOGD("leave");
}

void DragDrawing::OnDragStyle(std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode,
    std::shared_ptr<Media::PixelMap> stylePixelMap)
{
    FI_HILOGD("enter");
    CHKPV(dragStyleNode);
    CHKPV(stylePixelMap);
#ifdef OHOS_DRAG_ENABLE_ANIMATION
    if (handler_ == nullptr) {
        auto runner = AppExecFwk::EventRunner::Create(THREAD_NAME);
        CHKPV(runner);
        handler_ = std::make_shared<AppExecFwk::EventHandler>(std::move(runner));
    }
    if (drawSVGModifier_ != nullptr) {
        dragStyleNode->RemoveModifier(drawSVGModifier_);
        drawSVGModifier_ = nullptr;
    }
    if (!handler_->PostTask(std::bind(&DragDrawing::OnDragStyleAnimation, this))) {
        FI_HILOGE("Drag style animation failed");
        DrawStyle(dragStyleNode, stylePixelMap);
    }
#else // OHOS_DRAG_ENABLE_ANIMATION
    DrawStyle(dragStyleNode, stylePixelMap);
#endif // OHOS_DRAG_ENABLE_ANIMATION
    FI_HILOGD("leave");
}

void DragDrawing::OnStopAnimationSuccess()
{
    FI_HILOGI("enter");
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return;
    }
    if (g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    if (dragStyleNode != nullptr && drawStyleScaleModifier_ != nullptr) {
        dragStyleNode->RemoveModifier(drawStyleScaleModifier_);
        dragStyleNode->RemoveAllAnimations();
        drawStyleScaleModifier_ = nullptr;
        needBreakStyleScaleAnimation_ = true;
    }
    CHKPV(g_drawingInfo.rootNode);
    hasRunningStopAnimation_ = true;
    if (drawDragStopModifier_ != nullptr) {
        g_drawingInfo.rootNode->RemoveModifier(drawDragStopModifier_);
        drawDragStopModifier_ = nullptr;
    }
    drawDragStopModifier_ = std::make_shared<DrawDragStopModifier>();
    g_drawingInfo.rootNode->AddModifier(drawDragStopModifier_);
    drawDragStopModifier_->SetAlpha(BEGIN_ALPHA);
    drawDragStopModifier_->SetScale(BEGIN_SCALE);
    drawDragStopModifier_->SetStyleScale(START_STYLE_SCALE);
    drawDragStopModifier_->SetStyleAlpha(START_STYLE_ALPHA);
    Rosen::RSAnimationTimingProtocol windowProtocol;
    Rosen::RSAnimationTimingProtocol styleProtocol;
    windowProtocol.SetDuration(TIME_STOP_SUCCESS_WINDOW);
    styleProtocol.SetDuration(TIME_STOP_SUCCESS_STYLE);
    auto springCurveSuccessWindow = Rosen::RSAnimationTimingCurve::CreateCubicCurve(BEZIER_040, BEZIER_000,
        BEZIER_100, BEZIER_100);
    auto springCurveSuccessStyle = Rosen::RSAnimationTimingCurve::CreateCubicCurve(BEZIER_000, BEZIER_000,
        BEZIER_100, BEZIER_100);
    Rosen::RSNode::Animate(windowProtocol, springCurveSuccessWindow, [&]() {
        drawDragStopModifier_->SetAlpha(BEGIN_ALPHA);
        drawDragStopModifier_->SetScale(END_SCALE_SUCCESS);
        Rosen::RSNode::Animate(styleProtocol, springCurveSuccessStyle, [&]() {
            drawDragStopModifier_->SetStyleAlpha(END_STYLE_ALPHA);
            drawDragStopModifier_->SetStyleScale(START_STYLE_SCALE);
        });
    });
    DoEndAnimation();
    FI_HILOGI("leave");
}

void DragDrawing::OnStopDragSuccess(std::shared_ptr<Rosen::RSCanvasNode> shadowNode,
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode)
{
    FI_HILOGD("enter");
    auto animateCb = std::bind(&DragDrawing::InitVSync, this, END_ALPHA, END_SCALE_SUCCESS);
#ifdef OHOS_DRAG_ENABLE_ANIMATION
    ResetAnimationParameter();
    auto runner = AppExecFwk::EventRunner::Create(THREAD_NAME);
    CHKPV(runner);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(std::move(runner));
    if (!handler_->PostTask(std::bind(&DragDrawing::OnStopAnimationSuccess, this))) {
        FI_HILOGE("Failed to stop style animation");
        RunAnimation(animateCb);
    }
#else // OHOS_DRAG_ENABLE_ANIMATION
    RunAnimation(animateCb);
#endif // OHOS_DRAG_ENABLE_ANIMATION
    FI_HILOGD("leave");
}

void DragDrawing::OnStopAnimationFail()
{
    FI_HILOGI("enter");
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return;
    }
    if (g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    if (dragStyleNode != nullptr && drawStyleScaleModifier_ != nullptr) {
        dragStyleNode->RemoveModifier(drawStyleScaleModifier_);
        dragStyleNode->RemoveAllAnimations();
        drawStyleScaleModifier_ = nullptr;
        needBreakStyleScaleAnimation_ = true;
    }
    CHKPV(g_drawingInfo.rootNode);
    if (drawDragStopModifier_ != nullptr) {
        g_drawingInfo.rootNode->RemoveModifier(drawDragStopModifier_);
        drawDragStopModifier_ = nullptr;
    }
    drawDragStopModifier_ = std::make_shared<DrawDragStopModifier>();
    hasRunningStopAnimation_ = true;
    g_drawingInfo.rootNode->AddModifier(drawDragStopModifier_);
    drawDragStopModifier_->SetAlpha(BEGIN_ALPHA);
    drawDragStopModifier_->SetScale(BEGIN_SCALE);
    drawDragStopModifier_->SetStyleScale(START_STYLE_SCALE);
    drawDragStopModifier_->SetStyleAlpha(START_STYLE_ALPHA);
    Rosen::RSAnimationTimingProtocol protocol;
    protocol.SetDuration(TIME_STOP_FAIL_WINDOW);
    auto springCurveFail = Rosen::RSAnimationTimingCurve::CreateCubicCurve(BEZIER_033, BEZIER_000,
        BEZIER_067, BEZIER_100);
    Rosen::RSNode::Animate(protocol, springCurveFail, [&]() {
        drawDragStopModifier_->SetAlpha(END_ALPHA);
        drawDragStopModifier_->SetScale(END_SCALE_FAIL);
        drawDragStopModifier_->SetStyleScale(START_STYLE_SCALE);
        drawDragStopModifier_->SetStyleAlpha(END_STYLE_ALPHA);
    });
    DoEndAnimation();
    FI_HILOGI("leave");
}

void DragDrawing::OnStopDragFail(std::shared_ptr<Rosen::RSSurfaceNode> surfaceNode,
    std::shared_ptr<Rosen::RSNode> rootNode)
{
    FI_HILOGD("enter");
    auto animateCb = std::bind(&DragDrawing::InitVSync, this, END_ALPHA, END_SCALE_FAIL);
#ifdef OHOS_DRAG_ENABLE_ANIMATION
    ResetAnimationParameter();
    auto runner = AppExecFwk::EventRunner::Create(THREAD_NAME);
    CHKPV(runner);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(std::move(runner));
    if (!handler_->PostTask(std::bind(&DragDrawing::OnStopAnimationFail, this))) {
        FI_HILOGE("Failed to stop style animation");
        RunAnimation(animateCb);
    }
#else // OHOS_DRAG_ENABLE_ANIMATION
    RunAnimation(animateCb);
#endif // OHOS_DRAG_ENABLE_ANIMATION
    FI_HILOGD("leave");
}

void DragDrawing::OnStopAnimation()
{
    FI_HILOGD("enter");
}

int32_t DragDrawing::RunAnimation(std::function<int32_t()> cb)
{
    FI_HILOGD("enter");
    ResetAnimationParameter();
    auto runner = AppExecFwk::EventRunner::Create(THREAD_NAME);
    CHKPR(runner, RET_ERR);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(std::move(runner));
    if (!handler_->PostTask(cb)) {
        FI_HILOGE("Send vsync event failed");
        return RET_ERR;
    }
    FI_HILOGD("leave");
    return RET_OK;
}

int32_t DragDrawing::DrawShadow(std::shared_ptr<Rosen::RSCanvasNode> shadowNode)
{
    FI_HILOGD("enter");
    CHKPR(shadowNode, RET_ERR);
    if (drawPixelMapModifier_ != nullptr) {
        shadowNode->RemoveModifier(drawPixelMapModifier_);
        drawPixelMapModifier_ = nullptr;
    }
    drawPixelMapModifier_ = std::make_shared<DrawPixelMapModifier>();
    shadowNode->AddModifier(drawPixelMapModifier_);
    FilterInfo filterInfo = g_drawingInfo.filterInfo;
    Rosen::Vector4f cornerRadiusVector = { filterInfo.cornerRadius1, filterInfo.cornerRadius2,
        filterInfo.cornerRadius3, filterInfo.cornerRadius4 };
    shadowNode->SetCornerRadius(cornerRadiusVector * filterInfo.dipScale);
    shadowNode->SetAlpha(filterInfo.opacity);
    FI_HILOGD("leave");
    return RET_OK;
}

int32_t DragDrawing::DrawMouseIcon()
{
    FI_HILOGD("enter");
    if (g_drawingInfo.nodes.size() < MOUSE_NODE_MIN_COUNT) {
        FI_HILOGE("Nodes size invalid, node size:%{public}zu", g_drawingInfo.nodes.size());
        return RET_ERR;
    }
    if (g_drawingInfo.nodes.size() <= MOUSE_ICON_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return RET_ERR;
    }
    std::shared_ptr<Rosen::RSCanvasNode> mouseIconNode = g_drawingInfo.nodes[MOUSE_ICON_INDEX];
    CHKPR(mouseIconNode, RET_ERR);
    if (drawMouseIconModifier_ != nullptr) {
        mouseIconNode->RemoveModifier(drawMouseIconModifier_);
        drawMouseIconModifier_ = nullptr;
    }
    int32_t ret = MMI::InputManager::GetInstance()->GetPointerStyle(GLOBAL_WINDOW_ID, pointerStyle_);
    if (ret != RET_OK) {
        FI_HILOGE("Get pointer style failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    drawMouseIconModifier_ = std::make_shared<DrawMouseIconModifier>(pointerStyle_);
    mouseIconNode->AddModifier(drawMouseIconModifier_);
    FI_HILOGD("leave");
    return RET_OK;
}

void DragDrawing::FlushDragPosition(uint64_t nanoTimestamp)
{
    DragMoveEvent event = dragSmoothProcessor_.SmoothMoveEvent(nanoTimestamp,
        vSyncStation_.GetVSyncPeriod());
    FI_HILOGD("Move position x:%{public}f, y:%{public}f, timestamp:%{public}" PRId64
        "displayId:%{public}d", event.displayX, event.displayY, event.timestamp, event.displayId);
    UpdateDragPosition(event.displayId, event.displayX, event.displayY);
}

void DragDrawing::OnDragMove(int32_t displayId, int32_t displayX, int32_t displayY, int64_t actionTime)
{
    if (isRunningRotateAnimation_) {
        FI_HILOGD("Doing rotate drag window animate, ignore draw drag window");
        return;
    }
    std::chrono::microseconds microseconds(actionTime);
    TimeStamp time(microseconds);
    uint64_t actionTimeCount = static_cast<uint64_t>(time.time_since_epoch().count());
    DragMoveEvent event = {
        .displayX = displayX,
        .displayY = displayY,
        .displayId = displayId,
        .timestamp = actionTimeCount,
    };
    dragSmoothProcessor_.InsertEvent(event);
    if (frameCallback_ == nullptr) {
        frameCallback_ = std::make_shared<DragFrameCallback>(
            std::bind(&DragDrawing::FlushDragPosition, this, std::placeholders::_1));
    }
    vSyncStation_.RequestFrame(TYPE_FLUSH_DRAG_POSITION, frameCallback_);
}

int32_t DragDrawing::DrawStyle(std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode,
    std::shared_ptr<Media::PixelMap> stylePixelMap)
{
    FI_HILOGD("enter");
    CHKPR(dragStyleNode, RET_ERR);
    CHKPR(stylePixelMap, RET_ERR);
    if (drawSVGModifier_ != nullptr) {
        dragStyleNode->RemoveModifier(drawSVGModifier_);
        drawSVGModifier_ = nullptr;
    }
    drawSVGModifier_ = std::make_shared<DrawSVGModifier>(stylePixelMap);
    dragStyleNode->AddModifier(drawSVGModifier_);
    FI_HILOGD("leave");
    return RET_OK;
}

int32_t DragDrawing::InitVSync(float endAlpha, float endScale)
{
    FI_HILOGD("enter");
    CHKPR(g_drawingInfo.rootNode, RET_ERR);
    if (drawDynamicEffectModifier_ != nullptr) {
        g_drawingInfo.rootNode->RemoveModifier(drawDynamicEffectModifier_);
        drawDynamicEffectModifier_ = nullptr;
    }
    drawDynamicEffectModifier_ = std::make_shared<DrawDynamicEffectModifier>();
    g_drawingInfo.rootNode->AddModifier(drawDynamicEffectModifier_);
    drawDynamicEffectModifier_->SetAlpha(BEGIN_ALPHA);
    drawDynamicEffectModifier_->SetScale(BEGIN_SCALE);

    Rosen::RSAnimationTimingProtocol protocol;
    protocol.SetDuration(SUCCESS_ANIMATION_DURATION);
    Rosen::RSNode::Animate(protocol, Rosen::RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
        drawDynamicEffectModifier_->SetAlpha(endAlpha);
        drawDynamicEffectModifier_->SetScale(endScale);
    });
    Rosen::RSTransaction::FlushImplicitTransaction();
    DoEndAnimation();
    FI_HILOGD("leave");
    return RET_OK;
}

int32_t DragDrawing::StartVsync()
{
    FI_HILOGI("enter");
    if (receiver_ == nullptr) {
        CHKPR(handler_, RET_ERR);
        receiver_ = Rosen::RSInterfaces::GetInstance().CreateVSyncReceiver("DragDrawing", handler_);
        CHKPR(receiver_, RET_ERR);
    }
    int32_t ret = receiver_->Init();
    if (ret != RET_OK) {
        FI_HILOGE("Receiver init failed");
        return RET_ERR;
    }
    Rosen::VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = std::bind(&DragDrawing::OnVsync, this)
    };
    ret = receiver_->RequestNextVSync(fcb);
    if (ret != RET_OK) {
        FI_HILOGE("Request next vsync failed");
    }
    FI_HILOGI("leave");
    return ret;
}

void DragDrawing::OnVsync()
{
    FI_HILOGD("enter");
    CHKPV(rsUiDirector_);
    bool hasRunningAnimation = rsUiDirector_->FlushAnimation(g_drawingInfo.startNum);
    rsUiDirector_->FlushModifier();
    rsUiDirector_->SendMessages();
    if (!hasRunningAnimation) {
        FI_HILOGI("Stop runner, hasRunningAnimation:%{public}d, needDestroyDragWindow:%{public}d",
            hasRunningAnimation, g_drawingInfo.needDestroyDragWindow.load());
        if (g_drawingInfo.needDestroyDragWindow) {
            ResetAnimationFlag();
        }
        return;
    }
    Rosen::VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = std::bind(&DragDrawing::OnVsync, this)
    };
    CHKPV(receiver_);
    int32_t ret = receiver_->RequestNextVSync(fcb);
    if (ret != RET_OK) {
        FI_HILOGE("Request next vsync failed");
    }
    rsUiDirector_->SendMessages();
    g_drawingInfo.startNum += INTERVAL_TIME;
    FI_HILOGD("leave");
}

void DragDrawing::InitDrawingInfo(const DragData &dragData)
{
    g_drawingInfo.isRunning = true;
    if (dragData.shadowInfos.empty()) {
        FI_HILOGE("ShadowInfos is empty");
        return;
    }
    g_drawingInfo.pixelMap = dragData.shadowInfos.front().pixelMap;
    g_drawingInfo.pixelMapX = dragData.shadowInfos.front().x;
    g_drawingInfo.pixelMapY = dragData.shadowInfos.front().y;
    float dragOriginDpi = DRAG_DATA_MGR.GetDragOriginDpi();
    if (dragOriginDpi > EPSILON) {
        float scalingValue = GetScaling() / dragOriginDpi;
        CHKPV(g_drawingInfo.pixelMap);
        g_drawingInfo.pixelMap->scale(scalingValue, scalingValue, Media::AntiAliasingOption::HIGH);
        g_drawingInfo.pixelMapX = g_drawingInfo.pixelMapX * scalingValue;
        g_drawingInfo.pixelMapY = g_drawingInfo.pixelMapY * scalingValue;
        if (fabs(scalingValue - 1.0f) > EPSILON) {
            float widthScale = CalculateWidthScale();
            CHKPV(g_drawingInfo.pixelMap);
            g_drawingInfo.pixelMap->scale(widthScale, widthScale, Media::AntiAliasingOption::HIGH);
            g_drawingInfo.pixelMapX = g_drawingInfo.pixelMapX * widthScale;
            g_drawingInfo.pixelMapY = g_drawingInfo.pixelMapY * widthScale;
        }
    }
    g_drawingInfo.lastPixelMapX = g_drawingInfo.pixelMapX;
    g_drawingInfo.lastPixelMapY = g_drawingInfo.pixelMapY;
    g_drawingInfo.currentDragNum = dragData.dragNum;
    g_drawingInfo.sourceType = dragData.sourceType;
    g_drawingInfo.displayId = dragData.displayId;
    g_drawingInfo.displayX = dragData.displayX;
    g_drawingInfo.displayY = dragData.displayY;
    RotateDisplayXY(g_drawingInfo.displayX, g_drawingInfo.displayY);
    if (!ParserExtraInfo(dragData.extraInfo, g_drawingInfo.extraInfo)) {
        FI_HILOGI("No parser valid extraInfo data");
    }
    if (!ParserFilterInfo(dragData.filterInfo, g_drawingInfo.filterInfo)) {
        FI_HILOGI("No parser valid filterInfo data");
    }
    size_t shadowInfosSize = dragData.shadowInfos.size();
    for (size_t i = 1; i < shadowInfosSize; ++i) {
        std::shared_ptr<Media::PixelMap> pixelMap = dragData.shadowInfos[i].pixelMap;
        if (dragOriginDpi > EPSILON) {
            float scalingValue = GetScaling() / dragOriginDpi;
            CHKPV(pixelMap);
            pixelMap->scale(scalingValue, scalingValue, Media::AntiAliasingOption::HIGH);
        }
        g_drawingInfo.multiSelectedPixelMaps.emplace_back(pixelMap);
    }
}

int32_t DragDrawing::InitDragAnimationData(DragAnimationData &dragAnimationData)
{
    CHKPR(g_drawingInfo.pixelMap, RET_ERR);
    dragAnimationData.pixelMap = g_drawingInfo.pixelMap;
    dragAnimationData.displayX = g_drawingInfo.displayX;
    dragAnimationData.displayY = g_drawingInfo.displayY;
    dragAnimationData.offsetX = g_drawingInfo.pixelMapX;
    dragAnimationData.offsetY = g_drawingInfo.pixelMapY;
    return RET_OK;
}

int32_t DragDrawing::InitLayer()
{
    FI_HILOGI("enter");
    if (g_drawingInfo.surfaceNode == nullptr) {
        FI_HILOGE("Init layer failed, surfaceNode is nullptr");
        return RET_ERR;
    }
    auto surface = g_drawingInfo.surfaceNode->GetSurface();
    if (surface == nullptr) {
        g_drawingInfo.surfaceNode->DetachToDisplay(g_drawingInfo.displayId);
        g_drawingInfo.surfaceNode = nullptr;
        FI_HILOGE("Init layer failed, surface is nullptr");
        Rosen::RSTransaction::FlushImplicitTransaction();
        return RET_ERR;
    }
    if (g_drawingInfo.isInitUiDirector) {
        g_drawingInfo.isInitUiDirector = false;
        rsUiDirector_ = Rosen::RSUIDirector::Create();
        CHKPR(rsUiDirector_, RET_ERR);
        rsUiDirector_->Init();
    }
    rsUiDirector_->SetRSSurfaceNode(g_drawingInfo.surfaceNode);
    sptr<Rosen::Display> display = Rosen::DisplayManager::GetInstance().GetDisplayById(g_drawingInfo.displayId);
    if (display == nullptr) {
        FI_HILOGD("Get display info failed, display:%{public}d", g_drawingInfo.displayId);
        display = Rosen::DisplayManager::GetInstance().GetDisplayById(0);
        if (display == nullptr) {
            FI_HILOGE("Get display info failed, display is nullptr");
            return RET_ERR;
        }
    }
    int32_t rootNodeSize = std::max(display->GetWidth(), display->GetHeight());
    InitCanvas(rootNodeSize, rootNodeSize);
    if (rotation_ != Rosen::Rotation::ROTATION_0) {
        RotateDragWindow(rotation_);
    } else {
        DragWindowRotateInfo_.rotation = ROTATION_0;
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
    FI_HILOGI("leave");
    return RET_OK;
}

void DragDrawing::InitCanvas(int32_t width, int32_t height)
{
    FI_HILOGI("enter");
    if (g_drawingInfo.rootNode == nullptr) {
        g_drawingInfo.rootNode = Rosen::RSRootNode::Create();
        CHKPV(g_drawingInfo.rootNode);
    }
    g_drawingInfo.rootNode->SetBounds(0, 0, width, height);
    g_drawingInfo.rootNode->SetFrame(0, 0, width, height);
    g_drawingInfo.rootNode->SetBackgroundColor(SK_ColorTRANSPARENT);
    std::shared_ptr<Rosen::RSCanvasNode> filterNode = Rosen::RSCanvasNode::Create();
    CHKPV(filterNode);
    g_drawingInfo.nodes.emplace_back(filterNode);
    ProcessFilter();
    std::shared_ptr<Rosen::RSCanvasNode> pixelMapNode = Rosen::RSCanvasNode::Create();
    CHKPV(pixelMapNode);
    pixelMapNode->SetForegroundColor(TRANSPARENT_COLOR_ARGB);
    g_drawingInfo.nodes.emplace_back(pixelMapNode);
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = Rosen::RSCanvasNode::Create();
    CHKPV(dragStyleNode);
    g_drawingInfo.nodes.emplace_back(dragStyleNode);
    if (g_drawingInfo.parentNode == nullptr) {
        g_drawingInfo.parentNode = Rosen::RSCanvasNode::Create();
        CHKPV(g_drawingInfo.parentNode);
    }
    g_drawingInfo.parentNode->AddChild(filterNode);
    g_drawingInfo.parentNode->AddChild(pixelMapNode);
    if (!g_drawingInfo.multiSelectedPixelMaps.empty()) {
        InitMultiSelectedNodes();
        if (!g_drawingInfo.multiSelectedNodes.empty()) {
            size_t multiSelectedNodesSize = g_drawingInfo.multiSelectedNodes.size();
            for (size_t i = 0; i < multiSelectedNodesSize; ++i) {
                g_drawingInfo.rootNode->AddChild(g_drawingInfo.multiSelectedNodes[i]);
            }
        }
    }
    g_drawingInfo.rootNode->AddChild(g_drawingInfo.parentNode);
    CHKPV(rsUiDirector_);
    if (g_drawingInfo.sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        std::shared_ptr<Rosen::RSCanvasNode> mouseIconNode = Rosen::RSCanvasNode::Create();
        CHKPV(mouseIconNode);
        g_drawingInfo.nodes.emplace_back(mouseIconNode);
        g_drawingInfo.rootNode->AddChild(mouseIconNode);
        rsUiDirector_->SetRoot(g_drawingInfo.rootNode->GetId());
        return;
    }
    rsUiDirector_->SetRoot(g_drawingInfo.rootNode->GetId());
    FI_HILOGI("leave");
}

void DragDrawing::CreateWindow()
{
    FI_HILOGD("Parameter screen number:%{public}llu", static_cast<unsigned long long>(screenId_));
    Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "drag window";
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    g_drawingInfo.surfaceNode = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    CHKPV(g_drawingInfo.surfaceNode);
    sptr<Rosen::Display> display = Rosen::DisplayManager::GetInstance().GetDisplayById(g_drawingInfo.displayId);
    if (display == nullptr) {
        FI_HILOGD("Get display info failed, display:%{public}d", g_drawingInfo.displayId);
        display = Rosen::DisplayManager::GetInstance().GetDisplayById(0);
        if (display == nullptr) {
            FI_HILOGE("Get display info failed, display is nullptr");
            return;
        }
    }
    uint64_t rsScreenId = screenId_;
    sptr<Rosen::Screen> screen = Rosen::ScreenManager::GetInstance().GetScreenById(screenId_);
    if ((screen != nullptr) && (!screen->IsReal())) {
        if (!Rosen::DisplayManager::GetInstance().ConvertScreenIdToRsScreenId(screenId_, rsScreenId)) {
            FI_HILOGE("ConvertScreenIdToRsScreenId failed");
            return;
        }
    }
    int32_t surfaceNodeSize = std::max(display->GetWidth(), display->GetHeight());
    g_drawingInfo.surfaceNode->SetBounds(0, 0, surfaceNodeSize, surfaceNodeSize);
    g_drawingInfo.surfaceNode->SetFrameGravity(Rosen::Gravity::RESIZE_ASPECT_FILL);
    g_drawingInfo.surfaceNode->SetPositionZ(DRAG_WINDOW_POSITION_Z);
    g_drawingInfo.surfaceNode->SetBackgroundColor(SK_ColorTRANSPARENT);
    g_drawingInfo.surfaceNode->AttachToDisplay(rsScreenId);
    g_drawingInfo.surfaceNode->SetVisible(false);
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void DragDrawing::RemoveModifier()
{
    FI_HILOGD("enter");
    if ((g_drawingInfo.nodes.size() < TOUCH_NODE_MIN_COUNT)) {
        FI_HILOGE("Nodes size invalid, node size:%{public}zu", g_drawingInfo.nodes.size());
        return;
    }

    if (g_drawingInfo.nodes.size() <= PIXEL_MAP_INDEX || g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> pixelMapNode = g_drawingInfo.nodes[PIXEL_MAP_INDEX];
    CHKPV(pixelMapNode);
    if (drawPixelMapModifier_ != nullptr) {
        pixelMapNode->RemoveModifier(drawPixelMapModifier_);
        drawPixelMapModifier_ = nullptr;
    }
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPV(dragStyleNode);
    if (drawSVGModifier_ != nullptr) {
        dragStyleNode->RemoveModifier(drawSVGModifier_);
        drawSVGModifier_ = nullptr;
    }
    FI_HILOGD("leave");
}

int32_t DragDrawing::UpdateSvgNodeInfo(xmlNodePtr curNode, int32_t extendSvgWidth)
{
    FI_HILOGD("enter");
    if (xmlStrcmp(curNode->name, BAD_CAST "svg")) {
        FI_HILOGE("Svg format invalid");
        return RET_ERR;
    }
    std::ostringstream oStrStream;
    oStrStream << xmlGetProp(curNode, BAD_CAST "width");
    std::string srcSvgWidth = oStrStream.str();
    if (srcSvgWidth.length() < STRING_PX_LENGTH) {
        FI_HILOGE("Svg width invalid, srcSvgWidth:%{public}s", srcSvgWidth.c_str());
        return RET_ERR;
    }
    srcSvgWidth = srcSvgWidth.substr(0, srcSvgWidth.length() - STRING_PX_LENGTH);
    if (!IsNum(srcSvgWidth)) {
        FI_HILOGE("srcSvgWidth is not digital, srcSvgWidth:%{public}s", srcSvgWidth.c_str());
        return RET_ERR;
    }
    int32_t number = std::stoi(srcSvgWidth) + extendSvgWidth;
    std::string tgtSvgWidth = std::to_string(number);
    tgtSvgWidth.append("px");
    xmlSetProp(curNode, BAD_CAST "width", BAD_CAST tgtSvgWidth.c_str());
    oStrStream.str("");
    oStrStream << xmlGetProp(curNode, BAD_CAST "viewBox");
    std::string srcViewBox = oStrStream.str();
    std::istringstream iStrStream(srcViewBox);
    std::string tmpString;
    std::string tgtViewBox;
    int32_t i = 0;
    while (iStrStream >> tmpString) {
        if (i == VIEW_BOX_POS) {
            if (!IsNum(tmpString)) {
                FI_HILOGE("tmpString is not digital, tmpString:%{public}s", tmpString.c_str());
                return RET_ERR;
            }
            number = std::stoi(tmpString) + extendSvgWidth;
            tmpString = std::to_string(number);
        }
        tgtViewBox.append(tmpString);
        tgtViewBox += " ";
        ++i;
    }

    xmlSetProp(curNode, BAD_CAST "viewBox", BAD_CAST tgtViewBox.c_str());
    FI_HILOGD("leave");
    return RET_OK;
}

xmlNodePtr DragDrawing::GetRectNode(xmlNodePtr curNode)
{
    FI_HILOGD("enter");
    curNode = curNode->xmlChildrenNode;
    while (curNode != nullptr) {
        if (!xmlStrcmp(curNode->name, BAD_CAST "g")) {
            while (!xmlStrcmp(curNode->name, BAD_CAST "g")) {
                curNode = curNode->xmlChildrenNode;
            }
            break;
        }
        curNode = curNode->next;
    }
    FI_HILOGD("leave");
    return curNode;
}

xmlNodePtr DragDrawing::UpdateRectNode(int32_t extendSvgWidth, xmlNodePtr curNode)
{
    FI_HILOGD("enter");
    while (curNode != nullptr) {
        if (!xmlStrcmp(curNode->name, BAD_CAST "rect")) {
            std::ostringstream oStrStream;
            oStrStream << xmlGetProp(curNode, BAD_CAST "width");
            std::string srcRectWidth = oStrStream.str();
            if (!IsNum(srcRectWidth)) {
                FI_HILOGE("srcRectWidth is not digital, srcRectWidth:%{public}s", srcRectWidth.c_str());
                return nullptr;
            }
            int32_t number = std::stoi(srcRectWidth) + extendSvgWidth;
            xmlSetProp(curNode, BAD_CAST "width", BAD_CAST std::to_string(number).c_str());
        }
        if (!xmlStrcmp(curNode->name, BAD_CAST "text")) {
            return curNode->xmlChildrenNode;
        }
        curNode = curNode->next;
    }
    FI_HILOGE("Empty node of XML");
    return nullptr;
}

void DragDrawing::UpdateTspanNode(xmlNodePtr curNode)
{
    FI_HILOGD("enter");
    while (curNode != nullptr) {
        if (!xmlStrcmp(curNode->name, BAD_CAST "tspan")) {
            xmlNodeSetContent(curNode, BAD_CAST std::to_string(g_drawingInfo.currentDragNum).c_str());
        }
        curNode = curNode->next;
    }
    FI_HILOGD("leave");
}

int32_t DragDrawing::ParseAndAdjustSvgInfo(xmlNodePtr curNode)
{
    FI_HILOGD("enter");
    CHKPR(curNode, RET_ERR);
    std::string strStyle = std::to_string(g_drawingInfo.currentDragNum);
    if (strStyle.empty()) {
        FI_HILOGE("strStyle size:%{public}zu invalid", strStyle.size());
        return RET_ERR;
    }
    int32_t extendSvgWidth = (static_cast<int32_t>(strStyle.size()) - 1) * EIGHT_SIZE;
    xmlKeepBlanksDefault(0);
    int32_t ret = UpdateSvgNodeInfo(curNode, extendSvgWidth);
    if (ret != RET_OK) {
        FI_HILOGE("Update svg node info failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    curNode = GetRectNode(curNode);
    CHKPR(curNode, RET_ERR);
    curNode = UpdateRectNode(extendSvgWidth, curNode);
    CHKPR(curNode, RET_ERR);
    UpdateTspanNode(curNode);
    FI_HILOGD("leave");
    return RET_OK;
}

std::shared_ptr<Media::PixelMap> DragDrawing::DecodeSvgToPixelMap(
    const std::string &filePath)
{
    FI_HILOGD("enter");
    xmlDocPtr xmlDoc = xmlReadFile(filePath.c_str(), 0, XML_PARSE_NOBLANKS);
    if (NeedAdjustSvgInfo()) {
        xmlNodePtr node = xmlDocGetRootElement(xmlDoc);
        CHKPP(node);
        int32_t ret = ParseAndAdjustSvgInfo(node);
        if (ret != RET_OK) {
            FI_HILOGE("Parse and adjust svg info failed, ret:%{public}d", ret);
            return nullptr;
        }
    }
    xmlChar *xmlbuff = nullptr;
    int32_t buffersize = 0;
    xmlDocDumpFormatMemory(xmlDoc, &xmlbuff, &buffersize, 1);
    std::ostringstream oStrStream;
    oStrStream << xmlbuff;
    std::string content = oStrStream.str();
    xmlFree(xmlbuff);
    xmlFreeDoc(xmlDoc);
    Media::SourceOptions opts;
    opts.formatHint = "image/svg+xml";
    uint32_t errCode = 0;
    auto imageSource = Media::ImageSource::CreateImageSource(reinterpret_cast<const uint8_t*>(content.c_str()),
        content.size(), opts, errCode);
    CHKPP(imageSource);
    Media::DecodeOptions decodeOpts;
    SetDecodeOptions(decodeOpts);
    std::shared_ptr<Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errCode);
    FI_HILOGD("leave");
    return pixelMap;
}

bool DragDrawing::NeedAdjustSvgInfo()
{
    FI_HILOGD("enter");
    if (g_drawingInfo.currentStyle == DragCursorStyle::DEFAULT) {
        return false;
    }
    if ((g_drawingInfo.currentStyle == DragCursorStyle::COPY) &&
        (g_drawingInfo.currentDragNum == DRAG_NUM_ONE)) {
        return false;
    }
    if ((g_drawingInfo.currentStyle == DragCursorStyle::MOVE) &&
        (g_drawingInfo.currentDragNum == DRAG_NUM_ONE)) {
        return false;
    }
    if ((g_drawingInfo.currentStyle == DragCursorStyle::FORBIDDEN) &&
        (g_drawingInfo.currentDragNum == DRAG_NUM_ONE)) {
        return false;
    }
    FI_HILOGD("leave");
    return true;
}

int32_t DragDrawing::GetFilePath(std::string &filePath)
{
    FI_HILOGD("enter");
    switch (g_drawingInfo.currentStyle) {
        case DragCursorStyle::COPY: {
            if (g_drawingInfo.currentDragNum == DRAG_NUM_ONE) {
                filePath = COPY_ONE_DRAG_PATH;
            } else {
                filePath = COPY_DRAG_PATH;
            }
            break;
        }
        case DragCursorStyle::MOVE: {
            filePath = MOVE_DRAG_PATH;
            break;
        }
        case DragCursorStyle::FORBIDDEN: {
            if (g_drawingInfo.currentDragNum == DRAG_NUM_ONE) {
                filePath = FORBID_ONE_DRAG_PATH;
            } else {
                filePath = FORBID_DRAG_PATH;
            }
            break;
        }
        case DragCursorStyle::DEFAULT:
        default: {
            FI_HILOGW("Not need draw svg style, DragCursorStyle:%{public}d", g_drawingInfo.currentStyle);
            break;
        }
    }
    FI_HILOGD("leave");
    return RET_OK;
}

void DragDrawing::SetDecodeOptions(Media::DecodeOptions &decodeOpts)
{
    FI_HILOGD("enter");
    std::string strStyle = std::to_string(g_drawingInfo.currentDragNum);
    if (strStyle.empty()) {
        FI_HILOGE("strStyle size:%{public}zu invalid", strStyle.size());
        return;
    }
    int32_t extendSvgWidth = (static_cast<int32_t>(strStyle.size()) - 1) * EIGHT_SIZE;
    if ((g_drawingInfo.currentStyle == DragCursorStyle::COPY) && (g_drawingInfo.currentDragNum == DRAG_NUM_ONE)) {
        decodeOpts.desiredSize = {
            .width = DEVICE_INDEPENDENT_PIXEL * GetScaling(),
            .height = DEVICE_INDEPENDENT_PIXEL * GetScaling()
        };
    } else {
        decodeOpts.desiredSize = {
            .width = (DEVICE_INDEPENDENT_PIXEL + extendSvgWidth) * GetScaling(),
            .height = DEVICE_INDEPENDENT_PIXEL * GetScaling()
        };
    }
    FI_HILOGD("leave");
}


void DragDrawing::ParserDragShadowInfo(cJSON* filterInfoParser, FilterInfo &filterInfo)
{
    CHKPV(filterInfoParser);
    cJSON *offsetX = cJSON_GetObjectItemCaseSensitive(filterInfoParser, "drag_shadow_offsetX");
    if (cJSON_IsNumber(offsetX)) {
        filterInfo.offsetX = static_cast<float>(offsetX->valuedouble);
    }
    cJSON *offsetY = cJSON_GetObjectItemCaseSensitive(filterInfoParser, "drag_shadow_offsetY");
    if (cJSON_IsNumber(offsetY)) {
        filterInfo.offsetY = static_cast<float>(offsetY->valuedouble);
    }
    cJSON *argb = cJSON_GetObjectItemCaseSensitive(filterInfoParser, "drag_shadow_argb");
    if (cJSON_IsNumber(argb)) {
        filterInfo.argb = static_cast<uint32_t>(argb->valueint);
    }
    cJSON *shadowIsFilled   = cJSON_GetObjectItemCaseSensitive(filterInfoParser, "shadow_is_filled");
    if (cJSON_IsBool(shadowIsFilled)) {
        filterInfo.shadowIsFilled = cJSON_IsTrue(shadowIsFilled);
    }
    cJSON *shadowMask   = cJSON_GetObjectItemCaseSensitive(filterInfoParser, "shadow_mask");
    if (cJSON_IsBool(shadowMask)) {
        filterInfo.shadowMask = cJSON_IsTrue(shadowMask);
    }
    cJSON *shadowColorStrategy  = cJSON_GetObjectItemCaseSensitive(filterInfoParser, "shadow_color_strategy");
    if (cJSON_IsNumber(shadowColorStrategy)) {
        filterInfo.shadowColorStrategy = shadowColorStrategy->valueint;
    }
    cJSON *isHardwareAcceleration  = cJSON_GetObjectItemCaseSensitive(
        filterInfoParser, "shadow_is_hardwareacceleration");
    if (cJSON_IsBool(isHardwareAcceleration)) {
        filterInfo.isHardwareAcceleration = cJSON_IsTrue(isHardwareAcceleration);
    }
    if (filterInfo.isHardwareAcceleration) {
        cJSON *elevation  = cJSON_GetObjectItemCaseSensitive(filterInfoParser, "shadow_elevation");
        if (cJSON_IsNumber(elevation)) {
            filterInfo.elevation = static_cast<float>(elevation->valuedouble);
        }
    } else {
        cJSON *shadowCorner  = cJSON_GetObjectItemCaseSensitive(filterInfoParser, "shadow_corner");
        if (cJSON_IsNumber(shadowCorner)) {
            filterInfo.shadowCorner = static_cast<float>(shadowCorner->valuedouble);
        }
    }
}

void DragDrawing::ParserTextDragShadowInfo(cJSON* filterInfoParser, FilterInfo &filterInfo)
{
    CHKPV(filterInfoParser);
    cJSON *path = cJSON_GetObjectItemCaseSensitive(filterInfoParser, "drag_shadow_path");
    if (cJSON_IsString(path)) {
        float dragOriginDpi = DRAG_DATA_MGR.GetDragOriginDpi();
        if (dragOriginDpi > EPSILON) {
            filterInfo.path = "";
        } else {
            filterInfo.path = path->valuestring;
        }
    }
}

void DragDrawing::PrintDragShadowInfo()
{
    FilterInfo filterInfo = g_drawingInfo.filterInfo;
    if (!filterInfo.shadowEnable) {
        FI_HILOGI("Not supported shadow");
        return;
    }
    FI_HILOGI("dragType:%{public}s, shadowIsFilled:%{public}s, shadowMask:%{public}s, shadowColorStrategy :%{public}d, "
        "shadowCorner:%{public}f, offsetX:%{public}f, offsetY:%{public}f, argb:%{public}u, elevation:%{public}f, "
        "isHardwareAcceleration:%{public}s", filterInfo.dragType.c_str(),
        filterInfo.shadowIsFilled ? "true" : "false", filterInfo.shadowMask ? "true" : "false",
        filterInfo.shadowColorStrategy, filterInfo.shadowCorner, filterInfo.offsetX, filterInfo.offsetY,
        filterInfo.argb, filterInfo.elevation, filterInfo.isHardwareAcceleration ? "true" : "false");
    if (!filterInfo.path.empty()) {
        FI_HILOGI("path:%{public}s", filterInfo.path.c_str());
    }
}

bool DragDrawing::ParserFilterInfo(const std::string &filterInfoStr, FilterInfo &filterInfo)
{
    FI_HILOGD("FilterInfo size:%{public}zu, filterInfo:%{public}s", filterInfoStr.size(), filterInfoStr.c_str());
    if (filterInfoStr.empty()) {
        FI_HILOGD("FilterInfo is empty");
        return false;
    }
    JsonParser filterInfoParser;
    filterInfoParser.json = cJSON_Parse(filterInfoStr.c_str());
    if (!cJSON_IsObject(filterInfoParser.json)) {
        FI_HILOGE("FilterInfo is not json object");
        return false;
    }
    cJSON *dipScale = cJSON_GetObjectItemCaseSensitive(filterInfoParser.json, "dip_scale");
    if (cJSON_IsNumber(dipScale)) {
        filterInfo.dipScale = AdjustDoubleValue(dipScale->valuedouble);
    }
    cJSON *cornerRadius1 = cJSON_GetObjectItemCaseSensitive(filterInfoParser.json, "drag_corner_radius1");
    if (cJSON_IsNumber(cornerRadius1)) {
        filterInfo.cornerRadius1 = static_cast<float>(cornerRadius1->valuedouble);
    }
    cJSON *cornerRadius2 = cJSON_GetObjectItemCaseSensitive(filterInfoParser.json, "drag_corner_radius2");
    if (cJSON_IsNumber(cornerRadius2)) {
        filterInfo.cornerRadius2 = static_cast<float>(cornerRadius2->valuedouble);
    }
    cJSON *cornerRadius3 = cJSON_GetObjectItemCaseSensitive(filterInfoParser.json, "drag_corner_radius3");
    if (cJSON_IsNumber(cornerRadius3)) {
        filterInfo.cornerRadius3 = static_cast<float>(cornerRadius3->valuedouble);
    }
    cJSON *cornerRadius4 = cJSON_GetObjectItemCaseSensitive(filterInfoParser.json, "drag_corner_radius4");
    if (cJSON_IsNumber(cornerRadius4)) {
        filterInfo.cornerRadius4 = static_cast<float>(cornerRadius4->valuedouble);
    }
    cJSON *dragType = cJSON_GetObjectItemCaseSensitive(filterInfoParser.json, "drag_type");
    if (cJSON_IsString(dragType)) {
        filterInfo.dragType = dragType->valuestring;
    }
    cJSON *shadowEnable = cJSON_GetObjectItemCaseSensitive(filterInfoParser.json, "shadow_enable");
    if (cJSON_IsBool(shadowEnable)) {
        filterInfo.shadowEnable = cJSON_IsTrue(shadowEnable);
    }
    if (filterInfo.shadowEnable) {
        ParserDragShadowInfo(filterInfoParser.json, filterInfo);
        if (filterInfo.dragType == "text") {
            ParserTextDragShadowInfo(filterInfoParser.json, filterInfo);
        }
        PrintDragShadowInfo();
    }
    ParserBlurInfo(filterInfoParser.json, g_drawingInfo.filterInfo);
    return true;
}

void DragDrawing::ParserBlurInfo(const cJSON *BlurInfoInfoStr, FilterInfo &filterInfo)
{
    CHKPV(BlurInfoInfoStr);
    cJSON *opacity = cJSON_GetObjectItemCaseSensitive(BlurInfoInfoStr, "dip_opacity");
    if (cJSON_IsNumber(opacity)) {
        if ((opacity->valuedouble) > MAX_OPACITY || (opacity->valuedouble) <= MIN_OPACITY) {
            FI_HILOGE("Parser opacity limits abnormal, opacity:%{public}f", opacity->valuedouble);
        } else {
            filterInfo.opacity = static_cast<float>(opacity->valuedouble);
        }
    }
    float tempCoef1 = 0.0f;
    cJSON *coef1 = cJSON_GetObjectItemCaseSensitive(BlurInfoInfoStr, "blur_coef1");
    if (cJSON_IsNumber(coef1)) {
        tempCoef1 = static_cast<float>(coef1->valuedouble);
    }
    float tempCoef2 = 0.0f;
    cJSON *coef2 = cJSON_GetObjectItemCaseSensitive(BlurInfoInfoStr, "blur_coef2");
    if (cJSON_IsNumber(coef2)) {
        tempCoef2 = static_cast<float>(coef2->valuedouble);
    }
    filterInfo.coef = { tempCoef1, tempCoef2 };
    cJSON *blurRadius = cJSON_GetObjectItemCaseSensitive(BlurInfoInfoStr, "blur_radius");
    if (cJSON_IsNumber(blurRadius)) {
        filterInfo.blurRadius = AdjustDoubleValue(blurRadius->valuedouble);
    }
    cJSON *blurStaturation = cJSON_GetObjectItemCaseSensitive(BlurInfoInfoStr, "blur_staturation");
    if (cJSON_IsNumber(blurStaturation)) {
        filterInfo.blurStaturation = static_cast<float>(blurStaturation->valuedouble);
    }
    cJSON *blurBrightness = cJSON_GetObjectItemCaseSensitive(BlurInfoInfoStr, "blur_brightness");
    if (cJSON_IsNumber(blurBrightness)) {
        filterInfo.blurBrightness = static_cast<float>(blurBrightness->valuedouble);
    }
    cJSON *blurColor = cJSON_GetObjectItemCaseSensitive(BlurInfoInfoStr, "blur_color");
    if (cJSON_IsNumber(blurColor)) {
        filterInfo.blurColor = static_cast<uint32_t>(blurColor->valueint);
    }
    cJSON *blurStyle = cJSON_GetObjectItemCaseSensitive(BlurInfoInfoStr, "blur_style");
    if (cJSON_IsNumber(blurStyle)) {
        filterInfo.blurStyle = blurStyle->valueint;
    }
    return;
}

bool DragDrawing::ParserExtraInfo(const std::string &extraInfoStr, ExtraInfo &extraInfo)
{
    FI_HILOGD("ExtraInfo size:%{public}zu, extraInfo:%{public}s",
        extraInfoStr.size(), extraInfoStr.c_str());
    if (extraInfoStr.empty()) {
        FI_HILOGD("ExtraInfo is empty");
        return false;
    }
    JsonParser extraInfoParser;
    extraInfoParser.json = cJSON_Parse(extraInfoStr.c_str());
    if (!cJSON_IsObject(extraInfoParser.json)) {
        FI_HILOGE("ExtraInfo is not json object");
        return false;
    }
    cJSON *componentType = cJSON_GetObjectItemCaseSensitive(extraInfoParser.json, "drag_data_type");
    if (cJSON_IsString(componentType)) {
        extraInfo.componentType = componentType->valuestring;
    }
    cJSON *blurStyle = cJSON_GetObjectItemCaseSensitive(extraInfoParser.json, "drag_blur_style");
    if (cJSON_IsNumber(blurStyle)) {
        extraInfo.blurStyle = blurStyle->valueint;
    }
    cJSON *cornerRadius = cJSON_GetObjectItemCaseSensitive(extraInfoParser.json, "drag_corner_radius");
    if (cJSON_IsNumber(cornerRadius)) {
        extraInfo.cornerRadius = static_cast<float>(cornerRadius->valuedouble);
    }
    cJSON *allowDistributed = cJSON_GetObjectItemCaseSensitive(extraInfoParser.json, "drag_allow_distributed");
    if (cJSON_IsBool(allowDistributed)) {
        extraInfo.allowDistributed = cJSON_IsTrue(allowDistributed) ? true : false;
    }
    float tempCoef1 = 0.0f;
    cJSON *coef1 = cJSON_GetObjectItemCaseSensitive(extraInfoParser.json, "blur_coef1");
    if (cJSON_IsNumber(coef1)) {
        tempCoef1 = static_cast<float>(coef1->valuedouble);
    }
    float tempCoef2 = 0.0f;
    cJSON *coef2 = cJSON_GetObjectItemCaseSensitive(extraInfoParser.json, "blur_coef2");
    if (cJSON_IsNumber(coef2)) {
        tempCoef2 = static_cast<float>(coef2->valuedouble);
    }
    extraInfo.coef = { tempCoef1, tempCoef2 };
    return true;
}

bool DragDrawing::GetAllowDragState()
{
    return g_drawingInfo.extraInfo.allowDistributed;
}

void DragDrawing::SetScreenId(uint64_t screenId)
{
    FI_HILOGD("enter");
    screenId_ = screenId;
}

int32_t DragDrawing::RotateDragWindow(Rosen::Rotation rotation,
    const std::shared_ptr<Rosen::RSTransaction>& rsTransaction, bool isAnimated)
{
    if (needRotatePixelMapXY_) {
        CHKPR(g_drawingInfo.pixelMap, RET_ERR);
        g_drawingInfo.pixelMapX = -(HALF_RATIO * g_drawingInfo.pixelMap->GetWidth());
        g_drawingInfo.pixelMapY = -(EIGHT_SIZE * GetScaling());
    }
    float rotateAngle = (rotation == Rosen::Rotation::ROTATION_0) ? ROTATION_0 :
        ROTATION_360 - (ROTATION_90 * static_cast<int32_t>(rotation));
    FI_HILOGI("rotateAngle:%{public}f, isAnimated:%{public}d", rotateAngle, isAnimated);
    return DoRotateDragWindow(rotateAngle, rsTransaction, isAnimated);
}

void DragDrawing::RotateCanvasNode(float pivotX, float pivotY, float rotation)
{
    FI_HILOGD("enter");
    CHKPV(g_drawingInfo.parentNode);
    g_drawingInfo.parentNode->SetPivot(pivotX, pivotY);
    g_drawingInfo.parentNode->SetRotation(rotation);
    if (!g_drawingInfo.multiSelectedNodes.empty()) {
        size_t multiSelectedNodesSize = g_drawingInfo.multiSelectedNodes.size();
        for (size_t i = 0; i < multiSelectedNodesSize; ++i) {
            std::shared_ptr<Rosen::RSCanvasNode> multiSelectedNode = g_drawingInfo.multiSelectedNodes[i];
            CHKPV(multiSelectedNode);
            float degrees = DEFAULT_ANGLE;
            if (i == FIRST_PIXELMAP_INDEX) {
                degrees = rotation + POSITIVE_ANGLE;
            } else if (i == SECOND_PIXELMAP_INDEX) {
                degrees = rotation + NEGATIVE_ANGLE;
            }
            multiSelectedNode->SetPivot(pivotX, pivotY);
            multiSelectedNode->SetRotation(degrees);
        }
    }
    if (g_drawingInfo.sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        if (!CheckNodesValid()) {
            FI_HILOGE("Check nodes valid failed");
            return;
        }
        std::shared_ptr<Rosen::RSCanvasNode> mouseIconNode = g_drawingInfo.nodes[MOUSE_ICON_INDEX];
        CHKPV(mouseIconNode);
        mouseIconNode->SetPivot(DEFAULT_PIVOT, DEFAULT_PIVOT);
        mouseIconNode->SetRotation(rotation);
    }
    FI_HILOGD("leave");
}

void DragDrawing::SetRotation(Rosen::Rotation rotation)
{
    rotation_ = rotation;
}

void DragDrawing::ProcessFilter()
{
    FI_HILOGD("enter");
    if (g_drawingInfo.nodes.size() <= BACKGROUND_FILTER_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> filterNode = g_drawingInfo.nodes[BACKGROUND_FILTER_INDEX];
    CHKPV(filterNode);
    CHKPV(g_drawingInfo.pixelMap);
    FilterInfo filterInfo = g_drawingInfo.filterInfo;
    ExtraInfo extraInfo = g_drawingInfo.extraInfo;
    if (filterInfo.blurStyle != -1) {
        SetCustomDragBlur(filterInfo, filterNode);
    } else if (extraInfo.componentType == BIG_FOLDER_LABEL) {
        SetComponentDragBlur(filterInfo, extraInfo, filterNode);
    }
    FI_HILOGD("Add filter successfully");
    FI_HILOGD("leave");
}

void DragDrawing::SetCustomDragBlur(const FilterInfo &filterInfo, std::shared_ptr<Rosen::RSCanvasNode> filterNode)
{
    CHKPV(filterNode);
    CHKPV(g_drawingInfo.pixelMap);
    Rosen::BLUR_COLOR_MODE mode = (Rosen::BLUR_COLOR_MODE)filterInfo.blurStyle;
    std::shared_ptr<Rosen::RSFilter> backFilter = Rosen::RSFilter::CreateMaterialFilter(
        RadiusVp2Sigma(filterInfo.blurRadius, filterInfo.dipScale),
        filterInfo.blurStaturation, filterInfo.blurBrightness, filterInfo.blurColor, mode);
    if (backFilter == nullptr) {
        FI_HILOGE("Create backgroundFilter failed");
        return;
    }
    filterNode->SetBackgroundFilter(backFilter);
    filterNode->SetGreyCoef(filterInfo.coef);
    filterNode->SetAlpha(filterInfo.opacity);
    int32_t adjustSize = TWELVE_SIZE * GetScaling();
    filterNode->SetBounds(DEFAULT_POSITION_X, adjustSize, g_drawingInfo.pixelMap->GetWidth(),
        g_drawingInfo.pixelMap->GetHeight());
    filterNode->SetFrame(DEFAULT_POSITION_X, adjustSize, g_drawingInfo.pixelMap->GetWidth(),
        g_drawingInfo.pixelMap->GetHeight());
    if ((filterInfo.blurRadius < 0) || (filterInfo.dipScale < 0) ||
        (fabs(filterInfo.dipScale) < EPSILON) || ((std::numeric_limits<float>::max()
        / filterInfo.dipScale) < filterInfo.blurRadius)) {
        FI_HILOGE("Invalid parameters, cornerRadius:%{public}f, dipScale:%{public}f",
            filterInfo.blurRadius, filterInfo.dipScale);
        return;
    }
    Rosen::Vector4f cornerRadiusVector = { filterInfo.cornerRadius1, filterInfo.cornerRadius2,
        filterInfo.cornerRadius3, filterInfo.cornerRadius4 };
    filterNode->SetCornerRadius(cornerRadiusVector * filterInfo.dipScale);
    FI_HILOGD("Set custom drag blur successfully");
}

void DragDrawing::SetComponentDragBlur(const FilterInfo &filterInfo, const ExtraInfo &extraInfo,
    std::shared_ptr<Rosen::RSCanvasNode> filterNode)
{
    CHKPV(filterNode);
    CHKPV(g_drawingInfo.pixelMap);
    std::shared_ptr<Rosen::RSFilter> backFilter = Rosen::RSFilter::CreateMaterialFilter(
        RadiusVp2Sigma(RADIUS_VP, filterInfo.dipScale),
        DEFAULT_SATURATION, DEFAULT_BRIGHTNESS, DEFAULT_COLOR_VALUE);
    if (backFilter == nullptr) {
        FI_HILOGE("Create backgroundFilter failed");
        return;
    }
    filterNode->SetBackgroundFilter(backFilter);
    filterNode->SetGreyCoef(extraInfo.coef);
    filterNode->SetAlpha(filterInfo.opacity);
    int32_t adjustSize = TWELVE_SIZE * GetScaling();
    filterNode->SetBounds(DEFAULT_POSITION_X, adjustSize, g_drawingInfo.pixelMap->GetWidth(),
        g_drawingInfo.pixelMap->GetHeight());
    filterNode->SetFrame(DEFAULT_POSITION_X, adjustSize, g_drawingInfo.pixelMap->GetWidth(),
        g_drawingInfo.pixelMap->GetHeight());
    if ((extraInfo.cornerRadius < 0) || (filterInfo.dipScale < 0) ||
        (fabs(filterInfo.dipScale) < EPSILON) || ((std::numeric_limits<float>::max()
        / filterInfo.dipScale) < extraInfo.cornerRadius)) {
        FI_HILOGE("Invalid parameters, cornerRadius:%{public}f, dipScale:%{public}f",
            extraInfo.cornerRadius, filterInfo.dipScale);
        return;
    }
    filterNode->SetCornerRadius(extraInfo.cornerRadius * filterInfo.dipScale);
    FI_HILOGD("Set component drag blur successfully");
    return;
}

int32_t DragDrawing::SetNodesLocation(int32_t positionX, int32_t positionY)
{
    FI_HILOGD("enter");
    Rosen::RSAnimationTimingProtocol protocol;
    int32_t adjustSize = TWELVE_SIZE * GetScaling();
    CHKPR(g_drawingInfo.parentNode, RET_ERR);
    CHKPR(g_drawingInfo.pixelMap, RET_ERR);
    Rosen::RSNode::Animate(protocol, SPRING, [&]() {
        g_drawingInfo.parentNode->SetBounds(positionX, positionY, g_drawingInfo.pixelMap->GetWidth() + adjustSize,
            g_drawingInfo.pixelMap->GetHeight() + adjustSize);
        g_drawingInfo.parentNode->SetFrame(positionX, positionY, g_drawingInfo.pixelMap->GetWidth() + adjustSize,
            g_drawingInfo.pixelMap->GetHeight() + adjustSize);
    });
    g_drawingInfo.startNum = START_TIME;
    g_drawingInfo.needDestroyDragWindow = false;
    StartVsync();
    FI_HILOGD("leave");
    return RET_OK;
}


int32_t DragDrawing::EnterTextEditorArea(bool enable)
{
    FI_HILOGD("enter");
    if (enable) {
        DRAG_DATA_MGR.SetInitialPixelMapLocation({ g_drawingInfo.pixelMapX, g_drawingInfo.pixelMapY });
        needRotatePixelMapXY_ = true;
        RotatePixelMapXY(g_drawingInfo.pixelMapX, g_drawingInfo.pixelMapY);
    } else {
        needRotatePixelMapXY_ = false;
        auto initialPixelMapLocation = DRAG_DATA_MGR.GetInitialPixelMapLocation();
        g_drawingInfo.pixelMapX = initialPixelMapLocation.first;
        g_drawingInfo.pixelMapY = initialPixelMapLocation.second;
    }
    DRAG_DATA_MGR.SetPixelMapLocation({ g_drawingInfo.pixelMapX, g_drawingInfo.pixelMapY });
    int32_t positionX = g_drawingInfo.displayX + g_drawingInfo.pixelMapX;
    int32_t positionY = g_drawingInfo.displayY + g_drawingInfo.pixelMapY - TWELVE_SIZE * GetScaling();
    if (RunAnimation(std::bind(&DragDrawing::SetNodesLocation, this, positionX, positionY)) != RET_OK) {
        FI_HILOGE("RunAnimation to SetNodesLocation failed");
        return RET_ERR;
    }
    DRAG_DATA_MGR.SetTextEditorAreaFlag(enable);
    FI_HILOGI("EnterTextEditorArea %{public}s successfully", (enable ? "true" : "false"));
    return RET_OK;
}

float DragDrawing::RadiusVp2Sigma(float radiusVp, float dipScale)
{
    float radiusPx = radiusVp * dipScale;
    return radiusPx > 0.0f ? BLUR_SIGMA_SCALE * radiusPx + 0.5f : 0.0f;
}

int32_t DragDrawing::UpdatePreviewStyle(const PreviewStyle &previewStyle)
{
    FI_HILOGD("enter");
    if (g_drawingInfo.nodes.size() <= PIXEL_MAP_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return RET_ERR;
    } else if (ModifyPreviewStyle(g_drawingInfo.nodes[PIXEL_MAP_INDEX], previewStyle) != RET_OK) {
        FI_HILOGE("ModifyPreviewStyle failed");
        return RET_ERR;
    }
    if (ModifyMultiPreviewStyle(std::vector<PreviewStyle>(g_drawingInfo.multiSelectedNodes.size(), previewStyle)) !=
        RET_OK) {
        FI_HILOGE("ModifyPreviewStyle failed");
        return RET_ERR;
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
    FI_HILOGD("leave");
    return RET_OK;
}

int32_t DragDrawing::UpdatePreviewStyleWithAnimation(const PreviewStyle &previewStyle,
    const PreviewAnimation &animation)
{
    FI_HILOGD("enter");
    std::shared_ptr<Rosen::RSCanvasNode> pixelMapNode = g_drawingInfo.nodes[PIXEL_MAP_INDEX];
    CHKPR(pixelMapNode, RET_ERR);
    PreviewStyle originStyle;
    originStyle.types = previewStyle.types;
    if (auto color = pixelMapNode->GetShowingProperties().GetForegroundColor(); color.has_value()) {
        originStyle.foregroundColor = color->AsArgbInt();
        originStyle.radius = previewStyle.radius;
    }
    size_t multiSelectedNodesSize = g_drawingInfo.multiSelectedNodes.size();
    std::vector<PreviewStyle> multiOriginStyles;
    for (size_t i = 0; i < multiSelectedNodesSize; ++i) {
        if (auto color = g_drawingInfo.multiSelectedNodes[i]->GetShowingProperties().GetForegroundColor();
            color.has_value()) {
            PreviewStyle currentStyle;
            currentStyle.types = { PreviewType::FOREGROUND_COLOR, PreviewType::RADIUS };
            currentStyle.foregroundColor = color->AsArgbInt();
            currentStyle.radius = previewStyle.radius;
            multiOriginStyles.push_back(currentStyle);
        }
    }
    if (ModifyPreviewStyle(pixelMapNode, originStyle) != RET_OK) {
        FI_HILOGE("ModifyPreviewStyle failed");
        return RET_ERR;
    }
    if (ModifyMultiPreviewStyle(multiOriginStyles) != RET_OK) {
        FI_HILOGE("ModifyMultiPreviewStyle failed");
        return RET_ERR;
    }
    Rosen::RSAnimationTimingProtocol protocol;
    protocol.SetDuration(animation.duration);
    auto curve = AnimationCurve::CreateCurve(animation.curveName, animation.curve);
    Rosen::RSNode::Animate(protocol, curve, [&]() {
        if (ModifyPreviewStyle(pixelMapNode, previewStyle) != RET_OK) {
            FI_HILOGE("ModifyPreviewStyle failed");
        }
        if (ModifyMultiPreviewStyle(std::vector<PreviewStyle>(multiSelectedNodesSize, previewStyle)) != RET_OK) {
            FI_HILOGE("ModifyMultiPreviewStyle failed");
        }
    });
    FI_HILOGD("leave");
    return RET_OK;
}

void DragDrawing::UpdateMousePosition()
{
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return;
    }
    if (g_drawingInfo.nodes.size() <= MOUSE_ICON_INDEX) {
        FI_HILOGE("The index out of bounds, node size:%{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> mouseIconNode = g_drawingInfo.nodes[MOUSE_ICON_INDEX];
    CHKPV(mouseIconNode);
    if (pointerStyle_.id == MOUSE_DRAG_CURSOR_CIRCLE_STYLE || pointerStyle_.options == MAGIC_STYLE_OPT) {
        float positionX = g_drawingInfo.x - (static_cast<float>(g_drawingInfo.mouseWidth) / CURSOR_CIRCLE_MIDDLE);
        float positionY = g_drawingInfo.y - (static_cast<float>(g_drawingInfo.mouseHeight) / CURSOR_CIRCLE_MIDDLE);
        mouseIconNode->SetBounds(positionX, positionY, g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
        mouseIconNode->SetFrame(positionX, positionY, g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
    } else {
        mouseIconNode->SetBounds(g_drawingInfo.x, g_drawingInfo.y,
            g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
        mouseIconNode->SetFrame(g_drawingInfo.x, g_drawingInfo.y,
            g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
    }
}

int32_t DragDrawing::RotateDragWindowAsync(Rosen::Rotation rotation)
{
    CHKPR(context_, RET_ERR);
    isRunningRotateAnimation_ = true;
    int32_t repeatTime = 1;
    timerId_ = context_->GetTimerManager().AddTimer(ASYNC_ROTATE_TIME, repeatTime, [this]() {
        RotateDragWindow(rotation_, nullptr, true);
        isRunningRotateAnimation_ = false;
    });
    if (timerId_ < 0) {
        FI_HILOGE("Add timer failed, timerId_:%{public}d", timerId_);
        isRunningRotateAnimation_ = false;
        return RET_ERR;
    }
    return RET_OK;
}

int32_t DragDrawing::RotateDragWindowSync(const std::shared_ptr<Rosen::RSTransaction>& rsTransaction)
{
    FI_HILOGD("enter");
    isRunningRotateAnimation_ = true;
    RotateDragWindow(rotation_, rsTransaction, true);
    isRunningRotateAnimation_ = false;
    if ((context_ != nullptr) && (timerId_ >= 0)) {
        context_->GetTimerManager().RemoveTimer(timerId_);
        timerId_ = -1;
    }
    return RET_OK;
}

void DragDrawing::DoDrawMouse()
{
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return;
    }
    if (g_drawingInfo.nodes.size() <= MOUSE_ICON_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> mouseIconNode = g_drawingInfo.nodes[MOUSE_ICON_INDEX];
    CHKPV(mouseIconNode);
    if (pointerStyle_.id == MOUSE_DRAG_CURSOR_CIRCLE_STYLE || pointerStyle_.options == MAGIC_STYLE_OPT) {
        int32_t positionX = g_drawingInfo.displayX - (g_drawingInfo.mouseWidth / CURSOR_CIRCLE_MIDDLE);
        int32_t positionY = g_drawingInfo.displayY - (g_drawingInfo.mouseHeight / CURSOR_CIRCLE_MIDDLE);
        mouseIconNode->SetBounds(positionX, positionY, g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
        mouseIconNode->SetFrame(positionX, positionY, g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
    } else {
        mouseIconNode->SetBounds(g_drawingInfo.displayX, g_drawingInfo.displayY,
            g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
        mouseIconNode->SetFrame(g_drawingInfo.displayX, g_drawingInfo.displayY,
            g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
    }
}

int32_t DragDrawing::UpdateDefaultDragStyle(DragCursorStyle style)
{
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return RET_ERR;
    }
    if (g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return RET_ERR;
    }
    if (!g_drawingInfo.isCurrentDefaultStyle) {
        std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
        CHKPR(dragStyleNode, RET_ERR);
        CHKPR(g_drawingInfo.parentNode, RET_ERR);
        g_drawingInfo.parentNode->RemoveChild(dragStyleNode);
        CHKPR(rsUiDirector_, RET_ERR);
        rsUiDirector_->SendMessages();
    }
    g_drawingInfo.currentStyle = style;
    bool isPreviousDefaultStyle = g_drawingInfo.isCurrentDefaultStyle;
    g_drawingInfo.isPreviousDefaultStyle = isPreviousDefaultStyle;
    g_drawingInfo.isCurrentDefaultStyle = true;
    return RET_OK;
}

int32_t DragDrawing::UpdateValidDragStyle(DragCursorStyle style)
{
    g_drawingInfo.currentStyle = style;
    if (g_drawingInfo.isCurrentDefaultStyle) {
        if (!CheckNodesValid()) {
            FI_HILOGE("Check nodes valid failed");
            return RET_ERR;
        }
        if (g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX) {
            FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
            return RET_ERR;
        }
        std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
        CHKPR(dragStyleNode, RET_ERR);
        CHKPR(g_drawingInfo.parentNode, RET_ERR);
        g_drawingInfo.parentNode->AddChild(dragStyleNode);
    }
    std::string filePath;
    if (GetFilePath(filePath) != RET_OK) {
        FI_HILOGD("Get file path failed");
        return RET_ERR;
    }
    if (!IsValidSvgFile(filePath)) {
        FI_HILOGE("Svg file is invalid");
        return RET_ERR;
    }
    std::shared_ptr<Media::PixelMap> pixelMap = DecodeSvgToPixelMap(filePath);
    CHKPR(pixelMap, RET_ERR);
    bool isPreviousDefaultStyle = g_drawingInfo.isCurrentDefaultStyle;
    g_drawingInfo.isPreviousDefaultStyle = isPreviousDefaultStyle;
    g_drawingInfo.isCurrentDefaultStyle = false;
    g_drawingInfo.stylePixelMap = pixelMap;
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return RET_ERR;
    }
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPR(dragStyleNode, RET_ERR);
    OnDragStyle(dragStyleNode, pixelMap);
    CHKPR(rsUiDirector_, RET_ERR);
    rsUiDirector_->SendMessages();
    DragDFX::WriteUpdateDragStyle(style, OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR);
    return RET_OK;
}

int32_t DragDrawing::ModifyPreviewStyle(std::shared_ptr<Rosen::RSCanvasNode> node, const PreviewStyle &previewStyle)
{
    FI_HILOGD("enter");
    CHKPR(node, RET_ERR);
    if (float radius = 0.0F; ParserRadius(radius)) {
        node->SetCornerRadius(radius);
        FI_HILOGD("SetCornerRadius by radius:%{public}f", radius);
    }
    for (const auto &type : previewStyle.types) {
        switch (type) {
            case PreviewType::FOREGROUND_COLOR: {
                node->SetForegroundColor(previewStyle.foregroundColor);
                break;
            }
            case PreviewType::OPACITY: {
                node->SetAlpha(previewStyle.opacity / static_cast<float>(HEX_FF));
                break;
            }
            case PreviewType::RADIUS: {
                node->SetCornerRadius(previewStyle.radius);
                break;
            }
            case PreviewType::SCALE: {
                node->SetScale(previewStyle.scale);
                break;
            }
            default: {
                FI_HILOGE("Unsupported type");
                break;
            }
        }
    }
    FI_HILOGD("leave");
    return RET_OK;
}

int32_t DragDrawing::ModifyMultiPreviewStyle(const std::vector<PreviewStyle> &previewStyles)
{
    size_t multiSelectedNodesSize = g_drawingInfo.multiSelectedNodes.size();
    if (previewStyles.size() != multiSelectedNodesSize) {
        FI_HILOGE("Size of previewStyles:%{public}zu does not match multiSelectedNodesSize:%{public}zu",
            previewStyles.size(), multiSelectedNodesSize);
        return RET_ERR;
    }
    for (size_t i = 0; i < multiSelectedNodesSize; ++i) {
        if (ModifyPreviewStyle(g_drawingInfo.multiSelectedNodes[i], previewStyles[i]) != RET_OK) {
            FI_HILOGW("ModifyPreviewStyle No.%{public}zu failed", i);
        }
    }
    return RET_OK;
}

void DragDrawing::MultiSelectedAnimation(int32_t positionX, int32_t positionY, int32_t adjustSize)
{
    size_t multiSelectedNodesSize = g_drawingInfo.multiSelectedNodes.size();
    size_t multiSelectedPixelMapsSize = g_drawingInfo.multiSelectedPixelMaps.size();
    for (size_t i = 0; (i < multiSelectedNodesSize) && (i < multiSelectedPixelMapsSize); ++i) {
        std::shared_ptr<Rosen::RSCanvasNode> multiSelectedNode = g_drawingInfo.multiSelectedNodes[i];
        std::shared_ptr<Media::PixelMap> multiSelectedPixelMap = g_drawingInfo.multiSelectedPixelMaps[i];
        Rosen::RSAnimationTimingProtocol protocol;
        if (i == FIRST_PIXELMAP_INDEX) {
            protocol.SetDuration(SHORT_DURATION);
        } else {
            protocol.SetDuration(LONG_DURATION);
        }
        CHKPV(g_drawingInfo.pixelMap);
        CHKPV(multiSelectedNode);
        CHKPV(multiSelectedPixelMap);
        int32_t multiSelectedPositionX = positionX + (g_drawingInfo.pixelMap->GetWidth() / TWICE_SIZE) -
            (multiSelectedPixelMap->GetWidth() / TWICE_SIZE);
        int32_t multiSelectedPositionY = positionY + (g_drawingInfo.pixelMap->GetHeight() / TWICE_SIZE) -
            (multiSelectedPixelMap->GetHeight() / TWICE_SIZE);
        Rosen::RSNode::Animate(protocol, Rosen::RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
            multiSelectedNode->SetBounds(multiSelectedPositionX, multiSelectedPositionY + adjustSize,
                multiSelectedPixelMap->GetWidth(), multiSelectedPixelMap->GetHeight());
            multiSelectedNode->SetFrame(multiSelectedPositionX, multiSelectedPositionY + adjustSize,
                multiSelectedPixelMap->GetWidth(), multiSelectedPixelMap->GetHeight());
        });
    }
}

void DragDrawing::InitMultiSelectedNodes()
{
    FI_HILOGD("enter");
    size_t multiSelectedPixelMapsSize = g_drawingInfo.multiSelectedPixelMaps.size();
    for (size_t i = 0; i < multiSelectedPixelMapsSize; ++i) {
        std::shared_ptr<Media::PixelMap> multiSelectedPixelMap = g_drawingInfo.multiSelectedPixelMaps[i];
        std::shared_ptr<Rosen::RSCanvasNode> multiSelectedNode = Rosen::RSCanvasNode::Create();
        multiSelectedNode->SetBgImageWidth(multiSelectedPixelMap->GetWidth());
        multiSelectedNode->SetBgImageHeight(multiSelectedPixelMap->GetHeight());
        multiSelectedNode->SetBgImagePositionX(0);
        multiSelectedNode->SetBgImagePositionY(0);
        multiSelectedNode->SetForegroundColor(TRANSPARENT_COLOR_ARGB);
        auto rosenImage = std::make_shared<Rosen::RSImage>();
        rosenImage->SetPixelMap(multiSelectedPixelMap);
        rosenImage->SetImageRepeat(0);
        multiSelectedNode->SetBgImage(rosenImage);
        float alpha = DEFAULT_ALPHA;
        float degrees = DEFAULT_ANGLE;
        if (i == FIRST_PIXELMAP_INDEX) {
            alpha = FIRST_PIXELMAP_ALPHA;
            degrees = POSITIVE_ANGLE;
        } else if (i == SECOND_PIXELMAP_INDEX) {
            alpha = SECOND_PIXELMAP_ALPHA;
            degrees = NEGATIVE_ANGLE;
        }
        multiSelectedNode->SetRotation(degrees);
        multiSelectedNode->SetCornerRadius(g_drawingInfo.filterInfo.cornerRadius1 * g_drawingInfo.filterInfo.dipScale);
        multiSelectedNode->SetAlpha(alpha);
        g_drawingInfo.multiSelectedNodes.emplace_back(multiSelectedNode);
    }
    FI_HILOGD("leave");
}

void DragDrawing::ClearMultiSelectedData()
{
    FI_HILOGD("enter");
    if (!g_drawingInfo.multiSelectedNodes.empty()) {
        g_drawingInfo.multiSelectedNodes.clear();
        g_drawingInfo.multiSelectedNodes.shrink_to_fit();
    }
    if (!g_drawingInfo.multiSelectedPixelMaps.empty()) {
        g_drawingInfo.multiSelectedPixelMaps.clear();
        g_drawingInfo.multiSelectedPixelMaps.shrink_to_fit();
    }
    FI_HILOGD("leave");
}

void DragDrawing::RotateDisplayXY(int32_t &displayX, int32_t &displayY)
{
    sptr<Rosen::Display> display = Rosen::DisplayManager::GetInstance().GetDisplayById(g_drawingInfo.displayId);
    if (display == nullptr) {
        FI_HILOGD("Get display info failed, display:%{public}d", g_drawingInfo.displayId);
        display = Rosen::DisplayManager::GetInstance().GetDisplayById(0);
        CHKPV(display);
    }
    switch (rotation_) {
        case Rosen::Rotation::ROTATION_0: {
            break;
        }
        case Rosen::Rotation::ROTATION_90: {
            int32_t temp = displayY;
            displayY = display->GetWidth() - displayX;
            displayX = temp;
            break;
        }
        case Rosen::Rotation::ROTATION_180: {
            displayX = display->GetWidth() - displayX;
            displayY = display->GetHeight() - displayY;
            break;
        }
        case Rosen::Rotation::ROTATION_270: {
            int32_t temp = displayX;
            displayX = display->GetHeight() - displayY;
            displayY = temp;
            break;
        }
        default: {
            FI_HILOGW("Unknown parameter, rotation:%{public}d", static_cast<int32_t>(rotation_));
            break;
        }
    }
}

void DragDrawing::RotatePosition(float &displayX, float &displayY)
{
    sptr<Rosen::Display> display = Rosen::DisplayManager::GetInstance().GetDisplayById(g_drawingInfo.displayId);
    if (display == nullptr) {
        FI_HILOGD("Get display info failed, display:%{public}d", g_drawingInfo.displayId);
        display = Rosen::DisplayManager::GetInstance().GetDisplayById(0);
        CHKPV(display);
    }
    switch (rotation_) {
        case Rosen::Rotation::ROTATION_0: {
            break;
        }
        case Rosen::Rotation::ROTATION_90: {
            int32_t temp = displayY;
            displayY = display->GetWidth() - displayX;
            displayX = temp;
            break;
        }
        case Rosen::Rotation::ROTATION_180: {
            displayX = display->GetWidth() - displayX;
            displayY = display->GetHeight() - displayY;
            break;
        }
        case Rosen::Rotation::ROTATION_270: {
            int32_t temp = displayX;
            displayX = display->GetHeight() - displayY;
            displayY = temp;
            break;
        }
        default: {
            FI_HILOGE("Invalid parameter, rotation:%{public}d", static_cast<int32_t>(rotation_));
            break;
        }
    }
}

void DragDrawing::RotatePixelMapXY(int32_t &pixelMapX, int32_t &pixelMapY)
{
    FI_HILOGI("rotation:%{public}d", static_cast<int32_t>(rotation_));
    CHKPV(g_drawingInfo.pixelMap);
    switch (rotation_) {
        case Rosen::Rotation::ROTATION_0: {
            g_drawingInfo.pixelMapX = -(HALF_RATIO * g_drawingInfo.pixelMap->GetWidth());
            g_drawingInfo.pixelMapY = -(EIGHT_SIZE * GetScaling());
            break;
        }
        case Rosen::Rotation::ROTATION_90: {
            g_drawingInfo.pixelMapX = EIGHT_SIZE * GetScaling() + g_drawingInfo.lastPixelMapX;
            g_drawingInfo.pixelMapY = HALF_RATIO * g_drawingInfo.pixelMap->GetWidth() + g_drawingInfo.lastPixelMapX;
            break;
        }
        case Rosen::Rotation::ROTATION_180: {
            g_drawingInfo.pixelMapX = HALF_RATIO * g_drawingInfo.pixelMap->GetWidth() +
                TWICE_SIZE * g_drawingInfo.lastPixelMapX;
            g_drawingInfo.pixelMapY = -(EIGHT_SIZE * GetScaling());
            break;
        }
        case Rosen::Rotation::ROTATION_270: {
            g_drawingInfo.pixelMapX = -EIGHT_SIZE * GetScaling() + g_drawingInfo.lastPixelMapX;
            g_drawingInfo.pixelMapY = -HALF_RATIO * g_drawingInfo.pixelMap->GetWidth() - g_drawingInfo.lastPixelMapX;
            break;
        }
        default: {
            FI_HILOGE("Invalid parameter, rotation:%{public}d", static_cast<int32_t>(rotation_));
            break;
        }
    }
}

void DragDrawing::ResetAnimationParameter()
{
    FI_HILOGI("enter");
    CHKPV(handler_);
    handler_->RemoveAllEvents();
    handler_->RemoveAllFileDescriptorListeners();
    handler_ = nullptr;
    CHKPV(receiver_);
    receiver_ = nullptr;
    ResetSuperHubHandler();
    FI_HILOGI("leave");
}

void DragDrawing::ResetAnimationFlag(bool isForce)
{
    FI_HILOGI("enter");
    if (!isForce && (g_drawingInfo.context != nullptr) && (g_drawingInfo.timerId >= 0)) {
        g_drawingInfo.context->GetTimerManager().RemoveTimer(g_drawingInfo.timerId);
        g_drawingInfo.timerId = -1;
    }
    if (drawDynamicEffectModifier_ != nullptr) {
        CHKPV(g_drawingInfo.rootNode);
        g_drawingInfo.rootNode->RemoveModifier(drawDynamicEffectModifier_);
        drawDynamicEffectModifier_ = nullptr;
    }
    DestroyDragWindow();
    g_drawingInfo.isRunning = false;
    g_drawingInfo.timerId = -1;
    ResetAnimationParameter();
    FI_HILOGI("leave");
}

void DragDrawing::DoEndAnimation()
{
    FI_HILOGI("enter");
    g_drawingInfo.startNum = START_TIME;
    g_drawingInfo.needDestroyDragWindow = true;
    if (g_drawingInfo.context != nullptr) {
        int32_t repeatCount = 1;
        g_drawingInfo.timerId = g_drawingInfo.context->GetTimerManager().AddTimer(TIMEOUT_MS, repeatCount, [this]() {
            FI_HILOGW("Timeout, automatically reset animation flag");
            ResetAnimationFlag(true);
        });
    }
    StartVsync();
    FI_HILOGI("leave");
}

void DragDrawing::ResetParameter()
{
    FI_HILOGI("enter");
    g_drawingInfo.startNum = START_TIME;
    g_drawingInfo.needDestroyDragWindow = false;
    needRotatePixelMapXY_ = false;
    hasRunningStopAnimation_ = false;
    pointerStyle_ = {};
    g_drawingInfo.sourceType = -1;
    g_drawingInfo.currentDragNum = -1;
    g_drawingInfo.pixelMapX = -1;
    g_drawingInfo.pixelMapY = -1;
    g_drawingInfo.lastPixelMapX = -1;
    g_drawingInfo.lastPixelMapY = -1;
    g_drawingInfo.displayX = -1;
    g_drawingInfo.displayY = -1;
    g_drawingInfo.mouseWidth = 0;
    g_drawingInfo.mouseHeight = 0;
    g_drawingInfo.rootNodeWidth = -1;
    g_drawingInfo.rootNodeHeight = -1;
    g_drawingInfo.pixelMap = nullptr;
    g_drawingInfo.stylePixelMap = nullptr;
    g_drawingInfo.isPreviousDefaultStyle = false;
    g_drawingInfo.isCurrentDefaultStyle = false;
    g_drawingInfo.currentStyle = DragCursorStyle::DEFAULT;
    g_drawingInfo.filterInfo = {};
    g_drawingInfo.extraInfo = {};
    dragSmoothProcessor_.ResetParameters();
    vSyncStation_.StopVSyncRequest();
    frameCallback_ = nullptr;
    FI_HILOGI("leave");
}

int32_t DragDrawing::DoRotateDragWindow(float rotation,
    const std::shared_ptr<Rosen::RSTransaction>& rsTransaction, bool isAnimated)
{
    FI_HILOGD("rotation:%{public}f, isAnimated:%{public}d", rotation, isAnimated);
    CHKPR(g_drawingInfo.pixelMap, RET_ERR);
    if ((g_drawingInfo.pixelMap->GetWidth() <= 0) || (g_drawingInfo.pixelMap->GetHeight() <= 0)) {
        FI_HILOGE("Invalid parameter pixelmap");
        return RET_ERR;
    }
    float pivotX = 0.0f;
    float pivotY = 0.0f;
    int32_t adjustSize = TWELVE_SIZE * GetScaling();
    if ((g_drawingInfo.currentStyle == DragCursorStyle::DEFAULT) ||
        ((g_drawingInfo.currentStyle == DragCursorStyle::MOVE) && (g_drawingInfo.currentDragNum == DRAG_NUM_ONE))) {
        pivotX = -g_drawingInfo.pixelMapX * 1.0 / g_drawingInfo.pixelMap->GetWidth();
        pivotY = (-g_drawingInfo.pixelMapY + adjustSize) * 1.0 / g_drawingInfo.pixelMap->GetHeight();
    } else {
        pivotX = -g_drawingInfo.pixelMapX * 1.0 / (g_drawingInfo.pixelMap->GetWidth() + adjustSize);
        pivotY = (-g_drawingInfo.pixelMapY + adjustSize) * 1.0 / (g_drawingInfo.pixelMap->GetHeight() + adjustSize);
    }
    if (!isAnimated) {
        DragWindowRotateInfo_.rotation = rotation;
        DragWindowRotateInfo_.pivotX = pivotX;
        DragWindowRotateInfo_.pivotY = pivotY;
        RotateCanvasNode(pivotX, pivotY, rotation);
        Rosen::RSTransaction::FlushImplicitTransaction();
        return RET_OK;
    }
    return DoRotateDragWindowAnimation(rotation, pivotX, pivotY, rsTransaction);
}

int32_t DragDrawing::DoRotateDragWindowAnimation(float rotation, float pivotX, float pivotY,
    const std::shared_ptr<Rosen::RSTransaction>& rsTransaction)
{
    FI_HILOGD("enter");
    if (rsTransaction != nullptr) {
        Rosen::RSTransaction::FlushImplicitTransaction();
        rsTransaction->Begin();
    }
    if ((rotation == ROTATION_0) && (DragWindowRotateInfo_.rotation == ROTATION_270)) {
        RotateCanvasNode(DragWindowRotateInfo_.pivotX, DragWindowRotateInfo_.pivotY, -ROTATION_90);
    } else if ((rotation == ROTATION_270) && (DragWindowRotateInfo_.rotation == ROTATION_0)) {
        RotateCanvasNode(DragWindowRotateInfo_.pivotX, DragWindowRotateInfo_.pivotY, ROTATION_360);
    }

    Rosen::RSAnimationTimingProtocol protocol;
    protocol.SetDuration(ANIMATION_DURATION);
    Rosen::RSNode::Animate(protocol, SPRING, [&]() {
        RotateCanvasNode(pivotX, pivotY, rotation);
        DragWindowRotateInfo_.rotation = rotation;
        DragWindowRotateInfo_.pivotX = pivotX;
        DragWindowRotateInfo_.pivotY = pivotY;
        return RET_OK;
    });
    if (rsTransaction != nullptr) {
        rsTransaction->Commit();
    } else {
        Rosen::RSTransaction::FlushImplicitTransaction();
    }
    FI_HILOGD("leave");
    return RET_OK;
}

bool DragDrawing::ParserRadius(float &radius)
{
    FilterInfo filterInfo = g_drawingInfo.filterInfo;
    ExtraInfo extraInfo = g_drawingInfo.extraInfo;
    if ((extraInfo.cornerRadius < 0) || (filterInfo.dipScale < 0) ||
        (fabs(filterInfo.dipScale) < EPSILON) || ((std::numeric_limits<float>::max()
        / filterInfo.dipScale) < extraInfo.cornerRadius)) {
        FI_HILOGE("Invalid parameters, cornerRadius:%{public}f, dipScale:%{public}f",
            extraInfo.cornerRadius, filterInfo.dipScale);
        return false;
    }
    radius = extraInfo.cornerRadius * filterInfo.dipScale;
    return true;
}

DragDrawing::~DragDrawing()
{
    if (dragExtHandler_ != nullptr) {
        dlclose(dragExtHandler_);
        dragExtHandler_ = nullptr;
    }
}

void DrawSVGModifier::Draw(Rosen::RSDrawingContext& context) const
{
    FI_HILOGD("enter");
    CHKPV(stylePixelMap_);
    CHKPV(g_drawingInfo.pixelMap);
    float scalingValue = GetScaling();
    if (SCALE_THRESHOLD_EIGHT < scalingValue || fabsf(SCALE_THRESHOLD_EIGHT - scalingValue) < EPSILON) {
        FI_HILOGE("Invalid scalingValue:%{public}f", scalingValue);
        return;
    }
    int32_t adjustSize = EIGHT_SIZE * scalingValue;
    int32_t svgTouchPositionX = g_drawingInfo.pixelMap->GetWidth() + adjustSize - stylePixelMap_->GetWidth();
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPV(dragStyleNode);
    adjustSize = (TWELVE_SIZE - EIGHT_SIZE) * scalingValue;
    dragStyleNode->SetBounds(svgTouchPositionX, adjustSize, stylePixelMap_->GetWidth() + adjustSize,
        stylePixelMap_->GetHeight());
    dragStyleNode->SetFrame(svgTouchPositionX, adjustSize, stylePixelMap_->GetWidth() + adjustSize,
        stylePixelMap_->GetHeight());
    dragStyleNode->SetBgImageWidth(stylePixelMap_->GetWidth());
    dragStyleNode->SetBgImageHeight(stylePixelMap_->GetHeight());
    dragStyleNode->SetBgImagePositionX(0);
    dragStyleNode->SetBgImagePositionY(0);
    auto rosenImage = std::make_shared<Rosen::RSImage>();
    rosenImage->SetPixelMap(stylePixelMap_);
    rosenImage->SetImageRepeat(0);
    dragStyleNode->SetBgImage(rosenImage);
    Rosen::RSTransaction::FlushImplicitTransaction();
    FI_HILOGD("leave");
}

Rosen::SHADOW_COLOR_STRATEGY DrawPixelMapModifier::ConvertShadowColorStrategy(int32_t shadowColorStrategy) const
{
    if (shadowColorStrategy == static_cast<int32_t>(Rosen::SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE)) {
        return Rosen::SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE ;
    } else if (shadowColorStrategy == static_cast<int32_t>(Rosen::SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE)) {
        return Rosen::SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE ;
    } else if (shadowColorStrategy == static_cast<int32_t>(Rosen::SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN)) {
        return Rosen::SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN ;
    } else {
        return Rosen::SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;
    }
}

void DrawPixelMapModifier::SetTextDragShadow(std::shared_ptr<Rosen::RSCanvasNode> pixelMapNode) const
{
    if (!g_drawingInfo.filterInfo.path.empty()) {
        FI_HILOGD("path:%{public}s", g_drawingInfo.filterInfo.path.c_str());
        pixelMapNode->SetShadowPath(Rosen::RSPath::CreateRSPath(g_drawingInfo.filterInfo.path));
    } else {
        FI_HILOGW("path is empty");
    }
}

void DrawPixelMapModifier::SetDragShadow(std::shared_ptr<Rosen::RSCanvasNode> pixelMapNode) const
{
    if ((g_drawingInfo.filterInfo.dragType == "text") && (g_drawingInfo.filterInfo.path.empty())) {
        FI_HILOGI("path is empty");
        return;
    }
    pixelMapNode->SetShadowOffset(g_drawingInfo.filterInfo.offsetX, g_drawingInfo.filterInfo.offsetY);
    pixelMapNode->SetShadowColor(g_drawingInfo.filterInfo.argb);
    pixelMapNode->SetShadowMask(g_drawingInfo.filterInfo.shadowMask);
    pixelMapNode->SetShadowIsFilled(g_drawingInfo.filterInfo.shadowIsFilled);
    pixelMapNode->SetShadowColorStrategy(ConvertShadowColorStrategy(g_drawingInfo.filterInfo.shadowColorStrategy));
    if (g_drawingInfo.filterInfo.isHardwareAcceleration) {
        pixelMapNode->SetShadowElevation(g_drawingInfo.filterInfo.elevation);
    } else {
        pixelMapNode->SetShadowRadius(g_drawingInfo.filterInfo.shadowCorner);
    }
    if (g_drawingInfo.filterInfo.dragType == "text") {
        SetTextDragShadow(pixelMapNode);
    }
}

void DrawPixelMapModifier::Draw(Rosen::RSDrawingContext &context) const
{
    FI_HILOGD("enter");
    CHKPV(g_drawingInfo.pixelMap);
    int32_t pixelMapWidth = g_drawingInfo.pixelMap->GetWidth();
    int32_t pixelMapHeight = g_drawingInfo.pixelMap->GetHeight();
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> pixelMapNode = g_drawingInfo.nodes[PIXEL_MAP_INDEX];
    CHKPV(pixelMapNode);
    if (g_drawingInfo.filterInfo.shadowEnable) {
        SetDragShadow(pixelMapNode);
    }
    int32_t adjustSize = TWELVE_SIZE * GetScaling();
    pixelMapNode->SetBounds(DEFAULT_POSITION_X, adjustSize, pixelMapWidth, pixelMapHeight);
    pixelMapNode->SetFrame(DEFAULT_POSITION_X, adjustSize, pixelMapWidth, pixelMapHeight);
    pixelMapNode->SetBgImageWidth(pixelMapWidth);
    pixelMapNode->SetBgImageHeight(pixelMapHeight);
    pixelMapNode->SetBgImagePositionX(0);
    pixelMapNode->SetBgImagePositionY(0);
    Rosen::Drawing::AdaptiveImageInfo rsImageInfo = { 1, 0, {}, 1, 0, pixelMapWidth, pixelMapHeight };
    auto cvs = pixelMapNode->BeginRecording(pixelMapWidth, pixelMapHeight);
    CHKPV(cvs);
    Rosen::Drawing::Brush brush;
    cvs->AttachBrush(brush);
    FilterInfo filterInfo = g_drawingInfo.filterInfo;
    if (g_drawingInfo.filterInfo.shadowEnable && !filterInfo.path.empty() &&
        g_drawingInfo.filterInfo.dragType == "text") {
        auto rsPath = Rosen::RSPath::CreateRSPath(filterInfo.path);
        cvs->Save();
        cvs->ClipPath(rsPath->GetDrawingPath(), Rosen::Drawing::ClipOp::INTERSECT, true);
        cvs->DrawPixelMapWithParm(g_drawingInfo.pixelMap, rsImageInfo, Rosen::Drawing::SamplingOptions());
        cvs->Restore();
    } else {
        cvs->DrawPixelMapWithParm(g_drawingInfo.pixelMap, rsImageInfo, Rosen::Drawing::SamplingOptions());
    }
    cvs->DetachBrush();
    pixelMapNode->SetClipToBounds(true);
    pixelMapNode->FinishRecording();
    Rosen::RSTransaction::FlushImplicitTransaction();
    FI_HILOGD("leave");
}

void DrawMouseIconModifier::Draw(Rosen::RSDrawingContext &context) const
{
    FI_HILOGD("enter");
    std::string imagePath;
    if (pointerStyle_.id == MOUSE_DRAG_CURSOR_CIRCLE_STYLE) {
        imagePath = MOUSE_DRAG_CURSOR_CIRCLE_PATH;
    } else {
        imagePath = MOUSE_DRAG_DEFAULT_PATH;
    }
    int32_t pointerSize = pointerStyle_.size;
    int32_t pointerColor = pointerStyle_.color;
    int32_t cursorPixel = DEVICE_INDEPENDENT_PIXEL;
    if (pointerStyle_.options == MAGIC_STYLE_OPT) {
        imagePath = MOUSE_DRAG_MAGIC_DEFAULT_PATH;
        int32_t ret = MMI::InputManager::GetInstance()->GetPointerSize(pointerSize);
        if (ret != RET_OK) {
            FI_HILOGW("Get pointer size failed, ret:%{public}d", ret);
        }
        ret = MMI::InputManager::GetInstance()->GetPointerColor(pointerColor);
        if (ret != RET_OK) {
            FI_HILOGW("Get pointer color failed, ret:%{public}d", ret);
        }
        cursorPixel = MAGIC_INDEPENDENT_PIXEL;
    }
    Media::SourceOptions opts;
    opts.formatHint = "image/svg+xml";
    uint32_t errCode = 0;
    auto imageSource = Media::ImageSource::CreateImageSource(imagePath, opts, errCode);
    CHKPV(imageSource);
    if (pointerSize < DEFAULT_MOUSE_SIZE) {
        FI_HILOGD("Invalid pointerSize:%{public}d", pointerSize);
        pointerSize = DEFAULT_MOUSE_SIZE;
    }
    Media::DecodeOptions decodeOpts;
    decodeOpts.desiredSize = {
        .width = pow(INCREASE_RATIO, pointerSize - 1) * cursorPixel * GetScaling(),
        .height = pow(INCREASE_RATIO, pointerSize - 1) * cursorPixel * GetScaling()
    };
    if (pointerColor != INVALID_COLOR_VALUE) {
        decodeOpts.SVGOpts.fillColor = {.isValidColor = true, .color = pointerColor};
    }
    std::shared_ptr<Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errCode);
    CHKPV(pixelMap);
    OnDraw(pixelMap);
    FI_HILOGD("leave");
}

void DrawMouseIconModifier::OnDraw(std::shared_ptr<Media::PixelMap> pixelMap) const
{
    FI_HILOGD("enter");
    CHKPV(pixelMap);
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return;
    }
    g_drawingInfo.mouseWidth = pixelMap->GetWidth();
    g_drawingInfo.mouseHeight = pixelMap->GetHeight();
    if (g_drawingInfo.nodes.size() <= MOUSE_ICON_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> mouseIconNode = g_drawingInfo.nodes[MOUSE_ICON_INDEX];
    CHKPV(mouseIconNode);
    mouseIconNode->SetBgImageWidth(pixelMap->GetWidth());
    mouseIconNode->SetBgImageHeight(pixelMap->GetHeight());
    mouseIconNode->SetBgImagePositionX(0);
    mouseIconNode->SetBgImagePositionY(0);
    auto rosenImage = std::make_shared<Rosen::RSImage>();
    rosenImage->SetPixelMap(pixelMap);
    rosenImage->SetImageRepeat(0);
    mouseIconNode->SetBgImage(rosenImage);
    Rosen::RSTransaction::FlushImplicitTransaction();
    FI_HILOGD("leave");
}

void DrawDynamicEffectModifier::Draw(Rosen::RSDrawingContext &context) const
{
    FI_HILOGD("enter");
    CHKPV(alpha_);
    CHKPV(g_drawingInfo.parentNode);
    g_drawingInfo.parentNode->SetAlpha(alpha_->Get());
    CHKPV(scale_);
    g_drawingInfo.parentNode->SetScale(scale_->Get(), scale_->Get());
    Rosen::RSTransaction::FlushImplicitTransaction();
    FI_HILOGD("leave");
}

void DrawDynamicEffectModifier::SetAlpha(float alpha)
{
    FI_HILOGD("enter");
    if (alpha_ == nullptr) {
        alpha_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(alpha);
        Rosen::RSModifier::AttachProperty(alpha_);
        return;
    }
    alpha_->Set(alpha);
    FI_HILOGD("leave");
}

void DrawDynamicEffectModifier::SetScale(float scale)
{
    FI_HILOGD("enter");
    if (scale_ == nullptr) {
        scale_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(scale);
        Rosen::RSModifier::AttachProperty(scale_);
        return;
    }
    scale_->Set(scale);
    FI_HILOGD("leave");
}

void DrawStyleChangeModifier::Draw(Rosen::RSDrawingContext &context) const
{
    FI_HILOGD("enter");
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return;
    }
    if (g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPV(dragStyleNode);
    CHKPV(g_drawingInfo.pixelMap);
    float pixelMapWidth = g_drawingInfo.pixelMap->GetWidth();
    if (stylePixelMap_ == nullptr) {
        if (scale_ == nullptr) {
            return;
        }
        dragStyleNode->SetScale(scale_->Get());
        return;
    }
    float scalingValue = GetScaling();
    if ((1.0 * INT_MAX / EIGHT_SIZE) <= scalingValue) {
        return;
    }
    int32_t adjustSize = EIGHT_SIZE * scalingValue;
    int32_t svgTouchPositionX = pixelMapWidth + adjustSize - stylePixelMap_->GetWidth();
    dragStyleNode->SetBounds(svgTouchPositionX, (TWELVE_SIZE-EIGHT_SIZE)*scalingValue, stylePixelMap_->GetWidth(),
        stylePixelMap_->GetHeight());
    dragStyleNode->SetFrame(svgTouchPositionX, (TWELVE_SIZE-EIGHT_SIZE)*scalingValue, stylePixelMap_->GetWidth(),
        stylePixelMap_->GetHeight());
    dragStyleNode->SetBgImageWidth(stylePixelMap_->GetWidth());
    dragStyleNode->SetBgImageHeight(stylePixelMap_->GetHeight());
    dragStyleNode->SetBgImagePositionX(0);
    dragStyleNode->SetBgImagePositionY(0);
    auto rosenImage = std::make_shared<Rosen::RSImage>();
    rosenImage->SetPixelMap(stylePixelMap_);
    rosenImage->SetImageRepeat(0);
    dragStyleNode->SetBgImage(rosenImage);
    Rosen::RSTransaction::FlushImplicitTransaction();
    FI_HILOGD("leave");
}

void DrawStyleChangeModifier::SetScale(float scale)
{
    FI_HILOGD("enter");
    if (scale_ == nullptr) {
        scale_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(scale);
        Rosen::RSModifier::AttachProperty(scale_);
    } else {
        scale_->Set(scale);
    }
    FI_HILOGD("leave");
}

void DrawStyleScaleModifier::Draw(Rosen::RSDrawingContext &context) const
{
    FI_HILOGD("enter");
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return;
    }
    if (g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPV(dragStyleNode);
    CHKPV(scale_);
    dragStyleNode->SetScale(scale_->Get());
    FI_HILOGD("leave");
}

void DrawStyleScaleModifier::SetScale(float scale)
{
    FI_HILOGD("enter");
    if (scale_ == nullptr) {
        scale_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(scale);
        Rosen::RSModifier::AttachProperty(scale_);
    } else {
        scale_->Set(scale);
    }
    FI_HILOGD("leave");
}

void DrawDragStopModifier::Draw(Rosen::RSDrawingContext &context) const
{
    FI_HILOGD("enter");
    CHKPV(alpha_);
    CHKPV(scale_);
    if (!CheckNodesValid()) {
        FI_HILOGE("Check nodes valid failed");
        return;
    }
    CHKPV(g_drawingInfo.parentNode);
    g_drawingInfo.parentNode->SetAlpha(alpha_->Get());
    g_drawingInfo.parentNode->SetScale(scale_->Get(), scale_->Get());
    if (g_drawingInfo.nodes.size() <= DRAG_STYLE_INDEX) {
        FI_HILOGE("The index is out of bounds, node size is %{public}zu", g_drawingInfo.nodes.size());
        return;
    }
    std::shared_ptr<Rosen::RSCanvasNode> dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPV(dragStyleNode);
    dragStyleNode->SetScale(styleScale_->Get());
    dragStyleNode->SetAlpha(styleAlpha_->Get());
    FI_HILOGD("leave");
}

void DrawDragStopModifier::SetAlpha(float alpha)
{
    FI_HILOGD("enter");
    if (alpha_ == nullptr) {
        alpha_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(alpha);
        Rosen::RSModifier::AttachProperty(alpha_);
    } else {
        alpha_->Set(alpha);
    }
    FI_HILOGD("leave");
}

void DrawDragStopModifier::SetScale(float scale)
{
    FI_HILOGD("enter");
    if (scale_ == nullptr) {
        scale_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(scale);
        Rosen::RSModifier::AttachProperty(scale_);
    } else {
        scale_->Set(scale);
    }
    FI_HILOGD("leave");
}

void DrawDragStopModifier::SetStyleScale(float scale)
{
    FI_HILOGD("enter");
    if (styleScale_ == nullptr) {
        styleScale_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(scale);
        Rosen::RSModifier::AttachProperty(styleScale_);
    } else {
        styleScale_->Set(scale);
    }
    FI_HILOGD("leave");
}

void DrawDragStopModifier::SetStyleAlpha(float alpha)
{
    FI_HILOGD("enter");
    if (styleAlpha_ == nullptr) {
        styleAlpha_ = std::make_shared<Rosen::RSAnimatableProperty<float>>(alpha);
        Rosen::RSModifier::AttachProperty(styleAlpha_);
    } else {
        styleAlpha_->Set(alpha);
    }
    FI_HILOGD("leave");
}

float DragDrawing::CalculateWidthScale()
{
    sptr<Rosen::Display> display = Rosen::DisplayManager::GetInstance().GetDisplayById(g_drawingInfo.displayId);
    if (display == nullptr) {
        FI_HILOGD("Get display info failed, display:%{public}d", g_drawingInfo.displayId);
        display = Rosen::DisplayManager::GetInstance().GetDisplayById(0);
        if (display == nullptr) {
            FI_HILOGE("Get display info failed, display is nullptr");
            return DEFAULT_SCALING;
        }
    }
    auto defaultDisplay = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    if (defaultDisplay == nullptr) {
        FI_HILOGE("defaultDisplay is nullptr");
        return DEFAULT_SCALING;
    }
    int32_t width = display->GetWidth();
    float density = defaultDisplay->GetVirtualPixelRatio();
    FI_HILOGD("density:%{public}f, width:%{public}d", density, width);
    if (width < MAX_SCREEN_WIDTH_SM * density) {
        currentScreenSize_ = ScreenSizeType::XS;
    } else if (width < MAX_SCREEN_WIDTH_MD * density) {
        currentScreenSize_ = ScreenSizeType::SM;
    } else if (width < MAX_SCREEN_WIDTH_LG * density) {
        currentScreenSize_ = ScreenSizeType::MD;
    } else if (width < MAX_SCREEN_WIDTH_XL * density) {
        currentScreenSize_ = ScreenSizeType::LG;
    } else {
        currentScreenSize_ = ScreenSizeType::XL;
    }
    float widthScale = GetMaxWidthScale(width);
    return widthScale;
}

float DragDrawing::GetMaxWidthScale(int32_t width)
{
    float scale = 1.0;
    float widthScale = 1.0;
    if (g_drawingInfo.pixelMap == nullptr) {
        FI_HILOGE("pixelMap is nullptr");
        return DEFAULT_SCALING;
    }
    int32_t pixelMapWidth = g_drawingInfo.pixelMap->GetWidth();
    if (pixelMapWidth == 0) {
        FI_HILOGW("pixelMapWidth is 0");
        return DEFAULT_SCALING;
    }
    switch (currentScreenSize_) {
        case ScreenSizeType::XS: {
            return widthScale;
        }
        case ScreenSizeType::SM: {
            scale = width * SCALE_SM;
            if (pixelMapWidth > scale) {
                widthScale = scale / pixelMapWidth;
                return widthScale;
            }
            return widthScale;
        }
        case ScreenSizeType::MD: {
            scale = width * SCALE_MD;
            if (pixelMapWidth > scale) {
                widthScale = scale / pixelMapWidth;
                return widthScale;
            }
            return widthScale;
        }
        case ScreenSizeType::LG: {
            scale = width * SCALE_LG;
            if (pixelMapWidth > scale) {
                widthScale = scale / pixelMapWidth;
                return widthScale;
            }
            return widthScale;
        }
        default: {
            FI_HILOGI("Screen Size Type is XL");
            break;
        }
    }
    return widthScale;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
