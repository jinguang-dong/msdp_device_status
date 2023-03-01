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

#include <gtest/gtest.h>

#include "devicestatus_client.h"
#include "device_status_callback_stub.h"

using namespace testing::ext;
using namespace OHOS::Msdp;
using namespace OHOS::Msdp::DeviceStatus;
using namespace OHOS;
using namespace std;

class DeviceStatusModuleTest : public testing::Test {
public:
    class DeviceStatusModuleTestCallback : public DeviceStatusCallbackStub {
    public:
        DeviceStatusModuleTestCallback() = default;
        virtual ~DeviceStatusModuleTestCallback() {};
        virtual void OnDeviceStatusChanged(const Data& devicestatusData);
    };
};

void DeviceStatusModuleTest::DeviceStatusModuleTestCallback::OnDeviceStatusChanged(const Data& devicestatusData)
{
    GTEST_LOG_(INFO) << "DeviceStatusModuleTestCallback type: " << devicestatusData.type;
    GTEST_LOG_(INFO) << "DeviceStatusModuleTestCallback value: " << devicestatusData.value;
    EXPECT_EQ(true, ((devicestatusData.type == Type::TYPE_VERTICAL_POSITION) &&
        (devicestatusData.value == OnChangedValue::VALUE_ENTER))) << "DeviceStatusModuleTestCallback falied";
}

/**
 * @tc.name: DeviceStatusCallbackTest
 * @tc.desc: test devicestatus callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusModuleTest, DeviceStatusCallbackTest, TestSize.Level0)
{
    Type type = Type::TYPE_VERTICAL_POSITION;
    ActivityEvent event = ActivityEvent::EVENT_INVALID;
    ReportLatencyNs latency = ReportLatencyNs::Latency_INVALID;
    auto& devicestatusClient = DeviceStatusClient::GetInstance();
    sptr<IRemoteDevStaCallback> cb = new(std::nothrow) DeviceStatusModuleTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    devicestatusClient.SubscribeCallback(type, event, latency, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    devicestatusClient.UnsubscribeCallback(type, event, cb);
}

/**
 * @tc.name: GetDeviceStatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusModuleTest, GetDeviceStatusDataTest001, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDeviceStatusDataTest001 Enter");
    Type type = Type::TYPE_STILL;
    auto& devicestatusClient = DeviceStatusClient::GetInstance();
    Data data = devicestatusClient.GetDeviceStatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, ((data.type == Type::TYPE_STILL) && (data.value == OnChangedValue::VALUE_ENTER)))
        << "GetDeviceStatusData falied";
}

/**
 * @tc.name: GetDeviceStatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusModuleTest, GetDeviceStatusDataTest002, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDeviceStatusDataTest002 Enter");
    Type type = Type::TYPE_VERTICAL_POSITION;
    auto& devicestatusClient = DeviceStatusClient::GetInstance();
    Data data = devicestatusClient.GetDeviceStatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, ((data.type == Type::TYPE_VERTICAL_POSITION) && (data.value == OnChangedValue::VALUE_ENTER)))
        << "GetDeviceStatusData falied";
}

/**
 * @tc.name: GetDeviceStatusDataTest
 * @tc.desc: test get devicestatus data in proxy
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusModuleTest, GetDeviceStatusDataTest003, TestSize.Level0)
{
    DEV_HILOGI(SERVICE, "GetDeviceStatusDataTest003 Enter");
    Type type = Type::TYPE_HORIZONTAL_POSITION;
    auto& devicestatusClient = DeviceStatusClient::GetInstance();
    Data data = devicestatusClient.GetDeviceStatusData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, ((data.type == Type::TYPE_HORIZONTAL_POSITION) && (data.value == OnChangedValue::VALUE_ENTER)))
        << "GetDeviceStatusData falied";
}
