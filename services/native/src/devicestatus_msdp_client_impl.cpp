/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <string>

#include <dlfcn.h>

#include "devicestatus_define.h"
#include "devicestatus_dumper.h"
#include "fi_log.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DeviceStatusMsdpClientImpl" };
#ifdef __aarch64__
const std::string DEVICESTATUS_MOCK_LIB_PATH { "/system/lib64/libdevicestatus_mock.z.so" };
const std::string DEVICESTATUS_ALGO_LIB_PATH { "/system/lib64/libdevicestatus_algo.z.so" };
#else
const std::string DEVICESTATUS_MOCK_LIB_PATH { "/system/lib/libdevicestatus_mock.z.so" };
const std::string DEVICESTATUS_ALGO_LIB_PATH { "/system/lib/libdevicestatus_algo.z.so" };
#endif
using ClientType = Type;
using ClientValue = OnChangedValue;
} // namespace

DeviceStatusMsdpClientImpl::DeviceStatusMsdpClientImpl()
{
    for (int32_t type = 0; type < static_cast<int32_t>(Type::TYPE_MAX); ++type) {
        algoCallCount_[static_cast<Type>(type)] = 0;
        mockCallCount_[static_cast<Type>(type)] = 0;
    }
}

ErrCode DeviceStatusMsdpClientImpl::InitMsdpImpl(Type type)
{
    CALL_DEBUG_ENTER;
    if (GetSensorHdi(type) == RET_OK) {
        FI_HILOGI("GetSensorHdi is support");
        return RET_OK;
    }
    if (AlgoHandle(type) == RET_OK) {
        FI_HILOGI("AlgoHandle is support");
        return RET_OK;
    }
    if (MockHandle(type) == RET_OK) {
        FI_HILOGI("MockHandle is support");
        return RET_OK;
    }
    return RET_ERR;
}

ErrCode DeviceStatusMsdpClientImpl::MockHandle(Type type)
{
    if (StartMock(type) == RET_ERR) {
        FI_HILOGE("Start mock Library failed");
        return RET_ERR;
    }
    CHKPR(iMock_, RET_ERR);
    iMock_->Enable(type);
    auto iter = mockCallCount_.find(type);
    if (iter == mockCallCount_.end()) {
        auto ret = mockCallCount_.emplace(type, 0);
        if (!ret.second) {
            FI_HILOGW("type is duplicated");
            return RET_ERR;
        }
    } else {
        iter->second++;
    }
    RegisterMock();
    FI_HILOGI("mockCallCount_:%{public}d", mockCallCount_[type]);
    return RET_OK;
}

ErrCode DeviceStatusMsdpClientImpl::AlgoHandle(Type type)
{
    if (GetAlgoAbility(type) == RET_ERR) {
        FI_HILOGE("Algo Library is not support");
        return RET_ERR;
    }
    if (StartAlgo(type) == RET_ERR) {
        FI_HILOGE("Start algo Library failed");
        return RET_ERR;
    }
    CHKPR(iAlgo_, RET_ERR);
    if ((iAlgo_->Enable(type)) == RET_ERR) {
        FI_HILOGE("Enable algo Library failed");
        return RET_ERR;
    }

    auto iter = algoCallCount_.find(type);
    if (iter == algoCallCount_.end()) {
        auto ret =  algoCallCount_.emplace(type, 0);
        if (!ret.second) {
            FI_HILOGW("type is duplicated");
            return RET_ERR;
        }
    } else {
        iter->second++;
    }
    RegisterAlgo();
    FI_HILOGI("algoCallCount_:%{public}d", algoCallCount_[type]);
    return RET_OK;
}

ErrCode DeviceStatusMsdpClientImpl::StartAlgo(Type type)
{
    if (LoadAlgoLibrary() == RET_ERR) {
        FI_HILOGE("Load algo Library failed");
        return RET_ERR;
    }
    iAlgo_ = GetAlgoInst(type);
    CHKPR(iAlgo_, RET_ERR);
    return RET_OK;
}

ErrCode DeviceStatusMsdpClientImpl::StartMock(Type type)
{
    if (LoadMockLibrary() == RET_ERR) {
        FI_HILOGE("Load mock Library failed");
        return RET_ERR;
    }
    iMock_ = GetMockInst(type);
    if (iMock_ == nullptr) {
        FI_HILOGE("Get mock module failed");
        return RET_ERR;
    }
    return RET_OK;
}

ErrCode DeviceStatusMsdpClientImpl::GetSensorHdi(Type type)
{
    return RET_ERR;
}

ErrCode DeviceStatusMsdpClientImpl::GetAlgoAbility(Type type)
{
    if ((type == Type::TYPE_ABSOLUTE_STILL) || (type == Type::TYPE_HORIZONTAL_POSITION) ||
        (type == Type::TYPE_VERTICAL_POSITION)) {
        return RET_OK;
    }
    FI_HILOGI("Not support ability");
    return RET_ERR;
}

ErrCode DeviceStatusMsdpClientImpl::Disable(Type type)
{
    CALL_DEBUG_ENTER;
    return (((SensorHdiDisable(type) == RET_OK) || (AlgoDisable(type) == RET_OK) || (MockDisable(type) == RET_OK)) ?
        RET_OK : RET_ERR);
}

