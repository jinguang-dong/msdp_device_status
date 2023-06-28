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

//! IPC server implementation.

#![allow(unused_variables)]

use std::ffi::{ c_char, CString };
use std::ops::Deref;
use crate::hilog_rust::{ info, error, hilog, HiLogLabel, LogType };
use crate::ipc_rust::{ IRemoteBroker, RemoteObj };
use crate::system_ability_fwk_rust::{ define_system_ability, RSystemAbility, ISystemAbility, IMethod };
use crate::fusion_ipc_service_rust::{ FusionIpcStub, MSDP_DEVICESTATUS_SERVICE_ID };
use crate::fusion_ipc_delegator::FusionIpcDelegator;
use crate::fusion_services_rust::{ FusionServiceProxy };

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "FusionIpcServer"
};

fn on_start<T: ISystemAbility + IMethod>(ability: &T) {
    info!(LOG_LABEL, "Create remote stub");
    if let Some(service) = FusionIpcStub::new_remote_stub(FusionIpcDelegator::new()) {
        if let Some(obj) = service.as_object() {
            info!(LOG_LABEL, "Publishing service");
            ability.publish(&obj, MSDP_DEVICESTATUS_SERVICE_ID);

            if let Ok(guard) = FusionServiceProxy::get_instance().lock() {
                if let Some(proxy) = guard.deref() {
                    proxy.on_start();
                } else {
                    error!(LOG_LABEL, "No proxy");
                }
            } else {
                error!(LOG_LABEL, "Locking error of proxy");
            }
        } else {
            error!(LOG_LABEL, "Remote object is none");
        }
    } else {
        error!(LOG_LABEL, "Can not create remote stub");
    }
}

fn on_stop<T: ISystemAbility + IMethod>(ability: &T) {
    info!(LOG_LABEL, "in on_stop(): enter");
    if let Ok(guard) = FusionServiceProxy::get_instance().lock() {
        if let Some(proxy) = guard.deref() {
            proxy.on_stop();
        } else {
            error!(LOG_LABEL, "No proxy");
        }
    } else {
        error!(LOG_LABEL, "Locking error of proxy");
    }
}

define_system_ability!(
    sa: SystemAbility(on_start, on_stop),
);

#[used]
#[link_section = ".init_array"]
static A: extern fn() = {
    extern fn init() {
        let sa = SystemAbility::new_system_ability(MSDP_DEVICESTATUS_SERVICE_ID, true).expect(
            "fail to create sa instance"
        );
        sa.register();
    }

    init
};
