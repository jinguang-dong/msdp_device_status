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

#ifndef MOTION_DELAYED_SP_SINGLETON_H
#define MOTION_DELAYED_SP_SINGLETON_H

#include <mutex>
#include <memory>
#include <refbase.h>
#include "nocopyable.h"

namespace OHOS {
namespace Msdp {
#define MOTION_DECLARE_DELAYED_SP_SINGLETON(MyClass) \
public: \
    ~MyClass(); \
private: \
    friend MotionDelayedSpSingleton<MyClass>; \
    MyClass();

template<typename T>
class MotionDelayedSpSingleton : public NoCopyable {
public:
    static sptr<T> GetInstance();
    static void DestroyInstance();

private:
    static sptr<T> instance_;
    static std::mutex mutex_;
};

template<typename T>
sptr<T> MotionDelayedSpSingleton<T>::instance_ = nullptr;

template<typename T>
std::mutex MotionDelayedSpSingleton<T>::mutex_;

template<typename T>
sptr<T> MotionDelayedSpSingleton<T>::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance_ == nullptr) {
            instance_ = new T();
        }
    }

    return instance_;
}

template<typename T>
void MotionDelayedSpSingleton<T>::DestroyInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_) {
        instance_.clear();
        instance_ = nullptr;
    }
}
} // namespace Msdp
} // namespace OHOS
#endif // MOTION_DELAYED_SP_SINGLETON_H
