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

#include "virtual_keyboard_builder.h"

#include <getopt.h>
#include <fstream>
#include <iostream>

#include "devicestatus_define.h"
#include "fi_log.h"
#include "json.h"
#include "utility.h"
#include "virtual_keyboard.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "VirtualKeyboardBuilder" };
constexpr int32_t MAXIMUM_LEVEL_ALLOWED { 3 };
} // namespace

VirtualKeyboardBuilder::VirtualKeyboardBuilder() : VirtualDeviceBuilder(GetDeviceName(), BUS_USB, 0x24ae, 0x4035)
{
    eventTypes_ = { EV_KEY, EV_MSC, EV_LED, EV_REP };
    miscellaneous_ = { MSC_SCAN };
    leds_ = { LED_NUML, LED_CAPSL, LED_SCROLLL, LED_COMPOSE, LED_KANA };
    repeats_ = { REP_DELAY, REP_PERIOD };
    keys_ = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
        61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
        81, 82, 83, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 102,
        103, 104, 105, 106, 107, 108, 109, 110, 111, 113, 114, 115, 116, 117, 119, 121, 122, 123, 124, 125,
        126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 140, 142, 150, 152, 158, 159, 161,
        163, 164, 165, 166, 173, 176, 177, 178, 179, 180, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
        193, 194, 240, 211, 213, 214, 215, 218, 220, 221, 222, 223, 226, 227, 231, 232, 233, 236, 237, 238,
        239, 242, 243, 245, 246, 247, 248, 464, 522, 523, 141, 145, 146, 147, 148, 149, 151, 153, 154, 157,
        160, 162, 170, 175, 182, 200, 201, 202, 203, 204, 205, 101, 112, 118, 120 };
}

std::string VirtualKeyboardBuilder::GetDeviceName()
{
    return std::string("Virtual Keyboard");
}

void VirtualKeyboardBuilder::ShowUsage()
{
    std::cout << "Usage: vdevadm act -t K [-d <key>] [-u <key>] [-w <ms>] [-f <FILE>] [-r <FILE>]" << std::endl;
    std::cout << "      -d <key>    Down <key>" << std::endl;
    std::cout << "      -u <key>    Up <key>" << std::endl;
    std::cout << "      -w <ms>     Wait for <ms> milliseconds." << std::endl;
    std::cout << "      -f <FILE>   Read actions from <FILE>" << std::endl;
    std::cout << "      -r <FILE>   Read raw input data from <FILE>." << std::endl;
    std::cout << std::endl;
}

void VirtualKeyboardBuilder::Mount()
{
    CALL_DEBUG_ENTER;
    std::cout << "Start to mount virtual keyboard." << std::endl;
    if (VirtualKeyboard::GetDevice() != nullptr) {
        std::cout << "Virtual keyboard has been mounted." << std::endl;
        return;
    }
    VirtualKeyboardBuilder vKeyboard;
    if (!vKeyboard.SetUp()) {
        std::cout << "Failed to mount virtual keyboard." << std::endl;
        return;
    }

    int32_t nTries = 6;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
    } while ((nTries-- > 0) && (VirtualKeyboard::GetDevice() == nullptr));
    if (VirtualKeyboard::GetDevice() == nullptr) {
        std::cout << "Failed to mount virtual keyboard." << std::endl;
        return;
    }

    std::cout << "Mount virtual keyboard successfully." << std::endl;
    VirtualDeviceBuilder::Daemonize();

    for (;;) {
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}

void VirtualKeyboardBuilder::Unmount()
{
    CALL_DEBUG_ENTER;
    VirtualDeviceBuilder::Unmount("keyboard", "K");
}

