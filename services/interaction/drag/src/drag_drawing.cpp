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

#include "drag_drawing.h"

#include <atomic>
#include <cstdint>
#include <fstream>
#include <string>

#include "../wm/window.h"
#include "display_manager.h"
#include "include/core/SkTextBlob.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "libxml/tree.h"
#include "libxml/parser.h"
#include "parameters.h"
#include "pointer_event.h"
#include "string_ex.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

#include "devicestatus_define.h"
#include "drag_data_adapter.h"
#include "util.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "DragDrawing" };
constexpr int32_t BASELINE_DENSITY = 160;
constexpr int32_t DEVICE_INDEPENDENT_PIXEL = 40;
constexpr int32_t DRAG_NUM_ONE = 1;
constexpr int32_t STRING_PX_LENGTH = 2;
constexpr int32_t EIGHT_SIZE = 8;
constexpr int32_t IMAGE_WIDTH = 400;
constexpr int32_t IMAGE_HEIGHT = 500;
constexpr int64_t START_TIME = 181154000809;
constexpr int64_t INTERVAL_TIME = 16666667;
constexpr int32_t FRAMERATE = 30;
constexpr int32_t SVG_HEIGHT = 40;
constexpr int32_t SIXTEEN = 16;
constexpr int32_t SUCCESS_ANIMATION_DURATION = 300;
constexpr int32_t VIEW_BOX_POS = 2;
constexpr int32_t SHADOW_INDEX = 0;
constexpr int32_t DRAG_STYLE_INDEX = 1;
constexpr size_t NODE_COUNT = 2;
constexpr double ONETHOUSAND = 1000.0;
constexpr float BEGIN_ALPHA = 1.0f;
constexpr float END_ALPHA = 0.0f;
constexpr float BEGIN_SCALE = 1.0f;
constexpr float END_SCALE_SUCCESS = 1.2f;
constexpr float END_SCALE_FAIL = 0.1f;
constexpr float PIVOT_X = 0.5f;
constexpr float PIVOT_Y = 0.5f;
constexpr float SVG_ORIGINAL_SIZE = 40.0f;
const std::string DEVICE_TYPE_DEFAULT = "default";
const std::string DEVICE_TYPE_PHONE = "phone";
const std::string THREAD_NAME = "AnimationEventRunner";
const std::string MOUSE_COPY_DRAG_PATH = "/system/etc/device_status/drag_icon/Mouse_Copy_Drag.svg";
const std::string MOUSE_COPY_ONE_DRAG_PATH = "/system/etc/device_status/drag_icon/Mouse_Copy_One_Drag.svg";
const std::string MOUSE_DRAG_PATH = "/system/etc/device_status/drag_icon/Mouse_Drag.svg";
const std::string MOUSE_FORBID_DRAG_PATH = "/system/etc/device_status/drag_icon/Mouse_Forbid_Drag.svg";
const std::string MOUSE_MOVE_DRAG_PATH = "/system/etc/device_status/drag_icon/Mouse_Move_Drag.svg";
const std::string TOUCH_COPY_DRAG_PATH = "/system/etc/device_status/drag_icon/Touch_Copy_Drag.svg";
const std::string TOUCH_COPY_ONE_DRAG_PATH = "/system/etc/device_status/drag_icon/Touch_Copy_One_Drag.svg";
const std::string TOUCH_DEFAULT_DRAG_PATH = "/system/etc/device_status/drag_icon/Touch_Default_Drag.svg";
const std::string TOUCH_FORBID_DRAG_PATH = "/system/etc/device_status/drag_icon/Touch_Forbid_Drag.svg";
const std::string TOUCH_MOVE_DRAG_PATH = "/system/etc/device_status/drag_icon/Touch_Move_Drag.svg";
struct DrawingInfo {
    std::atomic_bool isRunning { false };
    bool isInitUiDirector { true };
    int32_t sourceType { -1 };
    int32_t currentDragNum { -1 };
    DragCursorStyle currentStyle { DragCursorStyle::DEFAULT };
    int32_t displayId { -1 };
    int32_t pixelMapX { -1 };
    int32_t pixelMapY { -1 };
    int32_t displayX { -1 };
    int32_t displayY { -1 };
    int32_t rootNodeWidth { -1 };
    int32_t rootNodeHeight { -1 };
    sptr<OHOS::Rosen::Window> dragWindow { nullptr };
    std::vector<std::shared_ptr<OHOS::Rosen::RSCanvasNode>> nodes;
    std::shared_ptr<OHOS::Rosen::RSNode> rootNode { nullptr };
    std::shared_ptr<OHOS::Rosen::RSSurfaceNode> surfaceNode { nullptr };
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap { nullptr };
} g_drawingInfo;

bool CheckNodesValid()
{
    CALL_DEBUG_ENTER;
    if (g_drawingInfo.nodes.size() < NODE_COUNT) {
        FI_HILOGE("Nodes size invalid when mouse type, node size:%{public}zu", g_drawingInfo.nodes.size());
        return false;
    }
    return true;
}

float GetScaling()
{
    CALL_DEBUG_ENTER;
    auto displayInfo = OHOS::Rosen::DisplayManager::GetInstance().GetDisplayById(g_drawingInfo.displayId);
    CHKPR(displayInfo, RET_ERR);
    return (displayInfo->GetDpi() * DEVICE_INDEPENDENT_PIXEL / BASELINE_DENSITY) / SVG_ORIGINAL_SIZE;
}
} // namespace

