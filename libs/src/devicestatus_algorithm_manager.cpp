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

#include <cerrno>
#include <linux/netlink.h>
#include <string>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "devicestatus_common.h"
#include "devicestatus_algorithm_manager.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
AlgoMgr* g_algo;
} // namespace

bool AlgoMgr::SensorStart(Type type)
{
    DEV_HILOGD(SERVICE, "enter");
    std::vector<int32_t> sensorTypes = GetSensorTypeId(type);
    if (sensorTypes.empty()) {
        DEV_HILOGE(SERVICE, "sensorTypes is empty");
        return false;
    }
    for (auto it = sensorTypes.begin(); it != sensorTypes.end(); it++) {
        if (!CheckSensorTypeId((*it))) {
            DEV_HILOGE(SERVICE, "Sensor type mismatch");
            return false;
        }
    }
    if (sensorEventCb_ == nullptr) {
        sensorEventCb_ = SensorDataCallback::GetInstance();
        sensorEventCb_->Init();
    }

    for (auto it = sensorTypes.begin(); it != sensorTypes.end(); it++) {
        sensorEventCb_->RegisterCallbackSensor((*it));
        callSensorNum_++;
    }

    return true;
}

bool AlgoMgr::SensorStop(std::vector<Type> algoTypes)
{
    DEV_HILOGD(SERVICE, "enter");
    for (auto it = algoTypes.begin(); it != algoTypes.end(); it++) {
        if (callAlgoNum_[(*it)] != 0) {
            return false;
        }
    }
    return true;
}

ErrCode AlgoMgr::RegisterCallback(std::shared_ptr<MsdpAlgoCallback> callback)
{
    DEV_HILOGD(SERVICE, "enter");
    switch (algoType_) {
        case Type::TYPE_STILL: {
            still_->RegisterCallback(callback);
            break;
        }
        case Type::TYPE_HORIZONTAL_POSITION: {
            horizontalPosition_->RegisterCallback(callback);
            break;
        }
        case Type::TYPE_VERTICAL_POSITION: {
            verticalPosition_->RegisterCallback(callback);
            break;
        }
        default: {
            DEV_HILOGE(SERVICE, "Unsupported algorithm type");
            return ERR_NG;
            break;
        }
    }
    return ERR_OK;
}

ErrCode AlgoMgr::UnregisterCallback()
{
    DEV_HILOGD(SERVICE, "enter");
    DEV_HILOGD(SERVICE, "exit");
    return ERR_OK;
}

bool AlgoMgr::CheckSensorTypeId(int32_t sensorTypeId)
{
    int32_t count = -1;
    SensorInfo *sensorInfo = nullptr;
    int32_t ret = GetAllSensors(&sensorInfo, &count);
    if (ret != 0) {
        return false;
    }
    for (int32_t i = 0; i < count; i++) {
        if ((sensorInfo + i)->sensorTypeId == sensorTypeId) {
            return true;
        }
    }
    return false;
}

std::vector<int32_t> AlgoMgr::GetSensorTypeId(Type type) 
{
    std::vector<int32_t> sensorTypes;
    switch (type) {
        case Type::TYPE_STILL: {
            sensorTypes.push_back(SensorTypeId::SENSOR_TYPE_ID_ACCELEROMETER);
            break;
        }
        case Type::TYPE_HORIZONTAL_POSITION: {
            sensorTypes.push_back(SensorTypeId::SENSOR_TYPE_ID_ACCELEROMETER);
            break;
        }
        case Type::TYPE_VERTICAL_POSITION: {
            sensorTypes.push_back(SensorTypeId::SENSOR_TYPE_ID_ACCELEROMETER);
            break;
        }
        default: {
            break;
        }
    }
    return sensorTypes;
}

std::vector<Type> AlgoMgr::GetAlgoType(int32_t type) 
{
    std::vector<Type> AlgoTypes;
    switch (type) {
        case SensorTypeId::SENSOR_TYPE_ID_ACCELEROMETER: {
            AlgoTypes.push_back(Type::TYPE_STILL);
            AlgoTypes.push_back(Type::TYPE_VERTICAL_POSITION);
            AlgoTypes.push_back(Type::TYPE_HORIZONTAL_POSITION);
            break;
        }
        default: {
            break;
        }
    }
    return AlgoTypes;
}

