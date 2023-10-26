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

#include "virtual_mouse_builder.h"

#include <getopt.h>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include <linux/input.h>

#include "input_manager.h"

#include "devicestatus_define.h"
#include "fi_log.h"
#include "json.h"
#include "utility.h"
#include "virtual_mouse.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "VirtualMouseBuilder" };
constexpr int32_t MAXIMUM_LEVEL_ALLOWED { 3 };
constexpr uint32_t IO_FLAG_WIDTH { 6 };
const std::unordered_map<std::string, int32_t> mouseBtns {
    { "BTN_LEFT", BTN_LEFT }, { "BTN_RIGHT", BTN_RIGHT },
    { "BTN_MIDDLE", BTN_MIDDLE }, { "BTN_SIDE", BTN_SIDE },
    { "BTN_EXTRA", BTN_EXTRA }, { "BTN_FORWARD", BTN_FORWARD },
    { "BTN_BACK", BTN_BACK }, { "BTN_TASK", BTN_TASK } };
} // namespace

VirtualMouseBuilder::VirtualMouseBuilder() : VirtualDeviceBuilder(GetDeviceName(), BUS_USB, 0x93a, 0x2510)
{
    eventTypes_ = { EV_KEY, EV_REL, EV_MSC };
    keys_ = { BTN_LEFT, BTN_RIGHT, BTN_MIDDLE, BTN_SIDE, BTN_EXTRA, BTN_FORWARD, BTN_BACK, BTN_TASK };
    relBits_ = { REL_X, REL_Y, REL_WHEEL, REL_WHEEL_HI_RES };
    miscellaneous_ = { MSC_SCAN };
}

class MouseEventMonitor final : public MMI::IInputEventConsumer {
public:
    MouseEventMonitor() = default;
    ~MouseEventMonitor() = default;

    void OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const override {};
    void OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const override;
    void OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const override {};
};

void MouseEventMonitor::OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    CHKPV(pointerEvent);
    if (pointerEvent->GetSourceType() != MMI::PointerEvent::SOURCE_TYPE_MOUSE) {
        return;
    }
    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
        return;
    }
    std::cout << "\rcurrent pointer position - x: " << std::setw(IO_FLAG_WIDTH) << std::left <<
        pointerItem.GetDisplayX() << "y: " << pointerItem.GetDisplayY() << "            ";
    std::cout.flush();
}

std::string VirtualMouseBuilder::GetDeviceName()
{
    return std::string("Virtual Mouse");
}

void VirtualMouseBuilder::ShowUsage()
{
    std::cout << "Usage: vdevadm act -t M [-d <mouse-button>] [-u <mouse-button>] [-s <dv>]" << std::endl;
    std::cout << "          [-m <dx> [<dy>]] [-M <x> <y>] [-w <ms>] [-f <FILE>] [-r <FILE>]" << std::endl;
    std::cout << "      -d <mouse-button>" << std::endl;
    std::cout << "                  Down the <mouse-button>" << std::endl;
    std::cout << "      -u <mouse-button>" << std::endl;
    std::cout << "                  Up the <mouse-button>" << std::endl;
    std::cout << "      -s <dy>     Scroll the mouse wheel" << std::endl;
    std::cout << "      -m <dx> [<dy>]" << std::endl;
    std::cout << "                  Move the mouse along <dx, dy>; if <dy> is missing, then set dy=dx" << std::endl;
    std::cout << "      -M <x> <y>  Move the pointer to <x, y>" << std::endl;
    std::cout << "      -D <SLOT> <sx> <sy> <tx> <ty> Drag the touch <SLOT> to (tx, ty)" << std::endl;
    std::cout << "      -w <ms>     Wait for <ms> milliseconds." << std::endl;
    std::cout << "      -f <FILE>   Read actions from <FILE>" << std::endl;
    std::cout << "      -r <FILE>   Read raw input data from <FILE>." << std::endl;
    std::cout << std::endl;
    std::cout << "          <mouse-button> can be:" << std::endl;
    std::cout << "              L   For left mouse button" << std::endl;
    std::cout << "              R   For right mouse button" << std::endl;
    std::cout << "              M   For middle mouse button" << std::endl;
}

