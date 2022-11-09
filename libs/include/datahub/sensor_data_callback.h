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

#ifndef SENSOR_DATA_CALLBACK_H
#define SENSOR_DATA_CALLBACK_H

#include <atomic>
#include <list>
#include <map>
#include <mutex>
#include <thread>

#include "sensor_agent.h"
#include "sensor_agent_type.h"

#include "devicestatus_data_define.h"

constexpr bool PS_VALUE = true;
constexpr float ALS_VALUE = 5.0;
constexpr int32_t RATE_MILLISEC  = 100000000;

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class SensorDataCallback {
public:
    static std::shared_ptr<SensorDataCallback> GetInstance()
    {
        if (instance_ == nullptr) {
            instance_ = std::make_shared<SensorDataCallback>(SensorDataCallback());
        }
        return instance_;
    };
    static void ReleaseInstance()
    {
        instance_ = nullptr;
    };

    ~SensorDataCallback();
    bool RegisterCallbackSensor(int32_t sensorTypeId);
    bool UnregisterCallbackSensor(int32_t sensorTypeId);
    bool Init();
    bool Unregister();
    bool SubscribeSensorEvent(int32_t sensorTypeId, SensorCallback callback);
    bool UnSubscribeSensorEvent(int32_t sensorTypeId, SensorCallback callback);
    bool PushData(int32_t sensorTypeId, uint8_t* data);
    bool PopData(int32_t sensorTypeId, AccelData& data);
    bool AlgorithmLoop();
    bool HandleSensorEvent();
    bool NotifyCallback(int32_t sensorTypeId, AccelData* data);
private:
    static std::shared_ptr<SensorDataCallback> instance_;
    SensorDataCallback();
    SensorUser user_;
    std::list<AccelData> accelDataList_;

    std::unique_ptr<std::thread> algorithmThread_ { nullptr };
    sem_t sem_;
    std::mutex callbackMutex_;
    std::mutex dataMutex_;
    std::atomic<bool> alive_ {false};

    std::map<int32_t, SensorCallback> algoMap_;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // SENSOR_DATA_CALLBACK_H