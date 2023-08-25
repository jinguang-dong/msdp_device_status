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

//! rust input binding sys

#![allow(dead_code)]
#![allow(missing_docs)]

use std::ffi::{ c_void, c_char };

///for dsoftbus
pub const INTERCEPT_STRING_LENGTH: usize = 20;
pub const DINPUT_LINK_TYPE_MAX: i32 = 4;
pub const DEVICE_ID_SIZE_MAX: usize = 65;
pub const SESSION_SIDE_SERVER: i32 = 0;
pub const SUCCESSFUL: i32 = 0;
pub const LINK_TYPE_WIFI_WLAN_5G: i32 = 1;
pub const LINK_TYPE_WIFI_WLAN_2G: i32 = 2;
pub const LINK_TYPE_WIFI_P2P: i32 = 3;
pub const LINK_TYPE_BR: i32 = 4;
pub const LINK_TYPE_MAX: usize = 9;
pub const TYPE_MESSAGE: i32 = 1;
pub const TYPE_BYTES: i32 = 2;
pub const TYPE_FILE: i32 = 3;
pub const TYPE_STREAM: i32 = 4;
pub const TYPE_BUTT: i32 = 5;
pub const NETWORK_ID_BUF_LEN: i32 = 65;
pub const DEVICE_NAME_BUF_LEN: i32 = 128;

/// for adapter
pub const FILTER_WAIT_TIMEOUT_SECOND: u64 = 1;
pub const MSG_MAX_SIZE: usize = 45 * 1024;
pub const STREAM_BUF_READ_FAIL: i32 = 1;
pub const MAX_STREAM_BUF_SIZE: usize = 256;
pub const PARAM_INPUT_INVALID: i32 = 5;
pub const MEM_OUT_OF_BOUNDS: i32 = 3;

/// struct for dsoftbus
#[repr(C)]
pub struct NodeBasicInfo {
    pub network_id: [i8; NETWORK_ID_BUF_LEN as usize],
    pub device_name: [i8; DEVICE_NAME_BUF_LEN as usize],
    pub device_type_id: u16,
}

/// struct for dsoftbus
#[repr(C)]
pub struct SessionAttribute {
    pub data_type: i32,
    pub link_type_num: i32,
    pub link_type: [i32; LINK_TYPE_MAX],
    pub stream_attr: i32,
    pub fast_trans_data: *const u8,
    pub fast_trans_data_size: u16,
}

/// struct for dsoftbus
#[repr(C)]
pub struct StreamData {
    pub buf_data: c_char,
    pub buf_len: i32,
}

/// struct for dsoftbus
#[repr(C)]
pub struct DataPacket {
    pub message_id: MessageId,
    pub buf_len: u32,
    pub data: Vec<*const c_char>,
}

/// enum for adapter
#[repr(C)]
#[derive(Eq, Hash, PartialEq)]
#[derive(Copy, Clone)]
pub enum MessageId {
    MinId,
    DraggingData,
    StopdragData,
    IsPullUp,
    MaxId,
}

/// enum for adapter
#[repr(C)]
pub enum ErrorStatus {
    ErrorStatusOk,
    ErrorStatusRead,
    ErrorStatusWrite,
}

/// struct for dsoftbus
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

/// struct for dsoftbus
#[repr(C)]
#[derive(Default)]
pub struct ISessionListener {
    pub on_session_opened: Option<OnSessionOpened>,
    pub on_session_closed: Option<OnSessionClosed>,
    pub on_bytes_received: Option<OnBytesReceived>,
    pub on_message_received: Option<OnMessageReceived>,
    pub on_stream_received: Option<OnstreamReceived>,
}

/// struct for adapter
#[repr(C)]
pub struct CPointerEvent {
    /// Corresponding to the C-side 'void' type, one way to avoid using 'unsafe'
    _private: [u8; 0],
}

/// struct for adapter
#[repr(C)]
pub struct CJsonStruct {
    /// Corresponding to the C-side 'void' type, one way to avoid using 'unsafe'
    _private: [u8; 0],
}

/// enum for adapter
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

/// callback type OnSessionOpened
pub type OnSessionOpened = extern "C" fn (session_id: i32, resultValue: i32) -> i32;
/// callback type OnSessionClosed
pub type OnSessionClosed = extern "C" fn (session_id: i32);
/// callback type OnBytesReceived
pub type OnBytesReceived = extern "C" fn (session_id: i32, byteData: *const c_void, data_len: u32);
/// callback type OnMessageReceived
pub type OnMessageReceived = extern "C" fn (session_id: i32, byteData: *const c_void, data_len: u32);
/// callback type OnstreamReceived
pub type OnstreamReceived = extern "C" fn (session_id: i32, byteData: *const StreamData,
    extData: *const StreamData, paramData: *const StreamFrameInfo);
