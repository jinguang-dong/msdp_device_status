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

//! Wrappers of C calls.

use std::ffi::{ c_int, CStr };

/// Retrieve system error number.
pub fn errno() -> c_int {
    unsafe {
        *libc::__errno_location()
    }
}

/// Retrieve an descriptive error message of the last failed call.
pub fn last_error() -> String {
    let msg = unsafe {
        CStr::from_ptr(libc::strerror(errno()))
    };
    if let Ok(serr) = msg.to_str() {
        serr.to_string()
    } else {
        String::default()
    }
}
