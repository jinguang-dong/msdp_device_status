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

//! adapter for coordination

#![allow(dead_code)]
#![allow(unused_variables)]

use std::cell::RefCell;
use std::ffi::{c_void, c_char, CString};
use std::sync::{Once, Mutex,Arc};
use std::time::Duration;
use fusion_utils_rust::{call_debug_enter};
use hilog_rust::{error, hilog, HiLogLabel, LogType};
use crate::dsoftbus::{DSoftbus};
use crate::binding::{ 
    FILTER_WAIT_TIMEOUT_SECOND,
    MSG_MAX_SIZE,
    CGetLastPointerEvent,
    CGetPressedButtons,
    SendBytes,
    CoordStatusType,
    CGetCJsonObj,
    CAddNumber,
    CAddbool,
    CAddString,
    CJsonPrint,
    CJsonDelete,
    CJsonFree,
};

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "Adapter"
};

/// Provide for cpp to call
/// # Safety
#[no_mangle]
extern "C" fn StartRemoteCoordination(local_network_id: &String, remote_network_id: &String) -> i32 {
    match Adapter::get_instance() {
        Some(adapter) => {
           adapter.start_remote_coordination(local_network_id, remote_network_id)             
        }
        None => {
            error!(LOG_LABEL, "Adapter is none");
            -1
        }
    }
}

/// Provide for cpp to call
/// # Safety
#[no_mangle]
extern "C" fn StartRemoteCoordinationResult(remote_network_id: &String, is_success: bool,
    start_device_dhid: &String, x_percent: i32, y_percent: i32) -> i32 {
    match Adapter::get_instance() {
        Some(adapter) => {
            adapter.start_remote_coordination_result(remote_network_id, is_success, start_device_dhid, x_percent,
                y_percent)
        }
        None => {
            error!(LOG_LABEL, "Adapter is none");
            -1
        }
    }
}

/// Provide for cpp to call
/// # Safety
#[no_mangle]
extern "C" fn StopRemoteCoordination(remote_network_id: &String, is_unchained: bool) -> i32 {
    match Adapter::get_instance() {
        Some(adapter) => {
            adapter.stop_remote_coordination(remote_network_id, is_unchained)
        }
        None => {
            error!(LOG_LABEL, "Adapter is none");
            -1
        }
    }
}

/// Provide for cpp to call
/// # Safety
#[no_mangle]
extern "C" fn StopRemoteCoordinationResult(remote_network_id: &String, is_success: bool) -> i32 {
    match Adapter::get_instance() {
        Some(adapter) => {
            adapter.stop_remote_coordination_result(remote_network_id, is_success)             
        }
        None => {
            error!(LOG_LABEL, "Adapter is none");
            -1
        }
    }
}

/// Provide for cpp to call
/// # Safety
#[no_mangle]
extern "C" fn NotifyUnchainedResult(local_network_id: &String, remote_network_id: &String, is_success: bool) -> i32 {
    match Adapter::get_instance() {
        Some(adapter) => {
           adapter.notify_unchained_result(local_network_id, remote_network_id, is_success)
        }
        None => {
            error!(LOG_LABEL, "Adapter is none");
            -1
        }
    }
}

/// Provide for cpp to call
/// # Safety
#[no_mangle]
extern "C" fn NotifyFilterAdded(remote_network_id: &String) -> i32 {
    match Adapter::get_instance() {
        Some(adapter) => {
            adapter.notify_filter_added(remote_network_id)
        }
        None => {
            error!(LOG_LABEL, "Adapter is none");
            -1
        }
    }
}

impl From<CoordStatusType> for i32 {
    fn from (value: CoordStatusType) -> Self {
        match value {
            CoordStatusType::RemoteCoordinationStart => 1,
            CoordStatusType::RemoteCoordinationStartRes => 2,
            CoordStatusType::RemoteCoordinationStop => 3,
            CoordStatusType::RemoteCoordinationStopRes => 4,
            CoordStatusType::RemoteCoordinationStopOtherRes => 5,
            CoordStatusType::NotifyUnChainedRes => 6,
            CoordStatusType::NotifyFilterAdded => 7,
        }
    }
}

