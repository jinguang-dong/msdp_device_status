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

#ifndef DEVICESTATUS_MSDP_CLIENT_IMPL_H
#define DEVICESTATUS_MSDP_CLIENT_IMPL_H

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

#include "devicestatus_data_utils.h"
#include "devicestatus_delayed_sp_singleton.h"
#include "devicestatus_dumper.h"
#include "devicestatus_msdp_interface.h"
#include "devicestatus_sensor_interface.h"
#include "devicestatus_algorithm_manager_interface.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class DevicestatusMsdpClientImpl :
    public IMsdp::MsdpAlgoCallback,
    public ISensor::SensorHdiCallback,
    public IAlgoMgr::AlgoCallback {
public:
    using CallbackManager = std::function<int32_t(const DataUtils::Data&)>;

    ErrCode InitMsdpImpl(const DataUtils::Type& type);
    ErrCode DisableMsdpImpl(const DataUtils::Type& type);
    ErrCode RegisterImpl(const CallbackManager& callback);
    ErrCode UnregisterImpl();
    int32_t MsdpCallback(const DataUtils::Data& data);
    ErrCode RegisterMsdp();
    ErrCode UnregisterMsdp(void);
    ErrCode RegisterSensor();
    ErrCode UnregisterSensor(void);
    ErrCode RegisterDevAlgorithm();
    ErrCode UnregisterDevAlgorithm(void);
    DataUtils::Data SaveObserverData(const DataUtils::Data& data);
    std::map<DataUtils::Type, DataUtils::Value> GetObserverData() const;
    void GetDevicestatusTimestamp();
    void GetLongtitude();
    void GetLatitude();
    int32_t LoadAlgorithmLibrary(bool bCreate);
    int32_t UnloadAlgorithmLibrary(bool bCreate);
    int32_t LoadSensorHdiLibrary(bool bCreate);
    int32_t UnloadSensorHdiLibrary(bool bCreate);
    int32_t LoadDevAlgorithmLibrary(bool bCreate);
    int32_t UnloadDevAlgorithmLibrary(bool bCreate);
private:
    std::shared_ptr<AlgoCallback> callback_ {nullptr};
    ErrCode ImplCallback(const DataUtils::Data& data);
    ISensor* GetSensorHdiInst();
    IMsdp* GetAlgorithmInst();
    IAlgoMgr* GetDevAlgorithmInst();
    MsdpAlgoHandle mAlgorithm_;
    SensorHdiHandle sensorHdi_;
    AlgoHandle devAlgorithm_;
    std::mutex mMutex_;
    bool notifyManagerFlag_ = false;
    void OnResult(const DataUtils::Data& data) override;
    void OnSensorHdiResult(const DataUtils::Data& data) override;
    void OnAlogrithmResult(const DataUtils::Data& data) override;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS

#endif // DEVICESTATUS_MSDP_CLIENT_IMPL_H
