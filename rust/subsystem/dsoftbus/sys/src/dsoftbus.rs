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
#![allow(unused_variables)]

use std::cell::RefCell;
use std::ffi::{c_void, c_char, c_uchar, CString, CStr};
use std::sync::{Once, Mutex, Arc, Condvar};
use std::collections::HashMap;
use std::time::Duration;
use fusion_data_rust::FusionResult;
use fusion_utils_rust::{ call_debug_enter };
use hilog_rust::{ error, info, hilog, HiLogLabel, LogType };
use crate::binding::{ ISessionListener, StreamData, StreamFrameInfo, SessionAttribute, NodeBasicInfo,
    MessageId,
    INTERCEPT_STRING_LENGTH,
    DINPUT_LINK_TYPE_MAX,
    DEVICE_ID_SIZE_MAX,
    SESSION_SIDE_SERVER,
    UNSUCCESSFUL,
    LINK_TYPE_WIFI_WLAN_5G,
    LINK_TYPE_WIFI_WLAN_2G,
    LINK_TYPE_WIFI_P2P,
    LINK_TYPE_BR,
    TYPE_BYTES,
    GetPeerDeviceId,
    GetSessionSide,
    OpenSession,
    CloseSession,
    CreateSessionServer,
    RemoveSessionServer,
    GetLocalNodeDeviceInfo,
    SendBytes,
};

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "DSoftbus"
};

extern "C" fn on_session_opened(session_id: i32, result: i32) -> i32 {
    match DSoftbus::get_instance() {
        Some(dsoftbus) => {
            match dsoftbus.on_session_opened(session_id, result) {
                Ok(_) => { 0 },
                Err(err) => { err },
            }
        }
        None => {
            error!(LOG_LABEL, "DSoftbus is none");
            -1
        }
    }
}

extern "C" fn on_session_closed(session_id: i32) {
    match DSoftbus::get_instance() {
        Some(dsoftbus) => {
            dsoftbus.on_session_closed(session_id);
        }
        None => {
            error!(LOG_LABEL, "DSoftbus is none");
        }
    }
}

extern "C" fn on_bytes_received(session_id: i32, data: *const c_void, data_len: u32) {
    match DSoftbus::get_instance() {
        Some(dsoftbus) => {
            dsoftbus.on_bytes_received(session_id, data, data_len);
        }
        None => {
            error!(LOG_LABEL, "DSoftbus is none");
        }
    }
}

extern "C" fn on_message_received(session_id: i32, data: *const c_void, data_len: u32) {
    let _id = session_id;
    let _dt = data;
    let _len = data_len;
}

extern "C" fn on_stream_received(session_id: i32, data: *const StreamData, ext: *const StreamData, param: *const StreamFrameInfo) {
    let _id = session_id;
    let _dt = data;
    let _ext1 = ext;
    let _param1 = param;
}

impl From<MessageId> for i32 {
    fn from(value: MessageId) -> Self {
        match value {
            MessageId::MinId => 0,
            MessageId::DraggingData => 1,
            MessageId::StopdragData => 2,
            MessageId::MaxId => 50,
        }
    }
}

#[derive(Default)]
struct DSoftbusImpl {
    sess_listener: ISessionListener,
    local_session_name: String,
    session_dev_map: HashMap<String, i32>,
    channel_status_map: HashMap<String, bool>,
    operation_mutex: Mutex<HashMap<String, i32>>,
    wait_cond: Arc<(Mutex<bool>, Condvar)>,
}

impl DSoftbusImpl {
    /// TODO: add documentation.
    fn init(&mut self) -> FusionResult<i32> {
        call_debug_enter!("DSoftbus::init");
        let session_name_pre = String::from("ohos.msdp.device_status");
        self.sess_listener = ISessionListener {
            on_session_opened: Some(on_session_opened),
            on_session_closed: Some(on_session_closed),
            on_bytes_received: Some(on_bytes_received),
            on_message_received: Some(on_message_received),
            on_stream_received: Some(on_stream_received),
        };

        let local_network_id = self.local_network_id();
        if local_network_id.is_empty() {
            error!(LOG_LABEL, "init::Local networkid is empty");
            return Err(-1);
        }

        let session_name = session_name_pre + &local_network_id[0..INTERCEPT_STRING_LENGTH];
        if self.local_session_name.eq(&session_name) {
            info!(LOG_LABEL, "init::Session server has already created");
            return Ok(0);
        }

        let fi_pkg_name: String = String::from("ohos.msdp.fusioninteraction");
        if !self.local_session_name.is_empty() {
            error!(LOG_LABEL, "init::Remove last sesison server, sessionName:{}", @public(self.local_session_name));
            let ret = unsafe { RemoveSessionServer(fi_pkg_name.as_ptr(), self.local_session_name.as_ptr()) };
            if ret != 0 {
                error!(LOG_LABEL, "init::Remove session server failed, error code:{}", @public(ret));
            }
        } 

        self.local_session_name = session_name;  
        let ret: i32 = unsafe { CreateSessionServer(fi_pkg_name.as_ptr(), self.local_session_name.as_ptr(), &self.sess_listener) };
        if ret != 0 {
            error!(LOG_LABEL, "init::Create session server failed, error code:{}", @public(ret));
            return Err(-1);
        }
        Ok(0)
    }

