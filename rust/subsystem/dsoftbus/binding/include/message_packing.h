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

#ifndef MESSAGE_PACKING_H
#define MESSAGE_PACKING_H
#include <cinttypes>

#ifdef __cplusplus
extern "C" {
#endif
struct CJsonStruct;

using HandleCb = void (*)(int32_t session_id, const char* str);

void CSaveHandleCb(HandleCb callback);

HandleCb CGetHandleCb();

CJsonStruct* CGetCJsonObj();

bool CAddNumber(CJsonStruct* cJsonObj, int32_t value, const char* str);

bool CAddbool(CJsonStruct* cJsonObj, bool value, const char* str);

bool CAddString(CJsonStruct* cJsonObj, const char* value, const char* str);

bool CJsonPrint(CJsonStruct* cJsonObj, char* msg);

bool CJsonDelete(CJsonStruct* cJsonObj);

void CJsonFree(char* str);

void CParse(const char* message, CJsonStruct* cJsonObj);

bool CIsJsonObj(CJsonStruct* cJsonObj);

bool CIsNumber(CJsonStruct* cJsonObj);

bool CIsBool(CJsonStruct* cJsonObj);

bool CIsString(CJsonStruct* cJsonObj);

bool CIsTrue(CJsonStruct* cJsonObj);

int32_t CGetValueInt(CJsonStruct* cJsonObj);

//bool CGetValueBool(CJsonStruct* cJsonObj);

const char* CGetValueString(CJsonStruct* cJsonObj);

bool CGetObjectItemCaseSensitive(CJsonStruct* cJsonObj, const char* cmd_type, CJsonStruct* comType);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // MESSAGE_PACKING_H