void VirtualMouseBuilder::Mount()
{
    CALL_DEBUG_ENTER;
    std::cout << "Start to mount virtual mouse." << std::endl;
    if (VirtualMouse::GetDevice() != nullptr) {
        std::cout << "Virtual mouse has been mounted." << std::endl;
        return;
    }
    VirtualMouseBuilder vMouse;
    if (!vMouse.SetUp()) {
        std::cout << "Failed to mount virtual mouse." << std::endl;
        return;
    }

    int32_t nTries = 6;
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
    } while ((nTries-- > 0) && (VirtualMouse::GetDevice() == nullptr));
    if (VirtualMouse::GetDevice() == nullptr) {
        std::cout << "Failed to mount virtual mouse." << std::endl;
        return;
    }

    std::cout << "Mount virtual mouse successfully." << std::endl;
    VirtualDeviceBuilder::Daemonize();

    for (;;) {
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}

void VirtualMouseBuilder::Unmount()
{
    CALL_DEBUG_ENTER;
    VirtualDeviceBuilder::Unmount("mouse", "M");
}

void VirtualMouseBuilder::Clone()
{
    CALL_DEBUG_ENTER;
    if (VirtualMouse::GetDevice() != nullptr) {
        std::cout << "Virtual mouse has been mounted." << std::endl;
        return;
    }

    std::vector<std::shared_ptr<VirtualDevice>> vDevs;
    int32_t ret = VirtualDeviceBuilder::ScanFor(
        [](std::shared_ptr<VirtualDevice> vDev) { return ((vDev != nullptr) && vDev->IsMouse()); }, vDevs);
    if (ret != RET_OK) {
        std::cout << "Failed while scanning for mouse." << std::endl;
        return;
    }
    auto vDev = VirtualDeviceBuilder::Select(vDevs, "mouse");
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
    } while ((nTries-- > 0) && (VirtualMouse::GetDevice() == nullptr));
    if (VirtualMouse::GetDevice() == nullptr) {
        std::cout << "Failed to clone \' " << vDev->GetName() << " \'." << std::endl;
        return;
    }

    std::cout << "Clone \'" << vDev->GetName() << "\' successfully." << std::endl;
    VirtualDeviceBuilder::Daemonize();
    for (;;) {
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}

