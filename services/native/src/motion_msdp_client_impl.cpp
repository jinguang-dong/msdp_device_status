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

#include "motion_msdp_client_impl.h"

#include <cerrno>
#include <dlfcn.h>
#include <errors.h>
#include <linux/netlink.h>
#include <string>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

// #include "dummy_values_bucket.h"
#include "motion_common.h"

using namespace OHOS::NativeRdb;
namespace OHOS {
namespace Msdp {
namespace {
const int ERR_OK = 0;
const int ERR_NG = -1;
const std::string MOTION_SENSOR_HDI_LIB_PATH = "libmotion_sensorhdi.z.so";
const std::string MOTION_MSDP_ALGORITHM_LIB_PATH = "libmotion_msdp.z.so";
std::map<MotionDataUtils::MotionType, MotionDataUtils::MotionValue> g_motionDataMap_;
MotionMsdpClientImpl::CallbackManager g_callbacksMgr_;
}

ErrCode MotionMsdpClientImpl::EnableMsdpImpl(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (msdpInterface_ == nullptr) {
        msdpInterface_ = GetAlgorithmInst();
        if (msdpInterface_ == nullptr) {
            MOTION_HILOGI(MOTION_MODULE_SERVICE, "get msdp module instance failed");
            return ERR_NG;
        }
    }

    if (sensorHdiInterface_ == nullptr) {
        sensorHdiInterface_ = GetSensorHdiInst();
        if (sensorHdiInterface_ == nullptr) {
            MOTION_HILOGI(MOTION_MODULE_SERVICE, "get sensor module instance failed");
            return ERR_NG;
        }
    }

    try {
        msdpInterface_->Enable(type);
        // sensorHdiInterface_->Enable(type);
    }
    catch (std::exception const &e) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "load algorithm library failed");
    }

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
    return ERR_OK;
}

ErrCode MotionMsdpClientImpl::DisableMsdpImpl(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    if (msdpInterface_ == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "disable msdp impl failed");
        return ERR_NG;
    }

    if (sensorHdiInterface_ == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "disable msdp impl failed");
        return ERR_NG;
    }

    try {
        msdpInterface_->Disable(type);
        // sensorHdiInterface_->Disable(type);
    }
    catch (std::exception const & e) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "disable msdp impl failed");
    }

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Exit");
    return ERR_OK;
}

ErrCode MotionMsdpClientImpl::DisableMsdpImplCount(const MotionDataUtils::MotionType& type)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");

    msdpInterface_->DisableCount(type);
    // sensorHdiInterface_->Disable(type);
    return ERR_OK;
}

ErrCode MotionMsdpClientImpl::RegisterImpl(CallbackManager& callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    g_callbacksMgr_ = callback;

    if (msdpInterface_ == nullptr) {
        msdpInterface_ = GetAlgorithmInst();
        if (msdpInterface_ == nullptr) {
            MOTION_HILOGI(MOTION_MODULE_SERVICE, "get msdp module instance failed");
            return ERR_NG;
        }
    }

    if (sensorHdiInterface_ == nullptr) {
        sensorHdiInterface_ = GetSensorHdiInst();
        if (sensorHdiInterface_ == nullptr) {
            MOTION_HILOGI(MOTION_MODULE_SERVICE, "get sensor module instance failed");
            return ERR_NG;
        }
    }

    RegisterMsdp(callback);
    RegisterSensor(callback);

    return ERR_OK;
}

ErrCode MotionMsdpClientImpl::UnregisterImpl()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "Enter");
    if (g_callbacksMgr_ == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "unregister callback failed");
        return ERR_NG;
    }

    UnregisterMsdp();
    UnregisterSensor();

    g_callbacksMgr_ = nullptr;

    return ERR_OK;
}

ErrCode MotionMsdpClientImpl::ImplCallback(MotionDataUtils::MotionData& data)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (g_callbacksMgr_ == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "g_callbacksMgr_ is nullptr");
        return ERR_NG;
    }
    g_callbacksMgr_(data);

    return ERR_OK;
}

