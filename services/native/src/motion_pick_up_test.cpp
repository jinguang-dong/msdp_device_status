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

void MotionPickUpTest::MotionPickUpTestCallback::OnMotionChanged(const MotionDataUtils::MotionData& motionData)
{
    GTEST_LOG_(INFO) << "MotionPickUpTestCallback type: " << motionData.type;
    GTEST_LOG_(INFO) << "MotionPickUpTestCallback value: " << motionData.value;
    EXPECT_EQ(true, motionData.type == MotionDataUtils::MotionType::TYPE_TAKE_OFF && \
        motionData.value == MotionDataUtils::MotionValue::VALUE_ENTER) << "MotionPickUpTestCallback falied";
}

/**
 * @tc.name: MotionCallbackTest
 * @tc.desc: test motion TYPE_PICKUP
 * @tc.type: FUNC
 */
HWTEST_F (MotionPickUpTest, MotionCallbackTest, TestSize.Level0)
{
    MOTION_HILOGI(MOTION_MODULE_SERVICE, "MotionCallbackTest Enter");
    MotionDataUtils::MotionType type = MotionDataUtils::MotionType::TYPE_PICKUP;
    auto& motionClient = MotionClient::GetInstance();
    sptr<ImotionCallback> cb = new MotionPickUpTestCallback();
    GTEST_LOG_(INFO) << "Start register";
    motionClient.SubscribeCallback(type, cb);
    GTEST_LOG_(INFO) << "Cancell register";
    motionClient.UnSubscribeCallback(type, cb);
}