/// struct AdapterImpl
#[derive(Default)]
struct AdapterImpl {
}

impl AdapterImpl {
    /// implementation of start_remote_coordination
    pub fn start_remote_coordination(&mut self, local_network_id: &String, remote_network_id: &String) -> i32 {
        call_debug_enter!("Adapter::start_remote_coordination");
        let session_id = match DSoftbus::get_instance() {
            Some(dsoftbus) => {
                let mut result = false;
                let map = dsoftbus.get_session_dev_map(&mut result);
                if  !result {
                    error!(LOG_LABEL, "get sessionDevMap failed");
                    return -1;
                }
                let get_result: Option<&i32> = map.get(remote_network_id);
                if get_result.is_none() {
                    error!(LOG_LABEL, "Start remote coordination error, not found this device");
                    return -1;
                }
                map[remote_network_id]                                
            }
            None => {
                error!(LOG_LABEL, "DSoftbus get_instance failed");
                return -1;
            }
        };
        
        let pointer_event = unsafe {
            CGetLastPointerEvent()
        };
        let is_pointer_button_pressed = unsafe {
            CGetPressedButtons(pointer_event)
        };

        let c_json_obj = unsafe {
            CGetCJsonObj()
        };
        unsafe {
            CAddNumber(c_json_obj, i32::from(CoordStatusType::RemoteCoordinationStart), String::from("fi_softbus_key_cmd_type").as_ptr() as *const c_char);
            CAddNumber(c_json_obj, session_id, String::from("fi_softbus_key_session_id").as_ptr() as *const c_char);
            CAddbool(c_json_obj, is_pointer_button_pressed, String::from("fi_softbus_pointer_button_is_press").as_ptr() as *const c_char);
            CAddString(c_json_obj, local_network_id.to_string().as_ptr() as *const c_char, String::from("fi_softbus_key_local_device_id").as_ptr() as *const c_char);
        }
        let mut msg = String::from("");
        unsafe {
            CJsonPrint(c_json_obj, msg.as_mut_ptr() as *mut c_char);
            CJsonDelete(c_json_obj);
        }
        let ret = self.send_msg(session_id, &msg);
        unsafe {
            CJsonFree(msg.as_mut_ptr() as *mut c_char);
        }
        if ret != 0 {
            error!(LOG_LABEL, "Start remote coordination send session msg failed, ret:{}", @public(ret));
            return -1;
        }

        if is_pointer_button_pressed {
            match DSoftbus::get_instance() {
                Some(dsoftbus) => {
                    let mut result = false;
                    let wait_cond = dsoftbus.get_wait_cond(&mut result);   
                    if !result {
                        error!(LOG_LABEL, "get waitCond failed");
                        return -1;
                    }
                    let pair = Arc::clone(&wait_cond);
                    let (lock, cvar) = &*pair;
                    let mut result = false;
                    let operation_mutex = dsoftbus.get_operation_mutex(&mut result);
                    if !result {
                        error!(LOG_LABEL, "get operationMutex failed");
                        return -1;
                    } 
                    match cvar.wait_timeout(operation_mutex.lock().unwrap(),
                    Duration::from_secs(FILTER_WAIT_TIMEOUT_SECOND)) {
                        Ok(guard) => {}
                        Err(err) => {
                            error!(LOG_LABEL, "Filter add timeout");
                            return -1;
                        }
                    };                                                  
                }
                None => {
                    error!(LOG_LABEL, "DSoftbus get_instance failed");
                    return -1;
                }
            };
        }
        0
    }

