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

#ifndef C_BASE_EVENT_H
#define C_BASE_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

struct CPointerEvent;
struct CPixelMap;

struct CPointerStyle;
struct CPointerStyleColor;
struct CExtraData;

// PointerEvent
int CGetPointerId(const CPointerEvent* cPointerEvent);
int CGetPointerAction(const CPointerEvent* cPointerEvent);
int CGetTargetWindowId(const CPointerEvent* cPointerEvent);
int CGetSourceType(const CPointerEvent* cPointerEvent);
int CGetTargetDisplayId(const CPointerEvent* cPointerEvent);
int CGetDisplayX(const CPointerEvent* cPointerEvent);
int CGetDisplayY(const CPointerEvent* cPointerEvent);

// InputManager
int CAddMonitor(void (*callback)(CPointerEvent *));
int CGetWindowPid(const CPointerEvent* cPointerEvent);
int CGetPointerStyle(CPointerStyle* cPointerStyle);
void CAppendExtraData(CExtraData cExtraData);
int CSetPointerVisible(bool visible);

#ifdef __cplusplus
}
#endif
#endif // C_BASE_EVENT_H