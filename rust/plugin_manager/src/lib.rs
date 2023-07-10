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

//! Plugin manager.

#![allow(unused_variables)]
#![allow(dead_code)]

extern crate fusion_data_rust;
extern crate fusion_utils_rust;
extern crate fusion_basic_server_rust;
extern crate hilog_rust;
extern crate ipc_rust;
extern crate libloading;

use std::collections::HashMap;
use std::path::Path;
use std::ffi::{ c_char, CString };
use fusion_data_rust::{ Intention, IPlugin };
use fusion_utils_rust::{ call_debug_enter };
use fusion_basic_server_rust::FusionBasicServer;
use hilog_rust::{ info, error, hilog, HiLogLabel, LogType };

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "FusionPluginManager"
};

/// TODO: add documentation
#[macro_export]
macro_rules! export_plugin {
    ($plugin_type:ty) => {
        /// TODO: add documentation.
        /// # Safety
        #[no_mangle]
        pub unsafe extern "C" fn _create_plugin() -> *mut dyn IPlugin {
            let boxed = Box::new(<$plugin_type>::default());
            Box::into_raw(boxed)
        }
    };
}

/// struct PluginManager
#[derive(Default)]
pub struct PluginManager {
    loaders: HashMap<Intention, libloading::Library>
}

impl PluginManager {
    fn instantiate_loader(&mut self, intention: &Intention) {
        call_debug_enter!("PluginManager::instantiate_loader");
        let file_path = match intention {
            Intention::Drag => {
                Path::new("/system/lib/libfusion_drag_server_ffi.z.so")
            }
            Intention::Coordination => {
                Path::new("/system/lib/libfusion_coordination_server_ffi.z.so")
            }
            _ => {
                return;
            }
        };

        let lib_result = unsafe {
            libloading::Library::new(file_path.as_os_str())
        };
        match lib_result {
            Ok(lib) => {
                info!(LOG_LABEL, "New Library instance created");
                self.loaders.insert(intention.clone(), lib);
            }
            Err(_) => {
                info!(LOG_LABEL, "Failed to create Library instance");
            }
        }
    }

    fn do_load_plugin(&mut self, intention: Intention) -> Option<Box<dyn IPlugin>> {
        call_debug_enter!("PluginManager::do_load_plugin");
        if !self.loaders.contains_key(&intention) {
            self.instantiate_loader(&intention);
        }
        match self.loaders.get(&intention) {
            Some(loader) => {
                if let Ok(creator) = unsafe {
                    loader.get::<libloading::Symbol<unsafe extern "C" fn() -> *mut dyn IPlugin>>(b"_create_plugin")
                } {
                    info!(LOG_LABEL, "Create plugin instance");
                    let plugin_ptr = unsafe {
                        creator()
                    };
                    if plugin_ptr.is_null() {
                        error!(LOG_LABEL, "Fail to create plugin instance");
                        None
                    } else {
                        Some(unsafe {
                            info!(LOG_LABEL, "Plugin is loaded");
                            Box::from_raw(plugin_ptr)
                        })
                    }
                } else {
                    error!(LOG_LABEL, "Symbol is not found");
                    None
                }
            }
            None => {
                error!(LOG_LABEL, "Can not instantiate loader");
                None
            }
        }
    }

    /// TODO: add documentation
    pub fn load_plugin(&mut self, intention: Intention) -> Option<Box<dyn IPlugin>>
    {
        call_debug_enter!("PluginManager::load_plugin");
        match intention {
            Intention::Basic => {
                Some(Box::<FusionBasicServer>::default())
            }
            _ => {
                self.do_load_plugin(intention)
            }
        }
    }

    /// TODO: add documentation
    pub fn unload_plugin(&self, intention: Intention) {
        todo!()
    }
}
