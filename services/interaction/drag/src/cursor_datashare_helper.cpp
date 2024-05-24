/*
  * Copyright (c) Huawei Technologies Co., Ltd. 2024. All rights reserved.
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

#include "cursor_datashare_helper.h"

#include "fi_log.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr const char *SETTINGS_DATASHARE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
constexpr const char *SETTINGS_DATA_COLUMN_KEYWORD = "KEYWORD";
constexpr const char *SETTINGS_DATA_COLUMN_VALUE = "VALUE";
} // namespace

CursorDataShareHelper::CursorDataShareHelper(int systemAbilityId)
{
    dataShareHelper_ = CreateDataShareHelper(systemAbilityId);
}

std::shared_ptr<DataShare::DataShareHelper> CursorDataShareHelper::CreateDataShareHelper(int systemAbilityId)
{
    sptr<ISystemAbilityManager> saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        FI_HILOGE("dengsc CreateDataShareHelper GetSystemAbilityManager failed.");
        return nullptr;
    }

    sptr<IRemoteObject> remote = saManager->GetSystemAbility(systemAbilityId);
    if (remote == nullptr) {
        FI_HILOGE("dengsc CreateDataShareHelper GetSystemAbility Service failed.");
        return nullptr;
    }

    FI_HILOGI("dengsc CreateDataShareHelper creator. systemAbilityId:%{public}d", systemAbilityId);
    return DataShare::DataShareHelper::Creator(remote, SETTINGS_DATASHARE_URI);
}

ErrCode CursorDataShareHelper::Query(Uri& uri, const std::string& key, std::string& value)
{
    if (dataShareHelper_ == nullptr) {
        FI_HILOGE("dengsc CursorDataShareHelper query error, dataShareHelper_ is nullptr");
        return ERR_NO_INIT;
    }

    DataShare::DataSharePredicates predicates;
    std::vector<std::string> columns;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    auto result = dataShareHelper_->Query(uri, predicates, columns);
    if (result == nullptr) {
        FI_HILOGE("dengsc CursorDataShareHelper query error, result is null");
        return ERR_INVALID_OPERATION;
    }

    if (result->GoToFirstRow() != DataShare::E_OK) {
        FI_HILOGE("dengsc CursorDataShareHelper query failed,go to first row error");
        result->Close();
        return ERR_INVALID_VALUE;
    }

    int columnIndex {0};
    result->GetColumnIndex(SETTINGS_DATA_COLUMN_VALUE, columnIndex);
    result->GetString(columnIndex, value);
    result->Close();
    FI_HILOGI("dengsc CursorDataShareHelper query success,value[%{public}s]", value.c_str());
    return ERR_OK;
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS