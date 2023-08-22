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

//! TODO: add documentation.

#![allow(dead_code)]
#![allow(missing_docs)]

use std::ffi::{ c_void, c_char };

///INTERCEPT_STRING_LENGTH
pub const INTERCEPT_STRING_LENGTH: usize = 20;

///DINPUT_LINK_TYPE_MAX
pub const DINPUT_LINK_TYPE_MAX: i32 = 4;

///DEVICE_ID_SIZE_MAX
pub const DEVICE_ID_SIZE_MAX: usize = 65;

///SESSION_SIDE_SERVER
pub const SESSION_SIDE_SERVER: i32 = 0;

///UNSUCCESSFUL
pub const UNSUCCESSFUL: i32 = 0;

///link_type
///LINK_TYPE_WIFI_WLAN_5G
pub const LINK_TYPE_WIFI_WLAN_5G: i32 = 1;
///LINK_TYPE_WIFI_WLAN_2G
pub const LINK_TYPE_WIFI_WLAN_2G: i32 = 2;
///LINK_TYPE_WIFI_P2P
pub const LINK_TYPE_WIFI_P2P: i32 = 3;
///LINK_TYPE_BR
pub const LINK_TYPE_BR: i32 = 4;
///LINK_TYPE_MAX
pub const LINK_TYPE_MAX: usize = 9;

// for SessionType enum 
///TYPE_MESSAGE
pub const TYPE_MESSAGE: i32 = 1;
///TYPE_BYTES
pub const TYPE_BYTES: i32 = 2;
///TYPE_FILE
pub const TYPE_FILE: i32 = 3;
///TYPE_STREAM
pub const TYPE_STREAM: i32 = 4;
///TYPE_BUTT
pub const TYPE_BUTT: i32 = 5;
///NETWORK_ID_BUF_LEN
pub const NETWORK_ID_BUF_LEN: i32 = 65;
///DEVICE_NAME_BUF_LEN
pub const DEVICE_NAME_BUF_LEN: i32 = 128;

/// for adapter
pub const FILTER_WAIT_TIMEOUT_SECOND: u64 = 1;
pub const MSG_MAX_SIZE: usize = 45 * 1024;
pub const STREAM_BUF_READ_FAIL: i32 = 1;
pub const MAX_STREAM_BUF_SIZE: usize = 256;
pub const PARAM_INPUT_INVALID: i32 = 5;
pub const MEM_OUT_OF_BOUNDS: i32 = 3;

/// TODO: add documentation.
#[repr(C)]
pub struct NodeBasicInfo {
    pub network_id: [i8; NETWORK_ID_BUF_LEN as usize],
    pub device_name: [i8; DEVICE_NAME_BUF_LEN as usize],
    pub device_type_id: u16,
}

/// TODO: add documentation.
#[repr(C)]
pub struct SessionAttribute {
    pub data_type: i32,
    pub link_type_num: i32,
    pub link_type: [i32; LINK_TYPE_MAX],
    pub stream_attr: i32,
    pub fast_trans_data: *const u8,
    pub fast_trans_data_size: u16,
}

/// TODO: add documentation.
#[repr(C)]
pub struct StreamData {
    pub buf_data: c_char,
    pub buf_len: i32,
}

/// TODO: add documentation.
#[repr(C)]
pub struct DataPacket {
    pub message_id: MessageId,
    pub buf_len: u32,
    pub data: Vec<*const c_char>,
}

/// TODO: add documentation.
#[repr(C)]
pub enum MessageId {
    MinId,
    DraggingData,
    StopdragData,
    MaxId,
}

// for adapter
#[repr(C)]
pub enum ErrorStatus {
    ErrorStatusOk,
    ErrorStatusRead,
    ErrorStatusWrite,
}

/// TODO: add documentation.
#[repr(C)]
pub struct StreamFrameInfo {
    pub frame_type: i32,
    pub time_stamp: i64,
    pub seq_num: i32,
    pub seq_sub_num: i32,
    pub level_num: i32,
    pub bit_map: i32,
    pub tv_count: i32,
    pub tv_list: i32,
}

