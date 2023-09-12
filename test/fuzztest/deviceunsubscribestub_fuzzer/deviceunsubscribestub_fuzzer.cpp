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

#include "deviceunsubscribestub_fuzzer.h"

#include "securec.h"
#include "singleton.h"

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#define private public
#include "devicestatus_service.h"
#include "message_parcel.h"

using namespace OHOS;
using namespace OHOS::Msdp::DeviceStatus;

namespace OHOS {

void SetUpTestCase()
{
    const char **perms = new (std::nothrow) const char *[1];
    if (perms == nullptr) {
        return;
    }
    perms[0] = "ohos.permission.ACTIVITY_MOTION";
    TokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "OnRemoteRequest",
        .aplStr = "system_core",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
}

void DoSomethingWithMyAPI(const uint8_t* data, size_t size)
{
    const std::u16string FORMMGR_DEVICE_TOKEN { u"ohos.msdp.Idevicestatus" };
    MessageParcel datas;
    datas.WriteInterfaceToken(FORMMGR_DEVICE_TOKEN);
    datas.WriteBuffer(data, size);
    datas.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    DelayedSingleton<DeviceStatusService>::GetInstance()->OnRemoteRequest(
        static_cast<uint32_t>(Msdp::DeviceInterfaceCode::DEVICESTATUS_UNSUBSCRIBE), datas, reply, option);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    SetUpTestCase();
    OHOS::DoSomethingWithMyAPI(data, size);
    return 0;
}

