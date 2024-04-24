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

//! Proxy for multi-device cooperation.

#![allow(dead_code)]
#![allow(unused_variables)]

use std::ffi::{c_char, CString};

use fusion_data_rust::{
    DefaultReply, GeneralCoordinationParam, GetCoordinationStateParam, Intention,
    StartCoordinationParam, StopCoordinationParam,
};
use fusion_ipc_client_rust::FusionIpcClient;
use fusion_utils_rust::{call_debug_enter, FusionErrorCode, FusionResult};
use hilog_rust::{debug, error, hilog, HiLogLabel, LogType};
use ipc::parcel::MsgParcel;

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "FusionCoordinationClient",
};

/// Definition of proxy for multi-device cooperation.
#[derive(Default)]
pub struct FusionCoordinationClient(i32);

impl FusionCoordinationClient {
    /// Request to enable multi-device cooperation.
    pub fn enable_coordination(
        &self,
        user_data: i32,
        ipc_client: &FusionIpcClient,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionCoordinationClient::enable_coordination");
        let mut reply_parcel = MsgParcel::new();
        let param = GeneralCoordinationParam { user_data };
        debug!(LOG_LABEL, "Call ipc_client::enable()");
        ipc_client.enable(Intention::Coordination, &param, &mut reply_parcel)
    }

    /// Request to disable multi-device cooperation.
    pub fn disable_coordination(
        &self,
        user_data: i32,
        ipc_client: &FusionIpcClient,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionCoordinationClient::disable_coordination");
        let mut reply_parcel = MsgParcel::new();
        let param = GeneralCoordinationParam { user_data };

        debug!(LOG_LABEL, "Call ipc_client::disable()");
        ipc_client.disable(Intention::Coordination, &param, &mut reply_parcel)
    }

    /// Request to start multi-device cooperation.
    pub fn start_coordination(
        &self,
        user_data: i32,
        remote_network_id: &str,
        start_device_id: i32,
        ipc_client: &FusionIpcClient,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionCoordinationClient::start_coordination");
        let mut reply_parcel = MsgParcel::new();
        let param = StartCoordinationParam {
            user_data,
            remote_network_id: remote_network_id.to_string(),
            start_device_id,
        };

        debug!(LOG_LABEL, "Call ipc_client::start()");
        ipc_client.start(Intention::Coordination, &param, &mut reply_parcel)
    }

    /// Request to stop multi-device cooperation.
    pub fn stop_coordination(
        &self,
        user_data: i32,
        is_unchained: i32,
        ipc_client: &FusionIpcClient,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionCoordinationClient::stop_coordination");
        let mut reply_parcel = MsgParcel::new();
        let param = StopCoordinationParam {
            user_data,
            is_unchained,
        };

        debug!(LOG_LABEL, "Call ipc_client::stop()");
        ipc_client.stop(Intention::Coordination, &param, &mut reply_parcel)
    }

    /// Request for current switch status of multi-device cooperation.
    pub fn get_coordination_state(
        &self,
        user_data: i32,
        device_id: &str,
        ipc_client: &FusionIpcClient,
    ) -> FusionResult<i32> {
        call_debug_enter!("FusionCoordinationClient::get_coordination_state");
        let mut reply_parcel = MsgParcel::new();
        let param = GetCoordinationStateParam {
            user_data,
            device_id: device_id.to_string(),
        };

        debug!(LOG_LABEL, "Call ipc_client::get_param()");
        ipc_client.get_param(Intention::Coordination, 0u32, &param, &mut reply_parcel)?;

        match reply_parcel.read::<DefaultReply>() {
            Ok(x) => Ok(x.reply),
            Err(_) => {
                error!(LOG_LABEL, "Failed to deserialize DefaultReply");
                Err(FusionErrorCode::Fail)
            }
        }
    }

    /// Request to listen for events of multi-device cooperation.
    pub fn register_coordination_listener(&self, ipc_client: &FusionIpcClient) -> FusionResult<()> {
        call_debug_enter!("FusionCoordinationClient::register_coordination_listener");
        let mut reply_parcel = MsgParcel::new();
        let param = GeneralCoordinationParam::default();

        debug!(LOG_LABEL, "Call ipc_client::add_watch()");
        ipc_client.add_watch(Intention::Coordination, 0u32, &param, &mut reply_parcel)
    }

    /// Request to stop listening for events of multi-device cooperation.
    pub fn unregister_coordination_listener(
        &self,
        ipc_client: &FusionIpcClient,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionCoordinationClient::unregister_coordination_listener");
        let mut reply_parcel = MsgParcel::new();
        let param = GeneralCoordinationParam::default();

        debug!(LOG_LABEL, "Call ipc_client::remove_watch()");
        ipc_client.remove_watch(Intention::Coordination, 0u32, &param, &mut reply_parcel)
    }
}
