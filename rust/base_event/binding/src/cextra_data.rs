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

//! rust base event sys

use crate::binding::CExtraData;

impl Default for CExtraData {
    fn default() -> Self {
        Self::new()
    }
}

impl CExtraData {
    /// Create a CExtraData object
    pub fn new() -> Self {
        CExtraData {
            appended: false,
            buffer: std::ptr::null(),
            buffer_size: 0usize,
            source_type: -1i32,
            pointer_id: -1i32,
        }
    }

    /// Set CExtraData appended property
    pub fn set_appended(&mut self, appended: bool) {
        self.appended = appended;
    }

    /// Set CExtraData buffer property
    pub fn set_buffer(&mut self, vec: &Vec<u8>) {
        let vec_ptr = vec.as_ptr();
        self.buffer = vec_ptr;
    }

    /// Set CExtraData buffer size property
    pub fn set_buffer_size(&mut self, buffer_size: usize) {
        self.buffer_size = buffer_size;
    }

    /// Set CExtraData source type property
    pub fn set_source_type(&mut self, source_type: i32) {
        self.source_type = source_type;
    }

    /// Set CExtraData pointer id property
    pub fn set_pointer_id(&mut self, pointer_id: i32) {
        self.pointer_id = pointer_id;
    }
}