ErrCode DeviceStatusMsdpClientImpl::SensorHdiDisable(Type type)
{
    return RET_ERR;
}

ErrCode DeviceStatusMsdpClientImpl::AlgoDisable(Type type)
{
    CALL_DEBUG_ENTER;
    CHKPR(iAlgo_, RET_ERR);
    auto iter = algoCallCount_.find(type);
    if (iter == algoCallCount_.end()) {
        FI_HILOGE("Failed to find record type");
        return RET_ERR;
    }
    if (iter->second == 0) {
        algoCallCount_.erase(type);
    } else {
        iAlgo_->Disable(type);
        UnregisterAlgo();
    }
    iter->second--;
    algoCallCount_.erase(type);
    if (algoCallCount_.empty()) {
        if (UnloadAlgoLibrary() == RET_ERR) {
            FI_HILOGE("Failed to close algorithm library");
            return RET_ERR;
        }
        FI_HILOGI("Close algorithm library");
        iAlgo_ = nullptr;
        callBacksMgr_ = nullptr;
    }
    FI_HILOGI("algoCallCount_:%{public}d", algoCallCount_[type]);
    return RET_OK;
}

ErrCode DeviceStatusMsdpClientImpl::MockDisable(Type type)
{
    CALL_DEBUG_ENTER;
    CHKPR(iMock_, RET_ERR);
    auto iter = mockCallCount_.find(type);
    if (iter == mockCallCount_.end()) {
        FI_HILOGE("Failed to find record type");
        return RET_ERR;
    }
    if (iter->second == 0) {
        mockCallCount_.erase(type);
    } else {
        iMock_->DisableCount(type);
        iMock_->Disable(type);
        UnregisterMock();
    }
    iter->second--;
    if (mockCallCount_.empty()) {
        if (UnloadMockLibrary() == RET_ERR) {
            FI_HILOGE("Failed to close library");
            return RET_ERR;
        }
        iMock_ = nullptr;
        callBacksMgr_ = nullptr;
    }
    return RET_OK;
}

ErrCode DeviceStatusMsdpClientImpl::ImplCallback(const Data& data)
{
    CHKPR(callBacksMgr_, RET_ERR);
    callBacksMgr_(data);
    return RET_OK;
}

ErrCode DeviceStatusMsdpClientImpl::RegisterImpl(const CallbackManager& callback)
{
    callBacksMgr_ = callback;
    return RET_OK;
}

void DeviceStatusMsdpClientImpl::OnResult(const Data& data)
{
    FI_HILOGD("type:%{public}d, value:%{public}d", data.type, data.value);
    MsdpCallback(data);
}

ErrCode DeviceStatusMsdpClientImpl::RegisterMock()
{
    CALL_DEBUG_ENTER;
    if (iMock_ != nullptr) {
        std::shared_ptr<IMsdp::MsdpAlgoCallback> callback = shared_from_this();
        iMock_->RegisterCallback(callback);
    }
    return RET_OK;
}

ErrCode DeviceStatusMsdpClientImpl::UnregisterMock()
{
    CALL_DEBUG_ENTER;
    CHKPR(iMock_, RET_ERR);
    iMock_->UnregisterCallback();
    return RET_OK;
}

ErrCode DeviceStatusMsdpClientImpl::RegisterAlgo()
{
    CALL_DEBUG_ENTER;
    if (iAlgo_ != nullptr) {
        std::shared_ptr<IMsdp::MsdpAlgoCallback> callback_ = shared_from_this();
        iAlgo_->RegisterCallback(callback_);
    }
    return RET_OK;
}

ErrCode DeviceStatusMsdpClientImpl::UnregisterAlgo()
{
    CALL_DEBUG_ENTER;
    CHKPR(iAlgo_, RET_ERR);
    iAlgo_->UnregisterCallback();
    return RET_OK;
}

int32_t DeviceStatusMsdpClientImpl::MsdpCallback(const Data& data)
{
    CALL_DEBUG_ENTER;
    DS_DUMPER->PushDeviceStatus(data);
    SaveObserverData(data);
    if (notifyManagerFlag_) {
        ImplCallback(data);
        notifyManagerFlag_ = false;
    }
    return RET_OK;
}

Data DeviceStatusMsdpClientImpl::SaveObserverData(const Data& data)
{
    CALL_DEBUG_ENTER;
    std::lock_guard guard(mutex_);
    for (auto iter = deviceStatusDataMap_.begin(); iter != deviceStatusDataMap_.end(); ++iter) {
        if (iter->first == data.type) {
            iter->second = data.value;
            notifyManagerFlag_ = true;
            return data;
        }
    }
    auto ret = deviceStatusDataMap_.insert(std::make_pair(data.type, data.value));
    if (!ret.second) {
        FI_HILOGW("type is duplicated");
        return data;
    }
    notifyManagerFlag_ = true;
    return data;
}

std::map<ClientType, ClientValue> DeviceStatusMsdpClientImpl::GetObserverData() const
{
    return deviceStatusDataMap_;
}

