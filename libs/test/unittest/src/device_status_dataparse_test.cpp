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

#include <cstdio>
#include <gtest/gtest.h>

#include "accesstoken_kit.h"

#include "devicestatus_common.h"
#include "devicestatus_data_parse.h"
#include "devicestatus_msdp_client_impl.h"
#include "devicestatus_msdp_mock.h"
#include "sensor_data_callback.h"

using namespace testing::ext;
using namespace OHOS::Msdp;
using namespace OHOS::Msdp::DeviceStatus;
using namespace OHOS;

namespace {
std::shared_ptr<DeviceStatusDataParse> g_dataparse;
const std::string MSDP_DATA_PATH = "/data/msdp/device_status_data.json";
}

class DeviceStatusDataParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DeviceStatusDataParseTest::SetUpTestCase()
{
    g_dataparse = std::make_shared<DeviceStatusDataParse>();
}

void DeviceStatusDataParseTest::TearDownTestCase()
{
    g_dataparse = nullptr;
}

void DeviceStatusDataParseTest::SetUp() {}

void DeviceStatusDataParseTest::TearDown() {}

/**
 * @tc.name: DeviceStatusDataParseTest
 * @tc.desc: test devicestatus CreateJsonFile
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDataParseTest, DeviceStatusDataParseTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusDataParseTest001 start";
    EXPECT_FALSE(g_dataparse->CreateJsonFile() == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusDataParseTest001 end";
}

/**
 * @tc.name: DeviceStatusDataParseTest
 * @tc.desc: test devicestatus CreateJsonFile
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDataParseTest, DeviceStatusDataParseTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusDataParseTest002 start";
    Data data;
    data.type = Type::TYPE_ABSOLUTE_STILL;
    data.value = OnChangedValue::VALUE_INVALID;
    EXPECT_TRUE(g_dataparse->ParseDeviceStatusData(data, Type::TYPE_ABSOLUTE_STILL) == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusDataParseTest002 end";
}

/**
 * @tc.name: DeviceStatusDataParseTest
 * @tc.desc: test devicestatus CreateJsonFile
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDataParseTest, DeviceStatusDataParseTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusDataParseTest003 start";
    Data data;
    data.type = Type::TYPE_ABSOLUTE_STILL;
    data.value = OnChangedValue::VALUE_INVALID;
    std::string fileData;
    Type type = TYPE_ABSOLUTE_STILL;
    EXPECT_TRUE(g_dataparse->DeviceStatusDataInit(fileData, 0, type, data) == ERR_OK);
    GTEST_LOG_(INFO) << "DeviceStatusDataParseTest003 end";
}

/**
 * @tc.name: DeviceStatusDataParseTest
 * @tc.desc: test devicestatus CreateJsonFile
 * @tc.type: FUNC
 */
HWTEST_F(DeviceStatusDataParseTest, DeviceStatusDataParseTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "DeviceStatusDataParseTest004 start";
    EXPECT_TRUE(g_dataparse->DisableCount(Type::TYPE_ABSOLUTE_STILL) == true);
    GTEST_LOG_(INFO) << "DeviceStatusDataParseTest004 end";
}
