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

#include <cstdio>
#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include <ipc_skeleton.h>

#include "devicestatus_common.h"
#include "devicestatus_msdp_client_impl.h"
#include "devicestatus_msdp_mock.h"
#include "sensor_data_callback.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace testing::ext;
using namespace OHOS;
using namespace std;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "DeviceStatusDatahubTest" };
AccessTokenID tokenID_ = 0;
PermissionDef g_infoManagerTestPermDef = {
    .permissionName = "ohos.permission.ACTIVITY_MOTION",
    .bundleName = "device_status_datahub_test",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "test device_status_datahub_test",
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
    .bundleName = "device_status_datahub_test",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "device_status_datahub_test"
};
} // namespace

class DeviceStatusDatahubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DeviceStatusDatahubTest::SetUpTestCase()
{
    AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = AccessTokenKit::AllocHapToken(g_infoManagerTestInfoParams, g_infoManagerTestPolicyParams);
    tokenID_ = tokenIdEx.tokenIdExStruct.tokenID;
    GTEST_LOG_(INFO) << "tokenID:" << tokenID_;
    ASSERT_NE(0, tokenID_);
    ASSERT_EQ(0, SetSelfTokenID(tokenID_));
    SENSOR_DATA_CB.Init();
}

void DeviceStatusDatahubTest::TearDownTestCase()
{
    ASSERT_NE(0, tokenID_);
    int32_t ret = AccessTokenKit::DeleteToken(tokenID_);
    ASSERT_EQ(RET_SUCCESS, ret);
}

void DeviceStatusDatahubTest::SetUp() {}

void DeviceStatusDatahubTest::TearDown() {}

