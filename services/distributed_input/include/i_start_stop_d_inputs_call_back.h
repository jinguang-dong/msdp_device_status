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
#ifndef I_START_STOP_D_INPUTS_CALL_BACK_H
#define I_START_STOP_D_INPUTS_CALL_BACK_H
#include "refbase.h"

namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
struct IStartStopDInputsCallback : public RefBase {
public:
    IStartStopDInputsCallback() = default;
    ~IStartStopDInputsCallback() = default;
};

struct IStartDInputCallback : public RefBase {
public:
    IStartDInputCallback() = default;
    ~IStartDInputCallback() = default;
};

struct IStopDInputCallback : public RefBase {
public:
    IStopDInputCallback() = default;
    ~IStopDInputCallback() = default;
};

struct IPrepareDInputCallback : public RefBase {
public:
    IPrepareDInputCallback() = default;
    ~IPrepareDInputCallback() = default;
};

struct IUnprepareDInputCallback : public RefBase {
public:
    IUnprepareDInputCallback() = default;
    ~IUnprepareDInputCallback() = default;
};
}
}
}
#endif