void DeviceStatusMsdpClientImpl::GetDeviceStatusTimestamp()
{}

void DeviceStatusMsdpClientImpl::GetLongtitude()
{}

void DeviceStatusMsdpClientImpl::GetLatitude()
{}

ErrCode DeviceStatusMsdpClientImpl::LoadMockLibrary()
{
    CALL_DEBUG_ENTER;
    if (mock_.handle != nullptr) {
        FI_HILOGE("mock handle is not nullptr");
        return RET_OK;
    }
    std::string dlName = DEVICESTATUS_MOCK_LIB_PATH;
    char libRealPath[PATH_MAX] = {};
    if (realpath(dlName .c_str(), libRealPath) == nullptr) {
        FI_HILOGE("get absolute algoPath is error, errno:%{public}d", errno);
        return RET_ERR;
    }
    mock_.handle = dlopen(libRealPath, RTLD_LAZY);
    if (mock_.handle == nullptr) {
        FI_HILOGE("Cannot load library error:%{public}s", dlerror());
        return RET_ERR;
    }
    FI_HILOGI("Start create pointer");
    mock_.create = reinterpret_cast<LoadMockLibraryFunc>(dlsym(mock_.handle, "Create"));
    FI_HILOGI("Start destroy pointer");
    mock_.destroy = reinterpret_cast<LoadMockLibraryPtr>(dlsym(mock_.handle, "Destroy"));
    if ((mock_.create == nullptr) || (mock_.destroy == nullptr)) {
        FI_HILOGE("%{public}s dlsym Create or Destroy failed",
            dlName.c_str());
        dlclose(mock_.handle);
        mock_.Clear();
        if (mock_.handle == nullptr) {
            return RET_OK;
        }
        FI_HILOGE("Load mock failed");
        return RET_ERR;
    }
    return RET_OK;
}

ErrCode DeviceStatusMsdpClientImpl::UnloadMockLibrary()
{
    CALL_DEBUG_ENTER;
    CHKPR(mock_.handle, RET_ERR);
    if (mock_.pAlgorithm != nullptr) {
        mock_.destroy(mock_.pAlgorithm);
        mock_.pAlgorithm = nullptr;
    }
    dlclose(mock_.handle);
    mock_.Clear();
    return RET_OK;
}

IMsdp* DeviceStatusMsdpClientImpl::GetMockInst(Type type)
{
    CALL_DEBUG_ENTER;
    CHKPP(mock_.handle);
    if (mock_.pAlgorithm == nullptr) {
        std::unique_lock lock(mutex_);
        mock_.pAlgorithm = mock_.create();
        mockCallCount_[type] = 0;
    }
    return mock_.pAlgorithm;
}

ErrCode DeviceStatusMsdpClientImpl::LoadAlgoLibrary()
{
    CALL_DEBUG_ENTER;
    if (algo_.handle != nullptr) {
        FI_HILOGE("algo handle has exists");
        return RET_OK;
    }
    std::string dlName = DEVICESTATUS_ALGO_LIB_PATH;
    char libRealPath[PATH_MAX] = {};
    if (realpath(dlName .c_str(), libRealPath) == nullptr) {
        FI_HILOGE("get absolute algoPath is error, errno:%{public}d", errno);
        return RET_ERR;
    }
    algo_.handle = dlopen(libRealPath, RTLD_LAZY);
    if (algo_.handle == nullptr) {
        FI_HILOGE("Cannot load library error:%{public}s", dlerror());
        return RET_ERR;
    }
    FI_HILOGI("Start create pointer");
    algo_.create = reinterpret_cast<LoadMockLibraryFunc>(dlsym(algo_.handle, "Create"));
    FI_HILOGI("Start destroy pointer");
    algo_.destroy = reinterpret_cast<LoadMockLibraryPtr>(dlsym(algo_.handle, "Destroy"));
    if ((algo_.create == nullptr) || (algo_.destroy == nullptr)) {
        FI_HILOGE("%{public}s dlsym Create or Destroy failed",
            dlName.c_str());
        dlclose(algo_.handle);
        algo_.Clear();
        if (algo_.handle == nullptr) {
            return RET_OK;
        }
        FI_HILOGE("Load algo failed");
        return RET_ERR;
    }
    return RET_OK;
}

ErrCode DeviceStatusMsdpClientImpl::UnloadAlgoLibrary()
{
    CALL_DEBUG_ENTER;
    CHKPR(algo_.handle, RET_ERR);
    if (algo_.pAlgorithm != nullptr) {
        algo_.destroy(algo_.pAlgorithm);
        algo_.pAlgorithm = nullptr;
    }
    dlclose(algo_.handle);
    algo_.Clear();
    return RET_OK;
}

IMsdp* DeviceStatusMsdpClientImpl::GetAlgoInst(Type type)
{
    CALL_DEBUG_ENTER;
    CHKPP(algo_.handle);
    if (algo_.pAlgorithm == nullptr) {
        std::unique_lock lock(mutex_);
        algo_.pAlgorithm = algo_.create();
    }
    return algo_.pAlgorithm;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