int32_t DragDrawing::Init(const DragData &dragData)
{
    CALL_DEBUG_ENTER;
    if (g_drawingInfo.isRunning) {
        FI_HILOGE("Drag drawing is running, can not init again");
        return INIT_CANCEL;
    }
    CHKPR(dragData.shadowInfo.pixelMap, INIT_FAIL);
    if ((dragData.sourceType != OHOS::MMI::PointerEvent::SOURCE_TYPE_MOUSE) &&
        (dragData.sourceType != OHOS::MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN)) {
        FI_HILOGE("Invalid sourceType:%{public}d", dragData.sourceType);
        return INIT_FAIL;
    }
    if (dragData.dragNum < 0) {
        FI_HILOGE("Invalid dragNum:%{public}d", dragData.dragNum);
        return INIT_FAIL;
    }
    InitDrawingInfo(dragData);
    CreateWindow(dragData.displayX, dragData.displayY);
    CHKPR(g_drawingInfo.dragWindow, INIT_FAIL);
    if (InitLayer() != RET_OK) {
        FI_HILOGE("Init layer failed");
        return INIT_FAIL;
    }
    if (!CheckNodesValid()) {
        FI_HILOGE("CheckNodesValid failed");
        return INIT_FAIL;
    }
    DragAnimationData dragAnimationData;
    dragAnimationData.displayX = g_drawingInfo.displayX;
    dragAnimationData.displayY = g_drawingInfo.displayY;
    dragAnimationData.offsetX = g_drawingInfo.pixelMapX;
    dragAnimationData.offsetY = g_drawingInfo.pixelMapY;
    CHKPR(g_drawingInfo.pixelMap, INIT_FAIL);
    dragAnimationData.pixelMap = g_drawingInfo.pixelMap;
    CHKPR(g_drawingInfo.nodes[SHADOW_INDEX], INIT_FAIL);
    CHKPR(g_drawingInfo.nodes[DRAG_STYLE_INDEX], INIT_FAIL);
    auto shadowNode = g_drawingInfo.nodes[SHADOW_INDEX];
    auto dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    OnStartDrag(dragAnimationData, shadowNode, dragStyleNode);
    CHKPR(rsUiDirector_, INIT_FAIL);
    rsUiDirector_->SendMessages();
    return INIT_SUCCESS;
}

