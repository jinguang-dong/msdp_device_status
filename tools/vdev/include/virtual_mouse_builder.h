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

#ifndef VIRTUAL_MOUSE_BUILDER_H
#define VIRTUAL_MOUSE_BUILDER_H

#include <memory>

#include "virtual_device_builder.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class VirtualMouseBuilder final : public VirtualDeviceBuilder {
public:
    VirtualMouseBuilder();
    ~VirtualMouseBuilder() = default;
    DISALLOW_COPY_AND_MOVE(VirtualMouseBuilder);

    static std::string GetDeviceName();
    static void ShowUsage();
    static void Mount();
    static void Unmount();
    static void Clone();
    static void Monitor();
    static void Act(int32_t argc, char *argv[]);

private:
    static void ReadActions(const char *path);
    static void ReadModel(const std::shared_ptr<Json> &model, int32_t level);
    static void ReadDownAction();
    static void ReadMoveAction(int32_t argc, char *argv[]);
    static void ReadMoveToAction(int32_t argc, char *argv[]);
    static void ReadDragToAction(int32_t argc, char *argv[]);
    static void ReadUpAction();
    static void ReadScrollAction();
    static void ReadAction(const std::shared_ptr<Json> &model);
    static void HandleDown(const std::shared_ptr<Json> &model);
    static void HandleMove(const std::shared_ptr<Json> &model);
    static void HandleUp(const std::shared_ptr<Json> &model);
    static void HandleScroll(const std::shared_ptr<Json> &model);
    static void HandleWait(const std::shared_ptr<Json> &model);
    static void ReadRawInput(const char *path);
    static void ReadRawModel(const std::shared_ptr<Json> &model, int32_t level);
    static void ReadRawData(const std::shared_ptr<Json> &model);
    using InterfaceParameterLess = void(*)();
    using InterfaceParameterOne = void(*)(const char*);
    using InterfaceParameterTwo = void(*)(int32_t, char**);
    inline static std::map<const char, InterfaceParameterLess> regularActions_ {
        { 'd', &ReadDownAction }, { 'u', &ReadUpAction }, { 's', &ReadScrollAction }
    };
    inline static std::map<const char, InterfaceParameterOne> readActions_ {
        { 'f', &ReadActions }, { 'r', &ReadRawInput}
    };
    inline static std::map<const char, InterfaceParameterTwo>  moveActions_{
        { 'm', &ReadMoveAction }, { 'M', &ReadMoveToAction }, { 'D', &ReadDragToAction }
    };
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif  // VIRTUAL_MOUSE_BUILDER_H