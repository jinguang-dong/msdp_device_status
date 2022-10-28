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

#include <dlfcn.h>
#include <string>
#include <cerrno>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <errors.h>
#include <linux/netlink.h>

#include "devicestatus_common.h"

using namespace OHOS::NativeRdb;
namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr int32_t ERR_OK = 0;
constexpr int32_t ERR_NG = -1;
const std::string DEVICESTATUS_SENSOR_HDI_LIB_PATH = "libdevicestatus_sensorhdi.z.so";
const std::string DEVICESTATUS_MSDP_ALGORITHM_LIB_PATH = "libdevicestatus_msdp.z.so";
const std::string DEVICESTATUS_ALGORITHM_MANAGER_LIB_PATH = "libdevicestatus_algorithm_manager.z.so";
std::map<DataUtils::Type, DataUtils::Value> g_devicestatusDataMap;
DevicestatusMsdpClientImpl::CallbackManager g_callbacksMgr;
using clientType = DataUtils::Type;
using clientValue = DataUtils::Value;
IMsdp* g_msdpInterface;
ISensor* g_sensorHdiInterface;
IAlgoMgr* g_devAlgorithmInterface;
}

ErrCode DevicestatusMsdpClientImpl::InitMsdpImpl(const DataUtils::Type& type)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (g_msdpInterface == nullptr) {
        g_msdpInterface = GetAlgorithmInst();
        if (g_msdpInterface == nullptr) {
            DEV_HILOGD(SERVICE, "get msdp module instance failed");
            return ERR_NG;
        }
    }

    if (g_sensorHdiInterface == nullptr) {
        g_sensorHdiInterface = GetSensorHdiInst();
        if (g_sensorHdiInterface == nullptr) {
            DEV_HILOGD(SERVICE, "get sensor module instance failed");
            return ERR_NG;
        }
    }

    if (g_devAlgorithmInterface == nullptr) {
        g_devAlgorithmInterface = GetDevAlgorithmInst();
        if (g_devAlgorithmInterface == nullptr) {
            DEV_HILOGD(SERVICE, "get dev_alogrithm_manager module instance failed");
            return ERR_NG;
        }
    }
    if (type == DataUtils::Type::TYPE_LID_OPEN) {
        g_sensorHdiInterface->Enable();
    } else {
        g_msdpInterface->Enable();
        g_sensorHdiInterface->Enable();
        g_devAlgorithmInterface->Enable(type);
    }

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::DisableMsdpImpl(const DataUtils::Type& type)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (g_msdpInterface == nullptr) {
        DEV_HILOGD(SERVICE, "disable msdp impl failed");
        return ERR_NG;
    }

    if (g_sensorHdiInterface == nullptr) {
        DEV_HILOGD(SERVICE, "disable msdp impl failed");
        return ERR_NG;
    }

    if (g_devAlgorithmInterface == nullptr) {
        DEV_HILOGD(SERVICE, "disable msdp impl failed");
        return ERR_NG;
    }
    g_msdpInterface->Disable();
    g_sensorHdiInterface->Disable();
    g_devAlgorithmInterface->Disable(type);
    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::RegisterSensor()
{
    DEV_HILOGD(SERVICE, "Enter");
    if (g_sensorHdiInterface != nullptr) {
        std::shared_ptr<SensorHdiCallback> callback = std::make_shared<DevicestatusMsdpClientImpl>();
        g_sensorHdiInterface->RegisterCallback(callback);
        DEV_HILOGD(SERVICE, "g_sensorHdiInterface is not nullptr");
    }

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::UnregisterSensor(void)
{
    DEV_HILOGD(SERVICE, "Enter");

    if (g_sensorHdiInterface == nullptr) {
        DEV_HILOGD(SERVICE, "unregister callback failed");
        return ERR_NG;
    }

    g_sensorHdiInterface->UnregisterCallback();
    g_sensorHdiInterface = nullptr;

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::RegisterImpl(const CallbackManager& callback)
{
    DEV_HILOGD(SERVICE, "Enter");
    g_callbacksMgr = callback;

    if (g_msdpInterface == nullptr) {
        g_msdpInterface = GetAlgorithmInst();
        if (g_msdpInterface == nullptr) {
            DEV_HILOGD(SERVICE, "get msdp module instance failed");
            return ERR_NG;
        }
    }

    if (g_sensorHdiInterface == nullptr) {
        g_sensorHdiInterface = GetSensorHdiInst();
        if (g_sensorHdiInterface == nullptr) {
            DEV_HILOGD(SERVICE, "get sensor module instance failed");
            return ERR_NG;
        }
    }

    if (g_devAlgorithmInterface == nullptr) {
        g_devAlgorithmInterface = GetDevAlgorithmInst();
        if (g_devAlgorithmInterface == nullptr) {
            DEV_HILOGD(SERVICE, "get dev_alogrithm_manager module instance failed");
            return ERR_NG;
        }
    }

    RegisterMsdp();
    RegisterSensor();
    RegisterDevAlgorithm();

    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::UnregisterImpl()
{
    DEV_HILOGD(SERVICE, "Enter");
    if (g_callbacksMgr == nullptr) {
        DEV_HILOGD(SERVICE, "unregister callback failed");
        return ERR_NG;
    }

    UnregisterMsdp();
    UnregisterSensor();
    UnregisterDevAlgorithm();

    g_callbacksMgr = nullptr;

    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::ImplCallback(const DataUtils::Data& data)
{
    if (g_callbacksMgr == nullptr) {
        DEV_HILOGD(SERVICE, "g_callbacksMgr is nullptr");
        return ERR_NG;
    }
    g_callbacksMgr(data);

    return ERR_OK;
}

void DevicestatusMsdpClientImpl::OnResult(const DataUtils::Data& data)
{
    MsdpCallback(data);
}

void DevicestatusMsdpClientImpl::OnSensorHdiResult(const DataUtils::Data& data)
{
    MsdpCallback(data);
}

void DevicestatusMsdpClientImpl::OnAlogrithmResult(const DataUtils::Data& data)
{
    MsdpCallback(data);
}

ErrCode DevicestatusMsdpClientImpl::RegisterMsdp()
{
    DEV_HILOGD(SERVICE, "Enter");
    if (g_msdpInterface != nullptr) {
        std::shared_ptr<MsdpAlgoCallback> callback = std::make_shared<DevicestatusMsdpClientImpl>();
        g_msdpInterface->RegisterCallback(callback);
    }

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::UnregisterMsdp(void)
{
    DEV_HILOGD(SERVICE, "Enter");

    if (g_msdpInterface == nullptr) {
        DEV_HILOGD(SERVICE, "unregister callback failed");
        return ERR_NG;
    }

    g_msdpInterface->UnregisterCallback();
    g_msdpInterface = nullptr;

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}
ErrCode DevicestatusMsdpClientImpl::RegisterDevAlgorithm()
{
    DEV_HILOGD(SERVICE, "Enter");
    if (g_devAlgorithmInterface != nullptr) {
        std::shared_ptr<AlgoCallback> callback_ = std::make_shared<DevicestatusMsdpClientImpl>();
        g_devAlgorithmInterface->RegisterCallback(callback_);
    }

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode DevicestatusMsdpClientImpl::UnregisterDevAlgorithm(void)
{
    DEV_HILOGD(SERVICE, "Enter");

    if (g_devAlgorithmInterface == nullptr) {
        DEV_HILOGD(SERVICE, "unregister callback failed");
        return ERR_NG;
    }

    g_devAlgorithmInterface->UnregisterCallback();
    g_devAlgorithmInterface = nullptr;

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

int32_t DevicestatusMsdpClientImpl::MsdpCallback(const DataUtils::Data& data)
{
    DEV_HILOGD(SERVICE, "Enter");
    DevicestatusDumper::GetInstance().pushDeviceStatus(data);
    SaveObserverData(data);
    if (notifyManagerFlag_) {
        ImplCallback(data);
        notifyManagerFlag_ = false;
    }
    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

DataUtils::Data DevicestatusMsdpClientImpl::SaveObserverData(
    const DataUtils::Data& data)
{
    DEV_HILOGD(SERVICE, "Enter");
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
    DEV_HILOGD(SERVICE, "Enter");
    return g_devicestatusDataMap;
}

void DevicestatusMsdpClientImpl::GetDevicestatusTimestamp()
{
    DEV_HILOGD(SERVICE, "Enter");

    DEV_HILOGD(SERVICE, "Exit");
}

void DevicestatusMsdpClientImpl::GetLongtitude()
{
    DEV_HILOGD(SERVICE, "Enter");

    DEV_HILOGD(SERVICE, "Exit");
}

void DevicestatusMsdpClientImpl::GetLatitude()
{
    DEV_HILOGD(SERVICE, "Enter");

    DEV_HILOGD(SERVICE, "Exit");
}

int32_t DevicestatusMsdpClientImpl::LoadSensorHdiLibrary(bool bCreate)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (sensorHdi_.handle != nullptr) {
        return ERR_OK;
    }
    sensorHdi_.handle = dlopen(DEVICESTATUS_SENSOR_HDI_LIB_PATH.c_str(), RTLD_LAZY);
    if (sensorHdi_.handle == nullptr) {
        DEV_HILOGE(SERVICE,
            "Cannot load sensor hdi library error = %{public}s", dlerror());
        return ERR_NG;
    }

    DEV_HILOGD(SERVICE, "start create sensor hdi pointer");
    sensorHdi_.create = (ISensor* (*)()) dlsym(sensorHdi_.handle, "Create");
    DEV_HILOGD(SERVICE, "start destroy sensor hdi pointer");
    sensorHdi_.destroy = (void *(*)(ISensor*))dlsym(sensorHdi_.handle, "Destroy");

    if (sensorHdi_.create == nullptr || sensorHdi_.destroy == nullptr) {
        DEV_HILOGD(SERVICE, "%{public}s dlsym Create or Destroy sensor hdi failed!",
            DEVICESTATUS_MSDP_ALGORITHM_LIB_PATH.c_str());
        dlclose(sensorHdi_.handle);
        sensorHdi_.Clear();
        bCreate = false;
        return ERR_NG;
    }

    if (bCreate) {
        sensorHdi_.pAlgorithm = sensorHdi_.create();
    }

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

int32_t DevicestatusMsdpClientImpl::UnloadSensorHdiLibrary(bool bCreate)
{
    DEV_HILOGD(SERVICE, "Enter");
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

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ISensor* DevicestatusMsdpClientImpl::GetSensorHdiInst()
{
    DEV_HILOGD(SERVICE, "Enter");
    if (sensorHdi_.handle == nullptr) {
        return nullptr;
    }

    if (sensorHdi_.pAlgorithm == nullptr) {
        std::unique_lock<std::mutex> lock(mMutex_);
        if (sensorHdi_.pAlgorithm == nullptr) {
            DEV_HILOGD(SERVICE, "Get mAlgorithm.pAlgorithm");
            sensorHdi_.pAlgorithm = sensorHdi_.create();
        }
    }

    return sensorHdi_.pAlgorithm;
}

int32_t DevicestatusMsdpClientImpl::LoadAlgorithmLibrary(bool bCreate)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (mAlgorithm_.handle != nullptr) {
        return ERR_OK;
    }
    mAlgorithm_.handle = dlopen(DEVICESTATUS_MSDP_ALGORITHM_LIB_PATH.c_str(), RTLD_LAZY);
    if (mAlgorithm_.handle == nullptr) {
        DEV_HILOGE(SERVICE, "Cannot load library error = %{public}s", dlerror());
        return ERR_NG;
    }

    DEV_HILOGD(SERVICE, "start create pointer");
    mAlgorithm_.create = (IMsdp* (*)()) dlsym(mAlgorithm_.handle, "Create");
    DEV_HILOGD(SERVICE, "start destroy pointer");
    mAlgorithm_.destroy = (void *(*)(IMsdp*))dlsym(mAlgorithm_.handle, "Destroy");

    if (mAlgorithm_.create == nullptr || mAlgorithm_.destroy == nullptr) {
        DEV_HILOGD(SERVICE, "%{public}s dlsym Create or Destroy failed!",
            DEVICESTATUS_MSDP_ALGORITHM_LIB_PATH.c_str());
        dlclose(mAlgorithm_.handle);
        mAlgorithm_.Clear();
        bCreate = false;
        return ERR_NG;
    }

    if (bCreate) {
        mAlgorithm_.pAlgorithm = mAlgorithm_.create();
    }

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

int32_t DevicestatusMsdpClientImpl::UnloadAlgorithmLibrary(bool bCreate)
{
    DEV_HILOGD(SERVICE, "Enter");
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

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

IMsdp* DevicestatusMsdpClientImpl::GetAlgorithmInst()
{
    DEV_HILOGD(SERVICE, "Enter");
    if (mAlgorithm_.handle == nullptr) {
        return nullptr;
    }

    if (mAlgorithm_.pAlgorithm == nullptr) {
        std::unique_lock<std::mutex> lock(mMutex_);
        if (mAlgorithm_.pAlgorithm == nullptr) {
            DEV_HILOGD(SERVICE, "Get mAlgorithm.pAlgorithm");
            mAlgorithm_.pAlgorithm = mAlgorithm_.create();
        }
    }

    return mAlgorithm_.pAlgorithm;
}

int32_t DevicestatusMsdpClientImpl::LoadDevAlgorithmLibrary(bool bCreate)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (devAlgorithm_.handle != nullptr) {
        return ERR_OK;
    }
    devAlgorithm_.handle = dlopen(DEVICESTATUS_ALGORITHM_MANAGER_LIB_PATH.c_str(), RTLD_LAZY);
    if (devAlgorithm_.handle == nullptr) {
        DEV_HILOGE(SERVICE, "Cannot load library error = %{public}s", dlerror());
        return ERR_NG;
    }

    DEV_HILOGD(SERVICE, "start create pointer");
    devAlgorithm_.create = (IAlgoMgr* (*)()) dlsym(devAlgorithm_.handle, "Create");
    DEV_HILOGD(SERVICE, "start destroy pointer");
    devAlgorithm_.destroy = (void *(*)(IAlgoMgr*))dlsym(devAlgorithm_.handle, "Destroy");

    if (devAlgorithm_.create == nullptr || devAlgorithm_.destroy == nullptr) {
        DEV_HILOGD(SERVICE, "%{public}s dlsym Create or Destroy failed!",
            DEVICESTATUS_ALGORITHM_MANAGER_LIB_PATH.c_str());
        dlclose(devAlgorithm_.handle);
        devAlgorithm_.Clear();
        bCreate = false;
        return ERR_NG;
    }

    if (bCreate) {
        devAlgorithm_.pAlgorithm = devAlgorithm_.create();
    }

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

int32_t DevicestatusMsdpClientImpl::UnloadDevAlgorithmLibrary(bool bCreate)
{
    DEV_HILOGD(SERVICE, "Enter");
    if (devAlgorithm_.handle == nullptr) {
        return ERR_NG;
    }

    if (devAlgorithm_.pAlgorithm != nullptr) {
        devAlgorithm_.destroy(devAlgorithm_.pAlgorithm);
        devAlgorithm_.pAlgorithm = nullptr;
    }

    if (!bCreate) {
        dlclose(devAlgorithm_.handle);
        devAlgorithm_.Clear();
    }

    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

IAlgoMgr* DevicestatusMsdpClientImpl::GetDevAlgorithmInst()
{
    DEV_HILOGD(SERVICE, "Enter");
    if (devAlgorithm_.handle == nullptr) {
        return nullptr;
    }

    if (devAlgorithm_.pAlgorithm == nullptr) {
        std::unique_lock<std::mutex> lock(mMutex_);
        if (devAlgorithm_.pAlgorithm == nullptr) {
            DEV_HILOGD(SERVICE, "Get mAlgorithm.pAlgorithm");
            devAlgorithm_.pAlgorithm = devAlgorithm_.create();
        }
    }

    return devAlgorithm_.pAlgorithm;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
