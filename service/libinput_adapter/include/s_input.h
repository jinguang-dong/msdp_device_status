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
#ifndef S_INPUT_H
#define S_INPUT_H
#include <thread>
#include <libudev.h>
#include <functional>
#include <libinput.h>
#include <sys/epoll.h>
#include "nocopyable.h"

namespace OHOS {
namespace MMI {
typedef std::function<void(void *event)> FunInputEvent;
class SInput {
public:
    SInput();
    DISALLOW_COPY_AND_MOVE(SInput);
    virtual ~SInput();
    static void LoginfoPackagingTool(struct libinput_event *event);
    bool Init(FunInputEvent funInputEvent, const std::string& seat_id = "seat0");
    void EventDispatch(struct epoll_event& ev);
    void Stop();

    int32_t GetInputFd() const
    {
        return fd_;
    }

protected:
    void OnEventHandler();

protected:
    int32_t fd_ = -1;
    udev *udev_ = nullptr;
    libinput *input_ = nullptr;

    FunInputEvent funInputEvent_;
    std::string seat_id_;
};
} // namespace MMI
} // namespace OHOS
#endif // S_INPUT_H