void DragDrawing::Draw(int32_t displayId, int32_t displayX, int32_t displayY)
{
    CALL_DEBUG_ENTER;
    if (displayId < 0) {
        FI_HILOGE("Invalid display:%{public}d", displayId);
        return;
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
    int32_t adjustSize = EIGHT_SIZE * GetScaling();
    int32_t positionY = g_drawingInfo.displayY + g_drawingInfo.pixelMapY - adjustSize;
    if (g_drawingInfo.dragWindow != nullptr) {
        g_drawingInfo.dragWindow->MoveTo(g_drawingInfo.displayX + g_drawingInfo.pixelMapX, positionY);
        return;
    }
    CreateWindow(g_drawingInfo.displayX + g_drawingInfo.pixelMapX, positionY);
    CHKPV(g_drawingInfo.dragWindow);
}

int32_t DragDrawing::UpdateDragStyle(DragCursorStyle style, bool isFirstTime)
{
    CALL_DEBUG_ENTER;
    if (style < DragCursorStyle::DEFAULT || style > DragCursorStyle::MOVE) {
        FI_HILOGE("Invalid style:%{public}d", style);
        return RET_ERR;
    }
    if (!isFirstTime && (g_drawingInfo.currentStyle == style)) {
        FI_HILOGD("Not need update drag style");
        return RET_OK;
    }
    g_drawingInfo.currentStyle = style;
    std::string filePath;
    if (GetFilePath(filePath) != RET_OK) {
        FI_HILOGD("Get file path failed");
        return RET_ERR;
    }
    if (!IsValidSvgFile(filePath)) {
        FI_HILOGE("Svg file is invalid");
        return RET_ERR;
    }
    auto pixelMap = DecodeSvgToPixelMap(filePath);
    CHKPV(pixelMap);
    if (!CheckNodesValid()) {
        FI_HILOGE("CheckNodesValid failed");
        return RET_ERR;
    }
    auto dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPR(dragStyleNode, RET_ERR);
    OnDragStyle(dragStyleNode, pixelMap);
    return RET_OK;
}

void DragDrawing::OnDragSuccess()
{
    CALL_DEBUG_ENTER;
    if (!CheckNodesValid()) {
        FI_HILOGE("CheckNodesValid failed");
        return;
    }
    CHKPV(g_drawingInfo.nodes[SHADOW_INDEX]);
    CHKPV(g_drawingInfo.nodes[DRAG_STYLE_INDEX]);
    auto shadowNode = g_drawingInfo.nodes[SHADOW_INDEX];
    auto styleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    OnStopDragSuccess(shadowNode, styleNode);
}

void DragDrawing::OnDragFail()
{
    CALL_DEBUG_ENTER;
    CHKPV(g_drawingInfo.dragWindow);
    CHKPV(g_drawingInfo.rootNode);
    auto dragWindow = g_drawingInfo.dragWindow;
    auto rootNode = g_drawingInfo.rootNode;
    OnStopDragFail(dragWindow, rootNode);
}

void DragDrawing::DestroyDragWindow()
{
    CALL_DEBUG_ENTER;
    startNum_ = START_TIME;
    g_drawingInfo.sourceType = -1;
    g_drawingInfo.currentDragNum = -1;
    g_drawingInfo.displayId = -1;
    g_drawingInfo.pixelMapX = -1;
    g_drawingInfo.pixelMapY = -1;
    g_drawingInfo.displayX = -1;
    g_drawingInfo.displayY = -1;
    g_drawingInfo.rootNodeWidth = -1;
    g_drawingInfo.rootNodeHeight = -1;
    g_drawingInfo.currentStyle = DragCursorStyle::DEFAULT;
    g_drawingInfo.pixelMap = nullptr;
    RemoveModifier();
    if (!g_drawingInfo.nodes.empty()) {
        g_drawingInfo.nodes.clear();
        g_drawingInfo.nodes.shrink_to_fit();
    }
    if (g_drawingInfo.rootNode != nullptr) {
        g_drawingInfo.rootNode->ClearChildren();
        g_drawingInfo.rootNode.reset();
        g_drawingInfo.rootNode = nullptr;
    }
    g_drawingInfo.surfaceNode = nullptr;
    if (g_drawingInfo.dragWindow != nullptr) {
        g_drawingInfo.dragWindow->Destroy();
        g_drawingInfo.dragWindow = nullptr;
    }
    rsUiDirector_->SetRoot(-1);
    rsUiDirector_->SendMessages();
}

void DragDrawing::UpdateDrawingState()
{
    CALL_DEBUG_ENTER;
    g_drawingInfo.isRunning = false;
}

void DragDrawing::UpdateDragWindowState(bool visible)
{
    CALL_DEBUG_ENTER;
    CHKPV(g_drawingInfo.dragWindow);
    if (visible) {
        g_drawingInfo.dragWindow->Show();
        FI_HILOGD("Drag window show success");
    } else {
        g_drawingInfo.dragWindow->Hide();
        FI_HILOGD("Drag window hide success");
    }
}

void DragDrawing::OnStartDrag(const DragAnimationData &dragAnimationData, std::shared_ptr<OHOS::Rosen::RSCanvasNode> shadowNode,
        std::shared_ptr<OHOS::Rosen::RSCanvasNode> dragStyleNode)
{
    CALL_DEBUG_ENTER;
    if (DrawShadow(shadowNode) != RET_OK) {
        FI_HILOGE("Draw shadow failed");
        return;
    }
    if (UpdateDragStyle(DragCursorStyle::DEFAULT, true) != RET_OK) {
        FI_HILOGE("Draw style failed");
        return;
    }
    CHKPV(rsUiDirector_);
    rsUiDirector_->SendMessages();
}

void DragDrawing::OnDragStyle(std::shared_ptr<OHOS::Rosen::RSCanvasNode> dragStyleNode, std::shared_ptr<OHOS::Media::PixelMap> stylePixelMap)
{
    CALL_DEBUG_ENTER;
    CHKPR(rsUiDirector_, RET_ERR);
    DrawStyle(dragStyleNode, stylePixelMap);
    rsUiDirector_->SendMessages();
}

void DragDrawing::OnStopDragSuccess(std::shared_ptr<OHOS::Rosen::RSCanvasNode> shadowNode, std::shared_ptr<OHOS::Rosen::RSCanvasNode> dragStyleNode)
{
    CALL_DEBUG_ENTER;
    RunAnimation(END_ALPHA, END_SCALE_SUCCESS);
}

void DragDrawing::OnStopDragFail(sptr<OHOS::Rosen::Window> window, std::shared_ptr<OHOS::Rosen::RSNode> rootNode)
{
    CALL_DEBUG_ENTER;
    RunAnimation(END_ALPHA, END_SCALE_FAIL);
}

void DragDrawing::OnStopAnimation()
{
    CALL_DEBUG_ENTER;
}

void DragDrawing::RunAnimation(float endAlpha, float endScale)
{
    CALL_DEBUG_ENTER;
    if (handler_ == nullptr) {
        auto runner = AppExecFwk::EventRunner::Create(THREAD_NAME);
        CHKPV(runner);
        handler_ = std::make_shared<AppExecFwk::EventHandler>(std::move(runner));
    }
    if (!handler_->PostTask(std::bind(&DragDrawing::InitVSync, this, endAlpha, endScale))) {
        FI_HILOGE("Send vsync event failed");
    }
}

int32_t DragDrawing::DrawShadow(std::shared_ptr<OHOS::Rosen::RSCanvasNode> shadowNode)
{
    CALL_DEBUG_ENTER;
    CHKPR(shadowNode, RET_ERR);
    if (drawPixelMapModifier_ != nullptr) {
        shadowNode->RemoveModifier(drawPixelMapModifier_);
        drawPixelMapModifier_ = nullptr;
    }
    drawPixelMapModifier_ = std::make_shared<DrawPixelMapModifier>();
    shadowNode->AddModifier(drawPixelMapModifier_);
    return RET_OK;
}

int32_t DragDrawing::DrawStyle(std::shared_ptr<OHOS::Rosen::RSCanvasNode> dragStyleNode, std::shared_ptr<OHOS::Media::PixelMap> stylePixelMap)
{
    CALL_DEBUG_ENTER;
    CHKPR(dragStyleNode, RET_ERR);
    if (drawSVGModifier_ != nullptr) {
        dragStyleNode->RemoveModifier(drawSVGModifier_);
        drawSVGModifier_ = nullptr;
    }
    drawSVGModifier_ = std::make_shared<DrawSVGModifier>(stylePixelMap);
    dragStyleNode->AddModifier(drawSVGModifier_);
    return RET_OK;
}

int32_t DragDrawing::InitVSync(float endAlpha, float endScale)
{
    CALL_DEBUG_ENTER;
    startNum_ = START_TIME;
    CHKPR(g_drawingInfo.rootNode, RET_ERR);
    if (drawDynamicEffectModifier_ != nullptr) {
        g_drawingInfo.rootNode->RemoveModifier(drawDynamicEffectModifier_);
        drawDynamicEffectModifier_ = nullptr;
    }
    drawDynamicEffectModifier_ = std::make_shared<DrawDynamicEffectModifier>();
    g_drawingInfo.rootNode->AddModifier(drawDynamicEffectModifier_);
    drawDynamicEffectModifier_->SetAlpha(BEGIN_ALPHA);
    drawDynamicEffectModifier_->SetScale(BEGIN_SCALE);

    OHOS::Rosen::RSAnimationTimingProtocol protocol;
    protocol.SetDuration(SUCCESS_ANIMATION_DURATION);
    OHOS::Rosen::RSNode::Animate(protocol, OHOS::Rosen::RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
        drawDynamicEffectModifier_->SetAlpha(endAlpha);
        drawDynamicEffectModifier_->SetScale(endScale);
    });

    CHKPR(g_drawingInfo.surfaceNode, RET_ERR);
    g_drawingInfo.surfaceNode->SetPivot(PIVOT_X, PIVOT_Y);
    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
    if (receiver_ == nullptr) {
        CHKPR(handler_, RET_ERR);
        auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
        receiver_ = rsClient.CreateVSyncReceiver("DragDrawing", handler_);
        CHKPR(receiver_, RET_ERR);
    }
    int32_t ret = receiver_->Init();
    if (ret != RET_OK) {
        FI_HILOGE("Receiver init failed");
        return RET_ERR;
    }
    OHOS::Rosen::VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = std::bind(&DragDrawing::OnVsync, this),
    };
    int32_t changeFreq = static_cast<int32_t>((ONETHOUSAND / FRAMERATE) / SIXTEEN);
    ret = receiver_->SetVSyncRate(fcb, changeFreq);
    if (ret != RET_OK) {
        FI_HILOGE("Set vsync rate failed");
        return RET_ERR;
    }
    return RET_OK;
}

