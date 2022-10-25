/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "devicestatusevent_fuzzer.h"
#include "devicestatus_event.h"

#include "devicestatus_data_utils.h"

#include <stddef.h>
#include <stdint.h>

namespace OHOS {
namespace Msdp {
namespace {
const uint8_t *g_devicestatusFuzzData = nullptr;
size_t g_devicestatusFuzzSize = 0;
size_t g_devicestatusFuzzPos = 0;
const int WAIT_TIME = 1000;
std::shared_ptr<DevicestatusEvent> devicestatusEvent_;
static constexpr uint8_t ARG_1 = 1;
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_devicestatusFuzzData == nullptr || objectSize > g_devicestatusFuzzSize - g_devicestatusFuzzPos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_devicestatusFuzzData + g_devicestatusFuzzPos, objectSize);
    if (ret != EOK) {
        return object;
    }
    g_devicestatusFuzzPos += objectSize;
    return object;
}

inline void setGlobalFuzzData(const uint8_t* data, size_t size)
{
    g_devicestatusFuzzData = data;
    g_devicestatusFuzzSize = size;
    g_devicestatusFuzzPos = 0;
}

inline void initDevicestatusEvent(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    setGlobalFuzzData(data, size);
    napi_env env(GetData<napi_env>());
    napi_value value(GetData<napi_value>());
    devicestatusEvent_ = std::make_shared<DevicestatusEvent>(env, value);
}

inline void testEvent(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    setGlobalFuzzData(data, size);
    napi_value value(GetData<napi_value>());
    bool result = false;
    for (int32_t i = 0; i < 2; i++) {
        bool isOnce = i%2==0;
        devicestatusEvent_->OnEvent(DevicestatusDataUtils::TYPE_HIGH_STILL, ARG_1,
            DevicestatusDataUtils::VALUE_ENTER, isOnce);
        std::cout << "OnEvent: " << result << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));

        result = devicestatusEvent_->Off(DevicestatusDataUtils::TYPE_HIGH_STILL, isOnce);
        std::cout << "Off: " << result << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));

        result = devicestatusEvent_->On(DevicestatusDataUtils::TYPE_HIGH_STILL, value, isOnce);
        std::cout << "On: " << result << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));

        result = devicestatusEvent_->On(DevicestatusDataUtils::TYPE_HIGH_STILL, value, isOnce);
        std::cout << "Second On: " << result << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));

        devicestatusEvent_->OnEvent(DevicestatusDataUtils::TYPE_HIGH_STILL, ARG_1,
            DevicestatusDataUtils::VALUE_ENTER, isOnce);
        std::cout << "OnEvent: " << result << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));

        result = devicestatusEvent_->Off(DevicestatusDataUtils::TYPE_HIGH_STILL, isOnce);
        std::cout << "Off: " << result << std::endl;
    }
}

} // namespace

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    initDevicestatusEvent(data, size);
    testEvent(data, size);
    return true;
}
} // Msdp
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Msdp::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

