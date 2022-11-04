/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef DEVICESTATUS_CLIENT_TEST_H
#define DEVICESTATUS_CLIENT_TEST_H

#include <gtest/gtest.h>

#include "devicestatus_callback_stub.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class DeviceStatusClientTest : public testing::Test {
public:

    class DeviceStatusClientTestCallback : public DeviceStatusCallbackStub {
    public:
        DeviceStatusClientTestCallback() {};
        virtual ~DeviceStatusClientTestCallback() {};
        virtual void OnDeviceStatusChanged(const Data& devicestatusData) override;
    };
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_CLIENT_TEST_H