void DragDrawing::OnVsync()
{
    CALL_DEBUG_ENTER;
    CHKPV(rsUiDirector_);
    bool hasRunningAnimation = rsUiDirector_->RunningCustomAnimation(startNum_);
    if (!hasRunningAnimation) {
        FI_HILOGD("Stop runner, hasRunningAnimation:%{public}d", hasRunningAnimation);
        CHKPV(handler_);
        handler_->RemoveAllEvents();
        handler_->RemoveAllFileDescriptorListeners();
        handler_ = nullptr;
        g_drawingInfo.isRunning = false;
        receiver_ = nullptr;
        CHKPV(g_drawingInfo.rootNode);
        if (drawDynamicEffectModifier_ != nullptr) {
            g_drawingInfo.rootNode->RemoveModifier(drawDynamicEffectModifier_);
            drawDynamicEffectModifier_ = nullptr;
        }
        DestroyDragWindow();
        return;
    }
    rsUiDirector_->SendMessages();
    startNum_ += INTERVAL_TIME;
}

void DragDrawing::InitDrawingInfo(const DragData &dragData)
{
    CALL_DEBUG_ENTER;
    g_drawingInfo.isRunning = true;
    g_drawingInfo.currentDragNum = dragData.dragNum;
    g_drawingInfo.sourceType = dragData.sourceType;
    g_drawingInfo.displayId = dragData.displayId;
    g_drawingInfo.pixelMap = dragData.shadowInfo.pixelMap;
    g_drawingInfo.pixelMapX = dragData.shadowInfo.x;
    g_drawingInfo.pixelMapY = dragData.shadowInfo.y;
}

int32_t DragDrawing::InitLayer()
{
    CALL_DEBUG_ENTER;
    if (g_drawingInfo.dragWindow == nullptr) {
        FI_HILOGE("Init layer failed, dragWindow is nullptr");
        return RET_ERR;
    }
    g_drawingInfo.surfaceNode = g_drawingInfo.dragWindow->GetSurfaceNode();
    if (g_drawingInfo.surfaceNode == nullptr) {
        g_drawingInfo.dragWindow->Destroy();
        g_drawingInfo.dragWindow = nullptr;
        FI_HILOGE("Init layer failed, surfaceNode is nullptr");
        return RET_ERR;
    }
    auto surface = g_drawingInfo.surfaceNode->GetSurface();
    if (surface == nullptr) {
        g_drawingInfo.dragWindow->Destroy();
        g_drawingInfo.dragWindow = nullptr;
        FI_HILOGE("Init layer failed, surface is nullptr");
        return RET_ERR;
    }
    if (g_drawingInfo.isInitUiDirector) {
        g_drawingInfo.isInitUiDirector = false;
        rsUiDirector_ = OHOS::Rosen::RSUIDirector::Create();
        CHKPR(rsUiDirector_, RET_ERR);
        rsUiDirector_->Init();
    }
    rsUiDirector_->SetRSSurfaceNode(g_drawingInfo.surfaceNode);
    InitCanvas(IMAGE_WIDTH, IMAGE_HEIGHT);
    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
    return RET_OK;
}

