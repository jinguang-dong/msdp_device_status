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

#ifndef I_COOPERATE_LISTENER_H
#define I_COOPERATE_LISTENER_H

#include <string>

#include "cooperate_message.h"

namespace OHOS {
namespace Msdp {
class ICooperateListener {
public:
    ICooperateListener() = default;
    virtual ~ICooperateListener() = default;
    virtual void OnCooperateMessage(const std::string &networkId, CooperateMessage msg) = 0;
};
} // namespace Msdp
} // namespace OHOS
#endif // I_COOPERATE_LISTENER_H