void SensorAccelCallbackData(int32_t sensorTypeId, AccelData* data)
{
    GTEST_LOG_(INFO) << sensorTypeId;
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest001, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest001 start");
    using SensorCallback = std::function<void(int32_t, AccelData*)>;
    SensorCallback callback = SensorAccelCallbackData;
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret = SENSOR_DATA_CB.SubscribeSensorEvent(sensorTypeId, callback);
    ASSERT_TRUE(ret);
    ret = SENSOR_DATA_CB.UnsubscribeSensorEvent(sensorTypeId, callback);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest002, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest002 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
    AccelData data;
    data.x = 10;
    data.y = 12;
    data.z = 163;
    ret = SENSOR_DATA_CB.PushData(sensorTypeId, reinterpret_cast<uint8_t*>(&data));
    EXPECT_FALSE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest003, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest003 start");
    using SensorCallback = std::function<void(int32_t, AccelData*)>;
    SensorCallback callback = SensorAccelCallbackData;
    int32_t SENSOR_TYPE_ID_ERROR = 300;
    bool ret = SENSOR_DATA_CB.SubscribeSensorEvent(static_cast<SensorTypeId>(SENSOR_TYPE_ID_ERROR), callback);
    ASSERT_TRUE(ret);
    ret = SENSOR_DATA_CB.UnsubscribeSensorEvent(static_cast<SensorTypeId>(SENSOR_TYPE_ID_ERROR), callback);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest004, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest004 start");
    using SensorCallback = std::function<void(int32_t, AccelData*)>;
    SensorCallback callback = SensorAccelCallbackData;
    int32_t SENSOR_TYPE_ID_ERROR = -1;
    bool ret = SENSOR_DATA_CB.SubscribeSensorEvent(static_cast<SensorTypeId>(SENSOR_TYPE_ID_ERROR), callback);
    ASSERT_TRUE(ret);
    ret = SENSOR_DATA_CB.UnsubscribeSensorEvent(static_cast<SensorTypeId>(SENSOR_TYPE_ID_ERROR), callback);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest005, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest005 start");
    using SensorCallback = std::function<void(int32_t, AccelData*)>;
    SensorCallback callback;
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret = SENSOR_DATA_CB.SubscribeSensorEvent(sensorTypeId, callback);
    ASSERT_TRUE(ret);
    ret = SENSOR_DATA_CB.UnsubscribeSensorEvent(sensorTypeId, callback);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest006, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest006 start");
    using SensorCallback = std::function<void(int32_t, AccelData*)>;
    SensorCallback callback = nullptr;
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret = SENSOR_DATA_CB.SubscribeSensorEvent(sensorTypeId, callback);
    ASSERT_TRUE(ret);
    ret = SENSOR_DATA_CB.UnsubscribeSensorEvent(sensorTypeId, callback);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest007, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest007 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest008, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest008 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
    int32_t sensorTypeIdt = SENSOR_TYPE_ID_TEMPERATURE;
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeIdt);
    EXPECT_FALSE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest009, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest009 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
    int32_t SENSOR_TYPE_ID_ERROR = 300;
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(static_cast<SensorTypeId>(SENSOR_TYPE_ID_ERROR));
    EXPECT_FALSE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest010, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest010 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
    int32_t SENSOR_TYPE_ID_ERROR = -1;
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(static_cast<SensorTypeId>(SENSOR_TYPE_ID_ERROR));
    EXPECT_FALSE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest011, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest011 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_TEMPERATURE;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(sensorTypeId);
    EXPECT_FALSE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest012, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest012 start");
    int32_t SENSOR_TYPE_ID_ERROR = 300;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(static_cast<SensorTypeId>(SENSOR_TYPE_ID_ERROR));
    EXPECT_FALSE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(static_cast<SensorTypeId>(SENSOR_TYPE_ID_ERROR));
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest013, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest013 start");
    int32_t SENSOR_TYPE_ID_ERROR = -1;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(static_cast<SensorTypeId>(SENSOR_TYPE_ID_ERROR));
    EXPECT_FALSE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(static_cast<SensorTypeId>(SENSOR_TYPE_ID_ERROR));
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest014, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest014 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest015, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest015 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
    AccelData data;
    data.x = 10;
    data.y = 12;
    data.z = 15;
    ret = SENSOR_DATA_CB.PushData(sensorTypeId, reinterpret_cast<uint8_t*>(&data));
    ASSERT_TRUE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest016, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest016 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
    AccelData data;
    data.x = -10;
    data.y = -12;
    data.z = -15;
    ret = SENSOR_DATA_CB.PushData(sensorTypeId, reinterpret_cast<uint8_t*>(&data));
    ASSERT_TRUE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest017, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest017 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
    AccelData data;
    data.x = 163;
    data.y = 12;
    data.z = 15;
    ret = SENSOR_DATA_CB.PushData(sensorTypeId, reinterpret_cast<uint8_t*>(&data));
    EXPECT_FALSE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest018, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest018 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
    AccelData data;
    data.x = -163;
    data.y = -12;
    data.z = -15;
    ret = SENSOR_DATA_CB.PushData(sensorTypeId, reinterpret_cast<uint8_t*>(&data));
    EXPECT_FALSE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest019, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest019 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
    AccelData data;
    data.x = 10;
    data.y = 163;
    data.z = 15;
    ret = SENSOR_DATA_CB.PushData(sensorTypeId, reinterpret_cast<uint8_t*>(&data));
    EXPECT_FALSE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDatahubTest, DeviceStatusDatahubTest020, TestSize.Level1)
{
    FI_HILOGI("DeviceStatusDatahubTest020 start");
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret =  SENSOR_DATA_CB.RegisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
    AccelData data;
    data.x = -10;
    data.y = -163;
    data.z = -15;
    ret = SENSOR_DATA_CB.PushData(sensorTypeId, reinterpret_cast<uint8_t*>(&data));
    EXPECT_FALSE(ret);
    ret = SENSOR_DATA_CB.UnregisterCallbackSensor(sensorTypeId);
    ASSERT_TRUE(ret);
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
