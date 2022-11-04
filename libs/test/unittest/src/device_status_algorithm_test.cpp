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

#include "device_status_algorithm_test.h"

#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <errors.h>

#include "accesstoken_kit.h"
#include "devicestatus_common.h"
#include "devicestatus_dumper.h"
#include "devicestatus_data_utils.h"
#include "devicestatus_msdp_interface.h"
#include "devicestatus_manager.h"
#include "devicestatus_msdp_client_impl.h"

using namespace testing::ext;
using namespace OHOS::Msdp;
using namespace OHOS::Msdp::DeviceStatus;
using namespace OHOS;


void DeviceStatusAlgorithmTest::SetUpTestCase()
{
}

void DeviceStatusAlgorithmTest::TearDownTestCase()
{
}

void DeviceStatusAlgorithmTest::SetUp()
{

}

void DeviceStatusAlgorithmTest::DeviceStatusAlgorithmTest::TearDown()
{
}
/**
 * @tc.name: DeviceStatusAbsoluteStillTest
 * @tc.desc: test devicestatus Absolute Still Algorithm
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusAlgorithmTest, DeviceStatusAlgorithmTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbsolutstillTest001 start";
    std::shared_ptr<SensorDataCallback> callback = SensorDataCallback::GetInstance();
    AbsoluteStill* still = new AbsoluteStill(callback);
    bool ret = still->Init(); 
    EXPECT_TRUE(ret == true);
    ret = still->UnSubscribe();
    EXPECT_TRUE(ret == true);
    std::shared_ptr <DeviceStatusMsdpClientImpl> callback_ = std::make_shared<DeviceStatusMsdpClientImpl>();
    ret = still->RegisterCallback(callback_);
    EXPECT_TRUE(ret == true);
    delete still;
    still = nullptr;
    GTEST_LOG_(INFO) << "DeviceStatusAlgorithmTest001 end";
}
/**
 * @tc.name: DeviceStatusHorizontalTest
 * @tc.desc: test devicestatus Horizontal Algorithm
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusAlgorithmTest, DeviceStatusAlgorithmTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbsolutstillTest002 start";
    std::shared_ptr<SensorDataCallback> callback = SensorDataCallback::GetInstance();
    Horizontal* horizontal = new Horizontal(callback);
    bool ret = horizontal->Init();
    EXPECT_TRUE(ret == true);
    std::shared_ptr <DeviceStatusMsdpClientImpl> callback_ = std::make_shared<DeviceStatusMsdpClientImpl>();
    ret = horizontal->RegisterCallback(callback_);
    EXPECT_TRUE(ret == true);
    ret = horizontal->UnSubscribe();
    EXPECT_TRUE(ret == true);
    delete horizontal;
    horizontal = nullptr;
    GTEST_LOG_(INFO) << "DeviceStatusAlgorithmTest002 end";
}
/**
 * @tc.name: DeviceStatusVeriticalTest
 * @tc.desc: test devicestatus Veritical Algorithm
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusAlgorithmTest, DeviceStatusAlgorithmTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbsolutstillTest003 start";
    std::shared_ptr<SensorDataCallback> callback = SensorDataCallback::GetInstance();
    Vertical* vertical = new Vertical(callback);
    bool ret = vertical->Init();
    EXPECT_TRUE(ret == true);
    std::shared_ptr <DeviceStatusMsdpClientImpl> callback_ = std::make_shared<DeviceStatusMsdpClientImpl>();
    ret = vertical->RegisterCallback(callback_);
    EXPECT_TRUE(ret == true);
    ret = vertical->UnSubscribe();
    EXPECT_TRUE(ret == true);
    delete vertical;
    vertical = nullptr;
    GTEST_LOG_(INFO) << "DeviceStatusAlgorithmTest003 end";
}
/**
 * @tc.name: DeviceStatusAlgorithmManagerTest
 * @tc.desc: test devicestatus Algorithm Manager
 * @tc.type: FUNC
 */
HWTEST_F (DeviceStatusAlgorithmTest, DeviceStatusAlgorithmTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "AbsolutstillTest004 start";
    std::shared_ptr<AlgoMgr> manager = std::make_shared<AlgoMgr>();
    int32_t ret = manager->UnregisterCallback();
    GTEST_LOG_(INFO) << "10";
    EXPECT_TRUE(ret == ERR_OK);
    bool cb = manager->SensorStart(Type::TYPE_LID_OPEN);
    EXPECT_TRUE(cb == false);  
    ret = manager->UnregisterCallback();
    EXPECT_TRUE(ret == ERR_OK);
    int32_t sensorTypeId = SENSOR_TYPE_ID_ACCELEROMETER;
    ret = manager->CheckSensorTypeId(sensorTypeId);
    EXPECT_TRUE(ret == true);
    std::vector<int32_t> retone =  manager->GetSensorTypeId(Type::TYPE_STILL);
    EXPECT_TRUE(retone.front() == SENSOR_TYPE_ID_ACCELEROMETER);
    std::vector<int32_t> rettwo =  manager->GetSensorTypeId(Type::TYPE_HORIZONTAL_POSITION);
    EXPECT_TRUE(rettwo.front() == SENSOR_TYPE_ID_ACCELEROMETER);
    std::vector<int32_t> retthree =  manager->GetSensorTypeId(Type::TYPE_VERTICAL_POSITION);
    EXPECT_TRUE(retthree.front() == SENSOR_TYPE_ID_ACCELEROMETER);
    int32_t typeget = SENSOR_TYPE_ID_ACCELEROMETER;
    std::vector<Type> retd  = manager->GetAlgoType(typeget);
    EXPECT_TRUE(retd.front() ==  OHOS::Msdp::DeviceStatus::TYPE_STILL);
    EXPECT_TRUE(retd[1] ==  OHOS::Msdp::DeviceStatus::TYPE_VERTICAL_POSITION);
    EXPECT_TRUE(retd.back() ==  OHOS::Msdp::DeviceStatus:: TYPE_HORIZONTAL_POSITION);
    GTEST_LOG_(INFO) << "DeviceStatusAlgorithmTest004 end";
}
