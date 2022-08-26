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

#ifndef MOTION_MSDP_INTERFACE_H
#define MOTION_MSDP_INTERFACE_H

#include <string>
#include <memory>
#include <map>
#include <errors.h>
#include "motion_data_utils.h"

namespace OHOS {
namespace Msdp {
class MotionMsdpInterface {
public:
    MotionMsdpInterface() {}
    virtual ~MotionMsdpInterface() {}
    class MsdpAlgorithmCallback {
    public:
        MsdpAlgorithmCallback() = default;
        virtual ~MsdpAlgorithmCallback() = default;
        virtual void OnResult(MotionDataUtils::MotionData& data) = 0;
    };

    virtual ErrCode RegisterCallback(std::shared_ptr<MsdpAlgorithmCallback>& callback) = 0;
    virtual ErrCode UnregisterCallback() = 0;
    virtual ErrCode Enable(const MotionDataUtils::MotionType& type) = 0;
    virtual ErrCode Disable(const MotionDataUtils::MotionType& type) = 0;
    virtual ErrCode DisableCount(const MotionDataUtils::MotionType& type) = 0;
};

struct MsdpAlgorithmHandle {
    void* handle;
    MotionMsdpInterface* (*create)();
    void* (*destroy)(MotionMsdpInterface*);
    MotionMsdpInterface* pAlgorithm;
    MsdpAlgorithmHandle() : handle(nullptr), create(nullptr), destroy(nullptr), pAlgorithm(nullptr) {}
    ~MsdpAlgorithmHandle() {}
    void Clear()
    {
        handle = nullptr;
        create = nullptr;
        destroy = nullptr;
        pAlgorithm = nullptr;
    }
};
}
}
#endif // MOTION_MSDP_INTERFACE_H
