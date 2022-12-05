/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef STOP_D_INPUT_CALL_BACK_STUB_H
#define STOP_D_INPUT_CALL_BACK_STUB_H
namespace OHOS {
namespace DistributedHardware {
namespace DistributedInput {
class StopDInputCallbackStub : public IStopDInputCallback{
public:
    StopDInputCallbackStub() = default;
    virtual void OnResult(const std::string &devId, const uint32_t &inputTypes, const int32_t &status);
};
}
}
}
#endif