/// TODO: add documentation.
pub type OnSessionOpened = extern "C" fn (session_id: i32, resultValue: i32) -> i32;
/// TODO: add documentation.
pub type OnSessionClosed = extern "C" fn (session_id: i32);
/// TODO: add documentation.
pub type OnBytesReceived = extern "C" fn (session_id: i32, byteData: *const c_void, data_len: u32);
/// TODO: add documentation.
pub type OnMessageReceived = extern "C" fn (session_id: i32, byteData: *const c_void, data_len: u32);
/// TODO: add documentation.
pub type OnstreamReceived = extern "C" fn (session_id: i32, byteData: *const StreamData,
    extData: *const StreamData, paramData: *const StreamFrameInfo);

/// TODO: add documentation.
#[repr(C)]
#[derive(Default)]
pub struct ISessionListener {
    pub on_session_opened: Option<OnSessionOpened>,
    pub on_session_closed: Option<OnSessionClosed>,
    pub on_bytes_received: Option<OnBytesReceived>,
    pub on_message_received: Option<OnMessageReceived>,
    pub on_stream_received: Option<OnstreamReceived>,
}

/// struct CPointerEvent
#[repr(C)]
pub struct CPointerEvent {
    /// Corresponding to the C-side 'void' type, one way to avoid using 'unsafe'
    _private: [u8; 0],
}

/// struct CPointerEvent
#[repr(C)]
pub struct CJsonStruct {
    /// Corresponding to the C-side 'void' type, one way to avoid using 'unsafe'
    _private: [u8; 0],
}

/// TODO: add documentation.
#[repr(C)]
pub enum CoordStatusType{
    RemoteCoordinationStart = 1,
    RemoteCoordinationStartRes = 2,
    RemoteCoordinationStop = 3,
    RemoteCoordinationStopRes = 4,
    RemoteCoordinationStopOtherRes = 5,
    NotifyUnChainedRes = 6,
    NotifyFilterAdded = 7
}

// C interface for coordination_sm
extern "C" {
    pub fn CGetLastPointerEvent() -> *const CPointerEvent;
    pub fn CGetPressedButtons(cPointerEvent: *const CPointerEvent) -> bool;
}

// C interface for adapter
extern "C" {
    pub fn CGetCJsonObj() -> *const CJsonStruct;
    pub fn CAddNumber(cJsonObj: *const CJsonStruct, value: i32, str: *const c_char) -> bool;
    pub fn CAddbool(cJsonObj: *const CJsonStruct, value: bool, str: *const c_char) -> bool;
    pub fn CAddString(cJsonObj: *const CJsonStruct, value: *const c_char, str: *const c_char) -> bool;
    pub fn CJsonPrint(cJsonObj: *const CJsonStruct, msg: *mut c_char) ->bool;
    pub fn CJsonDelete(cJsonObj: *const CJsonStruct);
    pub fn CJsonFree(str: *const c_char);
}

// C interface for main
extern "C" {
    pub fn GetAccessToken();
}

// C interface for session
extern "C" {
    pub fn CreateSessionServer(pkg_name: *const c_char, session_name: *const c_char, session_listener: *const ISessionListener) -> i32;
    pub fn RemoveSessionServer(pkg_name: *const c_char, session_name: *const c_char) -> i32;
    pub fn CloseSession(session_id: i32);
    pub fn OpenSession(my_session_name: *const c_char, peer_session_name: *const c_char, peer_device_id: *const c_char,
        groupId: *const c_char, attr: *const SessionAttribute) -> i32;
    pub fn GetPeerDeviceId(session_id: i32, peer_dev_id: *mut c_char, len: u32) -> i32;
    pub fn GetSessionSide(session_id: i32) -> i32;
    pub fn GetLocalNodeDeviceInfo(pkg_name: *const c_char, info: *mut NodeBasicInfo) -> i32;
    pub fn SendBytes(session_id: i32, data: *const c_void, len: u32) -> i32;
}