ErrCode AlgoMgr::Enable(Type type)
{
    DEV_HILOGD(SERVICE, "enter");
    DEV_HILOGI(SERVICE, "Enable type %{public}d", type);
    if (!SensorStart(type)) {
        DEV_HILOGE(SERVICE, "sensor init failed");
        return ERR_NG;
    }
    switch (type) {
        case Type::TYPE_STILL: {
            if (still_ == nullptr) {
                still_ = std::make_shared<AbsoluteStill>(sensorEventCb_);
                still_->Init(type);
                callAlgoNum_[type] = 0;
            }
            callAlgoNum_[type]++;
            break;
        }
        case Type::TYPE_HORIZONTAL_POSITION: {
            if (horizontalPosition_ == nullptr) {
                horizontalPosition_ = std::make_shared<Horizontal>(sensorEventCb_);
                horizontalPosition_->Init(type);
                callAlgoNum_[type] = 0;
            }
            callAlgoNum_[type]++;
            break;
        }
        case Type::TYPE_VERTICAL_POSITION: {
            if (verticalPosition_ == nullptr) {
                verticalPosition_ = std::make_shared<Vertical>(sensorEventCb_);
                verticalPosition_->Init(type);
                callAlgoNum_[type] = 0;
            }
            callAlgoNum_[type]++;
            break;
        }
        default: {
            DEV_HILOGE(SERVICE, "Unsupported algorithm type");
            return ERR_NG;
            break;
        }
    }
    algoType_ = type;
    DEV_HILOGD(SERVICE, "exit");
    return ERR_OK;
}

ErrCode AlgoMgr::Disable(Type type)
{
    DEV_HILOGD(SERVICE, "enter");
    DEV_HILOGI(SERVICE, "Disable type %{public}d", type);
    callAlgoNum_[type]--;
    DEV_HILOGI(SERVICE, "callAlgoNum_ %{public}d", callAlgoNum_[type]);
    if (callAlgoNum_[type] == 0) {
        switch (type) {
            case Type::TYPE_STILL: {
                if (still_ != nullptr) {
                    still_->UnSubscribe(type);
                    still_.reset();
                    still_ = nullptr;
                } else {
                    DEV_HILOGE(SERVICE, "NULLPTR");
                }
                break;
            }
            case Type::TYPE_HORIZONTAL_POSITION: {
                if (horizontalPosition_ != nullptr) {
                    horizontalPosition_->UnSubscribe(type);
                    horizontalPosition_.reset();
                    horizontalPosition_ = nullptr;
                }
                break;
            }
            case Type::TYPE_VERTICAL_POSITION: {
                if (verticalPosition_ != nullptr) {
                    verticalPosition_->UnSubscribe(type);
                    verticalPosition_.reset();
                    verticalPosition_ = nullptr;
                }
                break;
            }
            default: {
                DEV_HILOGE(SERVICE, "Unsupported algorithm type");
                break;
            }
        }
        callAlgoNum_.erase(type);
    }
    UnregisterSensor(type)
    return ERR_OK;
}

ErrCode AlgoMgr::UnregisterSensor(Type type)
{
    DEV_HILOGD(SERVICE, "enter");
    std::vector<int32_t> sensorTypes = GetSensorTypeId(type);
    std::vector<Type> algoTypes;
    for (std::vector<int32_t>::iterator it = sensorTypes.begin(); it != sensorTypes.end(); it++) {
        algoTypes = GetAlgoType((*it));
        if (SensorStop(algoTypes)) {
            sensorEventCb_->UnregisterCallbackSensor((*it));
            callSensorNum_--;
        }
    }
    if (callSensorNum_ == 0) {
        sensorEventCb_ = nullptr;
    }
    DEV_HILOGD(SERVICE, "exit");
    return ERR_OK;
}

extern "C" IMsdp *Create(void)
{
    DEV_HILOGD(SERVICE, "enter");
    g_algo = new AlgoMgr();
    return g_algo;
}

extern "C" void Destroy(const IMsdp* algorithm)
{
    DEV_HILOGD(SERVICE, "enter");
    delete algorithm;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