pub type OnHandleRecvData = extern "C" fn (session_id: i32, message: *const c_char);

// C interface for adapter,function definition in coordination_sm_rust.cpp
extern "C" {
    /// interface of CGetLastPointerEvent
    pub fn CGetLastPointerEvent() -> *const CPointerEvent;
    /// interface of CGetPressedButtons
    pub fn CGetPressedButtons(cPointerEvent: *const CPointerEvent) -> bool;
}

// C interface for adapter,function definition in message_packing.cpp
extern "C" {
    /// interface of CGetCJsonObj
    pub fn CGetCJsonObj() -> *mut CJsonStruct;
    /// interface of CAddNumber
    pub fn CAddNumber(cJsonObj: *mut CJsonStruct, value: i32, str: *const c_char) -> bool;
    /// interface of CAddbool
    pub fn CAddbool(cJsonObj: *mut CJsonStruct, value: bool, str: *const c_char) -> bool;
    /// interface of CAddString
    pub fn CAddString(cJsonObj: *mut CJsonStruct, value: *const c_char, str: *const c_char) -> bool;
    /// interface of CJsonPrint
    pub fn CJsonPrint(cJsonObj: *const CJsonStruct, msg: *mut c_char) ->bool;
    /// interface of CJsonDelete
    pub fn CJsonDelete(cJsonObj: *mut CJsonStruct);
    /// interface of CJsonFree
    pub fn CJsonFree(str: *const c_char);
    /// interface of CSaveHandleCb
    pub fn CSaveHandleCb(call_back: Option<OnHandleRecvData>);
    /// interface of CGetHandleCb
    pub fn CGetHandleCb() -> Option<OnHandleRecvData>;
    /// interface of CParse
    pub fn CParse(message: *const c_char, cJsonObj: *mut CJsonStruct);
    /// interface of CIsJsonObj
    pub fn CIsJsonObj(cJsonObj: *mut CJsonStruct) -> bool;
    /// interface of CGetObjectItemCaseSensitive
    pub fn CGetObjectItemCaseSensitive(cJsonObj: *mut CJsonStruct, cmd_type: *const c_char,
        com_type: *mut CJsonStruct) -> bool;
    /// interface of CIsNumber
    pub fn CIsNumber(cJsonObj: *mut CJsonStruct) -> bool;
    /// interface of CIsBool
    pub fn CIsBool(cJsonObj: *mut CJsonStruct) -> bool;
    /// interface of CIsString
    pub fn CIsString(cJsonObj: *mut CJsonStruct) -> bool;
    /// interface of CIsTrue
    pub fn CIsTrue(cJsonObj: *mut CJsonStruct) -> bool;
    /// interface of CGetValueInt
    pub fn CGetValueInt(cJsonObj: *mut CJsonStruct) -> i32;
    /// interface of CGetValueString
    pub fn CGetValueString(cJsonObj: *mut CJsonStruct) -> *const c_char;
    /// interface of CGetValueBool
    //pub fn CGetValueBool(cJsonObj: *const CJsonStruct) -> bool;
    /// interface of CStartRemoteCoordinationResult
    pub fn CStartRemoteCoordinationResult(is_success: bool, start_device_dhid: *const c_char, x_percent: i32,
        y_percent: i32);
    /// interface of CStartRemoteCoordination
    pub fn CStartRemoteCoordination(remoteNetworkId: *const c_char, buttonIsPressed: bool);
}

// C interface for main
extern "C" {
    /// interface of GetAccessToken
    pub fn GetAccessToken();
}

// C interface for dsoftbus,function definition in lib: dsoftbus:softbus_client
extern "C" {
    /// interface of CreateSessionServer
    pub fn CreateSessionServer(pkg_name: *const c_char, session_name: *const c_char, session_listener: *const ISessionListener) -> i32;
    /// interface of RemoveSessionServer
    pub fn RemoveSessionServer(pkg_name: *const c_char, session_name: *const c_char) -> i32;
    /// interface of CloseSession
    pub fn CloseSession(session_id: i32);
    /// interface of OpenSession
    pub fn OpenSession(my_session_name: *const c_char, peer_session_name: *const c_char, peer_device_id: *const c_char,
        groupId: *const c_char, attr: *const SessionAttribute) -> i32;
    /// interface of GetPeerDeviceId
    pub fn GetPeerDeviceId(session_id: i32, peer_dev_id: *mut c_char, len: u32) -> i32;
    /// interface of GetSessionSide
    pub fn GetSessionSide(session_id: i32) -> i32;
    /// interface of GetLocalNodeDeviceInfo
    pub fn GetLocalNodeDeviceInfo(pkg_name: *const c_char, info: *mut NodeBasicInfo) -> i32;
    /// interface of SendBytes
    pub fn SendBytes(session_id: i32, data: *const c_void, len: u32) -> i32;
}
