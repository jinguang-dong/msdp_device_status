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

#include <chrono>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <string_ex.h>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#include "fi_log.h"
#include "stationary_manager.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace testing::ext;
using namespace OHOS;
using namespace std;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DeviceStatusModuleTest" };
AccessTokenID tokenID_ = 0;
PermissionDef g_infoManagerTestPermDef = {
    .permissionName = "ohos.permission.ACTIVITY_MOTION",
    .bundleName = "devicestatus_module_test",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "test devicestatus_module_test",
    .descriptionId = 1,
    .availableLevel = APL_NORMAL
};
PermissionStateFull g_infoManagerTestState = {
    .grantFlags = { 1 },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.ACTIVITY_MOTION",
    .resDeviceID = { "localTest" }
};
HapPolicyParams g_infoManagerTestPolicyParams = {
    .apl = APL_NORMAL,
    .domain = "test.domain",
    .permList = { g_infoManagerTestPermDef },
    .permStateList = { g_infoManagerTestState },
};

HapInfoParams g_infoManagerTestInfoParams = {
    .bundleName = "devicestatus_module_test",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "devicestatus_module_test"
};
} // namespace

Type DeviceStatusModuleTest::g_moduleTest = Type::TYPE_INVALID;

void DeviceStatusModuleTest::DeviceStatusModuleTestCallback::OnDeviceStatusChanged(const Data& devicestatusData)
{
    GTEST_LOG_(INFO) << "DeviceStatusModuleTestCallback type: " << devicestatusData.type;
    GTEST_LOG_(INFO) << "DeviceStatusModuleTestCallback value: " << devicestatusData.value;
    EXPECT_EQ(true, devicestatusData.type == g_moduleTest &&
        (devicestatusData.value >= OnChangedValue::VALUE_INVALID &&
        devicestatusData.value <= OnChangedValue::VALUE_EXIT)) <<
        "DeviceStatusModuleTestCallback failed";
}

void DeviceStatusModuleTest::SetUpTestCase()
{
    AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = AccessTokenKit::AllocHapToken(g_infoManagerTestInfoParams, g_infoManagerTestPolicyParams);
    tokenID_ = tokenIdEx.tokenIdExStruct.tokenID;
    GTEST_LOG_(INFO) << "tokenID:" << tokenID_;
    ASSERT_NE(0, tokenID_);
    ASSERT_EQ(0, SetSelfTokenID(tokenID_));
}

void DeviceStatusModuleTest::TearDownTestCase()
{
    ASSERT_NE(0, tokenID_);
    int32_t ret = AccessTokenKit::DeleteToken(tokenID_);
    ASSERT_EQ(RET_SUCCESS, ret);
}

void DeviceStatusModuleTest::SetUp() {}

void DeviceStatusModuleTest::TearDown() {}

namespace {
/**
 * @tc.name: DeviceStatusCallbackTest
 * @tc.desc: test devicestatus callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusModuleTest, DeviceStatusCallbackTest, TestSize.Level0)
{
    g_moduleTest = Type::TYPE_ABSOLUTE_STILL;
    Type type = g_moduleTest;
    auto stationaryManager = StationaryManager::GetInstance();
    sptr<IRemoteDevStaCallback> cb = new (std::nothrow) DeviceStatusModuleTestCallback();
    EXPECT_FALSE(cb == nullptr);
    GTEST_LOG_(INFO) << "Start register";
    stationaryManager->SubscribeCallback(type, ActivityEvent::ENTER_EXIT, ReportLatencyNs::LONG, cb);
    GTEST_LOG_(INFO) << "Cancel register";
    stationaryManager->UnsubscribeCallback(type, ActivityEvent::ENTER_EXIT, cb);
}

/**
 * @tc.name: GetDeviceStatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusModuleTest, GetDeviceStatusDataTest001, TestSize.Level0)
{
    CALL_TEST_DEBUG;
    g_moduleTest = Type::TYPE_HORIZONTAL_POSITION;
    Type type = g_moduleTest;
    auto stationaryManager = StationaryManager::GetInstance();
    Data data = stationaryManager->GetDeviceStatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == g_moduleTest &&
        data.value >= OnChangedValue::VALUE_INVALID &&
        data.value <= OnChangedValue::VALUE_EXIT) << "GetDeviceStatusData failed";
}

/**
 * @tc.name: GetDeviceStatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusModuleTest, GetDeviceStatusDataTest002, TestSize.Level0)
{
    CALL_TEST_DEBUG;
    g_moduleTest = Type::TYPE_ABSOLUTE_STILL;
    Type type = g_moduleTest;
    auto stationaryManager = StationaryManager::GetInstance();
    Data data = stationaryManager->GetDeviceStatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == g_moduleTest &&
        data.value >= OnChangedValue::VALUE_INVALID &&
        data.value <= OnChangedValue::VALUE_EXIT) << "GetDeviceStatusData failed";
}

/**
 * @tc.name: GetDeviceStatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusModuleTest, GetDeviceStatusDataTest003, TestSize.Level0)
{
    CALL_TEST_DEBUG;
    g_moduleTest = Type::TYPE_VERTICAL_POSITION;
    Type type = g_moduleTest;
    auto stationaryManager = StationaryManager::GetInstance();
    Data data = stationaryManager->GetDeviceStatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == g_moduleTest &&
        data.value >= OnChangedValue::VALUE_INVALID &&
        data.value <= OnChangedValue::VALUE_EXIT) << "GetDeviceStatusData failed";
}
} // namespace
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
