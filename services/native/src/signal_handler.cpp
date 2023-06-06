/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "signal_handler.h"

#include <csignal>
#include <sys/signalfd.h>

#include "devicestatus_define.h"
#include "fi_log.h"
#include "napi_constants.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "SignalHandler" };
} // namespace

int32_t SignalHandler::Init()
{
    CALL_DEBUG_ENTER;
    sigset_t mask { 0 };
    int32_t retCode = sigfillset(&mask);
    if (retCode < 0) {
        FI_HILOGE("Fill signal set failed:%{public}d", errno);
        return RET_ERR;
    }

    retCode = sigprocmask(SIG_SETMASK, &mask, nullptr);
    if (retCode < 0) {
        FI_HILOGE("Sigprocmask failed:%{public}d", errno);
        return RET_ERR;
    }

    fdSignal_ = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
    if (fdSignal_ < 0) {
        FI_HILOGE("Signal fd failed:%{public}d", errno);
        return RET_ERR;
    }

    return RET_OK;
}

void SignalHandler::Dispatch(const struct epoll_event &ev)
{
    CALL_DEBUG_ENTER;
    if ((ev.events & EPOLLIN) == EPOLLIN) {
        signalfd_siginfo sigInfo;
        int32_t size = read(fdSignal_, &sigInfo, sizeof(signalfd_siginfo));
        if (size != static_cast<int32_t>(sizeof(signalfd_siginfo))) {
            FI_HILOGE("Read signal info failed, invalid size:%{public}d,errno:%{public}d", size, errno);
            return;
        }
        signo_ = static_cast<int32_t>(sigInfo.ssi_signo);
        FI_HILOGD("Receive signal:%{public}d", signo_);
        switch (signo_) {
            case SIGQUIT:
            case SIGILL:
            case SIGABRT:
            case SIGBUS:
            case SIGFPE:
            case SIGKILL:
            case SIGSEGV:
            case SIGTERM: {
                state_ = ServiceRunningState::STATE_EXIT;
                break;
            }
            default: {
                break;
            }
        }
    } else if ((ev.events & (EPOLLHUP | EPOLLERR)) != 0) {
        FI_HILOGE("Epoll hangup: %{public}s", strerror(errno));
    }
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
