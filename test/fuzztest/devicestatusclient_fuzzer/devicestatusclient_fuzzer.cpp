/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "devicestatusclient_fuzzer.h"

#include <cstring>
#include <cstddef>
#include <cstdint>
#include "securec.h"

#include "fi_log.h"

#include "securec.h"

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::Msdp::DeviceStatus;
using namespace Security::AccessToken;
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, OHOS::Msdp::MSDP_DOMAIN_ID, "DeviceStatusClientFuzzTest" };
constexpr int32_t WAIT_TIME { 1000 };
} // namespace

auto stationaryMgr = StationaryManager::GetInstance();
sptr<DeviceStatusClientFuzzer::DeviceStatusTestCallback> cb =
    new (std::nothrow) DeviceStatusClientFuzzer::DeviceStatusTestCallback();

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

void DeviceStatusClientFuzzer::DeviceStatusTestCallback::OnDeviceStatusChanged(const \
    Data& devicestatusData)
{
    std::cout << "DeviceStatusTestCallback type: " << devicestatusData.type << std::endl;
    std::cout << "DeviceStatusTestCallback value: " << devicestatusData.value << std::endl;
}

void DeviceStatusClientFuzzer::TestSubscribeCallback(const uint8_t* data)
{
    std::cout << "TestSubscribeCallback: Enter" << std::endl;
    int32_t type[1];
    int32_t idSize = 4;
    errno_t ret = memcpy_s(type, sizeof(type), data, idSize);
    if (ret != EOK) {
        FI_HILOGE("memcpy_s failed");
        return;
    }

    stationaryMgr->SubscribeCallback(static_cast<Type>(type[0]), ActivityEvent::ENTER_EXIT, ReportLatencyNs::LONG, cb);

    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));
    TestGetDevicestatusData(static_cast<Type>(type[0]));
}

void DeviceStatusClientFuzzer::TestGetDevicestatusData(Type type)
{
    std::cout << "TestGetDevicestatusData: Enter" << std::endl;
    stationaryMgr->GetDeviceStatusData(type);

    std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));
    TestUnSubscribeCallback(type);
}

void DeviceStatusClientFuzzer::TestUnSubscribeCallback(Type type)
{
    std::cout << "TestUnSubscribeCallback: Enter" << std::endl;

    stationaryMgr->UnsubscribeCallback(type, ActivityEvent::ENTER_EXIT, cb);
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    int32_t idSize = 8;
    if (static_cast<int32_t>(size) > idSize) {
        DeviceStatusClientFuzzer::TestSubscribeCallback(data);
    }
    return true;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    SetUpTestCase();
    /* Run your code on data */
    DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
