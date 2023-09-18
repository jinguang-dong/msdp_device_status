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

//! Binding for calling c++

#![allow(dead_code)]

use std::ffi::{ c_void, c_char };

/// struct of CBusinessEvent
#[repr(C)]
pub struct CBusinessEvent {
    /// number of vec elements
    pub pressed_keys_len: usize,
    /// pointer of vec 
    pub pressed_keys: *const i32,
    /// key_code
    pub key_code: i32,
    /// key_action
    pub key_action: i32,
}

/// callback type CallbackInput
pub type CallbackInput = unsafe extern "C" fn (bool, *mut Callbacks);
/// struct of Callbacks
#[repr(C)]
pub struct Callbacks {
    _private:[u8; 0],
}

/// type DInputCb
pub type DInputCb = extern "C" fn (devId: *const c_char, status: i32, id: usize, userdata: *mut c_void);

extern "C" {
    /// interface of PrepareRemoteInput
    pub fn PrepareRemoteInput(srcId: *const c_char, sinkId: *const c_char, callback: Option<DInputCb>,
        id: usize, userdata: *mut c_void) -> i32;
    /// interface of UnPrepareRemoteInput
    pub fn UnPrepareRemoteInput(srcId: *const c_char, sinkId: *const c_char, callback: Option<DInputCb>,
        id: usize, userdata: *mut c_void) -> i32;
    /// interface of StartRemoteInput
    pub fn StartRemoteInput(srcId: *const c_char, sinkId: *const c_char, dhIds: *const *const c_char, nDhIds: usize,
        callback: Option<DInputCb>, id: usize, userdata: *mut c_void) -> i32;
    /// interface of StopRemoteInput
    pub fn StopRemoteInput(srcId: *const c_char, sinkId: *const c_char, dhIds: *const *const c_char, nDhIds: usize,
        callback: Option<DInputCb>, id: usize, userdata: *mut c_void) -> i32;
    /// interface of IsNeedFilterOut
    pub fn IsNeedFilterOut(sinkId: *const c_char, event: *const CBusinessEvent) -> i32;
}

