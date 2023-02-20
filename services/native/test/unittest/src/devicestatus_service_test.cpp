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

#include "devicestatus_service_test.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <gtest/gtest.h>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <string_ex.h>

#include "devicestatus_common.h"
#include "devicestatus_client.h"
#include "devicestatus_dumper.h"
#include "devicestatus_service.h"

using namespace testing::ext;
using namespace OHOS::Msdp;
using namespace OHOS;
using namespace std;

namespace {
static constexpr int32_t WAIT_TIME = 2000;
sptr<IdevicestatusCallback> cb = nullptr;
}

void DevicestatusServiceTest::SetUpTestCase()
{
    cb = new (std::nothrow) DevicestatusServiceTestCallback();
}

void DevicestatusServiceTest::TearDownTestCase()
{
}

void DevicestatusServiceTest::SetUp()
{
}

void DevicestatusServiceTest::TearDown()
{
}

void DevicestatusServiceTest::DevicestatusServiceTestCallback::OnDevicestatusChanged(const \
    DevicestatusDataUtils::DevicestatusData& devicestatusData)
{
    GTEST_LOG_(INFO) << "DevicestatusServiceTestCallback type: " << devicestatusData.type;
    GTEST_LOG_(INFO) << "DevicestatusServiceTestCallback value: " << devicestatusData.value;
    EXPECT_TRUE(devicestatusData.value >= DevicestatusDataUtils::DevicestatusValue::VALUE_INVALID)
        << "DevicestatusServiceTestCallback failed";
}