void DragDrawing::InitCanvas(int32_t width, int32_t height)
{
    CALL_DEBUG_ENTER;
    if (g_drawingInfo.rootNode == nullptr) {
        g_drawingInfo.rootNode = OHOS::Rosen::RSRootNode::Create();
        CHKPV(g_drawingInfo.rootNode);
    }
    int32_t adjustSize = EIGHT_SIZE * GetScaling();
    g_drawingInfo.rootNode->SetBounds(g_drawingInfo.displayX, g_drawingInfo.displayY - adjustSize, width, height);
    g_drawingInfo.rootNode->SetFrame(g_drawingInfo.displayX, g_drawingInfo.displayY - adjustSize, width, height);
    g_drawingInfo.rootNode->SetBackgroundColor(SK_ColorTRANSPARENT);

    auto pixelMapNode = OHOS::Rosen::RSCanvasNode::Create();
    CHKPV(pixelMapNode);
    CHKPV(g_drawingInfo.pixelMap);
    pixelMapNode->SetBounds(0, adjustSize, g_drawingInfo.pixelMap->GetWidth(), g_drawingInfo.pixelMap->GetHeight());
    pixelMapNode->SetFrame(0, adjustSize, g_drawingInfo.pixelMap->GetWidth(), g_drawingInfo.pixelMap->GetHeight());
    g_drawingInfo.nodes.emplace_back(pixelMapNode);
    auto dragStyleNode = OHOS::Rosen::RSCanvasNode::Create();
    CHKPV(dragStyleNode);
    if (g_drawingInfo.sourceType == OHOS::MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        dragStyleNode->SetBounds(0, 0, SVG_HEIGHT, SVG_HEIGHT);
        dragStyleNode->SetFrame(0, 0, SVG_HEIGHT, SVG_HEIGHT);
        g_drawingInfo.nodes.emplace_back(dragStyleNode);
        g_drawingInfo.rootNode->AddChild(pixelMapNode);
        g_drawingInfo.rootNode->AddChild(dragStyleNode);
        rsUiDirector_->SetRoot(g_drawingInfo.rootNode->GetId());
        return;
    }
    dragStyleNode->SetBounds(ChangeNumber(g_drawingInfo.pixelMapX), ChangeNumber(g_drawingInfo.pixelMapY),
        SVG_HEIGHT, SVG_HEIGHT);
    dragStyleNode->SetFrame(ChangeNumber(g_drawingInfo.pixelMapX), ChangeNumber(g_drawingInfo.pixelMapY),
        SVG_HEIGHT, SVG_HEIGHT);
    g_drawingInfo.nodes.emplace_back(dragStyleNode);
    g_drawingInfo.rootNode->AddChild(pixelMapNode);
    g_drawingInfo.rootNode->AddChild(dragStyleNode);
    rsUiDirector_->SetRoot(g_drawingInfo.rootNode->GetId());
}

void DragDrawing::CreateWindow(int32_t displayX, int32_t displayY)
{
    CALL_DEBUG_ENTER;
    sptr<OHOS::Rosen::WindowOption> option = new (std::nothrow) OHOS::Rosen::WindowOption();
    CHKPV(option);
    option->SetWindowType(OHOS::Rosen::WindowType::WINDOW_TYPE_POINTER);
    option->SetWindowMode(OHOS::Rosen::WindowMode::WINDOW_MODE_FLOATING);
    OHOS::Rosen::Rect rect = {
        .posX_ = displayX,
        .posY_ = displayY,
        .width_ = IMAGE_WIDTH,
        .height_ = IMAGE_HEIGHT,
    };
    option->SetWindowRect(rect);
    option->SetFocusable(false);
    option->SetTouchable(false);
    std::string windowName = "drag window";
    g_drawingInfo.dragWindow = OHOS::Rosen::Window::Create(windowName, option, nullptr);
}

void DragDrawing::RemoveModifier()
{
    CALL_DEBUG_ENTER;
    auto pixelMapNode = g_drawingInfo.nodes[SHADOW_INDEX];
    CHKPV(pixelMapNode);
    if (drawPixelMapModifier_ != nullptr) {
        pixelMapNode->RemoveModifier(drawPixelMapModifier_);
        drawPixelMapModifier_ = nullptr;
    }
    auto dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPV(dragStyleNode);
    if (drawSVGModifier_ != nullptr) {
        dragStyleNode->RemoveModifier(drawSVGModifier_);
        drawSVGModifier_ = nullptr;
    }
}

int32_t DragDrawing::UpdateSvgNodeInfo(xmlNodePtr curNode, int32_t extendSvgWidth)
{
    CALL_DEBUG_ENTER;
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
    std::string tgtSvgWidth  = std::to_string(number);
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
    return RET_OK;
}

xmlNodePtr DragDrawing::GetRectNode(xmlNodePtr curNode)
{
    CALL_DEBUG_ENTER;
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
    return curNode;
}

xmlNodePtr DragDrawing::UpdateRectNode(xmlNodePtr curNode, int32_t extendSvgWidth)
{
    CALL_DEBUG_ENTER;
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
    return nullptr;
}

void DragDrawing::UpdateTspanNode(xmlNodePtr curNode)
{
    CALL_DEBUG_ENTER;
    while (curNode != nullptr) {
        if (!xmlStrcmp(curNode->name, BAD_CAST "tspan")) {
            xmlNodeSetContent(curNode, BAD_CAST std::to_string(g_drawingInfo.currentDragNum).c_str());
        }
        curNode = curNode->next;
    }
}

int32_t DragDrawing::ParseAndAdjustSvgInfo(xmlNodePtr curNode)
{
    CALL_DEBUG_ENTER;
    CHKPR(curNode, RET_ERR);
    std::string strStyle = std::to_string(g_drawingInfo.currentDragNum);
    if (strStyle.size() < 1) {
        FI_HILOGE("strStyle size:%{public}zu invalid", strStyle.size());
        return RET_ERR;
    }
    int32_t extendSvgWidth = (strStyle.size() - 1) * EIGHT_SIZE;
    xmlKeepBlanksDefault(0);
    int32_t ret = UpdateSvgNodeInfo(curNode, extendSvgWidth);
    if (ret != RET_OK) {
        FI_HILOGE("Update svg node info failed, ret:%{public}d", ret);
        return RET_ERR;
    }
    curNode = GetRectNode(curNode);
    CHKPR(curNode, RET_ERR);
    curNode = UpdateRectNode(curNode, extendSvgWidth);
    CHKPR(curNode, RET_ERR);
    UpdateTspanNode(curNode);
    return RET_OK;
}