    fn local_network_id(&mut self) -> String {
        call_debug_enter!("DSoftbus::local_network_id");
        let mut local_node = NodeBasicInfo {
            network_id: [0; 65],
            device_name: [0; 128],
            device_type_id: 0,
        };

        let fi_pkg_name: String = String::from("ohos.msdp.fusioninteraction");
        let ret = unsafe { GetLocalNodeDeviceInfo(fi_pkg_name.as_ptr(), &mut local_node as *mut NodeBasicInfo) };
        if ret != 0 {
            error!(LOG_LABEL, "local_network_id::GetLocalNodeDeviceInfo result:{}", @public(ret));
            return String::from("");
        }

        let local_node_ptr: *const c_uchar =  local_node.network_id.as_ptr() as *const c_uchar;
        unsafe{ return CStr::from_ptr(local_node_ptr).to_str().unwrap().to_owned() };
    }

    /// TODO: add documentation.
    fn release(&mut self) {
        call_debug_enter!("DSoftbus::release");      
        for (_key, value) in self.session_dev_map.iter() {
            unsafe { CloseSession(*value) };
        }
        let fi_pkg_name: String = String::from("ohos.msdp.fusioninteraction");
        unsafe { RemoveSessionServer(fi_pkg_name.as_ptr(), self.local_session_name.as_ptr()) };

        self.session_dev_map.clear();
        self.channel_status_map.clear();
    }

    /// TODO: add documentation.
    fn open_input_softbus(&mut self, remote_network_id: &String) -> FusionResult<i32> {
        call_debug_enter!("DSoftbus::open_input_softbus");
        let session_name = String::from("ohos.msdp.device_status");
        let group_id = String::from("fi_softbus_group_id");

        if self.check_device_session_state(remote_network_id) {
            error!(LOG_LABEL, "open_input_softbus::Softbus session has already opened");
            return Ok(0);
        }

        let init_ret = self.init();
        if init_ret != Ok(0) {
            error!(LOG_LABEL, "open_input_softbus::init failed");
            return Err(-1);
        }

        let peer_session_name = session_name + &remote_network_id[0..INTERCEPT_STRING_LENGTH];
        let data: u8 = 0;
        let session_attr =  SessionAttribute {
            data_type: TYPE_BYTES,
            link_type_num: DINPUT_LINK_TYPE_MAX,
            link_type: [LINK_TYPE_WIFI_WLAN_2G, LINK_TYPE_WIFI_WLAN_5G, LINK_TYPE_WIFI_P2P,
            LINK_TYPE_BR, 0, 0, 0, 0, 0],
            stream_attr: 0,
            fast_trans_data: data as *const u8,
            fast_trans_data_size: 0,
        };  
        let session_id = unsafe { OpenSession(self.local_session_name.as_ptr() as *const c_char,
            peer_session_name.as_ptr() as *const c_char, remote_network_id.as_ptr() as *const c_char,
            group_id.as_ptr() as *const c_char, &session_attr as *const SessionAttribute) };
        if session_id < 0 {
            error!(LOG_LABEL, "open_input_softbus::OpenSession failed, session_id:{}", @public(session_id));
            return Err(-1);
        }

        self.wait_session_opend(remote_network_id, session_id)
    }

    /// TODO: add documentation.
    fn close_input_softbus(&mut self, remote_network_id: &String) {
        call_debug_enter!("DSoftbus::close_input_softbus");
        let get_result: Option<&i32> = self.session_dev_map.get(remote_network_id);
        if get_result.is_none() {
            error!(LOG_LABEL, "close_input_softbus::SessionDevIdMap not found");
        }
        let session_id = get_result.copied().unwrap();
        unsafe { CloseSession(session_id) };
        self.session_dev_map.remove(remote_network_id);
        self.channel_status_map.remove(remote_network_id);
    }

