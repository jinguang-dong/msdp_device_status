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

//! Implementation of SA of device status service.

#![allow(unused_variables)]

use std::ffi::{c_char, CString};

use fusion_ipc_service_rust::MSDP_DEVICESTATUS_SERVICE_ID;
use fusion_services_rust::FusionService;
use hilog_rust::{error, hilog, info, HiLogLabel, LogType};
use system_ability_fwk::ability::{Ability, PublishHandler};

use crate::fusion_ipc_delegator::FusionIpcDelegator;

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "DeviceStatusSA",
};

struct DeviceStatusService;

impl Ability for DeviceStatusService {
    fn on_start(&self, handler: PublishHandler) {
        info!(LOG_LABEL, "Create remote stub");
        info!(LOG_LABEL, "Publishing service");

        handler.publish(FusionIpcDelegator::new());

        if let Some(proxy) = FusionService::get_instance() {
            proxy.on_start();
        } else {
            error!(LOG_LABEL, "No proxy");
        }
    }

    fn on_stop(&self) {
        info!(LOG_LABEL, "In on_stop(): enter");
        if let Some(proxy) = FusionService::get_instance() {
            proxy.on_stop();
        } else {
            error!(LOG_LABEL, "No proxy");
        }
    }
}

#[used]
#[link_section = ".init_array"]
static A: extern "C" fn() = {
    extern "C" fn init() {
        let system_ability = DeviceStatusService
            .build_system_ability(MSDP_DEVICESTATUS_SERVICE_ID, true)
            .unwrap();
        system_ability.register();
    }

    init
};
