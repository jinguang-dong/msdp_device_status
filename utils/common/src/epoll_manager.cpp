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

#include "epoll_manager.h"

#include <algorithm>
#include <cstring>
#include <regex>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/stat.h>

#ifdef OHOS_BUILD_ENABLE_COORDINATION
#include "coordination_util.h"
#endif // OHOS_BUILD_ENABLE_COORDINATION
#include "fi_log.h"
#include "napi_constants.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "EpollManager" };
} // namespace

int32_t EpollManager::EpollCreate()
{
    CALL_DEBUG_ENTER;
    epollFd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epollFd_ < 0) {
        FI_HILOGE("epoll_create1 failed");
        return RET_ERR;
    }
    return RET_OK;
}

int32_t EpollManager::EpollAdd(IEpollEventSource *source)
{
    CALL_DEBUG_ENTER;
    CHKPR(source, RET_ERR);
    struct epoll_event ev {
        .events = EPOLLIN | EPOLLHUP | EPOLLERR,
        .data.ptr = source,
    };
    int32_t ret = epoll_ctl(epollFd_, EPOLL_CTL_ADD, source->GetFd(), &ev);
    if (ret != 0) {
        FI_HILOGE("epoll_ctl failed: %{public}s", strerror(errno));
        return RET_ERR;
    }
    return RET_OK;
}

void EpollManager::EpollDel(IEpollEventSource *source)
{
    CALL_DEBUG_ENTER;
    CHKPV(source);
    int32_t ret = epoll_ctl(epollFd_, EPOLL_CTL_DEL, source->GetFd(), nullptr);
    if (ret != 0) {
        FI_HILOGE("epoll_ctl failed: %{public}s", strerror(errno));
    }
}

void EpollManager::EpollClose()
{
    CALL_DEBUG_ENTER;
    if (epollFd_ >= 0) {
        close(epollFd_);
        epollFd_ = -1;
    }
}

int32_t EpollManager::EpollWait(int32_t epollFd, struct epoll_event *events, int32_t maxevents, int32_t timeout)
{
    if (epollFd < 0) {
        epollFd = epollFd_;
    }
    if (epollFd < 0) {
        FI_HILOGE("Invalid param epollFd");
        return RET_ERR;
    }
    auto ret = epoll_wait(epollFd, events, maxevents, timeout);
    if (ret < 0) {
        FI_HILOGE("epoll_wait ret:%{public}d,errno:%{public}d", ret, errno);
    }
    return ret;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
