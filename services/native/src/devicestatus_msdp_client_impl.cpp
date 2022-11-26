/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "devicestatus_msdp_client_impl.h"

#include <cerrno>
#include <string>

#include <dlfcn.h>
#include <errors.h>
#include <linux/netlink.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "devicestatus_common.h"

using namespace OHOS::NativeRdb;
namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
const std::string DEVICESTATUS_SENSOR_HDI_LIB_PATH = "libdevicestatus_sensorhdi.z.so";
const std::string DEVICESTATUS_MSDP_ALGORITHM_LIB_PATH = "libdevicestatus_msdp.z.so";
std::map<DevicestatusDataUtils::DevicestatusType, DevicestatusDataUtils::DevicestatusValue> g_devicestatusDataMap;
DevicestatusMsdpClientImpl::CallbackManager g_callbacksMgr;
using clientType = DevicestatusDataUtils::DevicestatusType;
using clientValue = DevicestatusDataUtils::DevicestatusValue;
DevicestatusMsdpInterface* g_msdpInterface;
DevicestatusSensorInterface* g_sensorHdiInterface_;
}

ErrCode DevicestatusMsdpClientImpl::InitMsdpImpl()
{
    DEV_HILOGI(SERVICE, "Enter");
    if (g_msdpInterface == nullptr) {
        g_msdpInterface = GetAlgorithmInst();
        if (g_msdpInterface == nullptr) {
            DEV_HILOGI(SERVICE, "get msdp module instance failed");
            return ERR_NG;
        }
    }

    if (g_sensorHdiInterface_ == nullptr) {
        g_sensorHdiInterface_ = GetSensorHdiInst();
        if (g_sensorHdiInterface_ == nullptr) {
            DEV_HILOGI(SERVICE, "get sensor module instance failed");
            return ERR_NG;
        }
    }

    g_msdpInterface->Enable();
    g_sensorHdiInterface_->Enable();

    DEV_HILOGI(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::DisableMsdpImpl()
{
    DEV_HILOGI(SERVICE, "Enter");
    if (g_msdpInterface == nullptr) {
        DEV_HILOGI(SERVICE, "disable msdp impl failed");
        return ERR_NG;
    }

    if (g_sensorHdiInterface_ == nullptr) {
        DEV_HILOGI(SERVICE, "disable msdp impl failed");
        return ERR_NG;
    }

    g_msdpInterface->Disable();
    g_sensorHdiInterface_->Disable();
    DEV_HILOGI(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::RegisterSensor()
{
    DEV_HILOGI(SERVICE, "Enter");
    if (g_sensorHdiInterface_ != nullptr) {
        std::shared_ptr<DevicestatusSensorHdiCallback> callback = std::make_shared<DevicestatusMsdpClientImpl>();
        g_sensorHdiInterface_->RegisterCallback(callback);
        DEV_HILOGI(SERVICE, "g_sensorHdiInterface_ is not nullptr");
    }

    DEV_HILOGI(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::UnregisterSensor(void)
{
    DEV_HILOGI(SERVICE, "Enter");

    if (g_sensorHdiInterface_ == nullptr) {
        DEV_HILOGI(SERVICE, "unregister callback failed");
        return ERR_NG;
    }

    g_sensorHdiInterface_->UnregisterCallback();
    g_sensorHdiInterface_ = nullptr;

    DEV_HILOGI(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::RegisterImpl(const CallbackManager& callback)
{
    DEV_HILOGI(SERVICE, "Enter");
    g_callbacksMgr = callback;

    if (g_msdpInterface == nullptr) {
        g_msdpInterface = GetAlgorithmInst();
        if (g_msdpInterface == nullptr) {
            DEV_HILOGI(SERVICE, "get msdp module instance failed");
            return ERR_NG;
        }
    }

    if (g_sensorHdiInterface_ == nullptr) {
        g_sensorHdiInterface_ = GetSensorHdiInst();
        if (g_sensorHdiInterface_ == nullptr) {
            DEV_HILOGI(SERVICE, "get sensor module instance failed");
            return ERR_NG;
        }
    }

    RegisterMsdp();
    RegisterSensor();

    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::UnregisterImpl()
{
    DEV_HILOGI(SERVICE, "Enter");
    if (g_callbacksMgr == nullptr) {
        DEV_HILOGI(SERVICE, "unregister callback failed");
        return ERR_NG;
    }

    UnregisterMsdp();
    UnregisterSensor();

    g_callbacksMgr = nullptr;

    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::ImplCallback(const DevicestatusDataUtils::DevicestatusData& data)
{
    if (g_callbacksMgr == nullptr) {
        DEV_HILOGI(SERVICE, "g_callbacksMgr is nullptr");
        return ERR_NG;
    }
    g_callbacksMgr(data);

    return ERR_OK;
}

void DevicestatusMsdpClientImpl::OnResult(const DevicestatusDataUtils::DevicestatusData& data)
{
    MsdpCallback(data);
}

void DevicestatusMsdpClientImpl::OnSensorHdiResult(const DevicestatusDataUtils::DevicestatusData& data)
{
    MsdpCallback(data);
}

ErrCode DevicestatusMsdpClientImpl::RegisterMsdp()
{
    DEV_HILOGI(SERVICE, "Enter");
    if (g_msdpInterface != nullptr) {
        std::shared_ptr<MsdpAlgorithmCallback> callback = std::make_shared<DevicestatusMsdpClientImpl>();
        g_msdpInterface->RegisterCallback(callback);
    }

    DEV_HILOGI(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::UnregisterMsdp(void)
{
    DEV_HILOGI(SERVICE, "Enter");

    if (g_msdpInterface == nullptr) {
        DEV_HILOGI(SERVICE, "unregister callback failed");
        return ERR_NG;
    }

    g_msdpInterface->UnregisterCallback();
    g_msdpInterface = nullptr;

    DEV_HILOGI(SERVICE, "Exit");
    return ERR_OK;
}

int32_t DevicestatusMsdpClientImpl::MsdpCallback(const DevicestatusDataUtils::DevicestatusData& data)
{
    DEV_HILOGI(SERVICE, "Enter");
    DevicestatusDumper::GetInstance().pushDeviceStatus(data);
    SaveObserverData(data);
    if (notifyManagerFlag_) {
        ImplCallback(data);
        notifyManagerFlag_ = false;
    }
    DEV_HILOGI(SERVICE, "Exit");
    return ERR_OK;
}

DevicestatusDataUtils::DevicestatusData DevicestatusMsdpClientImpl::SaveObserverData(
    const DevicestatusDataUtils::DevicestatusData& data)
{
    DEV_HILOGI(SERVICE, "Enter");
    for (auto iter = g_devicestatusDataMap.begin(); iter != g_devicestatusDataMap.end(); ++iter) {
        if (iter->first == data.type) {
            iter->second = data.value;
            notifyManagerFlag_ = true;
            return data;
        }
    }

    g_devicestatusDataMap.insert(std::make_pair(data.type, data.value));
    notifyManagerFlag_ = true;

    return data;
}

std::map<clientType, clientValue> DevicestatusMsdpClientImpl::GetObserverData() const
{
    DEV_HILOGI(SERVICE, "Enter");
    return g_devicestatusDataMap;
}

int32_t DevicestatusMsdpClientImpl::LoadSensorHdiLibrary(bool bCreate)
{
    DEV_HILOGI(SERVICE, "Enter");
    if (sensorHdi_.handle != nullptr) {
        return ERR_OK;
    }
    DEV_HILOGI(SERVICE, "Exit");
    return ERR_OK;
}

int32_t DevicestatusMsdpClientImpl::UnloadSensorHdiLibrary(bool bCreate)
{
    DEV_HILOGI(SERVICE, "Enter");
    if (sensorHdi_.handle == nullptr) {
        return ERR_NG;
    }

    if (sensorHdi_.pAlgorithm != nullptr) {
        sensorHdi_.destroy(sensorHdi_.pAlgorithm);
        sensorHdi_.pAlgorithm = nullptr;
    }

    if (!bCreate) {
        dlclose(sensorHdi_.handle);
        sensorHdi_.Clear();
    }

    DEV_HILOGI(SERVICE, "Exit");
    return ERR_OK;
}

DevicestatusSensorInterface* DevicestatusMsdpClientImpl::GetSensorHdiInst()
{
    DEV_HILOGI(SERVICE, "Enter");
    if (sensorHdi_.handle == nullptr) {
        return nullptr;
    }

    if (sensorHdi_.pAlgorithm == nullptr) {
        std::unique_lock<std::mutex> lock(mMutex_);
        if (sensorHdi_.pAlgorithm == nullptr) {
            DEV_HILOGI(SERVICE, "Get mAlgorithm.pAlgorithm");
            sensorHdi_.pAlgorithm = sensorHdi_.create();
        }
    }

    return sensorHdi_.pAlgorithm;
}

int32_t DevicestatusMsdpClientImpl::LoadAlgorithmLibrary(bool bCreate)
{
    DEV_HILOGI(SERVICE, "Enter");
    if (mAlgorithm_.handle != nullptr) {
        return ERR_OK;
    }
    DEV_HILOGI(SERVICE, "Exit");
    return ERR_OK;
}

int32_t DevicestatusMsdpClientImpl::UnloadAlgorithmLibrary(bool bCreate)
{
    DEV_HILOGI(SERVICE, "Enter");
    if (mAlgorithm_.handle == nullptr) {
        return ERR_NG;
    }

    if (mAlgorithm_.pAlgorithm != nullptr) {
        mAlgorithm_.destroy(mAlgorithm_.pAlgorithm);
        mAlgorithm_.pAlgorithm = nullptr;
    }

    if (!bCreate) {
        dlclose(mAlgorithm_.handle);
        mAlgorithm_.Clear();
    }

    DEV_HILOGI(SERVICE, "Exit");
    return ERR_OK;
}

DevicestatusMsdpInterface* DevicestatusMsdpClientImpl::GetAlgorithmInst()
{
    DEV_HILOGI(SERVICE, "Enter");
    if (mAlgorithm_.handle == nullptr) {
        return nullptr;
    }

    if (mAlgorithm_.pAlgorithm == nullptr) {
        std::unique_lock<std::mutex> lock(mMutex_);
        DEV_HILOGI(SERVICE, "Get mAlgorithm.pAlgorithm");
        mAlgorithm_.pAlgorithm = mAlgorithm_.create();
    }

    return mAlgorithm_.pAlgorithm;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
