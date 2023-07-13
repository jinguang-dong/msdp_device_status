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

//! Distributed Input Adapter implementation.

#![allow(dead_code)]
#![allow(unused_variables)]

extern crate hilog_rust;
extern crate ipc_rust;
extern crate fusion_utils_rust;
extern crate fusion_ipc_client_rust;
//extern crate fusion_data_rust;
extern crate fusion_basic_client_rust;
//extern crate fusion_drag_client_rust;

use crate::fusion_utils_rust::{ call_debug_enter };
use hilog_rust::{ info, hilog, HiLogLabel, LogType };
use std::ffi::{ c_char, CString };
use std::sync::{Mutex, Once};
use std::collections::HashMap;

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "DistributedInputAdapter"
};

/// struct DistributedInputAdapter
pub struct DistributedInputAdapter {
    watching_map: Mutex<HashMap<CallbackType, TimerInfo>>,
    callback_map: Mutex<HashMap<CallbackType, DInputCallback>>,
}

/// #Safety
pub type DInputCallback = unsafe extern "C" fn (bool);
//pub type SimulateEventCallback = unsafe extern "C" fn (u32, u32, i32);

/// type of callback
#[derive(Eq, Hash, PartialEq, Clone)]
enum CallbackType {
    /// start distributed input callback
    StartDInputCallback,
    /// start distributed input dhids callback 
    StartDInputCallbackDHIds,
    /// start distributed input sink callback
    StartDInputCallbackSink,
    /// stop distributed input callback
    StopDInputCallback,
    /// stop distributed input dhids callback
    StopDInputCallbackDHIds,
    /// stop distributed input sink callback
    StopDInputCallbackSink,
    /// prepare start distributed input callback
    PrepareStartDInputCallback,
    /// unprepare stop distributed input callback
    UnPrepareStopDInputCallback,
    /// prepare start distributed input sink callback
    PrepareStartDInputCallbackSink,
    /// unprepare stop distributed input sink callback
    UnPrepareStopDInputCallbackSink
}

/// struct TimerInfo
pub struct TimerInfo {
    times: i32,
    timer_id: i32,
}


/// trait DistributedAdapter1
pub trait DistributedAdapter1 {
    /// TODO:
    fn start_remote_input(remote_network_id: &mut str, origin_network_id: &mut str, input_device_dhids: &mut [String], callback: DInputCallback) -> i32;
    /// TODO:
    fn stop_remote_input(remote_network_id: &mut str, origin_network_id: &mut str, input_device_dhids: &mut [String], callback: DInputCallback) -> i32;
    /// TODO:
    fn prepare_remote_input(remote_network_id: &mut str, origin_network_id: &mut str, callback: DInputCallback) -> i32;
    /// TODO:
    fn unprepare_remote_input(remote_network_id: &mut str, origin_network_id: &mut str, callback: DInputCallback) -> i32;
}

/// trait DistributedAdapter2
pub trait DistributedAdapter2 {
    /// TODO:
    fn stop_remote_input(origin_network_id: &mut str, input_device_dhids: &mut [String], callback: DInputCallback) -> i32;
    /// TODO:
    fn prepare_remote_input(device_id: &mut str, callback: DInputCallback) -> i32;
    /// TODO:
    fn unprepare_remote_input(device_id: &mut str, callback: DInputCallback) -> i32;
}

/*pub fn IsNeedFilterOut(deviceId: &mut String, event: &mut BusinessEvent) -> bool {
    return false;
}*/

impl DistributedAdapter1 for DistributedInputAdapter{
    /// to start remote input
    fn start_remote_input(remote_network_id: &mut str, origin_network_id: &mut str, input_device_dhids: &mut [String], callback: DInputCallback) -> i32 {
        info!(LOG_LABEL, "DistributedAdapter1::start_remote_input");
        0
    }
    /// to stop remote input
    fn stop_remote_input(remote_network_id: &mut str, origin_network_id: &mut str, input_device_dhids: &mut [String], callback: DInputCallback) -> i32 {
        info!(LOG_LABEL, "DistributedAdapter1::stop_remote_input");
        0
    }
    /// to prepare remote input
    fn prepare_remote_input(remote_network_id: &mut str, origin_network_id: &mut str, callback: DInputCallback) -> i32 {
        info!(LOG_LABEL, "DistributedAdapter1::prepare_remote_input");
        0
    }
    /// to unprepare remote input
    fn unprepare_remote_input(remote_network_id: &mut str, origin_network_id: &mut str, callback: DInputCallback) -> i32 {
        info!(LOG_LABEL, "DistributedAdapter1::unprepare_remote_input");
        0
    }
}