std::shared_ptr<OHOS::Media::PixelMap> DragDrawing::DecodeSvgToPixelMap(const std::string &filePath)
{
    CALL_DEBUG_ENTER;
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
    xmlChar *xmlbuff;
    int32_t buffersize;
    xmlDocDumpFormatMemory(xmlDoc, &xmlbuff, &buffersize, 1);
    std::ostringstream oStrStream;
    oStrStream << xmlbuff;
    std::string content = oStrStream.str();
    xmlFree(xmlbuff);
    xmlFreeDoc(xmlDoc);
    OHOS::Media::SourceOptions opts;
    opts.formatHint = "image/svg+xml";
    uint32_t errCode = 0;
    auto imageSource = OHOS::Media::ImageSource::CreateImageSource(reinterpret_cast<const uint8_t*>(content.c_str()),
        content.size(), opts, errCode);
    OHOS::Media::DecodeOptions decodeOpts;
    SetDecodeOptions(decodeOpts);
    CHKPP(imageSource);
    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errCode);
    return pixelMap;
}

bool DragDrawing::NeedAdjustSvgInfo()
{
    CALL_DEBUG_ENTER;
    if (g_drawingInfo.currentStyle == DragCursorStyle::DEFAULT) {
        return false;
    }
    if ((g_drawingInfo.currentStyle == DragCursorStyle::COPY) &&
        (g_drawingInfo.currentDragNum == DRAG_NUM_ONE)) {
        return false;
    }
    std::string deviceType = OHOS::system::GetDeviceType();
    if ((g_drawingInfo.currentStyle == DragCursorStyle::MOVE) &&
        ((deviceType.compare(0, DEVICE_TYPE_DEFAULT.size(), DEVICE_TYPE_DEFAULT) == 0) ||
        (deviceType.compare(0, DEVICE_TYPE_PHONE.size(), DEVICE_TYPE_PHONE) == 0)) &&
        (g_drawingInfo.currentDragNum == DRAG_NUM_ONE)) {
        return false;
    }
    return true;
}

int32_t DragDrawing::GetFilePath(std::string &filePath)
{
    CALL_DEBUG_ENTER;
    if (!CheckNodesValid()) {
        FI_HILOGE("CheckNodesValid failed");
        return RET_ERR;
    }
    if (g_drawingInfo.sourceType == OHOS::MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        return GetMouseFilePath(filePath);
    }
    return GetTouchscreenFilePath(filePath);
}

int32_t DragDrawing::GetMouseFilePath(std::string &filePath)
{
    CALL_DEBUG_ENTER;
    switch (g_drawingInfo.currentStyle) {
        case DragCursorStyle::COPY: {
            if (g_drawingInfo.currentDragNum == DRAG_NUM_ONE) {
                filePath = MOUSE_COPY_ONE_DRAG_PATH;
            } else {
                filePath = MOUSE_COPY_DRAG_PATH;
            }
            break;
        }
        case DragCursorStyle::MOVE: {
            std::string deviceType = OHOS::system::GetDeviceType();
            if (((deviceType.compare(0, DEVICE_TYPE_DEFAULT.size(), DEVICE_TYPE_DEFAULT) == 0) ||
                (deviceType.compare(0, DEVICE_TYPE_PHONE.size(), DEVICE_TYPE_PHONE) == 0)) &&
                (g_drawingInfo.currentDragNum == DRAG_NUM_ONE)) {
                filePath = MOUSE_DRAG_PATH;
            } else {
                filePath = MOUSE_MOVE_DRAG_PATH;
            }
            break;
        }
        case DragCursorStyle::FORBIDDEN: {
            filePath = MOUSE_FORBID_DRAG_PATH;
            break;
        }
        case DragCursorStyle::DEFAULT:
        default: {
            filePath = MOUSE_DRAG_PATH;
        }
    }
    return RET_OK;
}

int32_t DragDrawing::GetTouchscreenFilePath(std::string &filePath)
{
    CALL_DEBUG_ENTER;
    auto dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPR(dragStyleNode, RET_ERR);
    switch (g_drawingInfo.currentStyle) {
        case DragCursorStyle::COPY: {
            if (g_drawingInfo.currentDragNum == DRAG_NUM_ONE) {
                filePath = TOUCH_COPY_ONE_DRAG_PATH;
            } else {
                filePath = TOUCH_COPY_DRAG_PATH;
            }
            break;
        }
        case DragCursorStyle::MOVE: {
            std::string deviceType = OHOS::system::GetDeviceType();
            if (((deviceType.compare(0, DEVICE_TYPE_DEFAULT.size(), DEVICE_TYPE_DEFAULT) == 0) ||
                (deviceType.compare(0, DEVICE_TYPE_PHONE.size(), DEVICE_TYPE_PHONE) == 0)) &&
                (g_drawingInfo.currentDragNum == DRAG_NUM_ONE)) {
                FI_HILOGD("Device type is phone, not need draw svg style, deviceType:%{public}s", deviceType.c_str());
                filePath = TOUCH_DEFAULT_DRAG_PATH;
            } else {
                filePath = TOUCH_MOVE_DRAG_PATH;
            }
            break;
        }
        case DragCursorStyle::FORBIDDEN: {
            filePath = TOUCH_FORBID_DRAG_PATH;
            break;
        }
        case DragCursorStyle::DEFAULT:
        default: {
            FI_HILOGW("Not need draw svg style, DragCursorStyle:%{public}d", g_drawingInfo.currentStyle);
            filePath = TOUCH_DEFAULT_DRAG_PATH;
        }
    }
    return RET_OK;
}

