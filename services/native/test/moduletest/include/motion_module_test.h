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

#ifndef MOTION_MODULE_TEST_H
#define MOTION_MODULE_TEST_H

#include <gtest/gtest.h>
#include "motion_callback_stub.h"
#include "motion_service.h"

namespace OHOS {
namespace Msdp {
class MotionModuleTest : public testing::Test {
public:

    class MotionTestCallback : public MotionCallbackStub {
    public:
        MotionTestCallback() {};
        virtual ~MotionTestCallback() {};
        virtual void OnMotionChanged(const MotionDataUtils::MotionData& motionData) override;
    };
};
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_MODULE_TEST_H
