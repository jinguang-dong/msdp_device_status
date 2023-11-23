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

#include "entertexteditorarea_fuzzer.h"

#include "singleton.h"

#include "devicestatus_service.h"
#include "fi_log.h"
#include "message_parcel.h"

using namespace OHOS::Msdp::DeviceStatus;
namespace OHOS {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, OHOS::Msdp::MSDP_DOMAIN_ID, "EnterTextEditorAreaFuzzTest" };

bool EnterTextEditorAreaFuzzTest(const uint8_t* data, size_t size)
{
    const std::u16string formmgrDeviceToken { u"ohos.msdp.Idevicestatus" };
    MessageParcel datas;
    if (!datas.WriteInterfaceToken(formmgrDeviceToken)) {
        FI_HILOGE("Write failed");
        return false;
    }
    if (!datas.WriteBuffer(data, size)) {
        FI_HILOGE("Write data failed");
        return false;
    }
    if (!datas.RewindRead(0)) {
        FI_HILOGE("Read failed");
        return false;
    }
    MessageParcel reply;
    MessageOption option;
    DelayedSingleton<DeviceStatusService>::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(Msdp::DeviceInterfaceCode::ENTER_TEXT_EDITOR_AREA), datas, reply, option);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    OHOS::EnterTextEditorAreaFuzzTest(data, size);
    return 0;
}