/**
 * @tc.name: DevicestatusCallbackTest
 * @tc.desc: test devicestatus callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DevicestatusServiceTest, DevicestatusCallbackTest001, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "DevicestatusCallbackTest001 Enter");
    DevicestatusDataUtils::DevicestatusType type = DevicestatusDataUtils::DevicestatusType::TYPE_RELATIVE_STILL;
    auto& devicestatusClient = DevicestatusClient::GetInstance();
    GTEST_LOG_(INFO) << "Start register";
    devicestatusClient.SubscribeCallback(type, cb);
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));
}

/**
 * @tc.name: GetDevicestatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DevicestatusServiceTest, GetDevicestatusDataTest002, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDevicestatusDataTest002 Enter");
    DevicestatusDataUtils::DevicestatusType type = DevicestatusDataUtils::DevicestatusType::TYPE_RELATIVE_STILL;
    auto& devicestatusClient = DevicestatusClient::GetInstance();
    DevicestatusDataUtils::DevicestatusData data = devicestatusClient.GetDevicestatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_TRUE(data.type == DevicestatusDataUtils::DevicestatusType::TYPE_RELATIVE_STILL
        && data.value >= DevicestatusDataUtils::DevicestatusValue::VALUE_INVALID) << "GetDevicestatusData failed";
    DEV_HILOGI(SERVICE, "GetDevicestatusDataTest002 end");
}

/**
 * @tc.name: GetDevicestatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DevicestatusServiceTest, GetDevicestatusDataTest003, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDevicestatusDataTest003 Enter");
    DevicestatusDataUtils::DevicestatusType type = DevicestatusDataUtils::DevicestatusType::TYPE_RELATIVE_STILL;
    auto& devicestatusClient = DevicestatusClient::GetInstance();
    devicestatusClient.UnSubscribeCallback(type, cb);
}

/**
 * @tc.name: DevicestatusCallbackTest
 * @tc.desc: test devicestatus callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DevicestatusServiceTest, DevicestatusCallbackTest004, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "DevicestatusCallbackTest004 Enter");
    DevicestatusDataUtils::DevicestatusType type = DevicestatusDataUtils::DevicestatusType::TYPE_STILL;
    auto& devicestatusClient = DevicestatusClient::GetInstance();
    GTEST_LOG_(INFO) << "Start register";
    devicestatusClient.SubscribeCallback(type, cb);
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));
}

/**
 * @tc.name: GetDevicestatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DevicestatusServiceTest, GetDevicestatusDataTest005, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDevicestatusDataTest005 Enter");
    DevicestatusDataUtils::DevicestatusType type = DevicestatusDataUtils::DevicestatusType::TYPE_STILL;
    auto& devicestatusClient = DevicestatusClient::GetInstance();
    DevicestatusDataUtils::DevicestatusData data = devicestatusClient.GetDevicestatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_TRUE(data.type == DevicestatusDataUtils::DevicestatusType::TYPE_STILL
        && data.value >= DevicestatusDataUtils::DevicestatusValue::VALUE_INVALID) << "GetDevicestatusData failed";
    DEV_HILOGI(SERVICE, "GetDevicestatusDataTest002 end");
}

/**
 * @tc.name: GetDevicestatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DevicestatusServiceTest, GetDevicestatusDataTest006, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDevicestatusDataTest006 Enter");
    DevicestatusDataUtils::DevicestatusType type = DevicestatusDataUtils::DevicestatusType::TYPE_STILL;
    auto& devicestatusClient = DevicestatusClient::GetInstance();
    devicestatusClient.UnSubscribeCallback(type, cb);
}

/**
 * @tc.name: DevicestatusCallbackTest
 * @tc.desc: test devicestatus callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DevicestatusServiceTest, DevicestatusCallbackTest007, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "DevicestatusCallbackTest007 Enter");
    DevicestatusDataUtils::DevicestatusType type = DevicestatusDataUtils::DevicestatusType::TYPE_CAR_BLUETOOTH;
    auto& devicestatusClient = DevicestatusClient::GetInstance();
    GTEST_LOG_(INFO) << "Start register";
    devicestatusClient.SubscribeCallback(type, cb);
    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));
}

/**
 * @tc.name: GetDevicestatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DevicestatusServiceTest, GetDevicestatusDataTest008, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDevicestatusDataTest008 Enter");
    DevicestatusDataUtils::DevicestatusType type = DevicestatusDataUtils::DevicestatusType::TYPE_CAR_BLUETOOTH;
    auto& devicestatusClient = DevicestatusClient::GetInstance();
    DevicestatusDataUtils::DevicestatusData data = devicestatusClient.GetDevicestatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_TRUE(data.type == DevicestatusDataUtils::DevicestatusType::TYPE_CAR_BLUETOOTH
        && data.value >= DevicestatusDataUtils::DevicestatusValue::VALUE_INVALID) << "GetDevicestatusData failed";
    DEV_HILOGI(SERVICE, "GetDevicestatusDataTest002 end");
}

/**
 * @tc.name: GetDevicestatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DevicestatusServiceTest, GetDevicestatusDataTest009, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDevicestatusDataTest003 Enter");
    DevicestatusDataUtils::DevicestatusType type = DevicestatusDataUtils::DevicestatusType::TYPE_CAR_BLUETOOTH;
    auto& devicestatusClient = DevicestatusClient::GetInstance();
    devicestatusClient.UnSubscribeCallback(type, cb);
}

HWTEST_F (DevicestatusServiceTest, GetDevicestatusDataTest010, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GetDevicestatusDataTest007 Enter";
    auto devicestatusService = DelayedSpSingleton<DevicestatusService>::GetInstance();
    devicestatusService->OnDump();
    GTEST_LOG_(INFO) << "GetDevicestatusDataTest007 end";
}

HWTEST_F (DevicestatusServiceTest, GetDevicestatusDataTest011, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GetDevicestatusDataTest008 Enter";
    auto devicestatusService = DelayedSpSingleton<DevicestatusService>::GetInstance();
    std::vector<std::u16string> args;
    int32_t ret = devicestatusService->Dump(-1, args);
    EXPECT_TRUE(ret == RET_ERR);
    ret = devicestatusService->Dump(1, args);
    EXPECT_TRUE(ret == RET_ERR);
    args.emplace_back(ARGS_H);
    ret = devicestatusService->Dump(1, args);
    EXPECT_TRUE(ret == RET_OK);
    GTEST_LOG_(INFO) << "GetDevicestatusDataTest008 end";
}

HWTEST_F (DevicestatusServiceTest, GetDevicestatusDataTest012, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GetDevicestatusDataTest009 Enter";
    auto devicestatusCBDeathRecipient = new (std::nothrow) DevicestatusManager::DevicestatusCallbackDeathRecipient();
    devicestatusCBDeathRecipient->OnRemoteDied(nullptr);
    GTEST_LOG_(INFO) << "GetDevicestatusDataTest009 end";
}

HWTEST_F (DevicestatusServiceTest, GetDevicestatusDataTest013, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GetDevicestatusDataTest010 Enter";
    auto devicestatusService = DelayedSpSingleton<DevicestatusService>::GetInstance();
    auto devicestatusManager = std::make_shared<DevicestatusManager>(devicestatusService);
    Security::AccessToken::AccessTokenID tokenId = static_cast<Security::AccessToken::AccessTokenID>(1);
    std::string packageName;
    devicestatusManager->GetPackageName(tokenId, packageName);
    GTEST_LOG_(INFO) << "GetDevicestatusDataTest010 end";
}

HWTEST_F (DevicestatusServiceTest, GetDevicestatusDataTest014, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GetDevicestatusDataTest011 Enter";
    auto devicestatusService = DelayedSpSingleton<DevicestatusService>::GetInstance();
    devicestatusService->OnStop();
    devicestatusService->OnStart();
    devicestatusService->OnStart();
    devicestatusService->OnStop();
    GTEST_LOG_(INFO) << "GetDevicestatusDataTest011 end";
}

HWTEST_F (DevicestatusServiceTest, GetDevicestatusDataTest015, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GetDevicestatusDataTest012 Enter";
    auto devicestatusService = DelayedSpSingleton<DevicestatusService>::GetInstance();
    auto devicestatusManager = std::make_shared<DevicestatusManager>(devicestatusService);
    bool result = devicestatusManager->Init();
    EXPECT_TRUE(result);
    int32_t ret = devicestatusManager->LoadAlgorithm();
    EXPECT_TRUE(ret == RET_OK);
    ret = devicestatusManager->UnloadAlgorithm();
    EXPECT_TRUE(ret == RET_OK);
    DelayedSpSingleton<DevicestatusService>::DestroyInstance();
    GTEST_LOG_(INFO) << "GetDevicestatusDataTest012 end";
}