void VirtualMouseBuilder::Monitor()
{
    CALL_DEBUG_ENTER;
    MMI::InputManager *inputMgr = MMI::InputManager::GetInstance();
    CHKPV(inputMgr);
    auto monitor = std::make_shared<MouseEventMonitor>();
    int32_t monitorId = inputMgr->AddMonitor(monitor);
    if (monitorId < 0) {
        std::cout << "Failed to add monitor." << std::endl;
        return;
    }
    for (;;) {
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}

void VirtualMouseBuilder::Act(int32_t argc, char *argv[])
{
    CALL_DEBUG_ENTER;
    int32_t opt = getopt(argc, argv, "d:u:s:m:M:f:r:w:D:");
    if (opt < 0) {
        std::cout << "Vdevadm act: required option is missing" << std::endl;
        VirtualMouseBuilder::ShowUsage();
        return;
    }
    if (VirtualMouse::GetDevice() == nullptr) {
        std::cout << "No virtual mouse." << std::endl;
        return;
    }
    do {
        {
            auto action = regularActions_.find(opt);
            if (action != regularActions_.end()) {
                action->second();
                continue;
            }
        }
        {
            auto action = readActions_.find(opt);
            if (action != readActions_.end()) {
                action->second(optarg);
                continue;
            }
        }
        {
            auto action = moveActions_.find(opt);
            if (action != moveActions_.end()) {
                action->second(argc, argv);
                continue;
            }
        }
        if (opt == 'w') {
            VirtualDeviceBuilder::WaitFor(optarg, "mouse");
        } else {
            ShowUsage();
        }
    } while ((opt = getopt(argc, argv, "d:u:s:m:M:f:r:w:D:")) >= 0);
}

void VirtualMouseBuilder::ReadDownAction()
{
    CALL_DEBUG_ENTER;
    CHKPV(optarg);

    if (strcmp(optarg, "L") == 0) {
        std::cout << "[mouse] down button: BTN_LEFT" << std::endl;
        VirtualMouse::GetDevice()->DownButton(BTN_LEFT);
    } else if (strcmp(optarg, "M") == 0) {
        std::cout << "[mouse] down button: BTN_MIDDLE" << std::endl;
        VirtualMouse::GetDevice()->DownButton(BTN_MIDDLE);
    } else if (strcmp(optarg, "R") == 0) {
        std::cout << "[mouse] down button: BTN_RIGHT" << std::endl;
        VirtualMouse::GetDevice()->DownButton(BTN_RIGHT);
    } else {
        std::cout << "Invalid argument for option \'-d\'." << std::endl;
        ShowUsage();
    }
}

void VirtualMouseBuilder::ReadMoveAction(int32_t argc, char *argv[])
{
    CALL_DEBUG_ENTER;
    CHKPV(optarg);
    if (!Utility::IsInteger(std::string(optarg)) || (optind < 0) || (optind >= argc)) {
        std::cout << "Invalid arguments for Option \'-m\'." << std::endl;
        ShowUsage();
        return;
    }
    int32_t dx = std::atoi(optarg);
    int32_t dy = dx;

    if ((argv[optind] != nullptr) && Utility::IsInteger(std::string(argv[optind]))) {
        dy = std::atoi(argv[optind++]);
    }
    std::cout << "[mouse] move: (" << dx << "," << dy << ")" << std::endl;
    VirtualMouse::GetDevice()->MoveProcess(dx, dy);
}

void VirtualMouseBuilder::ReadMoveToAction(int32_t argc, char *argv[])
{
    CALL_DEBUG_ENTER;
    CHKPV(optarg);

    if (!Utility::IsInteger(optarg) || (optind < 0) || (optind >= argc) || !Utility::IsInteger(argv[optind])) {
        std::cout << "Invalid arguments for Option \'-M\'." << std::endl;
        ShowUsage();
        return;
    }
    int32_t x = std::atoi(optarg);
    int32_t y = std::atoi(argv[optind]);
    std::cout << "[mouse] move-to (" << x << "," << y << ")" << std::endl;
    VirtualMouse::GetDevice()->MoveTo(x, y);
    while ((optind < argc) && Utility::IsInteger(argv[optind])) {
        optind++;
    }
}

void VirtualMouseBuilder::ReadDragToAction(int32_t argc, char *argv[])
{
    CALL_DEBUG_ENTER;
    CHKPV(optarg);
    if (!Utility::IsInteger(optarg) || (optind < 0) || (optind >= argc) || !Utility::IsInteger(argv[optind])) {
        std::cout << "Invalid arguments for Option \'-D\'." << std::endl;
        ShowUsage();
        return;
    }
    int32_t x = std::atoi(optarg);
    int32_t y = std::atoi(argv[optind]);

    std::cout << "[mouse] drag-to (" << x << "," << y << ")" << std::endl;
    VirtualMouse::GetDevice()->DownButton(BTN_LEFT);
    VirtualDeviceBuilder::WaitFor("mouse", SLEEP_TIME);
    VirtualMouse::GetDevice()->MoveTo(x, y);
    VirtualMouse::GetDevice()->UpButton(BTN_LEFT);
    while ((optind < argc) && Utility::IsInteger(argv[optind])) {
        optind++;
    }
}

void VirtualMouseBuilder::ReadUpAction()
{
    CALL_DEBUG_ENTER;
    CHKPV(optarg);

    if (strcmp(optarg, "L") == 0) {
        std::cout << "[mouse] up button: BTN_LEFT" << std::endl;
        VirtualMouse::GetDevice()->UpButton(BTN_LEFT);
    } else if (strcmp(optarg, "M") == 0) {
        std::cout << "[mouse] up button: BTN_MIDDLE" << std::endl;
        VirtualMouse::GetDevice()->UpButton(BTN_MIDDLE);
    } else if (strcmp(optarg, "R") == 0) {
        std::cout << "[mouse] up button: BTN_RIGHT" << std::endl;
        VirtualMouse::GetDevice()->UpButton(BTN_RIGHT);
    } else {
        std::cout << "Invalid argument for option \'-u\'." << std::endl;
        ShowUsage();
    }
}

void VirtualMouseBuilder::ReadScrollAction()
{
    CALL_DEBUG_ENTER;
    CHKPV(optarg);
    if (!Utility::IsInteger(std::string(optarg))) {
        std::cout << "Invalid arguments for Option \'-s\'." << std::endl;
        ShowUsage();
        return;
    }
    int32_t dy = std::atoi(optarg);
    std::cout << "[mouse] scroll: " << dy << std::endl;
    VirtualMouse::GetDevice()->Scroll(dy);
}

void VirtualMouseBuilder::ReadActions(const char *path)
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

void VirtualMouseBuilder::ReadModel(const std::shared_ptr<Json> &model, int32_t level)
{
    CALL_DEBUG_ENTER;
    if (model->IsObject() && model->HasItem("actions")) {
        std::vector<std::shared_ptr<Json>> actions = model->GetArrayItems("actions");
        for (const auto &action : actions) {
            ReadAction(action);
        }
    } else if (model->IsArray() && (level > 0)) {
        for (auto m : model->GetArrayItems()) {
            ReadModel(m, level - 1);
        }
    }
}

void VirtualMouseBuilder::ReadAction(const std::shared_ptr<Json> &model)
{
    CALL_DEBUG_ENTER;
    if (!model->IsObject()) {
        return;
    }
    auto it = model->GetItem("action");
    if (!it->IsNull() && it->IsString()) {
        static const std::unordered_map<std::string, std::function<void (const std::shared_ptr<Json> &model)>> actions {
            { "down", &HandleDown },
            { "move", &HandleMove },
            { "up", &HandleUp },
            { "scroll", &HandleScroll },
            { "wait", &HandleWait }
        };
        auto actionItr = actions.find(it->StringValue());
        if (actionItr != actions.cend()) {
            actionItr->second(model);
        }
    }
}

void VirtualMouseBuilder::HandleDown(const std::shared_ptr<Json> &model)
{
    if (!model->HasItem("button")) {
        return;
    }
    auto it = model->GetItem("button");
    if (!it->IsNull() && it->IsString()) {
        auto tIter = mouseBtns.find(it->StringValue());
        if (tIter != mouseBtns.cend()) {
            std::cout << "[mouse] down button: " << tIter->first << std::endl;
            VirtualMouse::GetDevice()->DownButton(tIter->second);
        }
    }
}

void VirtualMouseBuilder::HandleMove(const std::shared_ptr<Json> &model)
{
    int32_t dx = 0;
    int32_t dy = 0;
    if (!model->HasItem("dx") || !model->HasItem("dy")) {
        return;
    }
    auto it = model->GetItem("dx");
    if (!it->IsNull() && it->IsNumber()) {
        dx = it->IntValue();
    }

    it = model->GetItem("dy");
    if (!it->IsNull() && it->IsNumber()) {
        dy = it->IntValue();
    }

    std::cout << "[mouse] move: (" << dx << "," << dy << ")" << std::endl;
    VirtualMouse::GetDevice()->Move(dx, dy);
}

void VirtualMouseBuilder::HandleUp(const std::shared_ptr<Json> &model)
{
    if (!model->HasItem("button")) {
        return;
    }
    auto it = model->GetItem("button");
    if (!it->IsNull() && it->IsString()) {
        auto tIter = mouseBtns.find(it->StringValue());
        if (tIter != mouseBtns.cend()) {
            std::cout << "[mouse] up button: " << tIter->first << std::endl;
            VirtualMouse::GetDevice()->UpButton(tIter->second);
        }
    }
}

void VirtualMouseBuilder::HandleScroll(const std::shared_ptr<Json> &model)
{
    CALL_DEBUG_ENTER;
    if (!model->HasItem("dy")) {
        return;
    }
    auto it = model->GetItem("dy");
    if (!it->IsNull() && it->IsNumber()) {
        std::cout << "[mouse] scroll: " << it->IntValue() << std::endl;
        VirtualMouse::GetDevice()->Scroll(it->IntValue());
    }
}

void VirtualMouseBuilder::HandleWait(const std::shared_ptr<Json> &model)
{
    if (!model->HasItem("duration")) {
        return;
    }
    auto it = model->GetItem("duration");
    if (!it->IsNull() && it->IsNumber()) {
        int32_t waitTime = it->IntValue();
        std::cout << "[mouse] wait for " << waitTime << " milliseconds" << std::endl;
        VirtualDeviceBuilder::WaitFor("virtual mouse", waitTime);
    }
}

void VirtualMouseBuilder::ReadRawInput(const char *path)
{
    CALL_DEBUG_ENTER;
    CHKPV(path);
    std::shared_ptr<Json> model = Json::Load(path);
    CHKPV(model);
    if (!model->IsObject()) {
        FI_HILOGE("Failed to load the file");
        return;
    }
    ReadRawModel(model, MAXIMUM_LEVEL_ALLOWED);
}

void VirtualMouseBuilder::ReadRawModel(const std::shared_ptr<Json> &model, int32_t level)
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
            for (const auto& item : it->GetArrayItems()) {
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

void VirtualMouseBuilder::ReadRawData(const std::shared_ptr<Json> &model)
{
    CALL_DEBUG_ENTER;
    if (!model->IsObject()) {
        FI_HILOGE("model is not an object");
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
    std::cout << "[mouse] raw input: [" << typeIter->IntValue() << ", " << codeIter->IntValue() << ", " <<
        valueIter->IntValue() << "]" << std::endl;
    VirtualMouse::GetDevice()->SendEvent(typeIter->IntValue(), codeIter->IntValue(), valueIter->IntValue());
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS