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

#include "stream_buffer.h"

#include <algorithm>
#include <vector>

namespace OHOS {
namespace Msdp {
StreamBuffer::StreamBuffer(const StreamBuffer &buf)
{
    Clone(buf);
}

StreamBuffer &StreamBuffer::operator=(const StreamBuffer &other)
{
    Clone(other);
    return *this;
}

void StreamBuffer::Reset()
{
    reset(&rustStreamBuffer_);
}

void StreamBuffer::Clean()
{
    clean(&rustStreamBuffer_);
}

bool StreamBuffer::Read(std::string &buf)
{
    if (rustStreamBuffer_.rPos_ == rustStreamBuffer_.wPos_) {
        FI_HILOGE("Not enough memory to read, errCode:%{public}d", STREAM_BUF_READ_FAIL);
        rustStreamBuffer_.rwErrorStatus_ = ErrorStatus::ERROR_STATUS_Read;
        return false;
    }
    buf = ReadBuf();
    rustStreamBuffer_.rPos_ += static_cast<int32_t>(buf.length()) + 1;
    return (buf.length() > 0);
}

bool StreamBuffer::Write(const std::string &buf)
{
    return Write(buf.c_str(), buf.length()+1);
}

bool StreamBuffer::Read(StreamBuffer &buf)
{
    return read_streambuffer(&rustStreamBuffer_, &buf.rustStreamBuffer_);
}

bool StreamBuffer::Write(const StreamBuffer &buf)
{
    return write_streambuffer(&rustStreamBuffer_, &buf.rustStreamBuffer_);
}

bool StreamBuffer::Read(char *buf, size_t size)
{
    return read_char_usize(&rustStreamBuffer_, buf, size);
}

const char *StreamBuffer::ReadBuf() const
{
    return read_buf(&rustStreamBuffer_);
}

bool StreamBuffer::Write(const char *buf, size_t size)
{
    return write_char_usize(&rustStreamBuffer_, buf, size);
}

bool StreamBuffer::Clone(const StreamBuffer &buf)
{
    Clean();
    return Write(data(&buf.rustStreamBuffer_), size(&buf.rustStreamBuffer_));
}
} // namespace Msdp
} // namespace OHOS
