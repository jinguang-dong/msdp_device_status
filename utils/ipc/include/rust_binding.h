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
#ifndef RUST_BINDING_H
#define RUST_BINDING_H
#include <stdint.h>
#include <string.h>
#include "proto.h"

extern "C" {
    enum class ErrorStatus : int32_t {
        ERROR_STATUS_Ok = 0,
        ERROR_STATUS_Read = 1,
        ERROR_STATUS_Write = 2,
    };
    struct RustStreamSocket {
        int32_t fd_ { -1 };
        int32_t epollFd_ { -1 };
    };
    struct RustStreamSession {
        int32_t moduleType_ { -1 };
        int32_t fd_ { -1 };
        int32_t uid_ { -1 };
        int32_t pid_ { -1 };
        int32_t tokenType_ { -1 };
    };
    struct RustStreamClient {
        bool isExit { false };
        bool isRunning_ { false };
        bool isConnected_ { false };
    };
    struct RustStreamBuffer {
        ErrorStatus rwErrorStatus_ { ErrorStatus::ERROR_STATUS_Ok };
        int32_t rCount_ { 0 };
        int32_t wCount_ { 0 };
        int32_t rPos_ { 0 };
        int32_t wPos_ { 0 };
        char szBuff_[MAX_STREAM_BUF_SIZE+1] { };
    };
    struct RustNetPacket {
        MessageId msgId_ { MessageId::INVALID };
        struct RustStreamBuffer rustStreamBuffer;
    };
    int32_t StreamSocketGetFd(const RustStreamSocket* rustStreamSocket);
    int32_t StreamSocketGetEpollFd(const RustStreamSocket* rustStreamSocket);
    int32_t StreamSocketEpollCreate(RustStreamSocket* rustStreamSocket, int32_t fd);
    int32_t StreamSocketEpollCtl(RustStreamSocket* rustStreamSocket, int32_t fd, int32_t op, struct epoll_event* event, int32_t epollFd);
    int32_t StreamSocketEpollWait(RustStreamSocket* rustStreamSocket, struct epoll_event* events, int32_t maxevents, int32_t timeout, int32_t epollFd);
    int32_t StreamSocketEpollClose(RustStreamSocket* rustStreamSocket);
    int32_t StreamSocketClose(RustStreamSocket* rustStreamSocket);
    void StreamSessionClose(RustStreamSession* rustStreamSession);
    bool StreamSessionSendMsg(const RustStreamSession* rustStreamSession, const char* buf, size_t size);
    void StreamBufferReset(RustStreamBuffer* rustStreamBuffer);
    void StreamBufferClean(RustStreamBuffer* rustStreamBuffer);
    bool StreamBufferRead(RustStreamBuffer* rustStreamBuffer1, RustStreamBuffer* rustStreamBuffer2);
    bool StreamBufferWrite(RustStreamBuffer* rustStreamBuffer1, const RustStreamBuffer* rustStreamBuffer2);
    const char* StreamBufferReadBuf(const RustStreamBuffer* rustStreamBuffer);
    bool StreamBufferReadChar(RustStreamBuffer* rustStreamBuffer, char* buf, size_t size);
    bool StreamBufferWriteChar(RustStreamBuffer* rustStreamBuffer, const char* buf, size_t size);
    const char* StreamBufferData(const RustStreamBuffer* rustStreamBuffer);
    size_t StreamBufferSize(const RustStreamBuffer* rustStreamBuffer);
    const char* StreamBufferGetErrorStatusRemark(const RustStreamBuffer* rustStreamBuffer);
    bool StreamBufferChkRWError(const RustStreamBuffer* rustStreamBuffer);
    bool StreamBufferCheckWrite(RustStreamBuffer* rustStreamBuffer, size_t size);
    bool CircleStreamBufferWrite(RustStreamBuffer* rustStreamBuffer, const char* buf, size_t size);
    void CircleStreamBufferCopyDataToBegin(RustStreamBuffer* rustStreamBuffer);
    int32_t StreamSessionGetUid(const RustStreamSession* rustStreamSession);
    int32_t StreamSessionGetPid(const RustStreamSession* rustStreamSession);
    int32_t StreamSessionGetFd(const RustStreamSession* rustStreamSession);
    void StreamSessionSetTokenType(RustStreamSession* rustStreamSession, int32_t type);
    int32_t StreamSessionGetTokenType(const RustStreamSession* rustStreamSession);
}
#endif // RUST_BINDING_H