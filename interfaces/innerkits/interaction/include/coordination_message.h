/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef COORDINATION_MESSAGE_H
#define COORDINATION_MESSAGE_H

namespace OHOS {
namespace Msdp {
enum class CoordinationMessage {
    UNKNOW = -1,
    PREPARE = 0,
    UNPREPARE = 1,
    ACTIVATE = 2,
    ACTIVATE_SUCCESS = 3,
    ACTIVATE_FAIL = 4,
    DEACTIVATE_SUCCESS = 5,
    DEACTIVATE_FAIL = 6,
    SESSION_CLOSED = 7,
    COORDINATION_SUCCESS = 8
};

enum class CoordinationErrCode {
    COORDINATION_OK = 0,
    SOFTBUS_BIND_FAILED = 1,
    SEND_PACKET_FAILED = 2,
    UNEXPECTED_START_CALL = 3,
    WORKER_THREAD_TIMEOUT = 4,
    READ_DP_FAILED = 5,
    WRITE_DP_FAILED = 6
};

struct CoordinationMsgInfo {
    CoordinationMessage msg;
    CoordinationErrCode errCode;
};

enum class HotAreaType {
    AREA_LEFT = 0,
    AREA_RIGHT = 1,
    AREA_TOP = 2,
    AREA_BOTTOM = 3,
    AREA_NONE = 4
};
} // namespace Msdp
} // namespace OHOS
#endif // COORDINATION_MESSAGE_H