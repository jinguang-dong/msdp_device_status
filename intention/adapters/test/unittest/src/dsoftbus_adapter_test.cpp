/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <memory>
#include <vector>

#include <unistd.h>

#include "accesstoken_kit.h"
#include <gtest/gtest.h>
#include "nativetoken_kit.h"
#include "securec.h"
#include "token_setproc.h"

#include "devicestatus_define.h"
#include "devicestatus_errors.h"
#include "dsoftbus_adapter_impl.h"
#include "dsoftbus_adapter.h"
#include "utility.h"

#undef LOG_TAG
#define LOG_TAG "DsoftbusAdapterTest"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
using namespace testing::ext;
namespace {
constexpr int32_t TIME_WAIT_FOR_OP_MS { 20 };
const std::string SYSTEM_CORE { "system_core" };
uint64_t g_tokenID { 0 };
#define SERVER_SESSION_NAME "ohos.msdp.device_status.intention.serversession"
const std::string CLIENT_SESSION_NAME { "ohos.msdp.device_status.intention.clientsession." };
constexpr size_t DEVICE_NAME_SIZE_MAX { 256 };
constexpr size_t PKG_NAME_SIZE_MAX { 65 };
constexpr int32_t SOCKET_SERVER { 0 };
constexpr int32_t SOCKET_CLIENT { 1 };
constexpr int32_t SOCKET { 1 };
const std::string NetworkId { "cde2b5b4453a5b3ec566f836ffa7a4aab52c4b9c8a0b34f3d6aaca4566db24f0" };
const char* g_cores[] = { "ohos.permission.INPUT_MONITORING" };
} // namespace

class DsoftbusAdapterTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    static void SetPermission(const std::string &level, const char** perms, size_t permAmount);
    static void RemovePermission();
};



void DsoftbusAdapterTest::SetPermission(const std::string &level, const char** perms, size_t permAmount)
{
    CALL_DEBUG_ENTER;
    if (perms == nullptr || permAmount == 0) {
        FI_HILOGE("The perms is empty");
        return;
    }

    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = permAmount,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "DsoftbusAdapterTest",
        .aplStr = level.c_str(),
    };
    g_tokenID = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(g_tokenID);
    OHOS::Security::AccessToken::AccessTokenKit::AccessTokenKit::ReloadNativeTokenInfo();
}

void DsoftbusAdapterTest::RemovePermission()
{
    CALL_DEBUG_ENTER;
    int32_t ret = OHOS::Security::AccessToken::AccessTokenKit::DeleteToken(g_tokenID);
    if (ret != RET_OK) {
        FI_HILOGE("Failed to remove permission");
        return;
    }
}

void DsoftbusAdapterTest::SetUpTestCase() {}

void DsoftbusAdapterTest::SetUp() {}

void DsoftbusAdapterTest::TearDown()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP_MS));
}

class DSoftbusObserver final : public IDSoftbusObserver {
public:
    DSoftbusObserver() = default;
    ~DSoftbusObserver() = default;

    void OnBind(const std::string &networkId) {}
    void OnShutdown(const std::string &networkId) {}
    bool OnPacket(const std::string &networkId, NetPacket &packet)
    {
        return true;
    }
    bool OnRawData(const std::string &networkId, const void *data, uint32_t dataLen)
    {
        return true;
    }
};

