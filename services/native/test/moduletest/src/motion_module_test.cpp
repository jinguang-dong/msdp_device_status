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

#include "motion_module_test.h"

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

void MotionModuleTest::MotionTestCallback::OnMotionChanged(const MotionDataUtils::MotionData& motionData)
{
    GTEST_LOG_(INFO) << "MotionTestCallback type: " << motionData.type;
    GTEST_LOG_(INFO) << "MotionTestCallback value: " << motionData.value;
    EXPECT_EQ(true, motionData.type == MotionDataUtils::MotionType::TYPE_TAKE_OFF && \
        motionData.value == MotionDataUtils::MotionValue::VALUE_ENTER) << "MotionTestCallback falied";
}

/**
 * @tc.name: MotionCallbackTest
 * @tc.desc: test motion callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionModuleTest, MotionModuleTest001, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MovementModuleTest001 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_TAKE_OFF;
    auto& motionClient = MotionClient::GetInstance();
    sptr<ImotionCallback> cb = new MotionTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    motionClient.SubscribeCallback(type, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    motionClient.UnSubscribeCallback(type, cb);
}

/**
 * @tc.name: MotionCallbackTest
 * @tc.desc: test motion callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionModuleTest, MotionModuleTest002, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MovementModuleTest002 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_PICKUP;
    auto& motionClient = MotionClient::GetInstance();
    sptr<ImotionCallback> cb = new MotionTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    motionClient.SubscribeCallback(type, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    motionClient.UnSubscribeCallback(type, cb);
}

/**
 * @tc.name: MotionCallbackTest
 * @tc.desc: test motion callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionModuleTest, MotionModuleTest003, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MovementModuleTest003 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_CLOSE_TO_EAR;
    auto& motionClient = MotionClient::GetInstance();
    sptr<ImotionCallback> cb = new MotionTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    motionClient.SubscribeCallback(type, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    motionClient.UnSubscribeCallback(type, cb);
}

/**
 * @tc.name: MotionCallbackTest
 * @tc.desc: test motion callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionModuleTest, MotionModuleTest004, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MovementModuleTest004 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_FLIP;
    auto& motionClient = MotionClient::GetInstance();
    sptr<ImotionCallback> cb = new MotionTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    motionClient.SubscribeCallback(type, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    motionClient.UnSubscribeCallback(type, cb);
}

/**
 * @tc.name: MotionCallbackTest
 * @tc.desc: test motion callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionModuleTest, MotionModuleTest005, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MovementModuleTest005 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_WAVE;
    auto& motionClient = MotionClient::GetInstance();
    sptr<ImotionCallback> cb = new MotionTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    motionClient.SubscribeCallback(type, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    motionClient.UnSubscribeCallback(type, cb);
}

/**
 * @tc.name: MotionCallbackTest
 * @tc.desc: test motion callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionModuleTest, MotionModuleTest006, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MovementModuleTest006 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_SHAKE;
    auto& motionClient = MotionClient::GetInstance();
    sptr<ImotionCallback> cb = new MotionTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    motionClient.SubscribeCallback(type, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    motionClient.UnSubscribeCallback(type, cb);
}

/**
 * @tc.name: MotionCallbackTest
 * @tc.desc: test motion callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionModuleTest, MotionModuleTest007, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MovementModuleTest007 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_ROTATE;
    auto& motionClient = MotionClient::GetInstance();
    sptr<ImotionCallback> cb = new MotionTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    motionClient.SubscribeCallback(type, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    motionClient.UnSubscribeCallback(type, cb);
}

/**
 * @tc.name: MotionCallbackTest
 * @tc.desc: test motion callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionModuleTest, MotionModuleTest008, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MovementModuleTest008 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_POCKET;
    auto& motionClient = MotionClient::GetInstance();
    sptr<ImotionCallback> cb = new MotionTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    motionClient.SubscribeCallback(type, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    motionClient.UnSubscribeCallback(type, cb);
}

/**
 * @tc.name: MotionCallbackTest
 * @tc.desc: test motion callback in proxy
 * @tc.type: FUNC
 */
HWTEST_F (MotionModuleTest, MotionModuleTest009, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MovementModuleTest009 Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_WRIST_TILT;
    auto& motionClient = MotionClient::GetInstance();
    sptr<ImotionCallback> cb = new MotionTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    motionClient.SubscribeCallback(type, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    motionClient.UnSubscribeCallback(type, cb);
}