    /// implementation of start_remote_coordination_result
    pub fn start_remote_coordination_result(&mut self, remote_network_id: &String, is_success: bool,
        start_device_dhid: &String, x_percent: i32, y_percent: i32) -> i32 {
        call_debug_enter!("Adapter::start_remote_coordination_result");
        let session_id = match DSoftbus::get_instance() {
            Some(dsoftbus) => {
                let mut result = false;
                let map = dsoftbus.get_session_dev_map(&mut result);
                if  !result {
                    error!(LOG_LABEL, "get sessionDevMap failed");
                    return -1;
                }
                let get_result: Option<&i32> = map.get(remote_network_id);
                if get_result.is_none() {
                    error!(LOG_LABEL, "Start remote coordination error, not found this device");
                    return -1;
                }
                map[remote_network_id]                                
            }
            None => {
                error!(LOG_LABEL, "DSoftbus get_instance failed");
                return -1;
            }
        };       
        let c_json_obj = unsafe {
            CGetCJsonObj()
        };

        unsafe {
            CAddNumber(c_json_obj, i32::from(CoordStatusType::RemoteCoordinationStartRes), String::from("fi_softbus_key_cmd_type").as_ptr() as *const c_char);
            CAddNumber(c_json_obj, session_id, String::from("fi_softbus_key_session_id").as_ptr() as *const c_char);
            CAddNumber(c_json_obj, x_percent, String::from("fi_softbus_key_pointer_x").as_ptr() as *const c_char);
            CAddNumber(c_json_obj, y_percent, String::from("fi_softbus_key_pointer_y").as_ptr() as *const c_char);
            CAddbool(c_json_obj, is_success, String::from("fi_softbus_key_result").as_ptr() as *const c_char);
            CAddString(c_json_obj, start_device_dhid.to_string().as_ptr() as *const c_char, String::from("fi_softbus_key_start_dhid").as_ptr() as *const c_char);
        }

        let mut msg = String::from("");
        unsafe {
            CJsonPrint(c_json_obj, msg.as_mut_ptr() as *mut c_char);
            CJsonDelete(c_json_obj);
        }

        let ret = self.send_msg(session_id, &msg);
        unsafe {
            CJsonFree(msg.as_mut_ptr() as *mut c_char);
        }
        if ret != 0 {
            error!(LOG_LABEL, "Start remote coordination send session msg failed, ret:{}", @public(ret));
            return -1;
        }
        0
    }

    /// implementation of stop_remote_coordination
    pub fn stop_remote_coordination(&mut self, remote_network_id: &String, is_unchained: bool) -> i32 {
        call_debug_enter!("Adapter::stop_remote_coordination");
        let session_id = match DSoftbus::get_instance() {
            Some(dsoftbus) => {
                let mut result = false;
                let map = dsoftbus.get_session_dev_map(&mut result);
                if  !result {
                    error!(LOG_LABEL, "get sessionDevMap failed");
                    return -1;
                }
                let get_result: Option<&i32> = map.get(remote_network_id);
                if get_result.is_none() {
                    error!(LOG_LABEL, "Start remote coordination error, not found this device");
                    return -1;
                }
                map[remote_network_id]                                
            }
            None => {
                error!(LOG_LABEL, "DSoftbus get_instance failed");
                return -1;
            }
        };
        let c_json_obj = unsafe {
            CGetCJsonObj()
        };

        unsafe {
            CAddNumber(c_json_obj, i32::from(CoordStatusType::RemoteCoordinationStop), String::from("fi_softbus_key_cmd_type").as_ptr() as *const c_char);
            CAddNumber(c_json_obj, session_id, String::from("fi_softbus_key_session_id").as_ptr() as *const c_char);
            CAddbool(c_json_obj, is_unchained, String::from("fi_softbus_key_result").as_ptr() as *const c_char);
        }

        let mut msg = String::from("");
        unsafe {
            CJsonPrint(c_json_obj, msg.as_mut_ptr() as *mut c_char);
            CJsonDelete(c_json_obj);
        }

        let ret = self.send_msg(session_id, &msg);
        unsafe {
            CJsonFree(msg.as_mut_ptr() as *mut c_char);
        }
        if ret != 0 {
            error!(LOG_LABEL, "Start remote coordination send session msg failed, ret:{}", @public(ret));
            return -1;
        }
        0
    }

