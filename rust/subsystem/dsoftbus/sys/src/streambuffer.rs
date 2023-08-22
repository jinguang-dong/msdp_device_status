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
use std::ffi::{c_void, c_char, CString};
use fusion_utils_rust::{ call_debug_enter };
use std::sync::{Once, Mutex, Arc, Condvar};
use hilog_rust::{ error, hilog, HiLogLabel, LogType };
use crate::binding::{ STREAM_BUF_READ_FAIL,
    ErrorStatus,
    PARAM_INPUT_INVALID,
    MEM_OUT_OF_BOUNDS,
    MAX_STREAM_BUF_SIZE,
};

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "Streambuffer"
};

impl From<ErrorStatus> for i32 {
    fn from (value: ErrorStatus) -> Self {
        match value {
            ErrorStatus::ErrorStatusOk => 0,
            ErrorStatus::ErrorStatusRead => 1,
            ErrorStatus::ErrorStatusWrite => 2,
        }
    }
}

#[derive(Default)]
struct StreambufferImpl {
    r_pos: usize,
    w_pos: usize,
    w_count: usize,
    rw_error_status: i32,
    sz_buff: [c_char; MAX_STREAM_BUF_SIZE + 1],
}

impl StreambufferImpl {
    /// TODO: add documentation.
    pub fn read(&self, buf: &String) -> bool {
        call_debug_enter!("Streambuffer::read");
        if self.r_pos == self.w_pos {
            error!(LOG_LABEL, "Not enough memory to read, errCode:{}", @public(STREAM_BUF_READ_FAIL));
            self.rw_error_status = ErrorStatus::from(ErrorStatus::ErrorStatusRead);
            return false;
        }
        buf = self.read_buf();
        let buf_len = buf.len();
        self.rPos += buf_len + 1;
        return buf_len > 0;
    }

    /// TODO: add documentation.
    pub fn read_buf(&self) ->  *const c_char{
        call_debug_enter!("Streambuffer::read_buf");
        &(self.sz_buff[self.r_pos])
    }

    /// TODO: add documentation.
    pub fn write(&self, buf: String) -> bool {
        call_debug_enter!("Streambuffer::write");
        if self.chk_rw_error() {
            error!(LOG_LABEL, "Read and write status is error");
            return false;
        }
        if buf.is_empty() {
            error!(LOG_LABEL, "Invalid input parameter buf:empty, errCode:{}", @public(PARAM_INPUT_INVALID));
            self.rw_error_status = ErrorStatus::from(ErrorStatus::ErrorStatusWrite);
            return false;
        }
        if (self.w_pos + buf.len()) > MAX_STREAM_BUF_SIZE {
            error!(LOG_LABEL, "The write length exceeds buffer, wIdx:{}, size:{}, maxBufSize:{}, errCode:{}",
            @public(self.w_pos), @public(buf.len()), @public(MAX_STREAM_BUF_SIZE), @public(MEM_OUT_OF_BOUNDS));
            self.rw_error_status = ErrorStatus::from(ErrorStatus::ErrorStatusWrite);
            return false;
        }
        let sz_buff_end: usize = get_available_buf_size().try_into().unwrap();
        self.sz_buff[self.w_pos..sz_buff_end].copy_from_slice(&buf[..(buf.len() - 1)]);
        self.w_pos += buf.len();
        self.w_count += 1;
        return true;
    }

    pub fn chk_rw_error(&self) -> bool {
        call_debug_enter!("Streambuffer::chk_rw_error");
        return self.rw_error_status != ErrorStatus::from(ErrorStatus::ErrorStatusOk);
    }

    pub fn get_available_buf_size(&self) -> i32 {
        call_debug_enter!("Streambuffer::get_available_buf_size");
        if self.w_pos >= MAX_STREAM_BUF_SIZE {
            0
        }
        else {
            (MAX_STREAM_BUF_SIZE - self.w_pos).try_into().unwrap()
        }
    }
}

/// TODO: add documentation.
#[derive(Default)]
pub struct Streambuffer {
    adapter_impl: Mutex<RefCell<StreambufferImpl>>,
}

impl Streambuffer {
    /// TODO: add documentation.
    pub fn get_instance() -> Option<&'static Self> {
        static mut ADAPTER: Option<Streambuffer> = None;
        static INIT_ONCE: Once = Once::new();
        unsafe {
            INIT_ONCE.call_once(|| {
                ADAPTER = Some(Streambuffer::default());
            });
            ADAPTER.as_ref()
        }
    }

    /// TODO: add documentation.
    pub fn read(&self, buf: &String) -> bool {
        match self.adapter_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().read()
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                -1
            }
        }
    }
    
    /// TODO: add documentation.
    pub fn write(&self, buf: String) -> bool {
        match self.adapter_impl.lock() {
            Ok(guard) => {
                guard.borrow_mut().write(buf)
            }
            Err(err) => {
                error!(LOG_LABEL, "lock error: {}", err);
                -1
            }
        }
    }
}