void DragDrawing::SetDecodeOptions(OHOS::Media::DecodeOptions &decodeOpts)
{
    CALL_DEBUG_ENTER;
    std::string strStyle = std::to_string(g_drawingInfo.currentDragNum);
    if (strStyle.size() < 1) {
        FI_HILOGE("strStyle size:%{public}zu invalid", strStyle.size());
        return;
    }
    int32_t extendSvgWidth = (strStyle.size() - 1) * EIGHT_SIZE;
    std::string deviceType = OHOS::system::GetDeviceType();
    if ((g_drawingInfo.currentStyle == DragCursorStyle::COPY) && (g_drawingInfo.currentDragNum == DRAG_NUM_ONE)) {
        decodeOpts.desiredSize= {
            .width = DEVICE_INDEPENDENT_PIXEL * GetScaling(),
            .height = DEVICE_INDEPENDENT_PIXEL * GetScaling()
        };
    } else if (((deviceType.compare(0, DEVICE_TYPE_DEFAULT.size(), DEVICE_TYPE_DEFAULT) == 0) ||
        (deviceType.compare(0, DEVICE_TYPE_PHONE.size(), DEVICE_TYPE_PHONE) == 0)) &&
        (g_drawingInfo.currentStyle == DragCursorStyle::MOVE) && (g_drawingInfo.currentDragNum == DRAG_NUM_ONE)) {
        decodeOpts.desiredSize= {
            .width = DEVICE_INDEPENDENT_PIXEL * GetScaling(),
            .height = DEVICE_INDEPENDENT_PIXEL * GetScaling()
        };
    } else if ((g_drawingInfo.sourceType == OHOS::MMI::PointerEvent::SOURCE_TYPE_MOUSE) &&
        (g_drawingInfo.currentStyle == DragCursorStyle::DEFAULT)) {
        decodeOpts.desiredSize= {
            .width = DEVICE_INDEPENDENT_PIXEL * GetScaling(),
            .height = DEVICE_INDEPENDENT_PIXEL * GetScaling()
        };
    } else {
        decodeOpts.desiredSize = {
            .width = (DEVICE_INDEPENDENT_PIXEL + extendSvgWidth) * GetScaling(),
            .height = DEVICE_INDEPENDENT_PIXEL * GetScaling()
        };
    }
}

DrawSVGModifier::DrawSVGModifier(std::shared_ptr<OHOS::Media::PixelMap> stylePixelMap)
{
    CALL_DEBUG_ENTER;
    stylePixelMap_ = stylePixelMap;
}

void DrawSVGModifier::Draw(OHOS::Rosen::RSDrawingContext& context) const
{
    CALL_DEBUG_ENTER;
    CHKPV(stylePixelMap_);
    if (!CheckNodesValid()) {
        FI_HILOGE("CheckNodesValid failed");
        return;
    }
    auto dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPV(dragStyleNode);
    dragStyleNode->SetBgImageWidth(stylePixelMap_->GetWidth());
    dragStyleNode->SetBgImageHeight(stylePixelMap_->GetHeight());
    dragStyleNode->SetBgImagePositionX(0);
    dragStyleNode->SetBgImagePositionY(0);
    auto rosenImage = std::make_shared<OHOS::Rosen::RSImage>();
    rosenImage->SetPixelMap(stylePixelMap_);
    rosenImage->SetImageRepeat(0);
    dragStyleNode->SetBgImage(rosenImage);
    if (g_drawingInfo.sourceType == OHOS::MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        DrawTouchscreenSVG(stylePixelMap_);
        return;
    }
    DrawMouseSVG(stylePixelMap_);
}

void DrawSVGModifier::DrawMouseSVG(std::shared_ptr<OHOS::Media::PixelMap> pixelMap) const
{
    CALL_DEBUG_ENTER;
    CHKPV(g_drawingInfo.pixelMap);
    auto dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPV(dragStyleNode);
    int32_t adjustSize = EIGHT_SIZE * GetScaling();
    dragStyleNode->SetBounds(ChangeNumber(g_drawingInfo.pixelMapX), ChangeNumber(g_drawingInfo.pixelMapY) + adjustSize,
        pixelMap->GetWidth(), pixelMap->GetHeight());
    dragStyleNode->SetFrame(ChangeNumber(g_drawingInfo.pixelMapX), ChangeNumber(g_drawingInfo.pixelMapY) + adjustSize,
        pixelMap->GetWidth(), pixelMap->GetHeight());
    g_drawingInfo.rootNodeWidth = pixelMap->GetWidth() + ChangeNumber(g_drawingInfo.pixelMapX);
    g_drawingInfo.rootNodeHeight = pixelMap->GetHeight() + ChangeNumber(g_drawingInfo.pixelMapY) + adjustSize;
    g_drawingInfo.rootNodeWidth = g_drawingInfo.rootNodeWidth > g_drawingInfo.pixelMap->GetWidth()
        ? g_drawingInfo.rootNodeWidth : g_drawingInfo.pixelMap->GetWidth();
    g_drawingInfo.rootNodeHeight = g_drawingInfo.rootNodeHeight > g_drawingInfo.pixelMap->GetHeight()
        ? g_drawingInfo.rootNodeHeight : g_drawingInfo.pixelMap->GetHeight();
    CHKPV(g_drawingInfo.rootNode);
    g_drawingInfo.rootNode->SetBounds(0, 0, g_drawingInfo.rootNodeWidth, g_drawingInfo.rootNodeHeight);
    g_drawingInfo.rootNode->SetFrame(0, 0, g_drawingInfo.rootNodeWidth, g_drawingInfo.rootNodeHeight);
    CHKPV(g_drawingInfo.dragWindow);
    g_drawingInfo.dragWindow->Resize(g_drawingInfo.rootNodeWidth, g_drawingInfo.rootNodeHeight);
    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
}

