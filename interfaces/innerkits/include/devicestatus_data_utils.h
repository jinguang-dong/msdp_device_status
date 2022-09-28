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

#ifndef DEVICESTATUS_DATA_UTILS_H
#define DEVICESTATUS_DATA_UTILS_H

namespace OHOS {
namespace Msdp {
class DevicestatusDataUtils {
public:
    enum DevicestatusType {
        TYPE_INVALID = -1,
        TYPE_HIGH_STILL,
        TYPE_FINE_STILL,
        TYPE_CAR_BLUETOOTH,
		TYPE_STAND,
        TYPE_LID_OPEN
    };

    enum DevicestatusTypeValue {
        INVALID = 0,
        VALID,
    };

    enum DevicestatusValue {
        VALUE_INVALID = -1,
        VALUE_ENTER,
        VALUE_EXIT
    };

    enum DevicestatusActivityEvent {
        EVENT_INVALID = 0,
        ENTER = 1,
        EXIT = 2,
        ENTER_EXIT = 3
    };
    enum DevicestatusReportLatencyNs {
        Latency_INVALID = -1,
    };
    enum Status {
        STATUS_INVALID = -1,
    };
    enum Action {
        ACTION_INVALID = -1,
        ACTION_ENLARGE,
        ACTION_REDUCE,
        ACTION_UP,
        ACTION_LEFT,
        ACTION_DOWN,
        ACTION_RIGHT
    };
    struct DevicestatusData {
        DevicestatusType type;
        DevicestatusValue value;
        Status status;
        Action action;
        double move;
    };
};
	static int32_t in_vector_count[DevicestatusDataUtils::DevicestatusType::TYPE_LID_OPEN + 1] = {0};
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_DATA_UTILS_H