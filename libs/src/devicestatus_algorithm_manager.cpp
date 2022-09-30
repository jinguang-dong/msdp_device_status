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

#include "devicestatus_algorithm_manager.h"

#include <cerrno>
#include <linux/netlink.h>
#include <string>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "devicestatus_common.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
AlgoMgr* g_rdb;
}

bool AlgoMgr::Init()
{
    DEV_HILOGD(SERVICE, "enter");
    if (!sensorEventCb_) {
        sensorEventCb_ = SensorDataCallback::GetInstance();
        sensorEventCb_->Init();
    }
    DEV_HILOGD(SERVICE, "exit");
    return true;
}

bool AlgoMgr::End()
{
    DEV_HILOGD(SERVICE, "enter");
    if (sensorEventCb_) {
        sensorEventCb_->Unregister();
    }
    DEV_HILOGD(SERVICE, "exit");
    return true;
}

ErrCode AlgoMgr::RegisterCallback(std::shared_ptr<AlgoCallback>& callback)
{
    DEV_HILOGD(SERVICE, "enter");

    if (type_[DataUtils::Type::TYPE_STILL] == DataUtils::TypeValue::VALID) {
        if (!still_) {
            still_ = std::make_shared<AbsoluteStill>(sensorEventCb_);
            still_->RegisterCallback(callback);
            still_->Init();
        }
    };

    if (type_[DataUtils::Type::TYPE_HORIZONTAL_POSITION] == DataUtils::TypeValue::VALID) {
        if (!horizontalPosition_) {
            horizontalPosition_ = std::make_shared<Horizontal>(sensorEventCb_);
            horizontalPosition_->RegisterCallback(callback);
            horizontalPosition_->Init();
        }
    };

    if (type_[DataUtils::Type::TYPE_VERTICAL_POSITION] == DataUtils::TypeValue::VALID) {
        if (!verticalPosition_) {
            verticalPosition_ = std::make_shared<Vertical>(sensorEventCb_);
            verticalPosition_->RegisterCallback(callback);
            verticalPosition_->Init();
        }
    };

    return ERR_OK;
}

ErrCode AlgoMgr::UnregisterCallback()
{
    DEV_HILOGI(SERVICE, "enter");

    if (type_[DataUtils::Type::TYPE_STILL] == DataUtils::TypeValue::VALID) {
        if (still_ != nullptr) {
            still_.reset();
            // still_ = nullptr;
            DEV_HILOGI(SERVICE, "still_ unregistered");
        }
    }

    if (type_[DataUtils::Type::TYPE_HORIZONTAL_POSITION] == DataUtils::TypeValue::VALID) {
        if (horizontalPosition_ != nullptr) {
            horizontalPosition_.reset();
            // horizontalPosition_ = nullptr;
            DEV_HILOGI(SERVICE, "horizontalPosition_ unregistered");
        }
    }

    if (type_[DataUtils::Type::TYPE_VERTICAL_POSITION] == DataUtils::TypeValue::VALID) {
        if (verticalPosition_ != nullptr) {
            verticalPosition_.reset();
            // verticalPosition_ = nullptr;
            DEV_HILOGI(SERVICE, "verticalPosition_ unregistered");
        }
    }

    return ERR_OK;
}

ErrCode AlgoMgr::DisableCount(const DataUtils::Type& type)
{
    return ERR_OK;
}

ErrCode AlgoMgr::Enable(const DataUtils::Type& type)
{
    DEV_HILOGD(SERVICE, "enter");
    type_[static_cast<int32_t>(type)] = static_cast<int32_t>(DataUtils::TypeValue::VALID);
    Init();
    DEV_HILOGD(SERVICE, "exit");
    return ERR_OK;
}

ErrCode AlgoMgr::Disable(const DataUtils::Type& type)
{
    DEV_HILOGI(SERVICE, "enter");
    type_[static_cast<int32_t>(type)] = static_cast<int32_t>(DataUtils::TypeValue::INVALID);
    for (auto n : type_) {
        if (type_[n] == static_cast<int32_t>(DataUtils::TypeValue::VALID)) {
            return ERR_OK;
        }
    }
    End();
    DEV_HILOGI(SERVICE, "exit");
    return ERR_OK;
}

extern "C" IAlgoMgr *Create(void)
{
    DEV_HILOGD(SERVICE, "enter");
    g_rdb = new AlgoMgr();
    return g_rdb;
}

extern "C" void Destroy(const IAlgoMgr* algorithm)
{
    DEV_HILOGD(SERVICE, "enter");
    delete algorithm;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
