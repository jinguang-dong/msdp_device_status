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

use std::sync::{Arc, Mutex};
use std::ffi::{CString, c_char, c_void, c_int, CStr};
use std::env;
use std::vec::Vec;
use std::io::Read;
use std::io::Stdin;

use hilog_rust::{error, hilog, debug, HiLogLabel, LogType};
const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "main"
};

use fusion_dsoftbus_rust::{ dsoftbus::DSoftbus, binding::GetAccessToken };

fn main() {
    debug!(LOG_LABEL, "Main:test");
    let mut buf = String::new();
    let std_in: Stdin = std::io::stdin();
    if std_in.read_line(&mut buf).is_ok() {
        if "init\r\n" == buf {
            println!("i am init");
            unsafe { GetAccessToken() };
            match DSoftbus::get_instance() {
                Some(dsoftbus) => {
                    match dsoftbus.init() {
                        Ok(ret) => { 0 }
                        Err(err) => { err }
                    }
                }
                None => {
                    error!(LOG_LABEL, "DSoftbus is none");
                    -1
                }
            };
            
        } else {
            println!("i am open_input_softbus");
            let remote_network_id: Vec<String> = std::env::args().collect();
            error!(LOG_LABEL, "id={}", @public(&remote_network_id[0]));
            match DSoftbus::get_instance() {
                Some(dsoftbus) => {
                    match dsoftbus.open_input_softbus(&remote_network_id[0]) {
                        Ok(ret) => { 0 }
                        Err(err) => { err }
                    }
                }
                None => {
                    error!(LOG_LABEL, "DSoftbus is none");
                    -1
                }
            };
            std::thread::sleep(std::time::Duration::from_millis(1000*60*10));
        }
    }
}