void DrawSVGModifier::DrawTouchscreenSVG(std::shared_ptr<OHOS::Media::PixelMap> pixelMap) const
{
    CALL_DEBUG_ENTER;
    CHKPV(g_drawingInfo.pixelMap);
    int32_t adjustSize = EIGHT_SIZE * GetScaling();
    int32_t svgTouchPositionX;
    if ((g_drawingInfo.pixelMap->GetWidth() + adjustSize) < pixelMap->GetWidth()) {
        FI_HILOGD("pixelMap width:%{public}d", g_drawingInfo.pixelMap->GetWidth());
        svgTouchPositionX = 0;
    } else {
        svgTouchPositionX = g_drawingInfo.pixelMap->GetWidth() + adjustSize - pixelMap->GetWidth();
    }
    auto dragStyleNode = g_drawingInfo.nodes[DRAG_STYLE_INDEX];
    CHKPV(dragStyleNode);
    dragStyleNode->SetBounds(svgTouchPositionX, 0, pixelMap->GetWidth(), pixelMap->GetHeight());
    dragStyleNode->SetFrame(svgTouchPositionX, 0, pixelMap->GetWidth(), pixelMap->GetHeight());
    g_drawingInfo.rootNodeWidth = g_drawingInfo.pixelMap->GetWidth() + adjustSize;
    g_drawingInfo.rootNodeHeight = g_drawingInfo.pixelMap->GetHeight() + adjustSize;
    CHKPV(g_drawingInfo.rootNode);
    g_drawingInfo.rootNode->SetBounds(0, 0, g_drawingInfo.rootNodeWidth, g_drawingInfo.rootNodeHeight);
    g_drawingInfo.rootNode->SetFrame(0, 0, g_drawingInfo.rootNodeWidth, g_drawingInfo.rootNodeHeight);
    CHKPV(g_drawingInfo.dragWindow);
    g_drawingInfo.dragWindow->Resize(g_drawingInfo.rootNodeWidth, g_drawingInfo.rootNodeHeight);
    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
}

void DrawPixelMapModifier::Draw(OHOS::Rosen::RSDrawingContext &context) const
{
    CALL_DEBUG_ENTER;
    if (g_drawingInfo.pixelMap == nullptr) {
        FI_HILOGE("pixelMap is nullptr");
        return;
    }
    auto rosenImage = std::make_shared<OHOS::Rosen::RSImage>();
    rosenImage->SetPixelMap(g_drawingInfo.pixelMap);
    rosenImage->SetImageRepeat(0);
    int32_t pixelMapWidth = g_drawingInfo.pixelMap->GetWidth();
    int32_t pixelMapHeight = g_drawingInfo.pixelMap->GetHeight();
    auto pixelMapNode = g_drawingInfo.nodes[SHADOW_INDEX];
    CHKPV(pixelMapNode);
    pixelMapNode->SetBoundsWidth(pixelMapWidth);
    pixelMapNode->SetBoundsHeight(pixelMapHeight);
    pixelMapNode->SetBgImageWidth(pixelMapWidth);
    pixelMapNode->SetBgImageHeight(pixelMapHeight);
    pixelMapNode->SetBgImagePositionX(0);
    pixelMapNode->SetBgImagePositionY(0);
    pixelMapNode->SetBgImage(rosenImage);
    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
}

void DrawDynamicEffectModifier::Draw(OHOS::Rosen::RSDrawingContext &context) const
{
    CALL_DEBUG_ENTER;
    CHKPV(alpha_);
    CHKPV(g_drawingInfo.rootNode);
    g_drawingInfo.rootNode->SetAlpha(alpha_->Get());
    CHKPV(scale_);
    CHKPV(g_drawingInfo.surfaceNode);
    g_drawingInfo.surfaceNode->SetScale(scale_->Get(), scale_->Get());
    auto rsSurface = OHOS::Rosen::RSSurfaceExtractor::ExtractRSSurface(g_drawingInfo.surfaceNode);
    CHKPV(rsSurface);
    auto frame = rsSurface->RequestFrame(g_drawingInfo.rootNodeWidth, g_drawingInfo.rootNodeHeight);
    if (frame == nullptr) {
        FI_HILOGE("Failed to create frame");
        return;
    }
    FI_HILOGD("alpha_:%{public}f, scale_:%{public}f", alpha_->Get(), scale_->Get());
    frame->SetDamageRegion(0, 0, g_drawingInfo.rootNodeWidth, g_drawingInfo.rootNodeHeight);
    rsSurface->FlushFrame(frame);
    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
}

void DrawDynamicEffectModifier::SetAlpha(float alpha)
{
    CALL_DEBUG_ENTER;
    if (alpha_ == nullptr) {
        alpha_ = std::make_shared<OHOS::Rosen::RSAnimatableProperty<float>>(alpha);
        OHOS::Rosen::RSModifier::AttachProperty(alpha_);
    } else {
        alpha_->Set(alpha);
    }
}

void DrawDynamicEffectModifier::SetScale(float scale)
{
    CALL_DEBUG_ENTER;
    if (scale_ == nullptr) {
        scale_ = std::make_shared<OHOS::Rosen::RSAnimatableProperty<float>>(scale);
        OHOS::Rosen::RSModifier::AttachProperty(scale_);
    } else {
        scale_->Set(scale);
    }
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS