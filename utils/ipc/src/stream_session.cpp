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

#include "stream_session.h"

#include <cinttypes>
#include <sstream>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "stream_socket.h"
#include "proto.h"

namespace OHOS {
namespace Msdp {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MSDP_DOMAIN_ID, "StreamSession" };
const std::string FOUNDATION = "foundation";
} // namespace

StreamSession::StreamSession(const std::string &programName, const int32_t moduleType, const int32_t fd,
    const int32_t uid, const int32_t pid)
    : programName_(programName)
{
    rustStreamSession_.moduleType_ = moduleType;
    rustStreamSession_.fd_ = fd;
    rustStreamSession_.uid_ = uid;
    rustStreamSession_.pid_ = pid;
    UpdateDescript();
}

bool StreamSession::SendMsg(const char *buf, size_t size) const
{
    return session_send_msg(&rustStreamSession_, buf, size);
}

void StreamSession::Close()
{
    session_close(&rustStreamSession_);
    UpdateDescript();
}

void StreamSession::UpdateDescript()
{
    std::ostringstream oss;
    oss << "fd = " << rustStreamSession_.fd_
        << ", programName = " << programName_
        << ", moduleType = " << rustStreamSession_.moduleType_
        << ((rustStreamSession_.fd_ < 0) ? ", closed" : ", opened")
        << ", uid = " << rustStreamSession_.uid_
        << ", pid = " << rustStreamSession_.pid_
        << ", tokenType = " << rustStreamSession_.tokenType_
        << std::endl;
    descript_ = oss.str().c_str();
}

bool StreamSession::SendMsg(NetPacket &pkt) const
{
    if (chk_rwerror(&pkt.rustStreamBuffer_)) {
        FI_HILOGE("Read and write status is error");
        return false;
    }
    StreamBuffer buf;
    pkt.MakeData(buf);
    return SendMsg(data(&buf.rustStreamBuffer_), size(&buf.rustStreamBuffer_));
}
} // namespace Msdp
} // namespace OHOS