/**
 * @tc.name: TestEnable
 * @tc.desc: Test Enable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DsoftbusAdapterTest, TestEnable, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    std::shared_ptr<DSoftbusAdapter>dSoftbusAdapter = std::make_shared<DSoftbusAdapter>();
    ASSERT_NO_FATAL_FAILURE(dSoftbusAdapter->Enable());
    RemovePermission();
}

/**
 * @tc.name: TestDisable
 * @tc.desc: Test Disable
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DsoftbusAdapterTest, TestDisable, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    std::shared_ptr<DSoftbusAdapter>dSoftbusAdapter = std::make_shared<DSoftbusAdapter>();
    ASSERT_NO_FATAL_FAILURE(dSoftbusAdapter->Disable());
    RemovePermission();
}

/**
 * @tc.name: TestAddObserver
 * @tc.desc: Test AddObserver
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DsoftbusAdapterTest, TestAddObserver, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    std::shared_ptr<DSoftbusAdapter>dSoftbusAdapter = std::make_shared<DSoftbusAdapter>();
    std::shared_ptr<IDSoftbusObserver> observer = std::make_shared<DSoftbusObserver>();
    ASSERT_NO_FATAL_FAILURE(dSoftbusAdapter->Enable());
    ASSERT_NO_FATAL_FAILURE(dSoftbusAdapter->AddObserver(observer));
    ASSERT_NO_FATAL_FAILURE(dSoftbusAdapter->Disable());
    RemovePermission();
}
/**
 * @tc.name: TestRemoveObserver
 * @tc.desc: Test RemoveObserver
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DsoftbusAdapterTest, TestRemoveObserver, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    std::shared_ptr<DSoftbusAdapter>dSoftbusAdapter = std::make_shared<DSoftbusAdapter>();
    std::shared_ptr<IDSoftbusObserver> observer = std::make_shared<DSoftbusObserver>();
    ASSERT_NO_FATAL_FAILURE(dSoftbusAdapter->Enable());
    ASSERT_NO_FATAL_FAILURE(dSoftbusAdapter->AddObserver(observer));
    ASSERT_NO_FATAL_FAILURE(dSoftbusAdapter->RemoveObserver(observer));
    ASSERT_NO_FATAL_FAILURE(dSoftbusAdapter->Disable());
    RemovePermission();
}

/**
 * @tc.name: TestOpenSession
 * @tc.desc: Test OpenSession
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DsoftbusAdapterTest, TestOpenSession, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    std::shared_ptr<DSoftbusAdapter>dSoftbusAdapter = std::make_shared<DSoftbusAdapter>();
    int32_t ret = dSoftbusAdapter->OpenSession(NetworkId);
    ASSERT_EQ(ret, RET_ERR);
    RemovePermission();
}

/**
 * @tc.name: TestCloseSession
 * @tc.desc: Test CloseSession
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DsoftbusAdapterTest, TestCloseSession, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    std::shared_ptr<DSoftbusAdapter>dSoftbusAdapter = std::make_shared<DSoftbusAdapter>();
    int32_t ret = dSoftbusAdapter->OpenSession(NetworkId);
    ASSERT_EQ(ret, RET_ERR);
    ASSERT_NO_FATAL_FAILURE(dSoftbusAdapter->CloseSession(NetworkId));
    RemovePermission();
}


/**
 * @tc.name: TestSendPacket
 * @tc.desc: Test SendPacket
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DsoftbusAdapterTest, SendPacket, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    std::shared_ptr<DSoftbusAdapter>dSoftbusAdapter = std::make_shared<DSoftbusAdapter>();
    NetPacket packet(MessageId::DSOFTBUS_START_COOPERATE);
    ASSERT_NO_FATAL_FAILURE(dSoftbusAdapter->SendPacket(NetworkId, packet));
    RemovePermission();
}

/**
 * @tc.name: TestSendParcel
 * @tc.desc: Test SendParcel
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DsoftbusAdapterTest, SendParcel, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    std::shared_ptr<DSoftbusAdapter>dSoftbusAdapter = std::make_shared<DSoftbusAdapter>();
    Parcel parcel;
    ASSERT_NO_FATAL_FAILURE(dSoftbusAdapter->SendParcel(NetworkId, parcel));
    RemovePermission();
}

/**
 * @tc.name: TestSetupServer
 * @tc.desc: Test SetupServer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DsoftbusAdapterTest, SetupServer, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    std::shared_ptr<DSoftbusAdapter>dSoftbusAdapter = std::make_shared<DSoftbusAdapter>();
    int32_t ret = DSoftbusAdapterImpl::GetInstance()->SetupServer();
    ASSERT_EQ(ret, RET_ERR);
    RemovePermission();
}

/**
 * @tc.name: TestConfigTcpAlive
 * @tc.desc: Test ConfigTcpAlive
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DsoftbusAdapterTest, ConfigTcpAlive, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    std::shared_ptr<DSoftbusAdapter>dSoftbusAdapter = std::make_shared<DSoftbusAdapter>();
    DSoftbusAdapterImpl::GetInstance()->ConfigTcpAlive(SOCKET);
    RemovePermission();
}

/**
 * @tc.name: TestInitSocket
 * @tc.desc: Test InitSocket
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DsoftbusAdapterTest, InitSocket, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    std::shared_ptr<DSoftbusAdapter>dSoftbusAdapter = std::make_shared<DSoftbusAdapter>();
    char name[DEVICE_NAME_SIZE_MAX] {};
    char peerName[DEVICE_NAME_SIZE_MAX] { SERVER_SESSION_NAME };
    char peerNetworkId[PKG_NAME_SIZE_MAX] {};
    char pkgName[PKG_NAME_SIZE_MAX] { FI_PKG_NAME };
    SocketInfo info {
        .name = name,
        .peerName = peerName,
        .peerNetworkId = peerNetworkId,
        .pkgName = pkgName,
        .dataType = DATA_TYPE_BYTES
    };
    int32_t socket = 1;
    int32_t ret = DSoftbusAdapterImpl::GetInstance()->InitSocket(info, SOCKET_CLIENT, socket);
    ASSERT_EQ(ret, RET_ERR);
    ret = DSoftbusAdapterImpl::GetInstance()->InitSocket(info, SOCKET_SERVER, socket);
    ASSERT_EQ(ret, RET_ERR);
    RemovePermission();
}

/**
 * @tc.name: TestOnBind
 * @tc.desc: Test OnBind
 * @tc.type: FUNC
 * @tc.require:
 */
 HWTEST_F(DsoftbusAdapterTest, OnBind, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    PeerSocketInfo info;
    char deviceId[] = "softbus";
    info.networkId = deviceId;
    DSoftbusAdapterImpl::GetInstance()->OnBind(SOCKET, info);
    DSoftbusAdapterImpl::GetInstance()->OnShutdown(SOCKET, SHUTDOWN_REASON_UNKNOWN);
    RemovePermission();
}

