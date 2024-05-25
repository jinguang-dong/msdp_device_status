/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef CURSOR_DATASHARE_HELPER_H
#define CURSOR_DATASHARE_HELPER_H

#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "uri.h"
#include "errors.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class CursorDataShareHelper final {
public:
    CursorDataShareHelper(int systemAbilityId);
    ~CursorDataShareHelper() = default;
    ErrCode Query(Uri& uri, const std::string& key, std::string& value);

private:
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int systemAbilityId);
private:
    std::shared_ptr<DataShare::DataShareHelper> dataShareHelper_ = nullptr;
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif  // CURSOR_DATASHARE_HELPER_H