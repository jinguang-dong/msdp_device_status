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

#include <cstdio>

#include"device_status_datahub_test.h"

using namespace testing::ext;
using namespace OHOS::Msdp;
using namespace OHOS::Msdp::DeviceStatus;
using namespace OHOS;

void DeviceStatusDatahubTest::SetUpTestCase()
{
}

void DeviceStatusDatahubTest::TearDownTestCase()
{
}

void DeviceStatusDatahubTest::SetUp()
{

}

void DeviceStatusDatahubTest::TearDown()
{
}
/**
 * @tc.name: DeviceStatusDataCallbackTest
 * @tc.desc: test devicestatus Callback in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusDatahubTest, DeviceStatusDatahubTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusDatahubTest001 start";
    std::shared_ptr<SensorDataCallback> datahub = SensorDataCallback::GetInstance();
    bool ret = datahub->Init();
    EXPECT_TRUE(ret == true);
    using SensorCallback = std::function<void(int32_t, void*)>;
    SensorCallback callback;
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    int32_t sensorTypeIdt = SENSOR_TYPE_ID_PROXIMITY;
    ret = datahub->SubscribeSensorEvent(sensorTypeId,callback);
    EXPECT_TRUE(ret == true);
    ret = datahub->UnSubscribeSensorEvent(sensorTypeId,callback);
    EXPECT_TRUE(ret == true);
    AccelData data_;
    data_.x = 10;
    data_.y = 12;
    data_.z = 15;
    ret = datahub->NotifyCallback(sensorTypeId,(void*)(&data_));
    EXPECT_TRUE(ret == true);
    ret = datahub->PopData(sensorTypeId,data_);
    EXPECT_TRUE(ret == false);
    ret = datahub->PopData(sensorTypeIdt,data_);
    EXPECT_TRUE(ret == false);
    GTEST_LOG_(INFO) << "DeviceStatusDatahubTest001 end";
}

HWTEST_F (DeviceStatusDatahubTest, DeviceStatusDatahubTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusDatahubTest002 start";
    std::shared_ptr<SensorDataCallback> datahub = SensorDataCallback::GetInstance();
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    bool ret =  datahub->RegisterCallbackSensor(sensorTypeId);
    EXPECT_TRUE(ret == true);
    ret = datahub->UnregisterCallbackSensor(sensorTypeId);
    EXPECT_TRUE(ret == true);
    ret = datahub->Unregister();
    EXPECT_TRUE(ret == true);
    ret = datahub->AlgorithmLoop();
    EXPECT_TRUE(ret == true);
    ret = datahub->HandleSensorEvent();
    EXPECT_TRUE(ret = true);
    GTEST_LOG_(INFO) << "DeviceStatusDatahubTest002 end";
}
/**
 * @tc.name: DeviceStatusMsdpMockTest
 * @tc.desc: test devicestatus Mock in Algorithm
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusDatahubTest, DeviceStatusDatahubTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbsolutstillTest003 start";
    std::shared_ptr <DeviceStatusMsdpMock> TestMock = std::make_shared<DeviceStatusMsdpMock>();
    bool ret = TestMock->Init();
    EXPECT_TRUE(ret = true);
    std::shared_ptr <DeviceStatusMsdpClientImpl> callback_ = std::make_shared<DeviceStatusMsdpClientImpl>();
    int32_t cb = TestMock->RegisterCallback(callback_);
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->UnregisterCallback();
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->Enable(Type::TYPE_INVALID);
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->Enable(Type::TYPE_STILL);
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->Enable(Type::TYPE_HORIZONTAL_POSITION);
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->Enable(Type::TYPE_VERTICAL_POSITION);
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->Enable(Type::TYPE_LID_OPEN);
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->Enable(Type::TYPE_MAX);
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->Disable(Type::TYPE_INVALID);
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->Disable(Type::TYPE_STILL);
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->Disable(Type::TYPE_HORIZONTAL_POSITION);
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->Disable(Type::TYPE_VERTICAL_POSITION);
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->Disable(Type::TYPE_LID_OPEN);
    EXPECT_TRUE(cb == ERR_OK);
    cb = TestMock->Disable(Type::TYPE_MAX);
    EXPECT_TRUE(cb == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusDatahubTest003 end";
}
