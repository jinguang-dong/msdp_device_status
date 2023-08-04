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
use std::collections::HashMap;
use std::ffi::{ c_char, CString, c_void };
use std::sync::{ Mutex, Once };
use fusion_data_rust::{ FusionResult };
use fusion_utils_rust::{ call_debug_enter };
use hilog_rust::{ info, hilog, error, HiLogLabel, LogType };
use crate::binding::{ PrepareRemoteInput, UnPrepareRemoteInput, StartRemoteInput,
                      StopRemoteInput, IsNeedFilterOut, CBusinessEvent };

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "DInput",
};

#[derive(Default)]
struct DInputImpl {
    id_radix: usize,
    pending_callbacks: HashMap<usize, Box<dyn Fn(bool)>>,
}

impl DInputImpl {
    extern "C" fn on_prepare_remote_input(dev_id: *const c_char, status: i32, id: usize, userdata: *mut c_void)
    {
        info!(LOG_LABEL, "on prepare remote input, status:{}", status);
        let user_data = userdata as *mut Self;
        unsafe {
            if let Some(val_back) = user_data.as_mut() {
                call_debug_enter!("on_prepare_remote_input user_data::process_callbacks");
                val_back.process_callbacks(id, status);
            }
        }
    }

    fn prepare_dinput<F>(&mut self, remote_network_id: &str, origin_network_id: &str, callback: F) -> FusionResult<i32>
    where
        F: Fn(bool) + 'static
    {
        call_debug_enter!("DInputImpl::prepare_dinput");
        let id = self.save_callback(callback);
        let this = self as *mut Self as *mut c_void;
        let ret = unsafe {
            PrepareRemoteInput(remote_network_id.as_ptr() as *const c_char,
                origin_network_id.as_ptr() as *const c_char, Some(DInputImpl::on_prepare_remote_input), id, this)
        };
        if ret != 0 {
            error!(LOG_LABEL, "Preparing remote input fail");
            Err(-1)
        } else {
            Ok(0)
        }
    }

    fn unprepare_dinput<F>(&mut self, remote_network_id: &str,
        origin_network_id: &str, callback: F) -> FusionResult<i32>
    where
        F: Fn(bool) + 'static
    {
        call_debug_enter!("DInputImpl::unprepare_dinput");
        let id = self.save_callback(callback);
        let this = self as *mut Self as *mut c_void;
        let ret = unsafe {
            UnPrepareRemoteInput(remote_network_id.as_ptr() as *const c_char,
                origin_network_id.as_ptr() as *const c_char, Some(DInputImpl::on_prepare_remote_input), id, this)
        };
        if ret != 0 {
            error!(LOG_LABEL, "unprepare_dinput fail");
            Err(-1)
        } else {
            Ok(0)
        }
    }

    fn start_dinput<F>(&mut self, remote_network_id: &str, origin_network_id: &str,
        input_device_dhids: &[String], callback: F) -> FusionResult<i32>
    where
        F: Fn(bool) + 'static
    {
        call_debug_enter!("DInputImpl::start_dinput");
        let id = self.save_callback(callback);
        let this = self as *mut Self as *mut c_void;
        let _c_args: Vec<CString> = input_device_dhids
            .iter()
            .map(|s| CString::new(s.clone()).unwrap())
            .collect();
        let c_args: Vec<*const c_char> =
            _c_args.iter().map(|s| s.as_ptr()).collect();
        let ret = unsafe {
            StartRemoteInput(remote_network_id.as_ptr() as *const c_char, origin_network_id.as_ptr() as *const c_char,
                c_args.as_ptr(), c_args.len(), Some(DInputImpl::on_prepare_remote_input), id, this)
        };
        if ret != 0 {
            error!(LOG_LABEL, "start_dinput fail");
            Err(-1)
        } else {
            Ok(0)
        }
    }

    fn stop_dinput<F>(&mut self, remote_network_id: &str, origin_network_id: &str,
        input_device_dhids: &[String], callback: F) -> FusionResult<i32>
    where
        F: Fn(bool) + 'static
    {
        call_debug_enter!("DInputImpl::stop_dinput");
        let id = self.save_callback(callback);
        let this = self as *mut Self as *mut c_void;
        let _c_args: Vec<CString> = input_device_dhids
            .iter()
            .map(|s| CString::new(s.clone()).unwrap())
            .collect();
        let c_args: Vec<*const c_char> =
            _c_args.iter().map(|s| s.as_ptr()).collect();
        let ret = unsafe {
            StopRemoteInput(remote_network_id.as_ptr() as *const c_char, origin_network_id.as_ptr() as *const c_char,
                c_args.as_ptr(), c_args.len(), Some(DInputImpl::on_prepare_remote_input), id, this)
        };
        if ret != 0 {
            error!(LOG_LABEL, "stop_dinput fail");
            Err(-1)
        } else {
            Ok(0)
        }
    }

