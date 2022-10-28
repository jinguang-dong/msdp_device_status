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
namespace DeviceStatus {
class DataUtils {
public:
    enum Type {
        TYPE_INVALID = -1,
        TYPE_STILL,
        TYPE_HORIZONTAL_POSITION,
        TYPE_VERTICAL_POSITION,
        TYPE_LID_OPEN,
        TYPE_MAX
    };

    enum TypeValue {
        INVALID = 0,
        VALID,
    };

    enum Value {
        VALUE_INVALID = -1,
        VALUE_EXIT,
        VALUE_ENTER
    };

    enum ActivityEvent {
        EVENT_INVALID = 0,
        ENTER = 1,
        EXIT = 2,
        ENTER_EXIT = 3
    };

    enum ReportLatencyNs {
        Latency_INVALID = -1,
        SHORT = 1,
        MIDDLE = 2,
        LONG = 3
    };
    enum Status {
        STATUS_INVALID = -1,
        STATUS_CANCEL,
        STATUS_START,
        STATUS_PROCESS,
        STATUS_FINISH
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

    struct Data {
        Type type;
        Value value;
        Status status;
        Action action;
        double move;

        bool operator!= (Data const& data) const 
        {
            if (type == data.type && value == data.value
            && status == data.status && action == data.action && move == data.move) {
                return false;
            }
            return true;
        }  
    };
};

typedef struct JsonData {
    int Type;
    char Json[20];
}JsonD;

static JsonD Json[] = {
    {DataUtils::Type::TYPE_STILL, "still"},
    {DataUtils::Type::TYPE_HORIZONTAL_POSITION, "horizontalPosition"},
    {DataUtils::Type::TYPE_VERTICAL_POSITION, "verticalPosition"},
    {DataUtils::Type::TYPE_LID_OPEN, "lid_open"}
};
static int32_t in_vector_count[DataUtils::Type::TYPE_MAX] = {0};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS

#endif // DEVICESTATUS_DATA_UTILS_H
