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

#ifndef FUSION_DINPUT_BINDIONG_H
#define FUSION_DINPUT_BINDIONG_H
#include <cinttypes>

#include "dinput.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t fusion_prepare_remote_input(const char* src_id, const char* sink_id, CallbackInput* cb, Callbacks* callback);

int32_t fusion_unprepare_remote_input(const char* src_id, const char* sink_id, CallbackInput* cb, Callbacks* callback);

int32_t fusion_start_remote_input(const char* src_id, const char* sink_id, const char** dhIds, size_t nDhIds,
    CallbackInput* cb, Callbacks* callback);

int32_t fusion_stop_remote_input(const char* src_id, const char* sink_id, const char** dhIds, size_t nDhIds,
    CallbackInput* cb, Callbacks* callback);

bool fusion_is_need_filter_out(const char* sink_id, const CBusinessEvent* event);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif // FUSION_DINPUT_BINDIONG_H
