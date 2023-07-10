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

#ifndef EPOLL_MANAGER_H
#define EPOLL_MANAGER_H

#include <memory>

#include "devicestatus_define.h"
#include "i_epoll_event_source.h"
#include "proto.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class EpollManager {
public:
    EpollManager() = default;
    ~EpollManager() = default;

    int32_t GetFd() const;
    void SetFd(int fd);
    int32_t EpollCreate();
    int32_t EpollAdd(IEpollEventSource *source);
    void EpollDel(IEpollEventSource *source);
    void EpollClose();
    int32_t EpollWait(struct epoll_event *events, int32_t maxevents, int32_t timeout);

private:
    int32_t epollFd_ { -1 };
};

inline int32_t EpollManager::GetFd() const
{
    return epollFd_;
}

inline void EpollManager::SetFd(int fd)
{
    epollFd_ = fd;
}

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICE_MANAGER_EPOLL_H