    fn save_callback<F>(&mut self, callback: F) -> usize
    where
        F: Fn(bool) + 'static
    {
        call_debug_enter!("DInputImpl::save_callback");
        let id = self.id_radix;
        self.id_radix += 1;
        self.pending_callbacks.insert(id, Box::new(callback));
        id
    }

    pub fn process_callbacks(&mut self, callback_id: usize, status: i32)
    {
        call_debug_enter!("DInputImpl::process_callbacks");
        self.remove_timmer(callback_id);
        let callback = self.pending_callbacks.get(&callback_id);
        match callback {
            Some(callback) => {
                call_debug_enter!("Some of process_callbacks");
                callback(true);
                self.pending_callbacks.remove(&callback_id);
            }
            None => {
                call_debug_enter!("Some of process_callbacks None");
            }
        }
    }

    fn remove_timmer(&self, callback_id: usize) {
        call_debug_enter!("DInputImpl::remove_timmer");
    }

    fn need_filter_out(&self, network_id: &str, event: &mut BusinessEvent) -> bool
    {
        let cevent = CBusinessEvent{
            n_pressed_keys: event.pressed_keys.len(),
            pressed_keys: event.pressed_keys.as_mut_ptr(),
            key_code: event.key_code,
            key_action: event.key_action,
        };

        let ret = unsafe {
            IsNeedFilterOut(network_id.as_ptr() as *const c_char, &cevent as *const CBusinessEvent)
        };
        if ret != 0 {
            error!(LOG_LABEL, "need_filter_out fail");
            false
        } else {
            true
        }
    }
}

/// TODO: add documentation.
pub struct BusinessEvent {
    /// TODO: add documentation.
    pub pressed_keys: Vec<i32>,
    /// TODO: add documentation.
    pub key_code: i32,
    /// TODO: add documentation.
    pub key_action: i32,
}

/// TODO: add documentation.
#[derive(Default)]
pub struct DInput {
    dinput_impl: Mutex<RefCell<DInputImpl>>,
}

impl DInput {
    /// TODO: add documentation.
    pub fn get_instance() -> Option<&'static Self> {
        static mut DINPUT_ADAPTER: Option<DInput> = None;
        static INIT_ONCE: Once = Once::new();
        unsafe {
            INIT_ONCE.call_once(|| {
                DINPUT_ADAPTER = Some(Self::default());
            });
            DINPUT_ADAPTER.as_ref()
        }
    }

    /// TODO: add documentation.
    pub fn prepare_dinput<F>(&self, remote_network_id: &str, origin_network_id: &str,
        callback: F) -> FusionResult<i32>
    where
        F: Fn(bool) + 'static
    {
        call_debug_enter!("DInput::prepare_dinput");
        match self.dinput_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().prepare_dinput(remote_network_id, origin_network_id, callback)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                Err(-1)
            }
        }
    }

    /// TODO: add documentation.
    pub fn unprepare_dinput<F>(&self, remote_network_id: &str, origin_network_id: &str,
        callback: F) -> FusionResult<i32>
    where
        F: Fn(bool) + 'static
    {
        call_debug_enter!("DInput::unprepare_dinput");
        match self.dinput_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().unprepare_dinput(remote_network_id, origin_network_id, callback)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                Err(-1)
            }
        }
    }

    /// TODO: add documentation.
    pub fn start_dinput<F>(&self, remote_network_id: &str, origin_network_id: &str,
        input_device_dhids: &[String], callback: F) -> FusionResult<i32>
    where
        F: Fn(bool) + 'static
    {
        call_debug_enter!("DInput::start_dinput");
        match self.dinput_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().start_dinput(remote_network_id, origin_network_id, input_device_dhids, callback)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                Err(-1)
            }
        }
    }

    /// TODO: add documentation.
    pub fn stop_dinput<F>(&self, remote_network_id: &str, origin_network_id: &str,
        input_device_dhids: &[String], callback: F) -> FusionResult<i32>
    where
        F: Fn(bool) + 'static
    {
        call_debug_enter!("DInput::stop_dinput");
        match self.dinput_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().stop_dinput(remote_network_id, origin_network_id, input_device_dhids, callback)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                Err(-1)
            }
        }
    }

    /// TODO: add documentation.
    pub fn need_filter_out(&self, network_id: &str, event: &mut BusinessEvent) -> bool
    {
        call_debug_enter!("DInput::need_filter_out");
        match self.dinput_impl.lock() {
            Ok(guard) => {
                let iret = guard.borrow_mut().need_filter_out(network_id, event);
                iret
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                false
            }
        }
    }
}

