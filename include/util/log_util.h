/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef DEVICE_LOG_C_H
#define DEVICE_LOG_C_H

#include <stddef.h>

#include "hilog/log.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_DOMAIN
#define LOG_DOMAIN 0xD002B02
#endif
#ifndef LOG_TAG
#define LOG_TAG "huaweicloud-sdk"
#endif

#define OHOS_DEBUG
#ifndef OHOS_DEBUG
#define DECORATOR_HILOG(op, fmt, args...) \
    do {                                  \
        op(LOG_CORE, fmt, ##args);        \
    } while (0)
#else

#define DECORATOR_HILOG(op, fmt, args...)                                                                           \
    do {                                                                                                            \
        op(LOG_CORE, "[%{public}s:%{public}d)] " fmt, __FUNCTION__,  __LINE__, \
            ##args);                                                                                                \
    } while (0)
#endif

#define DEVICE_LOGE(fmt, args...) DECORATOR_HILOG(HILOG_ERROR, fmt, ##args)
#define DEVICE_LOGW(fmt, args...) DECORATOR_HILOG(HILOG_WARN, fmt, ##args)
#define DEVICE_LOGI(fmt, args...) DECORATOR_HILOG(HILOG_INFO, fmt, ##args)
#define DEVICE_LOGF(fmt, args...) DECORATOR_HILOG(HILOG_FATAL, fmt, ##args)

#ifdef __cplusplus
}
#endif

#endif // DEVICE_LOG_C_H
