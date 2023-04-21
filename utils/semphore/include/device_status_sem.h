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

#ifndef DEVICE_STATUS_SEM_H
#define DEVICE_STATUS_SEM_H
#include <memory>
#include "semaphore.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
#define THOUSAND (1000)
#define MILLION (THOUSAND * THOUSAND)
#define BILLION (MILLION * THOUSAND)
}
class Semphore {
public:
    Semphore() {};
    Semphore(const Semphore&) = delete;
    Semphore &operator=(const Semphore&) = delete;
    Semphore (const Semphore&&) = delete;
    
    int32_t Open(const std::string& name, int32_t flag);
    int32_t Post();
    int32_t Wait() const;
    int32_t TryWait() const;
    int32_t WaitFor(unsigned long timeout) const;
    int32_t GetValue() const;
    int32_t Close();
    int32_t Unlink();
private:
    static timespec* Semphore::GetAbsTime( size_t milliseconds, timespec& absTime );
    sem_t* sem_ { nullptr };
    std::string name_;
};

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICE_STATUS_SEM_H