    /// implementation of stop_remote_coordination_result
    pub fn stop_remote_coordination_result(&mut self, remote_network_id: &String, is_success: bool) -> i32 {
        call_debug_enter!("Adapter::stop_remote_coordination_result");
        let session_id = match DSoftbus::get_instance() {
            Some(dsoftbus) => {
                let mut result = false;
                let map = dsoftbus.get_session_dev_map(&mut result);
                if  !result {
                    error!(LOG_LABEL, "get sessionDevMap failed");
                    return -1;
                }
                let get_result: Option<&i32> = map.get(remote_network_id);
                if get_result.is_none() {
                    error!(LOG_LABEL, "Start remote coordination error, not found this device");
                    return -1;
                }
                map[remote_network_id]                                
            }
            None => {
                error!(LOG_LABEL, "DSoftbus get_instance failed");
                return -1;
            }
        };
        let c_json_obj = unsafe {
            CGetCJsonObj()
        };

        unsafe {
            CAddNumber(c_json_obj, i32::from(CoordStatusType::RemoteCoordinationStopRes), String::from("fi_softbus_key_cmd_type").as_ptr() as *const c_char);
            CAddNumber(c_json_obj, session_id, String::from("fi_softbus_key_session_id").as_ptr() as *const c_char);
            CAddbool(c_json_obj, is_success, String::from("fi_softbus_key_result").as_ptr() as *const c_char);
        }

        let mut msg = String::from("");
        unsafe {
            CJsonPrint(c_json_obj, msg.as_mut_ptr() as *mut c_char);
            CJsonDelete(c_json_obj);
        }

        let ret = self.send_msg(session_id, &msg);
        unsafe {
            CJsonFree(msg.as_mut_ptr() as *mut c_char);
        }
        if ret != 0 {
            error!(LOG_LABEL, "Start remote coordination send session msg failed, ret:{}", @public(ret));
            return -1;
        }
        0
    }

    /// implementation of notify_unchained_result
    pub fn notify_unchained_result(&mut self, local_network_id: &String, remote_network_id: &String, result: bool) -> i32 {
        call_debug_enter!("Adapter::notify_unchained_result");
        let session_id = match DSoftbus::get_instance() {
            Some(dsoftbus) => {
                let mut get_result = false;
                let map = dsoftbus.get_session_dev_map(&mut get_result);
                if  !get_result {
                    error!(LOG_LABEL, "get sessionDevMap failed");
                    return -1;
                }
                let get_result: Option<&i32> = map.get(remote_network_id);
                if get_result.is_none() {
                    error!(LOG_LABEL, "Start remote coordination error, not found this device");
                    return -1;
                }
                map[remote_network_id]                                
            }
            None => {
                error!(LOG_LABEL, "DSoftbus get_instance failed");
                return -1;
            }
        };
        let c_json_obj = unsafe {
            CGetCJsonObj()
        };

        unsafe {
            CAddNumber(c_json_obj, i32::from(CoordStatusType::NotifyUnChainedRes), String::from("fi_softbus_key_cmd_type").as_ptr() as *const c_char);
            CAddNumber(c_json_obj, session_id, String::from("fi_softbus_key_session_id").as_ptr() as *const c_char);
            CAddbool(c_json_obj, result, String::from("fi_softbus_key_result").as_ptr() as *const c_char);
            CAddString(c_json_obj, local_network_id.to_string().as_ptr() as *const c_char, String::from("fi_softbus_key_local_device_id").as_ptr() as *const c_char);
        }

        let mut msg = String::from("");
        unsafe {
            CJsonPrint(c_json_obj, msg.as_mut_ptr() as *mut c_char);
            CJsonDelete(c_json_obj);
        }

        let ret = self.send_msg(session_id, &msg);

        unsafe {
            CJsonFree(msg.as_mut_ptr() as *mut c_char);
        }
        if ret != 0 {
            error!(LOG_LABEL, "Start remote coordination send session msg failed, ret:{}", @public(ret));
            return -1;
        }
        0
    }

