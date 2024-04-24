// Copyright (C) 2023 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! IPC service definition.

#![allow(unused_variables)]

mod identity;

use fusion_data_rust::Intention;
use fusion_utils_rust::FusionResult;
use ipc::parcel::MsgParcel;

pub use crate::identity::{
    compose_param_id, split_action, split_intention, split_param, CommonAction,
};

/// SA ID for "ohos.msdp.Idevicestatus"
pub const MSDP_DEVICESTATUS_SERVICE_ID: i32 = 2902;

pub const MSDP_DEVICESTATUS_DESCRIPTOR: &str = "ohos.msdp.Idevicestatus";

/// Abstration of services.
///
/// By design, for ease of extention, all service implementations are required
/// to map its functions to this collection of interface, with services
/// identified by Intentions.
pub trait IDeviceStatus {
    /// Enable the service identified by [`intention`].
    fn enable(
        &self,
        intention: Intention,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()>;
    /// Disable the service identified by [`intention`].
    fn disable(
        &self,
        intention: Intention,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()>;
    /// Start the service identified by [`intention`].
    fn start(
        &self,
        intention: Intention,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()>;
    /// Stop the service identified by [`intention`].
    fn stop(
        &self,
        intention: Intention,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()>;
    /// Add a watch of state of service, with the service identified by
    /// [`intention`], the state to watch identified by [`id`], parameters
    /// packed in [`data`] parcel.
    fn add_watch(
        &self,
        intention: Intention,
        id: u32,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()>;
    /// Remove a watch of state of service.
    fn remove_watch(
        &self,
        intention: Intention,
        id: u32,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()>;
    /// Set a parameter of service, with the service identified by
    /// [`intention`], the parameter identified by [`id`], and values packed
    /// in [`data`] parcel.
    fn set_param(
        &self,
        intention: Intention,
        id: u32,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()>;
    /// Get a parameter of service, with the service identified by
    /// [`intention`], the parameter identified by [`id`].
    fn get_param(
        &self,
        intention: Intention,
        id: u32,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()>;
    /// Interact with service identified by [`intention`] for general purpose.
    /// This interface supplements functions of previous intefaces.
    /// Functionalities of this interface is service spicific.
    fn control(
        &self,
        intention: Intention,
        id: u32,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()>;
}
