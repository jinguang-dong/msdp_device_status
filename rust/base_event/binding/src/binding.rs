/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

//! rust base event sys

use super::*;

const POINTER_ACTION_PULL_MOVE: i32 = 13;
const POINTER_ACTION_PULL_UP: i32 = 14;
use hilog_rust::{error, hilog, debug, HiLogLabel, LogType};
const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "Binding"
};

/// struct CPointerEvent
#[repr(C)]
pub struct CPointerEvent {
    _private: [u8; 0],
}

/// struct CPixelMap
#[repr(C)]
pub struct CPixelMap {
    _private: [u8; 0],
}

/// struct CPointerStyleColor
#[repr(C)]
pub struct CPointerStyleColor {
    r: u8,
    g: u8,
    b: u8,
}

/// struct CPointerStyle
#[repr(C)]
pub struct CPointerStyle {
    size: i32,
    color: CPointerStyleColor,
    id: i32,
}

impl CPointerStyle {
    /// Create a CPointerStyle object
    pub fn new() -> Self {
        Self {
            size: -1, 
            color: { CPointerStyleColor {
                r: 0,
                g: 0,
                b: 0,
            } },
            id: 0,
        }
    }
}

/// struct CExtraData
#[repr(C)]
pub struct CExtraData {
    /// add struct field documentation.
    pub appended: bool,
    /// add struct field documentation.
    pub buffer: *const u8,
    /// add struct field documentation.
    pub buffer_size: usize,
    /// add struct field documentation.
    pub source_type: i32,
    /// add struct field documentation.
    pub pointer_id: i32,
}


// Callback function type for OnDragCallback() from native,
// this callback is invoked when listening for a pointer event.
pub type OnDragCallback = unsafe extern "C" fn(
    event: *const CPointerEvent
);

/// Callback when the pointer event is received
///
/// # Safety
///
/// 123456789
/// 987654321
pub unsafe extern "C" fn drag_callback(event: *const CPointerEvent) {
    debug!(LOG_LABEL, "enter, drag_callback");
    match CGetPointerAction(event) {
        POINTER_ACTION_PULL_MOVE => on_drag_move(event),
        POINTER_ACTION_PULL_UP => on_drag_up(event),
        _ =>  {
            debug!(LOG_LABEL, "Unknow action, sourceType:{}, pointerId:{}, pointerAction:{}",
                   CGetSourceType(event), CGetPointerId(event), CGetPointerAction(event));
        },
    }
}

fn on_drag_move(_event: *const CPointerEvent) {
    error!(LOG_LABEL, "this is on_drag_move test");
    todo!()
}

fn on_drag_up(_event: *const CPointerEvent) {
    error!(LOG_LABEL, "this is on_drag_up test");
    todo!()
}

// C interface for PointerEvent
extern "C" {
    pub fn CGetPointerId(event: *const CPointerEvent) -> i32;
    pub fn CGetPointerAction(event: *const CPointerEvent) -> i32;
    pub fn CGetTargetWindowId(event: *const CPointerEvent) -> i32;
    pub fn CGetSourceType(event: *const CPointerEvent) -> i32;
    pub fn CGetTargetDisplayId(event: *const CPointerEvent) -> i32;
    pub fn CGetDisplayX(event: *const CPointerEvent) -> i32;
    pub fn CGetDisplayY(event: *const CPointerEvent) -> i32;
}

// C interface for InputManager
extern "C" {
    pub fn CAddMonitor(callback: OnDragCallback) -> i32;
    pub fn CGetWindowPid(event: *const CPointerEvent) -> i32;
    pub fn CGetPointerStyle(pointer_style: &mut CPointerStyle) -> i32;
    pub fn CAppendExtraData(extra_data: &CExtraData);
    pub fn CSetPointerVisible(visible: bool) -> i32;
}