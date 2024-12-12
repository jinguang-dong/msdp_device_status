# MSDP设备状态感知框架<a name="ZH-CN_TOPIC_0000001148682248"></a>

-   [简介](#section_device_status_introduction)
-   [目录](#section_device_status_directory)
-   [约束](#section_device_status_constraint)
-   [使用](#section_device_status_usage)

## 简介<a name="section_device_status_introduction"></a>

MSDP设备状态感知框架能够识别出目前设备的状态并传递给订阅者，整个框架是基于MSDP算法库和系统SensorHDI组件组成的，将其接收到的感知事件传递给订阅者。根据感知用途分为以下三大类：

-   绝对静止类：利用加速度、陀螺仪等传感器信息识别设备处于绝对静止状态。
-   水平/垂直姿态类：利用加速度、陀螺仪等传感器信息识别设备处于绝对静止状态。
-   皮套开合事件感知：基于霍尔传感器识别皮套的开合的状态。

MSDP设备状态感知架构图如下所示：

**图 1**  MSDP设备状态感知架构图
![](figures/zh-cn_device_status_block.png)

## 目录<a name="section_device_status_directory"></a>

MSDP设备状态感知框架的示例代码如下：

```
/base/msdp/device_status
├── frameworks                 # 框架代码
│   └── native                 # device status客户端代码
├── interfaces                 # 对外接口存放目录
│   ├── innerkits              # device satus innerkits API
│   └── kits/js                # Js API
├── sa_profile                 # 服务名称和服务的动态库的配置文件
├── services                   # 服务的代码目录
│   └── native/src             # device status服务代码
└── utils                      # 公共代码，包括权限、通信等能力
```

## 约束<a name="section_device_status_constraint"></a>

要使用MSDP设备状态感知功能，设备必须具有对应的传感器器件和相应的MSDP算法库。

## 使用<a name="section_device_status_usage"></a>

本模块当前仅提供inner接口给其他系统应用，暂未提供应用js接口。


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
        cJSON *shadowCorner = cJSON_GetObjectItemCaseSensitive(filterInfoParser, "shadow_corner");
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
        "shadowCorner:%{public}f, offsetX:%{private}f, offsetY:%{private}f, argb:%{public}u, elevation:%{public}f, "
        "isHardwareAcceleration:%{public}s", filterInfo.dragType.c_str(),
        filterInfo.shadowIsFilled ? "true" : "false", filterInfo.shadowMask ? "true" : "false",
        filterInfo.shadowColorStrategy, filterInfo.shadowCorner, filterInfo.offsetX, filterInfo.offsetY,
        filterInfo.argb, filterInfo.elevation, filterInfo.isHardwareAcceleration ? "true" : "false");
    if (!filterInfo.path.empty()) {
        FI_HILOGI("path:%{private}s", filterInfo.path.c_str());
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
    cJSON *scale = cJSON_GetObjectItemCaseSensitive(filterInfoParser.json, "scale");
    if (cJSON_IsNumber(scale)) {
        filterInfo.scale = AdjustDoubleValue(scale->valuedouble);
    }
    ParserCornerRadiusInfo(filterInfoParser.json, g_drawingInfo.filterInfo);
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
    cJSON *dragNodeGrayscale = cJSON_GetObjectItemCaseSensitive(filterInfoParser.json, "drag_node_gray_scale");
    if (cJSON_IsNumber(dragNodeGrayscale)) {
        filterInfo.dragNodeGrayscale = static_cast<float>(dragNodeGrayscale->valuedouble);
    }
    return true;
}

void DragDrawing::ParserCornerRadiusInfo(const cJSON *cornerRadiusInfoStr, FilterInfo &filterInfo)
{
    CHKPV(cornerRadiusInfoStr);
    cJSON *cornerRadius1 = cJSON_GetObjectItemCaseSensitive(cornerRadiusInfoStr, "drag_corner_radius1");
    if (cJSON_IsNumber(cornerRadius1)) {
        filterInfo.cornerRadius1 = static_cast<float>(cornerRadius1->valuedouble);
    }
    cJSON *cornerRadius2 = cJSON_GetObjectItemCaseSensitive(cornerRadiusInfoStr, "drag_corner_radius2");
    if (cJSON_IsNumber(cornerRadius2)) {
        filterInfo.cornerRadius2 = static_cast<float>(cornerRadius2->valuedouble);
    }
    cJSON *cornerRadius3 = cJSON_GetObjectItemCaseSensitive(cornerRadiusInfoStr, "drag_corner_radius3");
    if (cJSON_IsNumber(cornerRadius3)) {
        filterInfo.cornerRadius3 = static_cast<float>(cornerRadius3->valuedouble);
    }
    cJSON *cornerRadius4 = cJSON_GetObjectItemCaseSensitive(cornerRadiusInfoStr, "drag_corner_radius4");
    if (cJSON_IsNumber(cornerRadius4)) {
        filterInfo.cornerRadius4 = static_cast<float>(cornerRadius4->valuedouble);
    }
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
        auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
        CHKPR(currentPixelMap, RET_ERR);
        g_drawingInfo.pixelMapX = -(HALF_RATIO * currentPixelMap->GetWidth());
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
            multiSelectedNode->SetPivot(HALF_PIVOT, HALF_PIVOT);
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
    float positionX = g_drawingInfo.currentPositionX;
    float positionY = g_drawingInfo.currentPositionY;
    AdjustRotateDisplayXY(positionX, positionY);
    DrawRotateDisplayXY(positionX, positionY);
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
    auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
    CHKPV(currentPixelMap);
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
    auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
    CHKPV(currentPixelMap);
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
    filterNode->SetBounds(DEFAULT_POSITION_X, adjustSize, currentPixelMap->GetWidth(),
        currentPixelMap->GetHeight());
    filterNode->SetFrame(DEFAULT_POSITION_X, adjustSize, currentPixelMap->GetWidth(),
        currentPixelMap->GetHeight());
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
    auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
    CHKPV(currentPixelMap);
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
    filterNode->SetBounds(DEFAULT_POSITION_X, adjustSize, currentPixelMap->GetWidth(),
        currentPixelMap->GetHeight());
    filterNode->SetFrame(DEFAULT_POSITION_X, adjustSize, currentPixelMap->GetWidth(),
        currentPixelMap->GetHeight());
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

int32_t DragDrawing::SetNodesLocation()
{
    FI_HILOGD("enter");
    Rosen::RSAnimationTimingProtocol protocol;
    Rosen::RSNode::Animate(protocol, SPRING, [&]() {
        float displayX = g_drawingInfo.currentPositionX;
        float displayY = g_drawingInfo.currentPositionY;
        AdjustRotateDisplayXY(displayX, displayY);
        int32_t positionX = displayX + g_drawingInfo.pixelMapX;
        int32_t positionY = displayY + g_drawingInfo.pixelMapY - TWELVE_SIZE * GetScaling();
        int32_t adjustSize = TWELVE_SIZE * GetScaling();
        CHKPV(g_drawingInfo.parentNode);
        auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
        CHKPV(currentPixelMap);
        g_drawingInfo.parentNode->SetBounds(positionX, positionY, currentPixelMap->GetWidth(),
            currentPixelMap->GetHeight() + adjustSize);
        g_drawingInfo.parentNode->SetFrame(positionX, positionY, currentPixelMap->GetWidth(),
            currentPixelMap->GetHeight() + adjustSize);
        if (!g_drawingInfo.multiSelectedNodes.empty() && !g_drawingInfo.multiSelectedPixelMaps.empty()) {
            size_t multiSelectedNodesSize = g_drawingInfo.multiSelectedNodes.size();
            size_t multiSelectedPixelMapsSize = g_drawingInfo.multiSelectedPixelMaps.size();
            for (size_t i = 0; (i < multiSelectedNodesSize) && (i < multiSelectedPixelMapsSize); ++i) {
                std::shared_ptr<Rosen::RSCanvasNode> multiSelectedNode = g_drawingInfo.multiSelectedNodes[i];
                std::shared_ptr<Media::PixelMap> multiSelectedPixelMap = g_drawingInfo.multiSelectedPixelMaps[i];
                auto pixelMap  = currentPixelMap;
                CHKPV(pixelMap);
                CHKPV(multiSelectedNode);
                CHKPV(multiSelectedPixelMap);
                float multiSelectedPositionX = positionX + (static_cast<float>(pixelMap->GetWidth()) / TWICE_SIZE) -
                    (static_cast<float>(multiSelectedPixelMap->GetWidth()) / TWICE_SIZE);
                float multiSelectedPositionY = positionY + (static_cast<float>(pixelMap->GetHeight()) / TWICE_SIZE) -
                    (static_cast<float>(multiSelectedPixelMap->GetHeight()) / TWICE_SIZE - adjustSize);
                    multiSelectedNode->SetBounds(multiSelectedPositionX, multiSelectedPositionY,
                        multiSelectedPixelMap->GetWidth(), multiSelectedPixelMap->GetHeight());
                    multiSelectedNode->SetFrame(multiSelectedPositionX, multiSelectedPositionY,
                        multiSelectedPixelMap->GetWidth(), multiSelectedPixelMap->GetHeight());
            }
        }
    }, [this]() {
        FI_HILOGD("SetNodesLocation end");
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
        RotatePixelMapXY();
    } else {
        needRotatePixelMapXY_ = false;
        auto initialPixelMapLocation = DRAG_DATA_MGR.GetInitialPixelMapLocation();
        g_drawingInfo.pixelMapX = initialPixelMapLocation.first;
        g_drawingInfo.pixelMapY = initialPixelMapLocation.second;
    }
    DRAG_DATA_MGR.SetPixelMapLocation({ g_drawingInfo.pixelMapX, g_drawingInfo.pixelMapY });
    if (RunAnimation([this] {
        return this->SetNodesLocation();
    }) != RET_OK) {
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
    }, []() { FI_HILOGD("UpdatePreviewStyleWithAnimation end"); });
    FI_HILOGD("leave");
    return RET_OK;
}

void DragDrawing::UpdateMousePosition(float mousePositionX, float mousePositionY)
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
        float positionX = mousePositionX - (static_cast<float>(g_drawingInfo.mouseWidth) / CURSOR_CIRCLE_MIDDLE);
        float positionY = mousePositionY - (static_cast<float>(g_drawingInfo.mouseHeight) / CURSOR_CIRCLE_MIDDLE);
        mouseIconNode->SetBounds(positionX, positionY, g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
        mouseIconNode->SetFrame(positionX, positionY, g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
    } else {
        mouseIconNode->SetBounds(mousePositionX, mousePositionY,
            g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
        mouseIconNode->SetFrame(mousePositionX, mousePositionY,
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

void DragDrawing::DoDrawMouse(int32_t mousePositionX, int32_t mousePositionY)
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
        int32_t positionX = mousePositionX - (g_drawingInfo.mouseWidth / CURSOR_CIRCLE_MIDDLE);
        int32_t positionY = mousePositionY - (g_drawingInfo.mouseHeight / CURSOR_CIRCLE_MIDDLE);
        mouseIconNode->SetBounds(positionX, positionY, g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
        mouseIconNode->SetFrame(positionX, positionY, g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
    } else {
        mouseIconNode->SetBounds(mousePositionX, mousePositionY,
            g_drawingInfo.mouseWidth, g_drawingInfo.mouseHeight);
        mouseIconNode->SetFrame(mousePositionX, mousePositionY,
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

void DragDrawing::MultiSelectedAnimation(int32_t positionX, int32_t positionY, int32_t adjustSize,
    bool isMultiSelectedAnimation)
{
    size_t multiSelectedNodesSize = g_drawingInfo.multiSelectedNodes.size();
    size_t multiSelectedPixelMapsSize = g_drawingInfo.multiSelectedPixelMaps.size();
    for (size_t i = 0; (i < multiSelectedNodesSize) && (i < multiSelectedPixelMapsSize); ++i) {
        std::shared_ptr<Rosen::RSCanvasNode> multiSelectedNode = g_drawingInfo.multiSelectedNodes[i];
        std::shared_ptr<Media::PixelMap> multiSelectedPixelMap = g_drawingInfo.multiSelectedPixelMaps[i];
        auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
        CHKPV(currentPixelMap);
        CHKPV(multiSelectedNode);
        CHKPV(multiSelectedPixelMap);
        int32_t multiSelectedPositionX = positionX + (currentPixelMap->GetWidth() / TWICE_SIZE) -
            (multiSelectedPixelMap->GetWidth() / TWICE_SIZE);
        int32_t multiSelectedPositionY = positionY + (currentPixelMap->GetHeight() / TWICE_SIZE) -
            ((multiSelectedPixelMap->GetHeight() / TWICE_SIZE) - adjustSize);
        if (isMultiSelectedAnimation) {
            Rosen::RSAnimationTimingProtocol protocol;
            if (i == FIRST_PIXELMAP_INDEX) {
                protocol.SetDuration(SHORT_DURATION);
            } else {
                protocol.SetDuration(LONG_DURATION);
            }
            Rosen::RSNode::Animate(protocol, Rosen::RSAnimationTimingCurve::EASE_IN_OUT, [&]() {
                multiSelectedNode->SetBounds(multiSelectedPositionX, multiSelectedPositionY,
                    multiSelectedPixelMap->GetWidth(), multiSelectedPixelMap->GetHeight());
                multiSelectedNode->SetFrame(multiSelectedPositionX, multiSelectedPositionY,
                    multiSelectedPixelMap->GetWidth(), multiSelectedPixelMap->GetHeight());
            }, []() { FI_HILOGD("MultiSelectedAnimation end"); });
        } else {
            multiSelectedNode->SetBounds(multiSelectedPositionX, multiSelectedPositionY,
                multiSelectedPixelMap->GetWidth(), multiSelectedPixelMap->GetHeight());
            multiSelectedNode->SetFrame(multiSelectedPositionX, multiSelectedPositionY,
                multiSelectedPixelMap->GetWidth(), multiSelectedPixelMap->GetHeight());
        }
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
        multiSelectedNode->SetCornerRadius(g_drawingInfo.filterInfo.cornerRadius1 * g_drawingInfo.filterInfo.dipScale *
            g_drawingInfo.filterInfo.scale);
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

void DragDrawing::RotatePixelMapXY()
{
    FI_HILOGI("rotation:%{public}d", static_cast<int32_t>(rotation_));
    auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
    CHKPV(currentPixelMap);
    switch (rotation_) {
        case Rosen::Rotation::ROTATION_0:
        case Rosen::Rotation::ROTATION_180: {
            g_drawingInfo.pixelMapX = -(HALF_RATIO * currentPixelMap->GetWidth());
            g_drawingInfo.pixelMapY = -(EIGHT_SIZE * GetScaling());
            break;
        }
        case Rosen::Rotation::ROTATION_90:
        case Rosen::Rotation::ROTATION_270: {
            g_drawingInfo.pixelMapX = -(HALF_RATIO * currentPixelMap->GetWidth());
            g_drawingInfo.pixelMapY = -(EIGHT_SIZE * GetScaling());
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
    hasRunningScaleAnimation_ = false;
    CHKPV(handler_);
    handler_->RemoveAllEvents();
    handler_->RemoveAllFileDescriptorListeners();
    receiver_ = nullptr;
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
    g_drawingInfo.currentPositionX = -1.0f;
    g_drawingInfo.currentPositionY = -1.0f;
    g_drawingInfo.sourceType = -1;
    g_drawingInfo.currentDragNum = -1;
    g_drawingInfo.pixelMapX = -1;
    g_drawingInfo.pixelMapY = -1;
    g_drawingInfo.displayX = -1;
    g_drawingInfo.displayY = -1;
    g_drawingInfo.mouseWidth = 0;
    g_drawingInfo.mouseHeight = 0;
    g_drawingInfo.rootNodeWidth = -1;
    g_drawingInfo.rootNodeHeight = -1;
    DragDrawing::UpdataGlobalPixelMapLocked(nullptr);
    g_drawingInfo.stylePixelMap = nullptr;
    g_drawingInfo.isPreviousDefaultStyle = false;
    g_drawingInfo.isCurrentDefaultStyle = false;
    g_drawingInfo.currentStyle = DragCursorStyle::DEFAULT;
    g_drawingInfo.filterInfo = {};
    g_drawingInfo.extraInfo = {};
    StopVSyncStation();
    frameCallback_ = nullptr;
    FI_HILOGI("leave");
}

void DragDrawing::StopVSyncStation()
{
    FI_HILOGI("enter");
    dragSmoothProcessor_.ResetParameters();
    vSyncStation_.StopVSyncRequest();
    FI_HILOGI("leave");
}

int32_t DragDrawing::DoRotateDragWindow(float rotation,
    const std::shared_ptr<Rosen::RSTransaction>& rsTransaction, bool isAnimated)
{
    FI_HILOGD("rotation:%{public}f, isAnimated:%{public}d", rotation, isAnimated);
    auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
    CHKPR(currentPixelMap, RET_ERR);
    if ((currentPixelMap->GetWidth() <= 0) || (currentPixelMap->GetHeight() <= 0)) {
        FI_HILOGE("Invalid parameter pixelmap");
        return RET_ERR;
    }
    float adjustSize = TWELVE_SIZE * GetScaling();
    float pivotX = HALF_PIVOT;
    float pivotY = 0.0f;
    if (fabsf(adjustSize + currentPixelMap->GetHeight()) < EPSILON) {
        pivotY = HALF_PIVOT;
    } else {
        pivotY = ((currentPixelMap->GetHeight() * 1.0 / TWICE_SIZE) + adjustSize) /
            (adjustSize + currentPixelMap->GetHeight());
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

template <typename T>
void DragDrawing::AdjustRotateDisplayXY(T &displayX, T &displayY)
{
    FI_HILOGD("rotation:%{public}d", static_cast<int32_t>(rotation_));
    auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
    CHKPV(currentPixelMap);
    switch (rotation_) {
        case Rosen::Rotation::ROTATION_0: {
            break;
        }
        case Rosen::Rotation::ROTATION_90: {
            displayX -= (currentPixelMap->GetWidth() - currentPixelMap->GetHeight()) / TWICE_SIZE +
                g_drawingInfo.pixelMapX - g_drawingInfo.pixelMapY;
            displayY -= (currentPixelMap->GetWidth() - currentPixelMap->GetHeight()) / TWICE_SIZE +
                g_drawingInfo.pixelMapX + currentPixelMap->GetHeight() + g_drawingInfo.pixelMapY;
            break;
        }
        case Rosen::Rotation::ROTATION_180: {
            displayX -= currentPixelMap->GetWidth() + (g_drawingInfo.pixelMapX * TWICE_SIZE);
            displayY -= currentPixelMap->GetHeight() + (g_drawingInfo.pixelMapY * TWICE_SIZE);
            break;
        }
        case Rosen::Rotation::ROTATION_270: {
            displayX -= (currentPixelMap->GetWidth() - currentPixelMap->GetHeight()) / TWICE_SIZE +
                g_drawingInfo.pixelMapX + currentPixelMap->GetHeight() + g_drawingInfo.pixelMapY;
            displayY += (currentPixelMap->GetWidth() - currentPixelMap->GetHeight()) / TWICE_SIZE +
                g_drawingInfo.pixelMapX - g_drawingInfo.pixelMapY;
            break;
        }
        default: {
            FI_HILOGE("Invalid parameter, rotation:%{public}d", static_cast<int32_t>(rotation_));
            break;
        }
    }
}

void DragDrawing::DrawRotateDisplayXY(float positionX, float positionY)
{
    FI_HILOGD("enter");
    float adjustSize = TWELVE_SIZE * GetScaling();
    float parentPositionX = positionX + g_drawingInfo.pixelMapX;
    float parentPositionY = positionY + g_drawingInfo.pixelMapY - adjustSize;
    auto parentNode = g_drawingInfo.parentNode;
    auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
    CHKPV(parentNode);
    CHKPV(currentPixelMap);
    parentNode->SetBounds(parentPositionX, parentPositionY, currentPixelMap->GetWidth(),
        currentPixelMap->GetHeight() + adjustSize);
    parentNode->SetFrame(parentPositionX, parentPositionY, currentPixelMap->GetWidth(),
        currentPixelMap->GetHeight() + adjustSize);
    if (!g_drawingInfo.multiSelectedNodes.empty() && !g_drawingInfo.multiSelectedPixelMaps.empty()) {
        DoMultiSelectedAnimation(parentPositionX, parentPositionY, adjustSize, false);
    }
    FI_HILOGD("leave");
}

void DragDrawing::ScreenRotateAdjustDisplayXY(
    Rosen::Rotation rotation, Rosen::Rotation lastRotation, float &displayX, float &displayY)
{
    FI_HILOGI("enter");
    sptr<Rosen::Display> display = Rosen::DisplayManager::GetInstance().GetDisplayById(g_drawingInfo.displayId);
    if (display == nullptr) {
        FI_HILOGD("Get display info failed, display:%{public}d", g_drawingInfo.displayId);
        display = Rosen::DisplayManager::GetInstance().GetDisplayById(0);
        CHKPV(display);
    }
    int32_t width = display->GetWidth();
    int32_t height = display->GetHeight();
    if ((static_cast<int32_t>(lastRotation) + NUM_ONE) % NUM_FOUR == static_cast<int32_t>(rotation)) {
        int32_t temp = displayX;
        displayX = width - displayY;
        displayY = temp;
    } else if ((static_cast<int32_t>(lastRotation) + NUM_TWO) % NUM_FOUR == static_cast<int32_t>(rotation)) {
        displayX = width - displayX;
        displayY = height - displayY;
    } else {
        int32_t temp = displayY;
        displayY = height - displayX;
        displayX = temp;
    }
    FI_HILOGI("leave");
}

void DragDrawing::UpdateDragDataForSuperHub(const DragData &dragData)
{
    CALL_DEBUG_ENTER;
    g_dragDataForSuperHub.extraInfo = dragData.extraInfo;
    g_dragDataForSuperHub.sourceType = dragData.sourceType;
    g_dragDataForSuperHub.displayX = dragData.displayX;
    g_dragDataForSuperHub.displayY = dragData.displayY;
    g_dragDataForSuperHub.dragNum = dragData.dragNum;
    g_dragDataForSuperHub.summarys = dragData.summarys;
}

std::shared_ptr<Media::PixelMap> DragDrawing::AccessGlobalPixelMapLocked()
{
    std::shared_lock<std::shared_mutex> lock(g_pixelMapLock);
    return g_drawingInfo.pixelMap;
}

void DragDrawing::UpdataGlobalPixelMapLocked(std::shared_ptr<Media::PixelMap> pixelmap)
{
    std::unique_lock<std::shared_mutex> lock(g_pixelMapLock);
    g_drawingInfo.pixelMap = pixelmap;
}

void DragDrawing::ScreenRotate(Rosen::Rotation rotation, Rosen::Rotation lastRotation)
{
    FI_HILOGI("enter, rotation:%{public}d, lastRotation:%{public}d", static_cast<int32_t>(rotation),
        static_cast<int32_t>(lastRotation));
    ScreenRotateAdjustDisplayXY(rotation, lastRotation, g_drawingInfo.x, g_drawingInfo.y);
    DrawRotateDisplayXY(g_drawingInfo.x, g_drawingInfo.y);

    if (g_drawingInfo.sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        ScreenRotateAdjustDisplayXY(
            rotation, lastRotation, g_drawingInfo.currentPositionX, g_drawingInfo.currentPositionY);
        UpdateMousePosition(g_drawingInfo.currentPositionX, g_drawingInfo.currentPositionY);
    }
    Rosen::RSTransaction::FlushImplicitTransaction();
    FI_HILOGI("leave");
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
    }, []() { FI_HILOGD("DoRotateDragWindowAnimation end"); });
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
    auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
    CHKPV(currentPixelMap);
    float scalingValue = GetScaling();
    if (SCALE_THRESHOLD_EIGHT < scalingValue || fabsf(SCALE_THRESHOLD_EIGHT - scalingValue) < EPSILON) {
        FI_HILOGE("Invalid scalingValue:%{public}f", scalingValue);
        return;
    }
    int32_t adjustSize = EIGHT_SIZE * scalingValue;
    int32_t svgTouchPositionX = currentPixelMap->GetWidth() + adjustSize - stylePixelMap_->GetWidth();
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
        FI_HILOGD("path:%{private}s", g_drawingInfo.filterInfo.path.c_str());
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
    auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
    CHKPV(currentPixelMap);
    int32_t pixelMapWidth = currentPixelMap->GetWidth();
    int32_t pixelMapHeight = currentPixelMap->GetHeight();
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
        cvs->DrawPixelMapWithParm(currentPixelMap, rsImageInfo, Rosen::Drawing::SamplingOptions());
        cvs->Restore();
    } else {
        cvs->DrawPixelMapWithParm(currentPixelMap, rsImageInfo, Rosen::Drawing::SamplingOptions());
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
    std::shared_ptr<Media::PixelMap> pixelMap = std::make_shared<Media::PixelMap>();
    int32_t ret = RET_ERR;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    ret = MMI::InputManager::GetInstance()->GetPointerSnapshot(&pixelMap);
#endif // OHOS_BUILD_ENABLE_MAGICCURSOR
    if (ret != RET_OK) {
        FI_HILOGE("Get pointer snapshot failed, ret:%{public}d", ret);
        pixelMap = DrawFromSVG();
    }
    CHKPV(pixelMap);
    OnDraw(pixelMap);
    FI_HILOGD("leave");
}

std::shared_ptr<Media::PixelMap> DrawMouseIconModifier::DrawFromSVG() const
{
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
    if (imageSource == nullptr) {
        FI_HILOGW("imageSource is null");
        return nullptr;
    }
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
    return imageSource->CreatePixelMap(decodeOpts, errCode);
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
    auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
    CHKPV(currentPixelMap);
    float pixelMapWidth = currentPixelMap->GetWidth();
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
    if (!g_drawingInfo.multiSelectedNodes.empty()) {
        size_t multiSelectedNodesSize = g_drawingInfo.multiSelectedNodes.size();
        for (size_t i = 0; i < multiSelectedNodesSize; ++i) {
            std::shared_ptr<Rosen::RSCanvasNode> multiSelectedNode = g_drawingInfo.multiSelectedNodes[i];
            CHKPV(multiSelectedNode);
            multiSelectedNode->SetAlpha(alpha_->Get());
            multiSelectedNode->SetScale(scale_->Get(), scale_->Get());
        }
    }
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
    auto currentPixelMap = DragDrawing::AccessGlobalPixelMapLocked();
    if (currentPixelMap == nullptr) {
        FI_HILOGE("pixelMap is nullptr");
        return DEFAULT_SCALING;
    }
    int32_t pixelMapWidth = currentPixelMap->GetWidth();
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

void DragDrawing::UpdateDragState(DragState dragState)
{
    dragState_ = dragState;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
