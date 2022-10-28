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

#ifndef DEVICESTATUS_ALGORITHM_MANAGER_INTERFACE_H
#define DEVICESTATUS_ALGORITHM_MANAGER_INTERFACE_H

#include <string>
#include <memory>
#include <map>
#include <errors.h>

#include "devicestatus_data_utils.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class IAlgoMgr {
public:
    IAlgoMgr() = default;
    virtual ~IAlgoMgr() = default;
    class AlgoCallback {
    public:
        AlgoCallback() = default;
        virtual ~AlgoCallback() = default;
        virtual void OnAlogrithmResult(const DataUtils::Data& data) = 0;
    };

    virtual ErrCode RegisterCallback(std::shared_ptr<AlgoCallback>& callback);
    virtual ErrCode UnregisterCallback();
    virtual ErrCode Enable(const DataUtils::Type& type);
    virtual ErrCode Disable(const DataUtils::Type& type);
    virtual ErrCode DisableCount(const DataUtils::Type& type);
};

struct AlgoHandle {
    void* handle;
    IAlgoMgr* (*create)();
    void* (*destroy)(IAlgoMgr*);
    IAlgoMgr* pAlgorithm;
    AlgoHandle() : handle(nullptr), create(nullptr), destroy(nullptr), pAlgorithm(nullptr) {}
    ~AlgoHandle() {}
    void Clear()
    {
        handle = nullptr;
        create = nullptr;
        destroy = nullptr;
        pAlgorithm = nullptr;
    }
};
} // namespace DeviceStatus
} // namespace Msdp
} // OHOS

#endif // DEVICESTATUS_ALGORITHM_MANAGER_INTERFACE_H