impl DistributedAdapter2 for DistributedInputAdapter {
    /// to stop remote input without remote_network_id
    fn stop_remote_input(origin_network_id: &mut str, input_device_dhids: &mut [String], callback: DInputCallback) -> i32 {
        info!(LOG_LABEL, "DistributedAdapter2::stop_remote_input");
        0
    }
    /// to prepare remote input with device_id
    fn prepare_remote_input(device_id: &mut str, callback: DInputCallback) -> i32 {
        info!(LOG_LABEL, "DistributedAdapter2::prepare_remote_input");
        0
    }
    /// to unprepare remote input with device_id
    fn unprepare_remote_input(device_id: &mut str, callback: DInputCallback) -> i32 {
        info!(LOG_LABEL, "DistributedAdapter2::unprepare_remote_input");
        0
    }
}

impl DistributedInputAdapter{
    /// to new distributed_input_adapter
    fn new() -> Self {
        DistributedInputAdapter {
            watching_map: Mutex::new(HashMap::new()),
            callback_map: Mutex::new(HashMap::new()),
        }
    }

    /// to get instance
    pub fn get_instance() -> &'static mut Self {
        static mut DIS_INPUT_ADAPTER: Option<DistributedInputAdapter> = None;
        static ONCE: Once = Once::new();

        ONCE.call_once(|| unsafe {
            DIS_INPUT_ADAPTER = Some(Self::new());
        });
        unsafe { DIS_INPUT_ADAPTER.as_mut().unwrap() }
    }

    /// to save callback
    fn save_callback(&self, cbtype: CallbackType, callback: DInputCallback) {
        info!(LOG_LABEL, "in DistributedInputAdapter::save_callback()");
        let callback_save = &callback;
        let cb_type = cbtype.clone();
        self.callback_map.lock().unwrap().insert(cb_type, *callback_save);
        
        self.add_timer(&cbtype);
    }

    /// to add timer
    fn add_timer(&self, cbtype: &CallbackType) {
        info!(LOG_LABEL, "in DistributedInputAdapter::add_timer()");

    }

    fn remove_timer(&self, cbtype: &CallbackType) {
        info!(LOG_LABEL, "in DistributedInputAdapter::remove_timer()");
    }

    fn process_dinput_callback(&self, cbtype: CallbackType, status: i32) {
        info!(LOG_LABEL, "in DistributedInputAdapter::process_dinput_callback()");
        let mut guard = self.callback_map.lock().unwrap();
        self.remove_timer(&cbtype);
        let find_type = guard.get(&cbtype);
        if find_type.is_none() {
            call_debug_enter!("Dinput callback not exist");
            return;
        }
        guard.remove(&cbtype);
    }

    /// to determine if filter out
    fn is_need_filter_out(&self, device_id: &str, event: BusinessEvent) -> bool {
        info!(LOG_LABEL, "DistributedAdapter1::is_need_filter_out");
        let ret = unsafe {
            IsNeedFilterOut(device_id.as_ptr() as *const c_char, event)
        };
        if ret == 0 {
            Ok(0)
        } else {
            Err(ret);
        }
    }

}


/// struct DistributedInputKit
#[repr(C)]
pub struct DistributedInputKit {
    _private: [u8; 0],
}

/// struct IStartStopDInputsCallback
#[repr(C)]
pub struct IStartStopDInputsCallback {
    _private: [u8; 0],
}

/// struct IPrepareDInputCallback
#[repr(C)]
pub struct IPrepareDInputCallback {
    _private: [u8; 0],
}

/// struct IUnprepareDInputCallback
#[repr(C)]
pub struct IUnprepareDInputCallback {
    _private: [u8; 0],
}

/// struct BusinessEvent
#[repr(C)]
pub struct BusinessEvent {
    pub pressedKeys: Vec<i32>,
    pub keyCode: i32,
    pub keyAction: i32,
}

// C interface
extern "C" {
    pub fn IsNeedFilterOut(spr: *const c_char, event: *const BusinessEvent) -> bool;
    pub fn StartRemoteInput(remoteNetworkId: *const c_char, originNetworkId: *const c_char, vecptr: *mut const c_char, veclen: usize, callback: DInputCallback) -> i32;
    pub fn StopRemoteInput(originNetworkId: *const c_char, vecptr: *mut const c_char, veclen: usize, callback: DInputCallback) -> i32;
    pub fn StopRemoteInputWithRemote(remoteNetworkId: *const c_char, originNetworkId: *const c_char, vecptr: *mut const c_char, veclen: usize, callback: DInputCallback) -> i32;
    pub fn PrepareRemoteInput(remoteNetworkId: *const c_char, originNetworkId: *const c_char, callback: DInputCallback) -> i32;
    pub fn UnPrepareRemoteInput(remoteNetworkId: *const c_char, originNetworkId: *const c_char, callback: DInputCallback) -> i32;
    pub fn PrepareRemoteInputWithDeviceId(deviceId: *const c_char, DInputCallback) -> i32;
    pub fn UnPrepareRemoteInputWithDeviceId(deviceId: *const c_char, callback: DInputCallback) -> i32;
}
