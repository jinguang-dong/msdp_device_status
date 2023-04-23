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

#include "device_status_sem.h"
#include <ctime>

#include "fcntl.h"
#include "devicestatus_define.h"

#include "util.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "Semphore" };
constexpr int32_t THOUSAND { 1000 };
constexpr int32_t MILLION { THOUSAND * THOUSAND };
constexpr int32_t BILLION { MILLION * THOUSAND };
}

Semphore::~Semphore()
{
    if (sem_ != nullptr) {
        Close();
    }
}

int32_t Semphore::Create(const std::string& name, int32_t flag, mode_t mode, unsigned int value)
{
    sem_t* sem = sem_open(name.c_str(), flag, mode, value);
    if (sem == SEM_FAILED) {
        FI_HILOGE("sem_open failed, errno:%{public}d", errno);
        return RET_ERR;
    }
    sem_ = sem;
    name_ = name;
    return RET_OK;
}

int32_t Semphore::Open(const std::string& name, int32_t flag)
{
    sem_t* sem = sem_open(name.c_str(), flag);
    if (sem == SEM_FAILED) {
        FI_HILOGE("sem_open failed, errno:%{public}d", errno);
        return RET_ERR;
    }
    sem_ = sem;
    name_ = name;
    return RET_OK;
}

int32_t Semphore::Post()
{
    CHKPR(sem_, RET_ERR);
    return sem_post(sem_);
}

int32_t Semphore::Wait()
{
    CHKPR(sem_, RET_ERR);
    return sem_wait(sem_);
}

int32_t Semphore::TryWait()
{
    CHKPR(sem_, RET_ERR);
    return sem_trywait(sem_);
}

int32_t Semphore::WaitFor(int32_t timeoutMs)
{
    CHKPR(sem_, RET_ERR);
    timespec ts;
    GetAbsTime(timeoutMs, ts);
    int32_t ret = sem_timedwait(sem_, &ts);
    if (ret != RET_OK) {
        FI_HILOGE("sem_timedwait timeout, errno:%{public}d", errno);
        return RET_ERR;
    }
    return RET_OK;
}

int32_t Semphore::GetValue() const
{
    CHKPR(sem_, RET_ERR);
    int32_t value = 0;
    sem_getvalue(sem_, &value);
    return value;
}

int32_t Semphore::Close()
{
    CHKPR(sem_, RET_ERR);
    return sem_close(sem_);
}

int32_t Semphore::Unlink()
{
    CHKPR(sem_, RET_ERR);
    return sem_unlink(name_.c_str());
}

void Semphore::GetAbsTime(int32_t milliseconds, timespec& absTime)
{
    clock_gettime( CLOCK_REALTIME, &absTime );
    long nanoSec = 0;
    if (!MultiplyLong((milliseconds % THOUSAND), MILLION, nanoSec)) {
        FI_HILOGE("overflow");
        return;
    }
    if (!AddLongLong(absTime.tv_sec, milliseconds / THOUSAND, absTime.tv_sec)) {
        FI_HILOGE("overflow");
        return;
    }
    if (!AddLong(absTime.tv_nsec, nanoSec, absTime.tv_nsec)) {
        FI_HILOGE("overflow");
        return;
    }
    if(absTime.tv_nsec >= BILLION ) {
        if (!AddLongLong(absTime.tv_sec, 1, absTime.tv_sec)) {
            FI_HILOGE("overflow");
            return;
        }
        absTime.tv_nsec -= BILLION;
    }
}

bool Semphore::isValid()
{
    return sem_ != nullptr;
}

std::string Semphore::GetSemName()
{
    if (!isValid()) {
        FI_HILOGE("Invalid sem");
        return "";
    }
    return name_;
}

} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
