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

#ifndef MOTION_MSDP_CLIENT_IMPL_H
#define MOTION_MSDP_CLIENT_IMPL_H

#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <thread>
#include <map>
#include <errors.h>
#include "rdb_store.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"
#include "rdb_store_config.h"
#include "values_bucket.h"
#include "result_set.h"
#include "motion_data_utils.h"
#include "motion_delayed_sp_singleton.h"
#include "motion_msdp_interface.h"
#include "motion_sensor_interface.h"

namespace OHOS {
namespace Msdp {
class MotionMsdpClientImpl :
    public MotionMsdpInterface::MsdpAlgorithmCallback,
    public MotionSensorInterface::MotionSensorHdiCallback {
public:
    using CallbackManager = std::function<int32_t(MotionDataUtils::MotionData&)>;

    ErrCode EnableMsdpImpl(const MotionDataUtils::MotionType& type);
    ErrCode DisableMsdpImpl(const MotionDataUtils::MotionType& type);
    ErrCode DisableMsdpImplCount(const MotionDataUtils::MotionType& type);
    ErrCode RegisterImpl(CallbackManager& callback);
    ErrCode UnregisterImpl();
    int32_t MsdpCallback(MotionDataUtils::MotionData& data);
    ErrCode RegisterMsdp(const CallbackManager& callback);
    ErrCode UnregisterMsdp();
    ErrCode RegisterSensor(const CallbackManager& callback);
    ErrCode UnregisterSensor();
    MotionDataUtils::MotionData SaveObserverData(MotionDataUtils::MotionData& data);
    std::map<MotionDataUtils::MotionType, MotionDataUtils::MotionValue> GetObserverData() const;
    void GetMotionTimestamp();
    void GetLongtitude();
    void GetLatitude();
    int32_t LoadAlgorithmLibrary();
    int32_t UnloadAlgorithmLibrary();
    int32_t LoadSensorHdiLibrary();
    int32_t UnloadSensorHdiLibrary();
private:
    ErrCode ImplCallback(MotionDataUtils::MotionData& data);
    MotionMsdpInterface* msdpInterface_;
    MotionSensorInterface* sensorHdiInterface_;
    MotionSensorInterface* GetSensorHdiInst();
    MotionMsdpInterface* GetAlgorithmInst();
    MsdpAlgorithmHandle mAlgorithm_;
    SensorHdiHandle sensorHdi_;
    std::mutex mMutex_;
    bool notifyManagerFlag_ = false;
    void OnResult(MotionDataUtils::MotionData& data) override;
};
}
}
#endif // MOTION_MSDP_CLIENT_IMPL_H
