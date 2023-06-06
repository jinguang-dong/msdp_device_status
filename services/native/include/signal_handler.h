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

#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <atomic>
#include <mutex>
#include <thread>
#include <unistd.h>

#include "i_epoll_event_source.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
enum class ServiceRunningState {STATE_NOT_START, STATE_RUNNING, STATE_EXIT};
class SignalHandler final : public IEpollEventSource {
public:
    int32_t GetFd() const override;
    ServiceRunningState GetState() const;
    void SetState(ServiceRunningState state);
    void Dispatch(const struct epoll_event &ev) override;
    int32_t Init();
    
private:
    int32_t fdSignal_;
    int32_t signo_;
    std::atomic<ServiceRunningState> state_ = ServiceRunningState::STATE_NOT_START;
};

inline int32_t SignalHandler::GetFd() const
{
    return fdSignal_;
}

inline ServiceRunningState SignalHandler::GetState() const
{
    return state_;
}

inline void SignalHandler::SetState(ServiceRunningState state)
{
    state_ = state;
}

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // SIGNAL_HANDLER_H
