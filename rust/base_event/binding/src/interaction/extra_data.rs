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

//! ExtraData data definitions of DRAG module.

use crate::{
    binding, error::DragErr, DragResult, binding::CExtraData
};
use crate::interaction::drag_data::DragData; // 需要修改位置

/// TODO: add documentation.
pub struct ExtraData {
    inner: CExtraData,
}

impl ExtraData {
    /// TODO: add documentation.
    pub fn appended_extra_data(&mut self, allow_appended: bool, drag_data: DragData) -> DragResult<()> {
        let buffer: &Vec<u8>= &drag_data.buffer;
        if buffer.is_empty() {
            return Err(DragErr::CreateExtraData)
        }
        self.inner.set_appended(allow_appended);
        self.inner.set_buffer(buffer);
        self.inner.set_buffer_size(buffer.len());
        self.inner.set_source_type(drag_data.source_type);
        self.inner.set_pointer_id(drag_data.pointer_id);

        unsafe {
            // SAFETY:  no `None` here, cause `cextra_data` is valid
            binding::CAppendExtraData(&self.inner);
        }
        Ok(())
    }

    /// TODO: add documentation.
    pub fn is_appended(&self) -> bool {
        self.inner.appended
    }
}

