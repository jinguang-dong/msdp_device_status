/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "devicestatus_manager_test.h"

#include "devicestatus_common.h"
#include "devicestatus_manager.h"
#include "devicestatus_service.h"

using namespace testing::ext;
using namespace OHOS::Msdp::DeviceStatus;
using namespace OHOS;
using namespace std;

static std::shared_ptr<DeviceStatusManager> g_mgr;
auto g_service = DelayedSpSingleton<DeviceStatusService>::GetInstance();

Type DeviceStatusManagerTest::g_mgrTest = Type::TYPE_INVALID;

void DeviceStatusManagerTest::SetUpTestCase() {}

void DeviceStatusManagerTest::TearDownTestCase() {}

void DeviceStatusManagerTest::SetUp()
{
    g_mgr = std::make_shared<DeviceStatusManager>(g_service);
}

void DeviceStatusManagerTest::TearDown() {}

void DeviceStatusManagerTest::DeviceStatusManagerTestCallback::OnDeviceStatusChanged(const Data& devicestatusData)
{
    GTEST_LOG_(INFO) << "DeviceStatusManagerTestCallback type: " << devicestatusData.type;
    GTEST_LOG_(INFO) << "DeviceStatusManagerTestCallback value: " << devicestatusData.value;
    EXPECT_TRUE(devicestatusData.type == g_mgrTest && (devicestatusData.value >= OnChangedValue::VALUE_INVALID &&
        devicestatusData.value <= OnChangedValue::VALUE_EXIT)) << "DeviceStatusManagerTestCallback failed";
}

/**
 * @tc.name: DeviceStatusManagerTest
 * @tc.desc: test GetLatestDeviceStatusData
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusManagerTest, DeviceStatusManagerTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusManagerTest001 start";
    g_mgrTest = Type::TYPE_ABSOLUTE_STILL;
    Type type = g_mgrTest;
    Data data = g_mgr->GetLatestDeviceStatusData(type);
    EXPECT_TRUE(sizeof(data) >= 0);
    GTEST_LOG_(INFO) << "DeviceStatusManagerTest001 end";
}

/**
 * @tc.name: DeviceStatusManagerTest
 * @tc.desc: test Enable
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusManagerTest, DeviceStatusManagerTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusManagerTest002 start";
    EXPECT_TRUE(g_mgr->Enable(Type::TYPE_INVALID) == false);
    GTEST_LOG_(INFO) << "DeviceStatusManagerTest002 end";
}

/**
 * @tc.name: DeviceStatusManagerTest
 * @tc.desc: test Disable
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusManagerTest, DeviceStatusManagerTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusManagerTest003 start";
    EXPECT_TRUE(g_mgr->Disable(Type::TYPE_INVALID) == false);
    GTEST_LOG_(INFO) << "DeviceStatusManagerTest003 end";
}

/**
 * @tc.name: DeviceStatusManagerTest
 * @tc.desc: test InitDataCallback
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusManagerTest, DeviceStatusManagerTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusManagerTest004 start";
    EXPECT_TRUE(g_mgr->InitDataCallback() == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusManagerTest004 end";
}

/**
 * @tc.name: DeviceStatusManagerTest
 * @tc.desc: test NotifyDeviceStatusChange
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusManagerTest, DeviceStatusManagerTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusManagerTest005 start";
    Data devicestatusData;
    EXPECT_TRUE(g_mgr->NotifyDeviceStatusChange(devicestatusData) == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusManagerTest005 end";
}

/**
 * @tc.name: DeviceStatusManagerTest
 * @tc.desc: test Subscribe
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusManagerTest, DeviceStatusManagerTest006, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "DeviceStatusManagerTest006 Enter");
    g_mgrTest = Type::TYPE_RELATIVE_STILL;
    Type type = g_mgrTest;
    sptr<IRemoteDevStaCallback> cb = new (std::nothrow) DeviceStatusManagerTestCallback();
    EXPECT_FALSE(cb == nullptr);
    GTEST_LOG_(INFO) << "Start register";
    g_mgr->Subscribe(type, ActivityEvent::ENTER_EXIT, ReportLatencyNs::LONG, cb);
    DEV_HILOGI(SERVICE, "DeviceStatusManagerTest006 end");
}

/**
 * @tc.name: DeviceStatusManagerTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusManagerTest, DeviceStatusManagerTest007, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "DeviceStatusManagerTest007 Enter");
    g_mgrTest = Type::TYPE_RELATIVE_STILL;
    Type type = g_mgrTest;
    sptr<IRemoteDevStaCallback> cb = new (std::nothrow) DeviceStatusManagerTestCallback();
    EXPECT_FALSE(cb == nullptr);
    GTEST_LOG_(INFO) << "Cancel register";
    g_mgr->Unsubscribe(type, ActivityEvent::ENTER_EXIT, cb);
    DEV_HILOGI(SERVICE, "DeviceStatusManagerTest007 end");
}
