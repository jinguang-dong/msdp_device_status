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

#include "motion_service_test.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <gtest/gtest.h>
#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <string_ex.h>

#include "motion_common.h"
#include "motion_client.h"

using namespace testing::ext;
using namespace OHOS::Msdp;
using namespace OHOS;
using namespace std;

void MotionServiceTest::MotionServiceTestCallback::OnMotionChanged(const MotionDataUtils::MotionData& motionData)
{
    GTEST_LOG_(INFO) << "MotionServiceTestCallback type: " << motionData.type;
    GTEST_LOG_(INFO) << "MotionServiceTestCallback value: " << motionData.value;
    EXPECT_EQ(true, motionData.type == MotionDataUtils::MotionType::TYPE_TAKE_OFF && \
        motionData.value == MotionDataUtils::MotionValue::VALUE_ENTER) << "MotionServiceTestCallback falied";
}

/**
 * @tc.name: MotionCallbackTest
 * @tc.desc: test motion callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionServiceTest, MotionCallbackTest, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MotionCallbackTest Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_TAKE_OFF;
    auto& motionClient = MotionClient::GetInstance();
    sptr<ImotionCallback> cb = new MotionServiceTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    motionClient.SubscribeCallback(type, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    motionClient.UnSubscribeCallback(type, cb);
}

/**
 * @tc.name: GetMotionDataTest
 * @tc.desc: test get motion data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionServiceTest, GetMotionDataTest001, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "GetMotionDataTest001 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_TAKE_OFF;
    auto& motionClient = MotionClient::GetInstance();
    MotionDataUtils::MotionData data = motionClient.GetMotionData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == MotionDataUtils::MotionType::TYPE_TAKE_OFF && \
        data.value == MotionDataUtils::MotionValue::VALUE_INVALID) << "GetMotionData falied";
}

/**
 * @tc.name: GetMotionDataTest
 * @tc.desc: test get motion data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionServiceTest, GetMotionDataTest002, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "GetMotionDataTest002 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_PICKUP;
    auto& motionClient = MotionClient::GetInstance();
    MotionDataUtils::MotionData data = motionClient.GetMotionData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == MotionDataUtils::MotionType::TYPE_PICKUP && \
        data.value == MotionDataUtils::MotionValue::VALUE_INVALID) << "GetMotionData falied";
}

/**
 * @tc.name: GetMotionDataTest
 * @tc.desc: test get motion data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionServiceTest, GetMotionDataTest003, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "GetMotionDataTest003 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_TAKE_OFF;
    auto& motionClient = MotionClient::GetInstance();
    MotionDataUtils::MotionData data = motionClient.GetMotionData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == MotionDataUtils::MotionType::TYPE_TAKE_OFF && \
        data.value == MotionDataUtils::MotionValue::VALUE_INVALID) << "GetMotionData falied";
}

/**
 * @tc.name: GetMotionDataTest
 * @tc.desc: test get motion data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionServiceTest, GetMotionDataTest004, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "GetMotionDataTest004 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_CLOSE_TO_EAR;
    auto& motionClient = MotionClient::GetInstance();
    MotionDataUtils::MotionData data = motionClient.GetMotionData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == MotionDataUtils::MotionType::TYPE_CLOSE_TO_EAR && \
        data.value == MotionDataUtils::MotionValue::VALUE_INVALID) << "GetMotionData falied";
}

/**
 * @tc.name: GetMotionDataTest
 * @tc.desc: test get motion data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionServiceTest, GetMotionDataTest005, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "GetMotionDataTest005 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_FLIP;
    auto& motionClient = MotionClient::GetInstance();
    MotionDataUtils::MotionData data = motionClient.GetMotionData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == MotionDataUtils::MotionType::TYPE_FLIP && \
        data.value == MotionDataUtils::MotionValue::VALUE_INVALID) << "GetMotionData falied";
}

/**
 * @tc.name: GetMotionDataTest
 * @tc.desc: test get motion data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionServiceTest, GetMotionDataTest006, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "GetMotionDataTest006 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_WAVE;
    auto& motionClient = MotionClient::GetInstance();
    MotionDataUtils::MotionData data = motionClient.GetMotionData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == MotionDataUtils::MotionType::TYPE_WAVE && \
        data.value == MotionDataUtils::MotionValue::VALUE_INVALID) << "GetMotionData falied";
}

/**
 * @tc.name: GetMotionDataTest
 * @tc.desc: test get motion data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionServiceTest, GetMotionDataTest007, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "GetMotionDataTest007 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_SHAKE;
    auto& motionClient = MotionClient::GetInstance();
    MotionDataUtils::MotionData data = motionClient.GetMotionData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == MotionDataUtils::MotionType::TYPE_SHAKE && \
        data.value == MotionDataUtils::MotionValue::VALUE_INVALID) << "GetMotionData falied";
}

/**
 * @tc.name: GetMotionDataTest
 * @tc.desc: test get motion data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionServiceTest, GetMotionDataTest008, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "GetMotionDataTest008 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_ROTATE;
    auto& motionClient = MotionClient::GetInstance();
    MotionDataUtils::MotionData data = motionClient.GetMotionData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == MotionDataUtils::MotionType::TYPE_ROTATE && \
        data.value == MotionDataUtils::MotionValue::VALUE_INVALID) << "GetMotionData falied";
}

/**
 * @tc.name: GetMotionDataTest
 * @tc.desc: test get motion data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionServiceTest, GetMotionDataTest009, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "GetMotionDataTest009 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_POCKET;
    auto& motionClient = MotionClient::GetInstance();
    MotionDataUtils::MotionData data = motionClient.GetMotionData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == MotionDataUtils::MotionType::TYPE_POCKET && \
        data.value == MotionDataUtils::MotionValue::VALUE_INVALID) << "GetMotionData falied";
}

/**
 * @tc.name: GetMotionDataTest
 * @tc.desc: test get motion data in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionServiceTest, GetMotionDataTest010, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "GetMotionDataTest010 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_WRIST_TILT;
    auto& motionClient = MotionClient::GetInstance();
    MotionDataUtils::MotionData data = motionClient.GetMotionData(type);
    GTEST_LOG_(INFO) << "type: " << data.type;
    GTEST_LOG_(INFO) << "value: " << data.value;
    EXPECT_EQ(true, data.type == MotionDataUtils::MotionType::TYPE_WRIST_TILT && \
        data.value == MotionDataUtils::MotionValue::VALUE_INVALID) << "GetMotionData falied";
}
