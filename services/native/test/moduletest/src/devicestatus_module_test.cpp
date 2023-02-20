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

#include "devicestatus_module_test.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <gtest/gtest.h>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <string_ex.h>

#include "devicestatus_common.h"
#include "devicestatus_client.h"

using namespace testing::ext;
using namespace OHOS::Msdp;
using namespace OHOS;
using namespace std;

namespace {
static constexpr int32_t WAIT_TIME = 2000;
sptr<IdevicestatusCallback> cb = nullptr;
}

void DevicestatusModuleTest::SetUpTestCase()
{
    cb = new (std::nothrow) DevicestatusModuleTestCallback();
}

void DevicestatusModuleTest::TearDownTestCase()
{
}

void DevicestatusModuleTest::SetUp()
{
}

void DevicestatusModuleTest::TearDown()
{
}

void DevicestatusModuleTest::DevicestatusModuleTestCallback::OnDevicestatusChanged(const \
    DevicestatusDataUtils::DevicestatusData& devicestatusData)
{
    GTEST_LOG_(INFO) << "DevicestatusModuleTestCallback type: " << devicestatusData.type;
    GTEST_LOG_(INFO) << "DevicestatusModuleTestCallback value: " << devicestatusData.value;
    EXPECT_EQ(true, devicestatusData.value >= DevicestatusDataUtils::DevicestatusValue::VALUE_INVALID)
        << "DevicestatusModuleTestCallback failed";
}

/**
 * @tc.name: DevicestatusCallbackTest
 * @tc.desc: test devicestatus callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DevicestatusModuleTest, DevicestatusCallbackTest001, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDevicestatusDataTest002 Enter");
    DevicestatusDataUtils::DevicestatusType type = DevicestatusDataUtils::DevicestatusType::TYPE_RELATIVE_STILL;
    auto& devicestatusClient = DevicestatusClient::GetInstance();
    GTEST_LOG_(INFO) << "Start register";
    devicestatusClient.SubscribeCallback(type, cb);
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));
    GTEST_LOG_(INFO) << "Cancell register";
}

/**
 * @tc.name: GetDevicestatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DevicestatusModuleTest, GetDevicestatusDataTest002, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDevicestatusDataTest002 Enter");
    DevicestatusDataUtils::DevicestatusType type = DevicestatusDataUtils::DevicestatusType::TYPE_RELATIVE_STILL;
    auto& devicestatusClient = DevicestatusClient::GetInstance();
    DevicestatusDataUtils::DevicestatusData data = devicestatusClient.GetDevicestatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == DevicestatusDataUtils::DevicestatusType::TYPE_RELATIVE_STILL &&
        data.value >= DevicestatusDataUtils::DevicestatusValue::VALUE_INVALID) << "GetDevicestatusData failed";
}

/**
 * @tc.name: GetDevicestatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DevicestatusModuleTest, GetDevicestatusDataTest003, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDevicestatusDataTest003 Enter");
    DevicestatusDataUtils::DevicestatusType type = DevicestatusDataUtils::DevicestatusType::TYPE_RELATIVE_STILL;
    auto& devicestatusClient = DevicestatusClient::GetInstance();
    devicestatusClient.UnSubscribeCallback(type, cb);
}