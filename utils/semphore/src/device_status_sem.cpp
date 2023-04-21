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

#include "device_status_sem.h"

#include <ctime>
#include <fctl.h>

#include "devicestatus_data_define.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "Semphore" };
}

int32_t Semphore::Open(const std::string& name, int32_t flag)
{
    sem_t* sem = sem_open(name.c_str(), flag);
    if (sem == SEM_FAILED) {
        FIHILOGE("sem_open failed, errno:%{public}d", errno);
        return RET_ERR;
    }
    sem_ = sem;
    name_ = name;
    return RET_OK;
}

int32_t Semphore::Post()
{
    return sem_post(sem_);
}

int32_t Semphore::Wait() const
{
    return sem_wait(sem_);
}

int32_t Semphore::TryWait() const
{
    return sem_trywait(sem_);
}

int32_t Semphore::WaitFor(unsigned long timeoutMs) const
{
    struct timespec ts;
    GetAbsTime(timeoutMs, ts);
    int32_t ret = sem_timedwait(sem_, &ts);
    if (ret != RET_OK) {
        printf("sem_timedwait timeout, errno:%{public}d", errno);
        return RET_ERR;
    }
    return RET_OK;
}

int32_t Semphore::GetValue() const
{
    int32_t value = 0;
    sem_getvalue(sem_, &value);
    return value;
}

int32_t Semphore::Close()
{
    return sem_close(sem_);
}

int32_t Semphore::Unlink()
{
    return sem_unlink(name_.c_str());
}

timespec* Semphore::GetAbsTime( size_t milliseconds, timespec& absTime )
{
    // todo 校验越界
    clock_gettime( CLOCK_REALTIME, &absTime );
    absTime.tv_sec += milliseconds / THOUSAND;
    absTime.tv_nsec += (milliseconds % THOUSAND) * MILLION;
    if( absTime.tv_nsec >= BILLION ) {
        absTime.tv_sec += 1;
        absTime.tv_nsec -= BILLION;
    }
   return &absTime;
}

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICE_STATUS_SEM_H
