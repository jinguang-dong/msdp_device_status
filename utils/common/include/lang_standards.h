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

#ifndef LANG_STANDARDS_H
#define LANG_STANDARDS_H
#if defined(__clang__) || defined(__GNUC__)
#define CPP_STANDARD    __cplusplus
#elif defined(_MSC_VER)
#define CPP_STANDARD    _MSVC_LANG
#endif

#define CPP_STANDARD_17     201703L
#define CPP_STANDARD_14     201402L
#define CPP_STANDARD_11     201103L
#endif // LANG_STANDARDS_H
