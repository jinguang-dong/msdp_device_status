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

#include "stream_socket.h"

#include <cinttypes>

namespace OHOS {
namespace Msdp {

StreamSocket::StreamSocket() {}

StreamSocket::~StreamSocket()
{
    rust_close(&rustStreamSocket_);
    rust_epoll_close(&rustStreamSocket_);
}

int32_t StreamSocket::EpollCreate(int32_t size)
{
    return rust_epoll_create(&rustStreamSocket_, size);
}

int32_t StreamSocket::EpollCtl(int32_t fd, int32_t op, struct epoll_event &event, int32_t epollFd)
{
    return rust_epoll_ctl(&rustStreamSocket_, fd, op, &event, epollFd);
}

int32_t StreamSocket::EpollWait(struct epoll_event &events, int32_t maxevents, int32_t timeout, int32_t epollFd)
{
    return rust_epoll_wait(&rustStreamSocket_, &events, maxevents, timeout, epollFd);
}

void StreamSocket::EpollClose()
{
    rust_epoll_close(&rustStreamSocket_);
}

void StreamSocket::Close()
{
    rust_close(&rustStreamSocket_);
}
} // namespace Msdp
} // namespace OHOS