void VirtualKeyboardBuilder::Clone()
{
    CALL_DEBUG_ENTER;
    if (VirtualKeyboard::GetDevice() != nullptr) {
        std::cout << "Virtual keyboard has been mounted" << std::endl;
        return;
    }

    std::vector<std::shared_ptr<VirtualDevice>> vDevs;
    int32_t ret = VirtualDeviceBuilder::ScanFor(
        [](std::shared_ptr<VirtualDevice> vDev) { return ((vDev != nullptr) && vDev->IsKeyboard()); }, vDevs);
    if (ret != RET_OK) {
        std::cout << "Failed while scanning for keyboard" << std::endl;
        return;
    }
    auto vDev = VirtualDeviceBuilder::Select(vDevs, "keyboard");
    CHKPV(vDev);

    std::cout << "Cloning \'" << vDev->GetName() << "\'." << std::endl;
    VirtualDeviceBuilder vBuilder(GetDeviceName(), vDev);
    if (!vBuilder.SetUp()) {
        std::cout << "Failed to clone \' " << vDev->GetName() << " \'." << std::endl;
        return;
    }

    int32_t nTries = 3;
    do {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } while ((nTries-- > 0) && (VirtualKeyboard::GetDevice() == nullptr));
    if (VirtualKeyboard::GetDevice() == nullptr) {
        std::cout << "Failed to clone \' " << vDev->GetName() << " \'." << std::endl;
        return;
    }

    std::cout << "Clone \'" << vDev->GetName() << "\' successfully" << std::endl;
    VirtualDeviceBuilder::Daemonize();
    for (;;) {
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}

void VirtualKeyboardBuilder::Act(int32_t argc, char *argv[])
{
    CALL_DEBUG_ENTER;
    int32_t opt = getopt(argc, argv, "d:u:f:r:w:");
    if (opt < 0) {
        std::cout << "Vdevadm act: required option is missing" << std::endl;
        ShowUsage();
        return;
    }
    if (VirtualKeyboard::GetDevice() == nullptr) {
        std::cout << "No virtual keyboard." << std::endl;
        return;
    }
    do {
        switch (opt) {
            case 'd': {
                ReadDownAction();
                break;
            }
            case 'u': {
                ReadUpAction();
                break;
            }
            case 'f': {
                ReadActions(optarg);
                break;
            }
            case 'r': {
                ReadRawInput(optarg);
                break;
            }
            case 'w': {
                VirtualDeviceBuilder::WaitFor(optarg, "keyboard");
                break;
            }
            default: {
                ShowUsage();
                break;
            }
        }
    } while ((opt = getopt(argc, argv, "d:u:f:r:w:")) >= 0);
}

void VirtualKeyboardBuilder::ReadDownAction()
{
    CALL_DEBUG_ENTER;
    CHKPV(optarg);
    if (!Utility::IsInteger(optarg)) {
        std::cout << "Require arguments for Option \'-d\'." << std::endl;
        ShowUsage();
        return;
    }

    int32_t key = std::atoi(optarg);
    std::cout << "[keyboard] down key: [" << key << "]" << std::endl;
    VirtualKeyboard::GetDevice()->Down(key);
}

void VirtualKeyboardBuilder::ReadUpAction()
{
    CALL_DEBUG_ENTER;
    CHKPV(optarg);
    if (!Utility::IsInteger(optarg)) {
        std::cout << "Require arguments for Option \'-u\'." << std::endl;
        ShowUsage();
        return;
    }

    int32_t key = std::atoi(optarg);
    std::cout << "[keyboard] up key: [" << key << "]" << std::endl;
    VirtualKeyboard::GetDevice()->Up(key);
}

void VirtualKeyboardBuilder::ReadActions(const char *path)
{
    CALL_DEBUG_ENTER;
    CHKPV(path);
    std::shared_ptr<Json> model = Json::Load(path);
    CHKPV(model);
    if (!model->IsObject()) {
        FI_HILOGE("Failed to load the file");
        return;
    }
    ReadModel(model, MAXIMUM_LEVEL_ALLOWED);
}

void VirtualKeyboardBuilder::ReadModel(const std::shared_ptr<Json> &model, int32_t level)
{
    CALL_DEBUG_ENTER;
    if (!model->IsObject() && !model->IsArray()) {
        FI_HILOGE("model is not an array or object");
        return;
    }
    if (model->IsObject() && model->HasItem("actions")) {
        std::vector<std::shared_ptr<Json>> actions = model->GetArrayItems("actions");
        for (const auto &action : actions) {
            ReadAction(action);
        }
    }
    if (model->IsArray() && (level > 0)) {
        for (const auto &m : model->GetArrayItems()) {
            ReadModel(m, level - 1);
        }
    }
}

void VirtualKeyboardBuilder::ReadAction(const std::shared_ptr<Json> &model)
{
    CALL_DEBUG_ENTER;
    if (!model->IsObject()) {
        return;
    }
    auto it = model->GetItem("action");
    if (!it->IsNull() && it->IsString()) {
        static const std::unordered_map<std::string, std::function<void(const std::shared_ptr<Json> &model)>> actions {
            { "down", &VirtualKeyboardBuilder::HandleDown },
            { "up", &VirtualKeyboardBuilder::HandleUp },
            { "wait", &VirtualKeyboardBuilder::HandleWait }
        };
        auto actionItr = actions.find(it->StringValue());
        if (actionItr != actions.cend()) {
            actionItr->second(model);
        }
    }
}

void VirtualKeyboardBuilder::HandleDown(const std::shared_ptr<Json> &model)
{
    if (!model->HasItem("key")) {
        return;
    }
    auto it = model->GetItem("key");
    if (!it->IsNull() && it->IsNumber()) {
        std::cout << "[keyboard] down key: " << it->IntValue() << std::endl;
        VirtualKeyboard::GetDevice()->Down(it->IntValue());
    }
}

void VirtualKeyboardBuilder::HandleUp(const std::shared_ptr<Json> &model)
{
    if (!model->HasItem("key")) {
        return;
    }
    auto it = model->GetItem("key");
    if (!it->IsNull() && it->IsNumber()) {
        std::cout << "[keyboard] up key: " << it->IntValue() << std::endl;
        VirtualKeyboard::GetDevice()->Up(it->IntValue());
    }
}

void VirtualKeyboardBuilder::HandleWait(const std::shared_ptr<Json> &model)
{
    if (!model->HasItem("duration")) {
        return;
    }
    auto it = model->GetItem("duration");
    if (!it->IsNull() && it->IsNumber()) {
        int32_t waitTime = it->IntValue();
        std::cout << "[keyboard] wait for " << waitTime << " milliseconds" << std::endl;
        VirtualDeviceBuilder::WaitFor("virtual keyboard", waitTime);
    }
}

void VirtualKeyboardBuilder::ReadRawInput(const char *path)
{
    CALL_DEBUG_ENTER;
    CHKPV(path);
    std::shared_ptr<Json> model = Json::Load(path);
    CHKPV(model);
    if (!model->IsObject()) {
        FI_HILOGE("Failed to load the file");
    }
    ReadRawModel(model, MAXIMUM_LEVEL_ALLOWED);
}

void VirtualKeyboardBuilder::ReadRawModel(const std::shared_ptr<Json> &model, int32_t level)
{
    CALL_DEBUG_ENTER;
    if (!model->IsObject() && !model->IsArray()) {
        FI_HILOGE("model is not an array or object");
        return;
    }
    if (model->IsObject()) {
        auto it = model->GetItem("type");
        if (it->IsNull() || !it->IsString() || (it->StringValue().compare("raw") != 0)) {
            std::cout << "Expect raw input data." << std::endl;
            return;
        }
        it = model->GetItem("actions");
        if (!it->IsNull() && it->IsArray()) {
            for (const auto &item : it->GetArrayItems()) {
                ReadRawData(item);
            }
        }
    }
    if (model->IsArray() && (level > 0)) {
        for (const auto &m : model->GetArrayItems()) {
            ReadRawModel(m, level - 1);
        }
    }
}

void VirtualKeyboardBuilder::ReadRawData(const std::shared_ptr<Json> &model)
{
    CALL_DEBUG_ENTER;
    if (!model->IsObject()) {
        return;
    }

    auto typeIter = model->GetItem("type");
    if (typeIter->IsNull() || !typeIter->IsNumber()) {
        return;
    }

    auto codeIter = model->GetItem("code");
    if (codeIter->IsNull() || !codeIter->IsNumber()) {
        return;
    }

    auto valueIter = model->GetItem("value");
    if (valueIter->IsNull() || !valueIter->IsNumber()) {
        return;
    }

    std::cout << "[keyboard] raw input: [" << typeIter->IntValue() << ", " << codeIter->IntValue() << ", " <<
        valueIter->IntValue() << "]" << std::endl;
    VirtualKeyboard::GetDevice()->SendEvent(typeIter->IntValue(), codeIter->IntValue(), valueIter->IntValue());
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS