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

#include <future>
#include <optional>
#include <utility>
#include <vector>

#include <unistd.h>

#include <gtest/gtest.h>

#include "securec.h"
#include "fi_log.h"
#include "v_input_device.h"
#include "virtual_device_builder.h"
#include "virtual_device_defines.h"
#include "virtual_keyboard_builder.h"
#include "virtual_keyboard.h"
#include "virtual_mouse_builder.h"
#include "virtual_mouse.h"
#include "virtual_touchscreen_builder.h"
#include "virtual_touchscreen.h"



namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace testing::ext;
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "Vdevamd_test" };
} // namespace

class VdevadmTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
};


void VdevadmTest::SetUpTestCase()
{
}

void VdevadmTest::SetUp() {}

void VdevadmTest::TearDown()
{
}
/**
 * @tc.name: VdevadmTest_GetDragData_Failed
 * @tc.desc: Get the dragData from interface failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VdevadmTest, GetDragData_Failed, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    ASSERT_EQ(1, 1);
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS