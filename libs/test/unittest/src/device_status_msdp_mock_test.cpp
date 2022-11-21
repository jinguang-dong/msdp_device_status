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

#include <cstdio>
#include <string>
#include <gtest/gtest.h>

#include "devicestatus_msdp_client_impl.h"
#include "devicestatus_msdp_interface.h"
#include "devicestatus_msdp_mock.h"

using namespace testing::ext;
using namespace OHOS::Msdp;
using namespace OHOS::Msdp::DeviceStatus;
using namespace OHOS;

namespace {
    std::shared_ptr <DeviceStatusMsdpMock> g_testMock;
}

class DeviceStatusMsdpMoclTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DeviceStatusMsdpMoclTest::SetUpTestCase()
{
    g_testMock = std::make_shared<DeviceStatusMsdpMock>();
}

void DeviceStatusMsdpMoclTest::TearDownTestCase()
{
    g_testMock = nullptr;
}

void DeviceStatusMsdpMoclTest::SetUp() {}

void DeviceStatusMsdpMoclTest::TearDown() {}

/**
 * @tc.name: DeviceStatusMsdpMockTest
 * @tc.desc: test devicestatus Mock in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusMsdpMoclTest, DeviceStatusMsdpMoclTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest001 start";
    bool ret = g_testMock->Init();
    ASSERT_TRUE(ret);
    std::shared_ptr <DeviceStatusMsdpClientImpl> callback_ = std::make_shared<DeviceStatusMsdpClientImpl>();
    int32_t cb = g_testMock->RegisterCallback(callback_);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->UnregisterCallback();
    EXPECT_TRUE(cb == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest001 end";
}

/**
 * @tc.name: DeviceStatusMsdpMockTest
 * @tc.desc: test devicestatus Mock in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusMsdpMoclTest, DeviceStatusMsdpMoclTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest002 start";
    bool ret = g_testMock->Init();
    ASSERT_TRUE(ret);
    std::shared_ptr <DeviceStatusMsdpClientImpl> callback_ = std::make_shared<DeviceStatusMsdpClientImpl>();
    int32_t cb = g_testMock->RegisterCallback(callback_);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->Enable(Type::TYPE_INVALID);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->Disable(Type::TYPE_INVALID);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->UnregisterCallback();
    EXPECT_TRUE(cb == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest002 end";
}

/**
 * @tc.name: DeviceStatusMsdpMockTest
 * @tc.desc: test devicestatus Mock in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusMsdpMoclTest, DeviceStatusMsdpMoclTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest003 start";
    bool ret = g_testMock->Init();
    ASSERT_TRUE(ret);
    std::shared_ptr <DeviceStatusMsdpClientImpl> callback_ = std::make_shared<DeviceStatusMsdpClientImpl>();
    int32_t cb = g_testMock->RegisterCallback(callback_);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->Enable(Type::TYPE_STILL);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->Disable(Type::TYPE_STILL);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->UnregisterCallback();
    EXPECT_TRUE(cb == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest003 end";
}

/**
 * @tc.name: DeviceStatusMsdpMockTest
 * @tc.desc: test devicestatus Mock in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusMsdpMoclTest, DeviceStatusMsdpMoclTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest004 start";
    bool ret = g_testMock->Init();
    ASSERT_TRUE(ret);
    std::shared_ptr <DeviceStatusMsdpClientImpl> callback_ = std::make_shared<DeviceStatusMsdpClientImpl>();
    int32_t cb = g_testMock->RegisterCallback(callback_);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->Enable(Type::TYPE_HORIZONTAL_POSITION);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->Disable(Type::TYPE_HORIZONTAL_POSITION);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->UnregisterCallback();
    EXPECT_TRUE(cb == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest004 end";
}

/**
 * @tc.name: DeviceStatusMsdpMockTest
 * @tc.desc: test devicestatus Mock in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusMsdpMoclTest, DeviceStatusMsdpMoclTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest005 start";
    bool ret = g_testMock->Init();
    ASSERT_TRUE(ret);
    std::shared_ptr <DeviceStatusMsdpClientImpl> callback_ = std::make_shared<DeviceStatusMsdpClientImpl>();
    int32_t cb = g_testMock->RegisterCallback(callback_);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->Enable(Type::TYPE_VERTICAL_POSITION);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->Disable(Type::TYPE_VERTICAL_POSITION);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->UnregisterCallback();
    EXPECT_TRUE(cb == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest005 end";
}

/**
 * @tc.name: DeviceStatusMsdpMockTest
 * @tc.desc: test devicestatus Mock in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusMsdpMoclTest, DeviceStatusMsdpMoclTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest006 start";
    bool ret = g_testMock->Init();
    ASSERT_TRUE(ret);
    std::shared_ptr <DeviceStatusMsdpClientImpl> callback_ = std::make_shared<DeviceStatusMsdpClientImpl>();
    int32_t cb = g_testMock->RegisterCallback(callback_);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->Enable(Type::TYPE_LID_OPEN);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->Disable(Type::TYPE_LID_OPEN);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->UnregisterCallback();
    EXPECT_TRUE(cb == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest006 end";
}

/**
 * @tc.name: DeviceStatusMsdpMockTest
 * @tc.desc: test devicestatus Mock in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusMsdpMoclTest, DeviceStatusMsdpMoclTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest007 start";
    bool ret = g_testMock->Init();
    ASSERT_TRUE(ret);
    std::shared_ptr <DeviceStatusMsdpClientImpl> callback_ = std::make_shared<DeviceStatusMsdpClientImpl>();
    int32_t cb = g_testMock->RegisterCallback(callback_);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->Enable(Type::TYPE_MAX);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->Disable(Type::TYPE_MAX);
    EXPECT_TRUE(cb == ERR_OK);
    cb = g_testMock->UnregisterCallback();
    EXPECT_TRUE(cb == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest007 end";
}

/**
 * @tc.name: DeviceStatusMsdpMockTest
 * @tc.desc: test devicestatus Mock in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusMsdpMoclTest, DeviceStatusMsdpMoclTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest007 start";
    g_testMock->InitTimer();
    g_testMock->StartThread();
    std::make_unique<std::thread>(&DeviceStatusMsdpMock::LoopingThreadEntry, g_testMock)->detach();
    g_testMock->CloseTimer();
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest007 end";
}

/**
 * @tc.name: DeviceStatusMsdpMockTest
 * @tc.desc: test devicestatus Mock in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusMsdpMoclTest, DeviceStatusMsdpMoclTest009, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest009 start";
    g_testMock->InitTimer();
    g_testMock->StartThread();
    std::make_unique<std::thread>(&DeviceStatusMsdpMock::LoopingThreadEntry, g_testMock)->detach();
    constexpr int32_t TIMER_INTERVAL = 3;
    g_testMock->SetTimerInterval(TIMER_INTERVAL);
    g_testMock->CloseTimer();
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest009 end";
}

/**
 * @tc.name: DeviceStatusMsdpMockTest
 * @tc.desc: test devicestatus Mock in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusMsdpMoclTest, DeviceStatusMsdpMoclTest010, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest010 start";
    g_testMock->InitTimer();
    g_testMock->StartThread();
    std::make_unique<std::thread>(&DeviceStatusMsdpMock::LoopingThreadEntry, g_testMock)->detach();
    constexpr int32_t TIMER_INTERVAL = -1;
    g_testMock->SetTimerInterval(TIMER_INTERVAL);
    g_testMock->CloseTimer();
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest010 end";
}

/**
 * @tc.name: DeviceStatusMsdpMockTest
 * @tc.desc: test devicestatus Mock in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusMsdpMoclTest, DeviceStatusMsdpMoclTest011, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest011 start";
    g_testMock->InitTimer();
    g_testMock->StartThread();
    std::make_unique<std::thread>(&DeviceStatusMsdpMock::LoopingThreadEntry, g_testMock)->detach();
    constexpr int32_t TIMER_INTERVAL = 0;
    g_testMock->SetTimerInterval(TIMER_INTERVAL);
    g_testMock->CloseTimer();
    GTEST_LOG_(INFO) << "DeviceStatusMsdpMoclTest011 end";
}