    /// implementation of notify_filter_added
    pub fn notify_filter_added(&mut self, remote_network_id: &String) -> i32 {
        call_debug_enter!("Adapter::notify_filter_added");
        let session_id = match DSoftbus::get_instance() {
            Some(dsoftbus) => {
                let mut result = false;
                let map = dsoftbus.get_session_dev_map(&mut result);
                if  !result {
                    error!(LOG_LABEL, "get sessionDevMap failed");
                    return -1;
                }
                let get_result: Option<&i32> = map.get(remote_network_id);
                if get_result.is_none() {
                    error!(LOG_LABEL, "Start remote coordination error, not found this device");
                    return -1;
                }
                map[remote_network_id]                                
            }
            None => {
                error!(LOG_LABEL, "DSoftbus get_instance failed");
                return -1;
            }
        };
        let c_json_obj = unsafe {
            CGetCJsonObj()
        };

        unsafe {
            CAddNumber(c_json_obj, i32::from(CoordStatusType::NotifyFilterAdded), String::from("fi_softbus_key_cmd_type").as_ptr() as *const c_char);
        }

        let mut msg = String::from("");
        unsafe {
            CJsonPrint(c_json_obj, msg.as_mut_ptr() as *mut c_char);
            CJsonDelete(c_json_obj);
        }

        let ret = self.send_msg(session_id, &msg);

        unsafe {
            CJsonFree(msg.as_mut_ptr() as *mut c_char);
        }
        if ret != 0 {
            error!(LOG_LABEL, "Start remote coordination send session msg failed, ret:{}", @public(ret));
            return -1;
        }
        0
    }

    /// implementation of send_msg
    pub fn send_msg(&mut self, session_id: i32, message: &String) -> i32 {
        call_debug_enter!("Adapter::send_msg");
        let message_len = std::mem::size_of_val(message);
        if message_len > MSG_MAX_SIZE {
            error!(LOG_LABEL, "error:message.size() > MSG_MAX_SIZE message size:{}", @public(message));
            return -1;
        }
        unsafe { SendBytes(session_id, message.as_ptr() as *const c_void, message_len.try_into().unwrap()) }
    }
}

/// struct Adapter
#[derive(Default)]
pub struct Adapter {
    adapter_impl: Mutex<RefCell<AdapterImpl>>,
}

impl Adapter {
    /// interface of get_instance
    pub fn get_instance() -> Option<&'static Self> {
        static mut ADAPTER: Option<Adapter> = None;
        static INIT_ONCE: Once = Once::new();
        unsafe {
            INIT_ONCE.call_once(|| {
                ADAPTER = Some(Adapter::default());
            });
            ADAPTER.as_ref()
        }
    }

    /// interface of start_remote_coordination
    pub fn start_remote_coordination(&self, local_network_id: &String, remote_network_id: &String) -> i32 {
        match self.adapter_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().start_remote_coordination(local_network_id, remote_network_id)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {:?}", err);
                -1
            }
        }
    }

    /// interface of start_remote_coordination_result
    pub fn start_remote_coordination_result(&self, remote_network_id: &String, is_success: bool,
        start_device_dhid: &String, x_percent: i32, y_percent: i32) -> i32 {
        match self.adapter_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().start_remote_coordination_result(remote_network_id, is_success, start_device_dhid,
                    x_percent, y_percent)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {:?}", err);
                -1
            }
        }
    }

    /// interface of stop_remote_coordination
    pub fn stop_remote_coordination(&self, remote_network_id: &String, is_unchained: bool) -> i32 {
        match self.adapter_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().stop_remote_coordination(remote_network_id, is_unchained)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {:?}", err);
                -1
            }
        }
    }

    /// interface of stop_remote_coordination_result
    pub fn stop_remote_coordination_result(&self, remote_network_id: &String, is_success: bool) -> i32 {
        match self.adapter_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().stop_remote_coordination_result(remote_network_id, is_success)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {:?}", err);
                -1
            }
        }
    }

    /// interface of notify_unchained_result
    pub fn notify_unchained_result(&self, local_network_id: &String, remote_network_id: &String, is_success: bool) -> i32 {
        match self.adapter_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().notify_unchained_result(local_network_id, remote_network_id, is_success)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {:?}", err);
                -1
            }
        }
    }

    /// interface of notify_filter_added
    pub fn notify_filter_added(&self, remote_network_id: &String) -> i32 {
        match self.adapter_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().notify_filter_added(remote_network_id)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {:?}", err);
                -1
            }
        }
    }
}
