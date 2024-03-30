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

#ifndef INTENTION_DEFAULT_PARAMS_H
#define INTENTION_DEFAULT_PARAMS_H

#include "intention_identity.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
struct DefaultParam final : public ParamBase {
    DefaultParam() = default;
    DefaultParam(int32_t userData);
    bool Marshalling(MessageParcel &parcel) const override;
    bool Unmarshalling(MessageParcel &parcel) override;

    int32_t userData { -1 };
};

struct DefaultReply final : public ParamBase {
    bool Marshalling(MessageParcel &parcel) const override;
    bool Unmarshalling(MessageParcel &parcel) override;
};

struct BoolenReply final : public ParamBase {
    DefaultParam() = default;
    DefaultParam(bool state);
    bool Marshalling(MessageParcel &parcel) const override;
    bool Unmarshalling(MessageParcel &parcel) override;
    bool state { false };
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // INTENTION_DEFAULT_PARAMS_H
