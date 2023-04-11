/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "stream_client.h"

#include "util.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "StreamClient" };
} // namespace

StreamClient::StreamClient()
{
    CALL_DEBUG_ENTER;
}

StreamClient::~StreamClient()
{
    CALL_DEBUG_ENTER;
}

int32_t StreamClient::ConnectTo()
{
    CALL_DEBUG_ENTER;
    if (Socket() < 0) {
        FI_HILOGE("Socket failed");
        return RET_ERR;
    }
    OnConnected();
    return RET_OK;
}

bool StreamClient::SendMsg(const char *buf, size_t size) const
{
    return send_msg_buf_size(&rustStreamSocket_, buf, size);
}

bool StreamClient::SendMsg(const NetPacket &pkt) const
{
    if (chk_rwerror(&pkt.rustStreamBuffer_)) {
        FI_HILOGE("Read and write status is error");
        return false;
    }
    StreamBuffer buf;
    pkt.MakeData(buf);
    return SendMsg(data(&buf.rustStreamBuffer_), size(&buf.rustStreamBuffer_));
}

bool StreamClient::StartClient(MsgClientFunCallback fun)
{
    CALL_DEBUG_ENTER;
    if (isRunning_ || isConnected_) {
        FI_HILOGE("Client is connected or started");
        return false;
    }
    hasClient_ = true;
    recvFun_ = fun;
    if (ConnectTo() < 0) {
        FI_HILOGW("Client connection failed, Try again later");
    }
    return true;
}

void StreamClient::Stop()
{
    CALL_DEBUG_ENTER;
    hasClient_ = false;
    Close();
}
}
} // namespace Msdp
} // namespace OHOS