    /// TODO: add documentation.
    fn wait_session_opend(&mut self, remote_network_id: &String, session_id: i32) -> FusionResult<i32> {
        call_debug_enter!("DSoftbus::wait_session_opend");
        self.session_dev_map.insert(remote_network_id.to_string(), session_id);
        self.wait_cond = Arc::new((Mutex::new(false), Condvar::new()));
        let pair = Arc::clone(&self.wait_cond);
        let (lock, cvar) = &*pair;
        let result = (cvar.wait_timeout(self.operation_mutex.lock().unwrap(), Duration::from_secs(5))).unwrap();

        let get_result = self.channel_status_map.get(remote_network_id);
        if get_result.is_some() && !get_result.copied().unwrap(){
            error!(LOG_LABEL, "wait_session_opend::OpenSession timeout");    
            return Err(-1);
        }
        self.channel_status_map.insert(remote_network_id.to_string(), false);
        Ok(0)
    }

    /// TODO: add documentation.
    fn on_session_opened(&mut self, session_id: i32, result: i32) -> FusionResult<i32> {
        call_debug_enter!("DSoftbus::on_session_opened");
        let mut peer_dev_id: String = String::from("00000000000000000000000000000000000000000000000000000000000000000");
        let getpeer_device_id_result: i32 = unsafe { GetPeerDeviceId(session_id, peer_dev_id.as_mut_ptr() as *mut c_char,
            ((std::mem::size_of::<c_char>()) * DEVICE_ID_SIZE_MAX).try_into().unwrap()) };

        if result != 0 {
            let session_id_temp = &session_id;
            let device_id: String = self.find_device(*session_id_temp);
            let get_result: Option<&i32> = self.session_dev_map.get(&device_id);
            if get_result.is_some() {
                self.session_dev_map.remove(&device_id);
            }

            if getpeer_device_id_result == 0 {
                self.channel_status_map.insert(peer_dev_id, true);
            }
            self.wait_cond.1.notify_all();
            return Ok(0);
        }
        let session_id_temp = &session_id;
        let session_side: i32 = unsafe { GetSessionSide(*session_id_temp) };
        if session_side == SESSION_SIDE_SERVER{
            if getpeer_device_id_result == 0 {
                self.session_dev_map.insert(peer_dev_id, session_id);
            }
        }
        else if getpeer_device_id_result == 0 {         
            self.channel_status_map.insert(peer_dev_id, true);
            self.wait_cond.1.notify_all();
        }
        Ok(0)
    }

    /// TODO: add documentation.
    fn find_device(&self, session_id: i32) -> String {
        call_debug_enter!("DSoftbus::find_device");
        for (key, value) in self.session_dev_map.iter() {
            if *value == session_id{
                return key.to_string();
            }
        }
        error!(LOG_LABEL, "find_device error");
        String::from("")
    }

    /// TODO: add documentation.
    fn on_session_closed(&mut self, session_id: i32) {
        call_debug_enter!("DSoftbus::on_session_closed");
        let device_id = self.find_device(session_id);
        let get_result: Option<&i32> = self.session_dev_map.get(&device_id);
        if get_result.is_some() {
            self.session_dev_map.remove(&device_id);
        }
        if unsafe { GetSessionSide(session_id) } != UNSUCCESSFUL {
            self.channel_status_map.remove(&device_id);
        }
    }

    /// TODO: add documentation.
    fn handle_session_data(&self, session_id: i32, message: String) {
        call_debug_enter!("handle_session_data");
        if message.is_empty(){
            error!(LOG_LABEL, "Message is empty");
            return;
        }
        let _id = session_id;
    }

    /// TODO: add documentation.
    fn on_bytes_received(&self, session_id: i32, data: *const c_void, data_len: u32) {
        call_debug_enter!("DSoftbus::on_bytes_received");
        if session_id < 0 || data.is_null()|| data_len == 0 {
           error!(LOG_LABEL, "Param check failed");
           return;
        }

        let dt = unsafe {CStr::from_ptr(data as *const c_char)};
        let dt_slice: &str = dt.to_str().unwrap();
        let message: String = dt_slice.to_owned();
        info!(LOG_LABEL, "on_bytes_received data:{}", @public(message));
        self.handle_session_data(session_id, message);
    }

