/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef COORPERATE_STATE_FREE_H
#define COORPERATE_STATE_FREE_H

#include "i_cooperate_state.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class CoorperateStateFree final : public ICoorperateState {
public:
    CoorperateStateFree() = default;
    int32_t ActivateCoorperate(const std::string &remoteNetworkId, int32_t startDeviceId) override;
    int32_t DeactivateCoorperate(const std::string &networkId, bool isUnchained,
        const std::pair<std::string, std::string> &preparedNetworkId) override;
    void SetStartDeviceDhid(const std::string &startDeviceDhid) override {}

private:
    int32_t ProcessStart(const std::string &remoteNetworkId, int32_t startDeviceId);
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // COORPERATE_STATE_FREE_H
