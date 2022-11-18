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
    DEV_HILOGD(SERVICE, "Enter");
    int32_t sensorType = GetSensorTypeId(type);
    if (sensorType == ERR_NG) {
        DEV_HILOGE(SERVICE, "Failed to get sensorType");
        return false;
    }
    if (!CheckSensorTypeId(sensorType)) {
        DEV_HILOGE(SERVICE, "Sensor type mismatch");
        return false;
    }
    if (sensorEventCb_ == nullptr) {
        sensorEventCb_ = SensorDataCallback::GetInstance();
        sensorEventCb_->Init();
    }

    if (!sensorEventCb_->RegisterCallbackSensor(sensorType)) {
        DEV_HILOGE(SERVICE, "Failed to register callback sensor");
        return false;
    }

    return true;
}

ErrCode AlgoMgr::RegisterCallback(const std::shared_ptr<MsdpAlgoCallback>& callback)
{
    DEV_HILOGD(SERVICE, "Enter");
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
    DEV_HILOGD(SERVICE, "Enter");
    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

bool AlgoMgr::CheckSensorTypeId(int32_t sensorTypeId)
{
    int32_t count = -1;
    SensorInfo *sensorInfo = nullptr;
    int32_t ret = GetAllSensors(&sensorInfo, &count);
    if (ret != 0) {
        DEV_HILOGE(SERVICE, "Get all sensors failed");
        return false;
    }
    SensorInfo *ps = sensorInfo;
    SensorInfo *pt = sensorInfo + count;
    for (; ps < pt; ++ps) {
        if (sensorInfo -> sensorTypeId == sensorTypeId) {
            return true;
        }
    }
    DEV_HILOGE(SERVICE, "Get sensor failed");
    return false;
}

int32_t AlgoMgr::GetSensorTypeId(Type type)
{
    switch (type) {
        case Type::TYPE_STILL: {
            return SensorTypeId::SENSOR_TYPE_ID_ACCELEROMETER;
        }
        case Type::TYPE_HORIZONTAL_POSITION: {
            return SensorTypeId::SENSOR_TYPE_ID_ACCELEROMETER;
        }
        case Type::TYPE_VERTICAL_POSITION: {
            return SensorTypeId::SENSOR_TYPE_ID_ACCELEROMETER;
        }
        default: {
            break;
        }
    }
    return ERR_NG;
}

ErrCode AlgoMgr::Enable(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    DEV_HILOGI(SERVICE, "Enable type %{public}d", type);
    if (!SensorStart(type)) {
        DEV_HILOGE(SERVICE, "sensor init failed");
        return ERR_NG;
    }
    switch (type) {
        case Type::TYPE_STILL: {
            if (still_ == nullptr) {
                still_ = std::make_shared<AlgoAbsoluteStill>(sensorEventCb_);
                still_->Init(type);
                callAlgoNum_[type] = 0;
            }
            callAlgoNum_[type]++;
            break;
        }
        case Type::TYPE_HORIZONTAL_POSITION: {
            if (horizontalPosition_ == nullptr) {
                horizontalPosition_ = std::make_shared<AlgoHorizontal>(sensorEventCb_);
                horizontalPosition_->Init(type);
                callAlgoNum_[type] = 0;
            }
            callAlgoNum_[type]++;
            break;
        }
        case Type::TYPE_VERTICAL_POSITION: {
            if (verticalPosition_ == nullptr) {
                verticalPosition_ = std::make_shared<AlgoVertical>(sensorEventCb_);
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
    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

ErrCode AlgoMgr::Disable(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    DEV_HILOGI(SERVICE, "Disable type:%{public}d", type);
    callAlgoNum_[type]--;
    DEV_HILOGI(SERVICE, "callAlgoNum_:%{public}d", callAlgoNum_[type]);
    if (callAlgoNum_[type] != 0) {
        DEV_HILOGE(SERVICE, "callAlgoNum_[type] != 0");
        return ERR_NG;
    }
    switch (type) {
        case Type::TYPE_STILL: {
            if (still_ != nullptr) {
                still_->Unsubscribe(type);
                still_ = nullptr;
            } else {
                DEV_HILOGE(SERVICE, "NULLPTR");
            }
            break;
        }
        case Type::TYPE_HORIZONTAL_POSITION: {
            if (horizontalPosition_ != nullptr) {
                horizontalPosition_->Unsubscribe(type);
                horizontalPosition_ = nullptr;
            }
            break;
        }
        case Type::TYPE_VERTICAL_POSITION: {
            if (verticalPosition_ != nullptr) {
                verticalPosition_->Unsubscribe(type);
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
    UnregisterSensor(type);
    return ERR_OK;
}

ErrCode AlgoMgr::DisableCount(const Type& type)
{
    return ERR_OK;
}

ErrCode AlgoMgr::UnregisterSensor(Type type)
{
    DEV_HILOGD(SERVICE, "Enter");
    int32_t sensorType = GetSensorTypeId(type);
    if (sensorType == ERR_NG) {
        DEV_HILOGE(SERVICE, "Failed to get sensorType");
        return false;
    }
    if (!sensorEventCb_->UnregisterCallbackSensor(sensorType)) {
        DEV_HILOGE(SERVICE, "Failed to unregister callback sensor");
        return ERR_NG;
    }
    sensorEventCb_ = nullptr;
    DEV_HILOGD(SERVICE, "Exit");
    return ERR_OK;
}

extern "C" IMsdp *Create(void)
{
    DEV_HILOGD(SERVICE, "Enter");
    g_algo = new AlgoMgr();
    return g_algo;
}

extern "C" void Destroy(const IMsdp* algorithm)
{
    DEV_HILOGD(SERVICE, "Enter");
    delete algorithm;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