    /// TODO: add documentation.
    fn check_device_session_state(&self, remote_network_id: &String) -> bool {
        call_debug_enter!("DSoftbus::check_device_session_state");
        let get_result: Option<&i32> = self.session_dev_map.get(remote_network_id);
        if get_result.is_none() {
            error!(LOG_LABEL, "Check session state error");
            return false;
        }
        true
    }

    /// TODO: add documentation.
    fn send_data(&self, device_id: &String, message_id: MessageId, data: *const c_void, data_len: u32) -> FusionResult<i32>{
        call_debug_enter!("DSoftbus::send_data");
        let result: i32 = unsafe {SendBytes(self.session_dev_map.get(device_id).copied().unwrap_or(0),
            data, data_len) };
        if result != 0 {
            error!(LOG_LABEL, "send_data::Send bytes failed, result:{}", @public(result));
            return Err(-1);
        }
        Ok(0)
    }
}

/// TODO: add documentation.
#[derive(Default)]
pub struct DSoftbus {
    dsoftbus_impl: Mutex<RefCell<DSoftbusImpl>>,
}

impl DSoftbus {
    /// TODO: add documentation.
    pub fn get_instance() -> Option<&'static Self> {
        static mut DSOFTBUS: Option<DSoftbus> = None;
        static INIT_ONCE: Once = Once::new();
        unsafe {
            INIT_ONCE.call_once(|| {
                DSOFTBUS = Some(DSoftbus::default());
            });
            DSOFTBUS.as_ref()
        }
    }

    /// TODO: add documentation.
    pub fn init(&self) -> FusionResult<i32> {
        match self.dsoftbus_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().init()
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                Err(-1)
            }
        }
    }
   
   /// TODO: add documentation.
   pub fn release(&self){
        match self.dsoftbus_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().release();
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
            }
        }
    }

    /// TODO: add documentation.
    pub fn open_input_softbus(&self, remote_network_id: &String) -> FusionResult<i32> {
        match self.dsoftbus_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().open_input_softbus(remote_network_id)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                Err(-1)
            }
        }
    }

     /// TODO: add documentation.
     pub fn close_input_softbus(&self, remote_network_id: &String) {
        match self.dsoftbus_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().close_input_softbus(remote_network_id);
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
            }
        }
    }   

    /// TODO: add documentation.
    pub fn wait_session_opend(&mut self, remote_network_id: &String, session_id: i32) -> FusionResult<i32> {
        match self.dsoftbus_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().wait_session_opend(remote_network_id, session_id)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                Err(-1)
            }
        }
    }

    /// TODO: add documentation.
    pub fn on_session_opened(&self, session_id: i32, result: i32) -> FusionResult<i32>{
        match self.dsoftbus_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().on_session_opened(session_id, result)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                Err(-1)
            }
        }
    }

    /// TODO: add documentation.
    pub fn on_session_closed(&self, session_id: i32) {
        match self.dsoftbus_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().on_session_closed(session_id);
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
            }
        }
    }

    /// TODO: add documentation.
    pub fn find_device(&self, session_id: i32) -> String {
        match self.dsoftbus_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().find_device(session_id)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                String::from("")
            }
        }
    }

    /// TODO: add documentation.
    pub fn handle_session_data(&self, session_id: i32, message: String) {
        match self.dsoftbus_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().handle_session_data(session_id, message);
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
            }
        }
    }

    /// TODO: add documentation.
    pub fn on_bytes_received(&self, session_id: i32, data: *const c_void, data_len: u32) {
        match self.dsoftbus_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().on_bytes_received(session_id, data, data_len);
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
            }
        }
    }

    /// TODO: add documentation.
    pub fn check_device_session_state(&self, remote_network_id: &String) -> bool {
        match self.dsoftbus_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().check_device_session_state(remote_network_id)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                false
            }
        }
    }

    /// TODO: add documentation.
    pub fn send_data(&self, device_id: &String, message_id: MessageId, data: *const c_void, data_len: u32) -> FusionResult<i32>{
        match self.dsoftbus_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().send_data(device_id, message_id, data, data_len)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                Err(-1)
            }
        }
    }

    /// TODO: add documentation.
    pub fn on_message_received(&self, session_id: i32, data: *const c_void, data_len: u32) {

    }

    /// TODO: add documentation.
    pub fn on_stream_received(&self, session_id: i32, data: *const StreamData,
        ext: *const StreamData, param: *const StreamFrameInfo){

    }
}