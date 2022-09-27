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

#ifndef MOTION_HILOG_WRAPPER_H
#define MOTION_HILOG_WRAPPER_H

#define CONFIG_HILOG
#ifdef CONFIG_HILOG
#include "hilog/log.h"
namespace OHOS {
namespace Msdp {
#define __FILENAME__            (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define __FORMATED(fmt, ...)    "[%{public}s] %{public}s# " fmt, __FILENAME__, __FUNCTION__, ##__VA_ARGS__

#ifdef MOTION_HILOGF
#undef MOTION_HILOGF
#endif

#ifdef MOTION_HILOGE
#undef MOTION_HILOGE
#endif

#ifdef MOTION_HILOGW
#undef MOTION_HILOGW
#endif

#ifdef MOTION_HILOGI
#undef MOTION_HILOGI
#endif

#ifdef MOTION_HILOGD
#undef MOTION_HILOGD
#endif

// param of log interface, such as MOTION_HILOGF.
enum MotionSubModule {
    MOTION_MODULE_INNERKIT = 0,
    MOTION_MODULE_SERVICE,
    MOTION_MODULE_JS_NAPI,
    MOTION_MODULE_ALGORITHM,
    MOTION_MODULE_COMMON,
    MOTION_MODULE_BUTT,
};

// 0xD002210: subsystem:Msdp module:Motion, 8 bits reserved.
static constexpr unsigned int BASE_MSDP_DOMAIN_ID = 0xD002210;

enum MotionDomainId {
    MOTION_INNERKIT_DOMAIN = BASE_MSDP_DOMAIN_ID + MOTION_MODULE_INNERKIT,
    MOTION_SERVICE_DOMAIN,
    MOTION_JS_NAPI,
    MOTION_ALGORITHM,
    MOTION_COMMON,
    MOTION_BUTT,
};

static constexpr OHOS::HiviewDFX::HiLogLabel MOTION_LABEL[MOTION_MODULE_BUTT] = {
    {LOG_CORE, MOTION_INNERKIT_DOMAIN, "MotionClient"},
    {LOG_CORE, MOTION_SERVICE_DOMAIN, "MotionService"},
    {LOG_CORE, MOTION_JS_NAPI, "MotionJsNapi"},
    {LOG_CORE, MOTION_ALGORITHM, "MotionAlgorithm"},
    {LOG_CORE, MOTION_COMMON, "MotionCommon"},
};

// In order to improve performance, do not check the module range.
// Besides, make sure module is less than MOTION_MODULE_BUTT.
#define MOTION_HILOGF(module, ...) (void)OHOS::HiviewDFX::HiLog::Fatal(MOTION_LABEL[module], __FORMATED(__VA_ARGS__))
#define MOTION_HILOGE(module, ...) (void)OHOS::HiviewDFX::HiLog::Error(MOTION_LABEL[module], __FORMATED(__VA_ARGS__))
#define MOTION_HILOGW(module, ...) (void)OHOS::HiviewDFX::HiLog::Warn(MOTION_LABEL[module], __FORMATED(__VA_ARGS__))
#define MOTION_HILOGI(module, ...) (void)OHOS::HiviewDFX::HiLog::Info(MOTION_LABEL[module], __FORMATED(__VA_ARGS__))
#define MOTION_HILOGD(module, ...) (void)OHOS::HiviewDFX::HiLog::Debug(MOTION_LABEL[module], __FORMATED(__VA_ARGS__))
} // namespace Msdp
} // namespace OHOS

#else

#define MOTION_HILOGF(...)
#define MOTION_HILOGE(...)
#define MOTION_HILOGW(...)
#define MOTION_HILOGI(...)
#define MOTION_HILOGD(...)

#endif // CONFIG_HILOG

#endif // MOTION_HILOG_WRAPPER_H
