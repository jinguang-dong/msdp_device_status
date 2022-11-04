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

#ifndef OHOS_MSDP_DEVICESTATUS_ABSOLUTE_STILL_TEST_H
#define OHOS_MSDP_DEVICESTATUS_ABSOLUTE_STILL_TEST_H

#include <gtest/gtest.h>

#include "devicestatus_algorithm_manager.h"
#include "device_status_absolute_still.h"
#include "devicestatus_data_utils.h"
#include "devicestatus_data_define.h"
#include "sensor_data_callback.h"
#include "device_status_horizontal.h"
#include "device_status_vertical.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class DeviceStatusAlgorithmTest : public testing::Test {
    public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    class DeviceStatusMock:public IMsdp::MsdpAlgoCallback{
        public:
        DeviceStatusMock() = default;
        virtual ~DeviceStatusMock() = default;
        void OnResult(const Data& data) {}
};
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif //OHOS_MSDP_DEVICESTATUS_ABSOLUTE_STILL_TEST_H