/**
 * @tc.name: TestOnBytes
 * @tc.desc: Test OnBytes
 * @tc.type: FUNC
 * @tc.require:
 */
 HWTEST_F(DsoftbusAdapterTest, OnBytes, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    int32_t *data = new int32_t(SOCKET);
    DSoftbusAdapterImpl::GetInstance()->OnBytes(SOCKET, data, sizeof(data));
    RemovePermission();
}

/**
 * @tc.name: TestHandleSessionData
 * @tc.desc: Test HandleSessionData
 * @tc.type: FUNC
 * @tc.require:
 */
 HWTEST_F(DsoftbusAdapterTest, HandleSessionData, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    CircleStreamBuffer circleBuffer;
    DSoftbusAdapterImpl::GetInstance()->HandleSessionData(NetworkId, circleBuffer);
    RemovePermission();
}
/**
 * @tc.name: TestHandleRawData
 * @tc.desc: Test HandleRawData
 * @tc.type: FUNC
 * @tc.require:
 */
 HWTEST_F(DsoftbusAdapterTest, HandleRawData, TestSize.Level1)
{
    CALL_TEST_DEBUG;
    SetPermission(SYSTEM_CORE, g_cores, sizeof(g_cores) / sizeof(g_cores[0]));
    CircleStreamBuffer circleBuffer;
    int32_t *data = new int32_t(SOCKET);
    DSoftbusAdapterImpl::GetInstance()->HandleRawData(NetworkId, data, sizeof(data));
    RemovePermission();
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS