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

//! rust deviceProfile test main

#![allow(unused_imports)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(missing_docs)]

use std::cell::RefCell;
use std::ffi::{ c_char, CString };
use fusion_utils_rust::{ call_debug_enter };
use hilog_rust::{ info, hilog, error, HiLogLabel, LogType };
use fusion_dinput_rust::{ dinput::BusinessEvent, dinput::DInput};

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "DInput",
};

fn cb_prepare_remote_input(isret: bool) {
    call_debug_enter!("main.rs::cb_prepare_remote_input");
}

fn cb_unprepare_remote_input(isret: bool) {
    call_debug_enter!("main.rs::cb_unprepare_remote_input");
}

fn cb_start_remote_input(isret: bool) {
    call_debug_enter!("main.rs::cb_start_remote_input");
}

fn cb_stop_remote_input(isret: bool) {
    call_debug_enter!("main.rs::cb_stop_remote_input");
}

pub type PrepareRemoteInput = fn(isret: bool);
pub type UnPrepareRemoteInput = fn(isret: bool);
pub type StartRemoteInput =  fn(isret: bool);
pub type StopRemoteInput = fn(isret: bool);

/*#[test]
fn test_need_filter_out() {
    // 11,33,55,66
    let device_id = "test";
    let buf: Vec<i32> = vec![11, 33];
    let mut event = BusinessEvent{
        pressed_keys: buf,
        key_code: 55,
        key_action: 66,
    };

    let ret = DInput::get_instance().expect("test_need_filter_out").need_filter_out(device_id, &mut event);
    assert!(ret);
}

#[test]
pub fn test_prepare_dinput() {
    let remote = "prepare_remote";
    let origin = "prepare_origin";
    let cb: PrepareRemoteInput  = cb_prepare_remote_input;
    let ret = DInput::get_instance().expect("test_prepare_dinput").prepare_dinput(remote, origin, cb);
    assert!(ret.is_ok());
}

#[test]
pub fn test_unprepare_dinput() {
    let remote = "unprepare_remote";
    let origin = "unprepare_origin";
    let cb: UnPrepareRemoteInput = cb_unprepare_remote_input;
    let ret = DInput::get_instance().expect("test_unprepare_dinput").unprepare_dinput(remote, origin, cb);
    assert!(ret.is_ok());
}

#[test]
pub fn test_start_dinput() {
    let remote = "start_remote";
    let origin = "start_origin";
    let inputdevice: Vec<String> = vec![
        "testStartInputDevice1".to_string(),
        "testStartInputDevice2".to_string(),
        "testStartInputDevice3".to_string()
    ];
    let cb :StartRemoteInput = cb_start_remote_input;
    let ret = DInput::get_instance().expect("test_start_dinput").start_dinput(remote, origin, &inputdevice, cb);
    assert!(ret.is_ok());
}

#[test]
pub fn test_stop_dinput() {
    let remote = "stop_remote";
    let origin = "stop_origin";
    let inputdevice: Vec<String> = vec![
        "testStopInputDevice1".to_string(),
        "testStopInputDevice2".to_string(),
        "testStopInputDevice3".to_string()
    ];
    let cb :StopRemoteInput = cb_stop_remote_input;
    let ret = DInput::get_instance().expect("test_stop_dinput").stop_dinput(remote, origin, &inputdevice, cb);
    assert!(ret.is_ok());
}*/

fn test_need_filter_out(remote: &str) {
    let device_id = remote;
    let buf: Vec<i32> = vec![11, 33];
    let mut event = BusinessEvent{
        pressed_keys: buf,
        key_code: 55,
        key_action: 66,
    };

    if let Some(filter_out) = DInput::get_instance() {
        assert!(filter_out.need_filter_out(device_id, &mut event));
    } else {
        println!("filter_out error");
    }
}

pub fn test_prepare_dinput(remote: &str) {
    let origin = "prepare_origin";
    let cb: PrepareRemoteInput  = cb_prepare_remote_input;
    if let Some(prepare) = DInput::get_instance() {
        let ret = prepare.prepare_dinput(remote, origin, cb);
        assert!(ret.is_ok());
    } else {
        println!("test_prepare_dinput error");
    }
}

pub fn test_unprepare_dinput(remote:&str) {
    let origin = "unprepare_origin";
    let cb: UnPrepareRemoteInput = cb_unprepare_remote_input;
    if let Some(unprepare) = DInput::get_instance() {
        let ret = unprepare.unprepare_dinput(remote, origin, cb);
        assert!(ret.is_ok());
    } else {
        println!("test_unprepare_dinput error");
    }
}

pub fn test_start_dinput(remote: &str) {
    let origin = "start_origin";
    let inputdevice: Vec<String> = vec![
        "testStartInputDevice1".to_string(),
        "testStartInputDevice2".to_string(),
        "testStartInputDevice3".to_string()
                                            ];
    let cb :StartRemoteInput = cb_start_remote_input;
    if let Some(start_remote) = DInput::get_instance() {
        let ret = start_remote.start_dinput(remote, origin, &inputdevice, cb);
        assert!(ret.is_ok());
    } else {
        println!("test_start_dinput error");
    }
}

pub fn test_stop_dinput(remote: &str) {
    let origin = "stop_origin";
    let inputdevice: Vec<String> = vec![
        "testStopInputDevice1".to_string(),
        "testStopInputDevice2".to_string(),
        "testStopInputDevice3".to_string()
    ];
    let cb :StopRemoteInput = cb_stop_remote_input;
    if let Some(stop_remote) = DInput::get_instance() {
        let ret = stop_remote.start_dinput(remote, origin, &inputdevice, cb);
        assert!(ret.is_ok());
    } else {
        println!("test_stop_dinput error");
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() > 1 {
        test_need_filter_out(&args[1]);
        test_prepare_dinput(&args[1]);
        test_unprepare_dinput(&args[1]);
        test_start_dinput(&args[1]);
        test_stop_dinput(&args[1]);
    } else {
        println!("Invalid arguments.");
    }
}