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

//! Implementation of client side of IPC.

#![allow(dead_code)]
#![allow(unused_variables)]

use std::ffi::{c_char, CString};

use fusion_data_rust::Intention;
use fusion_ipc_service_rust::{compose_param_id, CommonAction, IDeviceStatus};
use fusion_utils_rust::{call_debug_enter, FusionErrorCode, FusionResult};
use hilog_rust::{error, hilog, info, HiLogLabel, LogType};
use ipc::parcel::{MsgParcel, Serialize};
use ipc::remote::RemoteObj;
use samgr::definition::MSDP_DEVICESTATUS_SERVICE_ID;
use samgr::manage::SystemAbilityManager;

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "FusionIpcClient",
};

/// Representation of client side of IPC.
pub struct FusionIpcClient(FusionIpcClientImpl);

impl FusionIpcClient {
    /// Connect device status service.
    pub fn connect() -> FusionResult<Self> {
        call_debug_enter!("FusionIpcClient::connect");

        let remote = match SystemAbilityManager::get_system_ability(MSDP_DEVICESTATUS_SERVICE_ID) {
            Some(remote) => remote,
            None => {
                error!(LOG_LABEL, "Can not connect to service");
                return Err(FusionErrorCode::Fail);
            }
        };

        Ok(Self(FusionIpcClientImpl { remote }))
    }

    fn add_interface_token(&self, data_parcel: &mut MsgParcel) -> FusionResult<()> {
        call_debug_enter!("FusionIpcClient::add_interface_token");
        match data_parcel.write_interface_token("ohos.msdp.Idevicestatus") {
            Ok(_) => Ok(()),
            Err(_) => {
                error!(LOG_LABEL, "Failed to serialize interface token");
                Err(FusionErrorCode::Fail)
            }
        }
    }

    /// Request to enable the service identified by [`intention`].
    pub fn enable(
        &self,
        intention: Intention,
        data: &dyn Serialize,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcClient::enable");
        let mut data_parcel = MsgParcel::new();
        info!(LOG_LABEL, "Serialize interface token");
        self.add_interface_token(&mut data_parcel)?;

        if data.serialize(&mut data_parcel).is_err() {
            return Err(FusionErrorCode::Fail);
        }

        info!(LOG_LABEL, "Call proxy.enable()");
        self.0.enable(intention, &mut data_parcel, reply)
    }

    /// Request to disable the service identified by [`intention`].
    pub fn disable(
        &self,
        intention: Intention,
        data: &dyn Serialize,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcClient::disable");
        let mut data_parcel = MsgParcel::new();

        info!(LOG_LABEL, "Serialize interface token");

        self.add_interface_token(&mut data_parcel)?;

        if data.serialize(&mut data_parcel).is_err() {
            return Err(FusionErrorCode::Fail);
        }
        info!(LOG_LABEL, "Call proxy.disable()");
        self.0.disable(intention, &mut data_parcel, reply)
    }

    /// Request to start the service identified by [`intention`].
    pub fn start(
        &self,
        intention: Intention,
        data: &dyn Serialize,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcClient::start");
        let mut data_parcel = MsgParcel::new();
        info!(LOG_LABEL, "Serialize interface token");
        self.add_interface_token(&mut data_parcel)?;

        if data.serialize(&mut data_parcel).is_err() {
            return Err(FusionErrorCode::Fail);
        }
        info!(LOG_LABEL, "Call proxy.start()");
        self.0.start(intention, &mut data_parcel, reply)
    }

    /// Request to stop the service identified by [`intention`].
    pub fn stop(
        &self,
        intention: Intention,
        data: &dyn Serialize,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcClient::stop");
        let mut data_parcel = MsgParcel::new();

        info!(LOG_LABEL, "Serialize interface token");
        self.add_interface_token(&mut data_parcel)?;

        if data.serialize(&mut data_parcel).is_err() {
            return Err(FusionErrorCode::Fail);
        }
        info!(LOG_LABEL, "Call proxy.stop()");
        self.0.stop(intention, &mut data_parcel, reply)
    }

    /// Request to add a watch of state of service, with the service identified
    /// by [`intention`], the state to watch identified by [`id`],
    /// parameters packed in [`data`] parcel.
    pub fn add_watch(
        &self,
        intention: Intention,
        id: u32,
        data: &dyn Serialize,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcClient::add_watch");
        let mut data_parcel = MsgParcel::new();

        info!(LOG_LABEL, "Serialize interface token");
        self.add_interface_token(&mut data_parcel)?;

        if data.serialize(&mut data_parcel).is_err() {
            return Err(FusionErrorCode::Fail);
        }
        info!(LOG_LABEL, "Call proxy.add_watch()");
        self.0.add_watch(intention, id, &mut data_parcel, reply)
    }

    /// Request to remove a watch of state of service.
    pub fn remove_watch(
        &self,
        intention: Intention,
        id: u32,
        data: &dyn Serialize,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcClient::remove_watch");
        let mut data_parcel = MsgParcel::new();

        info!(LOG_LABEL, "Serialize interface token");
        self.add_interface_token(&mut data_parcel)?;

        if data.serialize(&mut data_parcel).is_err() {
            return Err(FusionErrorCode::Fail);
        }
        info!(LOG_LABEL, "Call proxy.remove_watch()");
        self.0.remove_watch(intention, id, &mut data_parcel, reply)
    }