ErrCode MotionMsdpClientImpl::RegisterSensor(const CallbackManager& callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (sensorHdiInterface_ != nullptr) {
        std::shared_ptr<MotionSensorHdiCallback> motionSensorHdiCallback = std::make_shared<MotionMsdpClientImpl>();
        sensorHdiInterface_->RegisterCallback(motionSensorHdiCallback);
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "in function registersensor, sensorHdiInterface_ is not nullptr");
    }

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
    return ERR_OK;
}

ErrCode MotionMsdpClientImpl::UnregisterSensor()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (sensorHdiInterface_ == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "unregister callback failed");
        return ERR_NG;
    }

    try {
        sensorHdiInterface_->UnregisterCallback();
    }
    catch (std::exception const &e) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "unregister callback failed");
    }
    sensorHdiInterface_ = nullptr;

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
    return ERR_OK;
}

void MotionMsdpClientImpl::OnResult(MotionDataUtils::MotionData& data)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "OnResult enter");

    MOTION_HILOGI(MOTION_MODULE_ALGORITHM, "%{public}s: motionData.type:%{public}d, motionData.status: %{public}d, motionData.action: %{public}d, motionData.move: %{public}f",
        __func__,  static_cast<int>(data.type), static_cast<int>(data.status), static_cast<int>(data.action), data.move);  
    MsdpCallback(data);
}

ErrCode MotionMsdpClientImpl::RegisterMsdp(const CallbackManager& callback)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (msdpInterface_ != nullptr) {
        std::shared_ptr<MsdpAlgorithmCallback> msdpAlgorithmCallback = std::make_shared<MotionMsdpClientImpl>();
        msdpInterface_->RegisterCallback(msdpAlgorithmCallback);
    }

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
    return ERR_OK;
}

ErrCode MotionMsdpClientImpl::UnregisterMsdp()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (msdpInterface_ == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "unregister callback failed");
        return ERR_NG;
    }

    try {
        msdpInterface_->UnregisterCallback();
    }
    catch (std::exception const &e) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "register callback failed");
    }

    msdpInterface_ = nullptr;

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
    return ERR_OK;
}

int32_t MotionMsdpClientImpl::MsdpCallback(MotionDataUtils::MotionData& data)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    SaveObserverData(data);
    if (notifyManagerFlag_) {
        ImplCallback(data);
        notifyManagerFlag_ = false;
    }

    return ERR_OK;
}

MotionDataUtils::MotionData MotionMsdpClientImpl::SaveObserverData(MotionDataUtils::MotionData& data)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    for (auto iter = g_motionDataMap_.begin(); iter != g_motionDataMap_.end(); ++iter) {
        if (iter->first == data.type) {
            iter->second = data.value;
            notifyManagerFlag_ = true;
            return data;
        }
    }

    g_motionDataMap_.insert(std::make_pair(data.type, data.value));
    notifyManagerFlag_ = true;

    return data;
}

std::map<MotionDataUtils::MotionType, MotionDataUtils::MotionValue> MotionMsdpClientImpl::GetObserverData() const
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    for (auto iter = g_motionDataMap_.begin(); iter != g_motionDataMap_.end(); ++iter) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "type = %{public}d", iter->first);
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "value = %{public}d", iter->second);
    }

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
    return g_motionDataMap_;
}

void MotionMsdpClientImpl::GetMotionTimestamp()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
}

void MotionMsdpClientImpl::GetLongtitude()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
}

void MotionMsdpClientImpl::GetLatitude()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
}

int32_t MotionMsdpClientImpl::LoadSensorHdiLibrary()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (sensorHdi_.handle != nullptr) {
        return ERR_OK;
    }
    sensorHdi_.handle = dlopen(MOTION_SENSOR_HDI_LIB_PATH.c_str(), RTLD_LAZY);
    if (sensorHdi_.handle == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "Cannot load sensor hdi library error = %{public}s", dlerror());
        return ERR_NG;
    }

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "start create sensor hdi pointer");
    sensorHdi_.create = (MotionSensorInterface* (*)()) dlsym(sensorHdi_.handle, "Create");
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "start destroy sensor hdi pointer");
    sensorHdi_.destroy = (void *(*)(MotionSensorInterface*))dlsym(sensorHdi_.handle, "Destroy");

    if (sensorHdi_.create == nullptr || sensorHdi_.destroy == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s dlsym Create or Destory sensor hdi failed!",
            MOTION_MSDP_ALGORITHM_LIB_PATH.c_str());
        dlclose(sensorHdi_.handle);
        sensorHdi_.Clear();
        return ERR_NG;
    }

    sensorHdi_.pAlgorithm = sensorHdi_.create();

    return ERR_OK;
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
}

