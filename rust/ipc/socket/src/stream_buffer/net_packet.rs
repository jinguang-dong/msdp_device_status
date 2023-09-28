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

use std::ffi::{CString, c_char};
use hilog_rust::{hilog, error, HiLogLabel, LogType};
use std::mem::size_of;
use crate::stream_buffer::StreamBuffer;
const STREAM_BUF_WRITE_FAIL: i32 = 2;
const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "NetPacket"
};

#[repr(packed(1))]
#[repr(C)]
pub(crate) struct PackHead {
    pub(crate) id_msg: MessageId,
    pub(crate) size: usize,
}

/// Message type of NetPacket
#[derive(Copy, Clone)]
#[repr(C)]
pub enum MessageId {
    /// Invalid message type
    Invalid = 0,
    /// Device message type
    Device,
    /// DeviceIds message type
    DeviceIds,
    /// DeviceSupportKeys message type
    DeviceSupportKeys,
    /// AddDeviceListener message type
    AddDeviceListener,
    /// DeviceKeyboardType message type
    DeviceKeyboardType,
    /// DisplayInfo message type
    DisplayInfo,
    /// NoticeAnr message type
    NoticeAnr,
    /// MarkProcess message type
    MarkProcess,
    /// OnSubscribeKey message type
    OnSubscribeKey,
    /// OnKeyEvent message type
    OnKeyEvent,
    /// OnPointerEvent message type
    OnPointerEvent,
    /// ReportKeyEvent message type
    ReportKeyEvent,
    /// ReportPointerEvent message type
    ReportPointerEvent,
    /// OnDeviceAdded message type
    OnDeviceAdded,
    /// OnDeviceRemoved message type
    OnDeviceRemoved,
    /// CoordinationAddListener message type
    CoordinationAddListener,
    /// CoordinationMessage message type
    CoordinationMessage,
    /// CoordinationGetState message type
    CoordinationGetState,
    /// DragNotifyResult message type
    DragNotifyResult,
    /// DragStateListener message type
    DragStateListener,
}

/// Message for communication between server and client
#[derive(Copy, Clone)]
#[repr(C)]
pub struct NetPacket {
    /// Message type
    pub msg_id: MessageId,
    /// Stream buffer for sending and receiving
    pub stream_buffer: StreamBuffer,
}

impl Default for NetPacket {
    fn default() -> Self {
        Self {
            msg_id: MessageId::Invalid,
            stream_buffer: Default::default(),
        }
    }
}

impl NetPacket {
    /// Obtain self's const referance
    ///
    /// # Safety
    ///
    /// The pointer which pointed the memory already initialized must be valid.
    /// If uninitialized memory requires special handling, please refer to std::mem::MaybeUninit.
    /// The pointer needs to be aligned for access. If the memory pointed to by the pointer is a compact
    /// memory layout and requires special consideration. Please refer to (#[repr(packed)]).
    /// Makesure the memory shouldn't be dropped while whose pointer is being used.
    pub unsafe fn as_ref<'a>(object: *const Self) -> Option<&'a Self> {
        // SAFETY: as_ref has already done no-null verification inside
        unsafe {
            object.as_ref()
        }
    }
    /// Obtain self's mut referance
    ///
    /// # Safety
    ///
    /// The pointer which pointed the memory already initialized must be valid.
    /// If uninitialized memory requires special handling, please refer to std::mem::MaybeUninit.
    /// The pointer needs to be aligned for access. If the memory pointed to by the pointer is a compact
    /// memory layout and requires special consideration. Please refer to (#[repr(packed)]).
    /// Makesure the memory shouldn't be dropped while whose pointer is being used.
    pub unsafe fn as_mut<'a>(object: *mut Self) -> Option<&'a mut Self> {
        // SAFETY: as_mut has already done no-null verification inside
        unsafe {
            object.as_mut()
        }
    }
    /// Write stream buffer
    pub fn write<T>(&mut self, data: T) {
        let data: *const c_char = &data as *const T as *const c_char;
        let size = size_of::<T>();
        self.stream_buffer.write_char_usize(data, size);
    }
    /// Read stream buffer
    pub fn read<T>(&mut self, data: &mut T) {
        let data: *mut c_char = data as *mut T as *mut c_char;
        let size = size_of::<T>();
        self.stream_buffer.read_char_usize(data, size);
    }
    /// get_size
    pub fn size(&self) -> usize {
        self.stream_buffer.size()
    }
    pub(super) fn get_packet_length(&self) -> usize {
        size_of::<PackHead>() + self.stream_buffer.w_pos
    }
    /// get_data
    pub fn get_data(&self) -> *const c_char {
        self.stream_buffer.data()
    }
    /// get_msg_id
    pub fn get_msg_id(&self) -> MessageId {
        self.msg_id
    }
    /// make_data
    pub fn make_data(&self, buf: &mut StreamBuffer) {
        let head = PackHead {
            id_msg: self.msg_id,
            size: self.stream_buffer.w_pos,
        };
        buf.write(head);
        if self.stream_buffer.w_pos > 0 && !buf.write_char_usize(&self.stream_buffer.rw_buff[0] as *const c_char,
            self.stream_buffer.w_pos) {
            error!(LOG_LABEL, "Write data to stream failed, errCode:{}", STREAM_BUF_WRITE_FAIL);
        }
    }
}

