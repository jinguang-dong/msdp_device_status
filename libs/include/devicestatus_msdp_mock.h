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

#ifndef DEVICESTATUS_MSDP_MOCK_H
#define DEVICESTATUS_MSDP_MOCK_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <errors.h>
#include "result_set.h"
#include "values_bucket.h"

#include "devicestatus_data_utils.h"
#include "devicestatus_msdp_interface.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class DeviceStatusMsdpMock : public IMsdp {
public:
    DeviceStatusMsdpMock() = default;
    virtual ~DeviceStatusMsdpMock() = default;

    enum EventType {
        EVENT_UEVENT_FD,
        EVENT_TIMER_FD,
    };
    bool Init();
    void InitRdbStore();
    void SetTimerInterval(int32_t interval);
    void CloseTimer();
    void InitTimer();
    void TimerCallback();
    int32_t RegisterTimerCallback(const int32_t fd, const EventType et);
    void StartThread();
    void LoopingThreadEntry();
    void GetEpollEvents(epoll_event events[], int32_t nevents);
    ErrCode Enable(Type type) override;
    ErrCode Disable(Type type) override;
    ErrCode RegisterCallback(std::shared_ptr<MsdpAlgoCallback> callback) override;
    ErrCode UnregisterCallback() override;
    ErrCode NotifyMsdpImpl(const Data& data);
    int32_t TriggerData(const std::unique_ptr<NativeRdb::ResultSet> &resultSet);
    int32_t TriggerDatabaseObserver();
    Data SaveRdbData(const Data& data);
    std::shared_ptr<MsdpAlgoCallback> GetCallbacksImpl() const
    {
        std::unique_lock lock(mutex_);
        return callback_;
    }

private:
    using Callback = std::function<void(DeviceStatusMsdpMock*)>;
    std::shared_ptr<MsdpAlgoCallback> callback_;
    std::map<int32_t, Callback> callbacks_;
    int32_t devicestatusType_ = -1;
    int32_t devicestatusStatus_ = -1;
    bool notifyFlag_ = false;
    bool scFlag = true;
    int32_t timerInterval_ = -1;
    int32_t timerFd_ = -1;
    int32_t epFd_ = -1;
    std::map<Type, OnChangedValue> rdbDataMap_;
    std::mutex mutex_;
};

class InsertOpenCallback : public NativeRdb::RdbOpenCallback {
public:
    int32_t OnCreate(NativeRdb::RdbStore &rdbStore) override;
    int32_t OnUpgrade(NativeRdb::RdbStore &rdbStore, int32_t oldVersion, int32_t newVersion) override;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_MSDP_MOCK_H