int32_t MotionMsdpClientImpl::UnloadSensorHdiLibrary()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (sensorHdi_.handle == nullptr) {
        return ERR_NG;
    }

    if (sensorHdi_.pAlgorithm != nullptr) {
        sensorHdi_.destroy(sensorHdi_.pAlgorithm);
        sensorHdi_.pAlgorithm = nullptr;
    }

    dlclose(sensorHdi_.handle);
    sensorHdi_.Clear();

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
    return ERR_OK;
}

MotionSensorInterface* MotionMsdpClientImpl::GetSensorHdiInst()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (sensorHdi_.handle == nullptr) {
        return nullptr;
    }

    if (sensorHdi_.pAlgorithm == nullptr) {
        std::unique_lock<std::mutex> lock(mMutex_);
        if (sensorHdi_.pAlgorithm == nullptr) {
            MOTION_HILOGI(MOTION_MODULE_SERVICE, "Get mAlgorithm.pAlgorithm");
            sensorHdi_.pAlgorithm = sensorHdi_.create();
        }
    }

    return sensorHdi_.pAlgorithm;
}

int32_t MotionMsdpClientImpl::LoadAlgorithmLibrary()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (mAlgorithm_.handle != nullptr) {
        return ERR_OK;
    }
    mAlgorithm_.handle = dlopen(MOTION_MSDP_ALGORITHM_LIB_PATH.c_str(), RTLD_LAZY);
    if (mAlgorithm_.handle == nullptr) {
        MOTION_HILOGE(MOTION_MODULE_SERVICE, "Cannot load library error = %{public}s", dlerror());
        return ERR_NG;
    }

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "start create pointer");
    mAlgorithm_.create = (MotionMsdpInterface* (*)()) dlsym(mAlgorithm_.handle, "Create");
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "start destroy pointer");
    mAlgorithm_.destroy = (void *(*)(MotionMsdpInterface*))dlsym(mAlgorithm_.handle, "Destroy");

    if (mAlgorithm_.create == nullptr || mAlgorithm_.destroy == nullptr) {
        MOTION_HILOGI(MOTION_MODULE_SERVICE, "%{public}s dlsym Create or Destory failed!",
            MOTION_MSDP_ALGORITHM_LIB_PATH.c_str());
        dlclose(mAlgorithm_.handle);
        mAlgorithm_.Clear();
        return ERR_NG;
    }

    mAlgorithm_.pAlgorithm = mAlgorithm_.create();

    return ERR_OK;
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
}

int32_t MotionMsdpClientImpl::UnloadAlgorithmLibrary()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (mAlgorithm_.handle == nullptr) {
        return ERR_NG;
    }

    if (mAlgorithm_.pAlgorithm != nullptr) {
        mAlgorithm_.destroy(mAlgorithm_.pAlgorithm);
        mAlgorithm_.pAlgorithm = nullptr;
    }

    dlclose(mAlgorithm_.handle);
    mAlgorithm_.Clear();

    MOTION_HILOGI(MOTION_MODULE_SERVICE, "exit");
    return ERR_OK;
}

MotionMsdpInterface* MotionMsdpClientImpl::GetAlgorithmInst()
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "enter");
    if (mAlgorithm_.handle == nullptr) {
        return nullptr;
    }

    if (mAlgorithm_.pAlgorithm == nullptr) {
        std::unique_lock<std::mutex> lock(mMutex_);
        if (mAlgorithm_.pAlgorithm == nullptr) {
            MOTION_HILOGI(MOTION_MODULE_SERVICE, "Get mAlgorithm.pAlgorithm");
            mAlgorithm_.pAlgorithm = mAlgorithm_.create();
        }
    }

    return mAlgorithm_.pAlgorithm;
}
}
}
