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

#include <iostream>
#include <gtest/gtest.h>

#include "devicestatus_client.h"
#include "device_status_callback_stub.h"

using namespace testing::ext;
using namespace OHOS::Msdp;
using namespace OHOS::Msdp::DeviceStatus;
using namespace OHOS;
using namespace std;

class DeviceStatusServiceTest : public testing::Test {
public:
    class DeviceStatusServiceTestCallback : public DeviceStatusCallbackStub {
    public:
        DeviceStatusServiceTestCallback() {};
        virtual ~DeviceStatusServiceTestCallback() {};
        virtual void OnDeviceStatusChanged(const Data& devicestatusData);
    };
};

void DeviceStatusServiceTest::DeviceStatusServiceTestCallback::OnDeviceStatusChanged(const Data& devicestatusData)
{
    GTEST_LOG_(INFO) << "DeviceStatusServiceTestCallback type: " << devicestatusData.type;
    GTEST_LOG_(INFO) << "DeviceStatusServiceTestCallback value: " << devicestatusData.value;
    EXPECT_TRUE(devicestatusData.type == Type::TYPE_VERTICAL_POSITION &&
        devicestatusData.value == OnChangedValue::VALUE_ENTER) << "DeviceStatusServiceTestCallback failed";
}

/**
 * @tc.name: DeviceStatusCallbackTest
 * @tc.desc: test devicestatus callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusServiceTest, DeviceStatusCallbackTest, TestSize.Level0)
{
    Type type = Type::TYPE_VERTICAL_POSITION;
    ActivityEvent event = ActivityEvent::EVENT_INVALID;
    ReportLatencyNs latency = ReportLatencyNs::Latency_INVALID;
    auto& devicestatusClient = DeviceStatusClient::GetInstance();
    sptr<IRemoteDevStaCallback> cb = new DeviceStatusServiceTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    devicestatusClient.SubscribeCallback(type, event, latency, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    devicestatusClient.UnsubscribeCallback(type, event, cb);
}

/**
 * @tc.name: GetDeviceStatusDataTest001
 * @tc.desc: test get devicestatus data when type is TYPE_STILL in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusServiceTest, GetDeviceStatusDataTest001, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDeviceStatusDataTest001 Enter");
    Type type = Type::TYPE_STILL;
    auto& devicestatusClient = DeviceStatusClient::GetInstance();
    Data data = devicestatusClient.GetDeviceStatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_TRUE(data.type == Type::TYPE_STILL &&
        data.value == OnChangedValue::VALUE_ENTER) << "GetDeviceStatusData suces";
    DEV_HILOGI(SERVICE, "GetDeviceStatusDataTest001 end");
}

/**
 * @tc.name: GetDeviceStatusDataTest002
 * @tc.desc: test get devicestatus data when type is TYPE_VERTICAL_POSITION in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusServiceTest, GetDeviceStatusDataTest002, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDeviceStatusDataTest002 Enter");
    Type type = Type::TYPE_VERTICAL_POSITION;
    auto& devicestatusClient = DeviceStatusClient::GetInstance();
    Data data = devicestatusClient.GetDeviceStatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_TRUE(data.type == Type::TYPE_VERTICAL_POSITION &&
        data.value == OnChangedValue::VALUE_ENTER);
    DEV_HILOGI(SERVICE, "GetDeviceStatusDataTest002 end");
}

/**
 * @tc.name: GetDeviceStatusDataTest003
 * @tc.desc: test get devicestatus data when type is TYPE_HORIZONTAL_POSITION in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusServiceTest, GetDeviceStatusDataTest003, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDeviceStatusDataTest003 Enter");
    Type type = Type::TYPE_HORIZONTAL_POSITION;
    auto& devicestatusClient = DeviceStatusClient::GetInstance();
    Data data = devicestatusClient.GetDeviceStatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_TRUE(data.type == Type::TYPE_HORIZONTAL_POSITION &&
        data.value == OnChangedValue::VALUE_ENTER);
    DEV_HILOGI(SERVICE, "GetDeviceStatusDataTest003 end");
}

/**
 * @tc.name: GetDeviceStatusDataTest004
 * @tc.desc: test get devicestatus data when type is TYPE_LID_OPEN in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusServiceTest, GetDeviceStatusDataTest004, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GetDeviceStatusDataTest004 Enter";
    Type type = Type::TYPE_LID_OPEN;
    auto& devicestatusClient = DeviceStatusClient::GetInstance();
    Data data = devicestatusClient.GetDeviceStatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_TRUE(data.type == Type::TYPE_LID_OPEN &&
        data.value == OnChangedValue::VALUE_INVALID);
    GTEST_LOG_(INFO) << "GetDeviceStatusDataTest004 end";
}

/**
 * @tc.name: GetDeviceStatusDataTest005
 * @tc.desc: test get devicestatus data when type is TYPE_INVALID in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusServiceTest, GetDeviceStatusDataTest005, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GetDeviceStatusDataTest005 Enter";
    Type type = Type::TYPE_INVALID;
    auto& devicestatusClient = DeviceStatusClient::GetInstance();
    Data data = devicestatusClient.GetDeviceStatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_TRUE(data.type == Type::TYPE_INVALID &&
        data.value == OnChangedValue::VALUE_INVALID);
    GTEST_LOG_(INFO) << "GetDeviceStatusDataTest005 end";
}

/**
 * @tc.name: GetDeviceStatusDataTest006
 * @tc.desc: test get devicestatus data when type is out of range in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusServiceTest, GetDeviceStatusDataTest006, TestSize.Level0)
{
    GTEST_LOG_(INFO) << "GetDeviceStatusDataTest006 Enter";
    Type type = static_cast<Type>(10);
    auto& devicestatusClient = DeviceStatusClient::GetInstance();
    Data data = devicestatusClient.GetDeviceStatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_TRUE(data.type == Type::TYPE_INVALID &&
        data.value == OnChangedValue::VALUE_INVALID);
    GTEST_LOG_(INFO) << "GetDeviceStatusDataTest006 end";
}
