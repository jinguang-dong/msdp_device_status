/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef PROTO_H
#define PROTO_H
#include <sys/types.h>
#define MAX_EVENT_SIZE 100
static constexpr int32_t STREAM_BUF_READ_FAIL { 1 };
static constexpr int32_t STREAM_BUF_WRITE_FAIL { 2 };
static constexpr int32_t MAX_VECTOR_SIZE { 10 };
static constexpr int32_t MEM_OUT_OF_BOUNDS { 3 };
static constexpr int32_t MEMCPY_SEC_FUN_FAIL { 4 };
static constexpr int32_t PARAM_INPUT_INVALID { 5 };
static constexpr int32_t MAX_STREAM_BUF_SIZE { 256 };
static constexpr size_t MAX_PACKET_BUF_SIZE { 256 };
static constexpr int32_t ONCE_PROCESS_NETPACKET_LIMIT { 100 };
static constexpr int32_t INVALID_FD { 6 };
static constexpr int32_t INVALID_PID { 7 };
static constexpr int32_t SESSION_NOT_FOUND { 8 };
static constexpr int32_t EPOLL_MODIFY_FAIL { 9 };
static constexpr int32_t ADD_SESSION_FAIL { 11 };
static constexpr size_t MAX_SESSON_ALARM { 100 };
static constexpr int32_t MAX_RECV_LIMIT { 13 };
static constexpr int32_t SERVICE_NOT_RUNNING { 14 };
#define CONNECT_MODULE_TYPE_FI_CLIENT 0
#define CLIENT_RECONNECT_COOLING_TIME 800
#define SEND_RETRY_LIMIT 32
#define SEND_RETRY_SLEEP_TIME 10000

enum class MessageId : int32_t {
    INVALID,

    COORDINATION_ADD_LISTENER,
    COORDINATION_MESSAGE,
    COORDINATION_GET_STATE,

    DRAG_NOTIFY_RESULT,
    DRAG_STATE_LISTENER
};

enum class TokenType : int32_t {
    TOKEN_INVALID = -1,
    TOKEN_HAP = 0,
    TOKEN_NATIVE,
    TOKEN_SHELL
};
#endif // PROTO_H