    /// Request to set a parameter of service, with the service identified by
    /// [`intention`], the parameter identified by [`id`], and values packed in
    /// [`data`] parcel.
    pub fn set_param(
        &self,
        intention: Intention,
        id: u32,
        data: &dyn Serialize,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcClient::set_param");
        let mut data_parcel = MsgParcel::new();

        info!(LOG_LABEL, "Serialize interface token");
        self.add_interface_token(&mut data_parcel)?;

        if data.serialize(&mut data_parcel).is_err() {
            return Err(FusionErrorCode::Fail);
        }
        info!(LOG_LABEL, "Call proxy.set_param()");
        self.0.set_param(intention, id, &mut data_parcel, reply)
    }

    /// Request to get a parameter of service, with the service identified by
    /// [`intention`], the parameter identified by [`id`].
    pub fn get_param(
        &self,
        intention: Intention,
        id: u32,
        data: &dyn Serialize,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcClient::get_param");
        let mut data_parcel = MsgParcel::new();

        info!(LOG_LABEL, "Serialize interface token");
        self.add_interface_token(&mut data_parcel)?;

        if data.serialize(&mut data_parcel).is_err() {
            return Err(FusionErrorCode::Fail);
        }
        info!(LOG_LABEL, "Call proxy.get_param()");
        self.0.get_param(intention, id, &mut data_parcel, reply)
    }

    /// Request to interact with service identified by [`intention`] for general
    /// purpose. This interface supplements functions of previous intefaces.
    /// Functionalities of this interface is service spicific.
    pub fn control(
        &self,
        intention: Intention,
        id: u32,
        data: &dyn Serialize,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcClient::control");
        let mut data_parcel = MsgParcel::new();
        info!(LOG_LABEL, "Serialize interface token");
        self.add_interface_token(&mut data_parcel)?;

        if data.serialize(&mut data_parcel).is_err() {
            return Err(FusionErrorCode::Fail);
        }
        info!(LOG_LABEL, "Call proxy.control()");
        self.0.control(intention, id, &mut data_parcel, reply)
    }
}

struct FusionIpcClientImpl {
    remote: RemoteObj,
}

impl FusionIpcClientImpl {
    fn transfer_data(&self, src: &mut MsgParcel, target: &mut MsgParcel) -> FusionResult<()> {
        call_debug_enter!("FusionIpcProxy::transfer_data");
        let data_size = src.size();
        match src.read_buffer(data_size) {
            Ok(data_vec) => {
                if target.write_buffer(data_vec.as_slice()).is_ok() {
                    Ok(())
                } else {
                    error!(LOG_LABEL, "write_buffer() failed");
                    Err(FusionErrorCode::Fail)
                }
            }
            Err(_) => {
                error!(LOG_LABEL, "read_buffer() failed");
                Err(FusionErrorCode::Fail)
            }
        }
    }

    fn send_request(
        &self,
        action: CommonAction,
        intention: Intention,
        id: u32,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcProxy::send_request");
        let code = compose_param_id(action, intention, id);
        let mut rep = {
            match self.remote.send_request(code, data) {
                Ok(tr) => tr,
                Err(_) => {
                    error!(LOG_LABEL, "Failed to send request");
                    return Err(FusionErrorCode::Fail);
                }
            }
        };

        self.transfer_data(&mut rep, reply)
    }
}

impl IDeviceStatus for FusionIpcClientImpl {
    fn enable(
        &self,
        intention: Intention,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcProxy::enable");
        self.send_request(CommonAction::Enable, intention, 0u32, data, reply)
    }

    fn disable(
        &self,
        intention: Intention,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcProxy::disable");
        self.send_request(CommonAction::Disable, intention, 0u32, data, reply)
    }

    fn start(
        &self,
        intention: Intention,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcProxy::start");
        self.send_request(CommonAction::Start, intention, 0u32, data, reply)
    }

    fn stop(
        &self,
        intention: Intention,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcProxy::stop");
        self.send_request(CommonAction::Stop, intention, 0u32, data, reply)
    }

    fn add_watch(
        &self,
        intention: Intention,
        id: u32,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcProxy::add_watch");
        self.send_request(CommonAction::AddWatch, intention, id, data, reply)
    }

    fn remove_watch(
        &self,
        intention: Intention,
        id: u32,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcProxy::remove_watch");
        self.send_request(CommonAction::RemoveWatch, intention, id, data, reply)
    }

    fn set_param(
        &self,
        intention: Intention,
        id: u32,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcProxy::set_param");
        self.send_request(CommonAction::SetParam, intention, id, data, reply)
    }

    fn get_param(
        &self,
        intention: Intention,
        id: u32,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcProxy::get_param");
        self.send_request(CommonAction::GetParam, intention, id, data, reply)
    }

    fn control(
        &self,
        intention: Intention,
        id: u32,
        data: &mut MsgParcel,
        reply: &mut MsgParcel,
    ) -> FusionResult<()> {
        call_debug_enter!("FusionIpcProxy::control");
        self.send_request(CommonAction::Control, intention, id, data, reply)
    }
}
