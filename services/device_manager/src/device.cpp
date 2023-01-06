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

#include "device.h"

#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <regex>
#include <sstream>

#include <sys/ioctl.h>

#include <openssl/sha.h>

#include "devicestatus_define.h"
#include "devicestatus_errors.h"
#include "fi_log.h"
#include "napi_constants.h"
#include "utility.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
struct range {
    size_t start { 0 };
    size_t end { 0 };
};

namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, MSDP_DOMAIN_ID, "Device" };
const std::string INPUT_VIRTUAL_DEVICE_NAME { "DistributedInput " };
const std::string SPLIT_SYMBOL { "|" };
const std::string DH_ID_PREFIX { "Input_" };
const std::string CONFIG_ITEM_KEYBOARD_TYPE { "Key.keyboard.type" };
constexpr int32_t COMMENT_SUBSCRIPT { 0 };
constexpr std::uintmax_t MAX_FILE_SIZE_ALLOWED { 0x5000 };

const struct range KEY_BLOCKS[] {
    { KEY_ESC, BTN_MISC },
    { KEY_OK, BTN_DPAD_UP },
    { KEY_ALS_TOGGLE, BTN_TRIGGER_HAPPY }
};
} // namespace

Device::Device(int32_t deviceId)
    : deviceId_(deviceId)
{}

int32_t Device::Open()
{
    CALL_DEBUG_ENTER;
    char buf[PATH_MAX] {};
    if (realpath(devPath_.c_str(), buf) == nullptr) {
        FI_HILOGE("Not real path: %{public}s", devPath_.c_str());
        return RET_ERR;
    }

    int32_t nRetries { 6 };
    for (;;) {
        Utility::ShowUserAndGroup();
        Utility::ShowFileAttributes(buf);

        fd_ = open(buf, O_RDWR | O_NONBLOCK | O_CLOEXEC);
        if (fd_ < 0) {
            FI_HILOGE("Unable to open device \'%{public}s\': %{public}s", buf, strerror(errno));
            if (nRetries-- > 0) {
                static constexpr int32_t DEFAULT_WAIT_TIME { 500 };
                std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_WAIT_TIME));
                FI_HILOGI("Retry opening device \'%{public}s\'", buf);
            } else {
                return RET_ERR;
            }
        } else {
            FI_HILOGD("Opening \'%{public}s\' successfully", buf);
            break;
        }
    }
    QueryDeviceInfo();
    QuerySupportedEvents();
    UpdateCapability();
    Populate();
    LoadDeviceConfig();
    return RET_OK;
}

void Device::Close()
{
    CALL_DEBUG_ENTER;
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
}

void Device::Dispatch(const struct epoll_event &ev)
{
    if ((ev.events & EPOLLIN) == EPOLLIN) {
        FI_HILOGD("input data received");
    } else if ((ev.events & (EPOLLHUP | EPOLLERR)) != 0) {
        FI_HILOGE("Epoll hangup: %{public}s", strerror(errno));
    }
}

void Device::QueryDeviceInfo()
{
    CALL_DEBUG_ENTER;
    int32_t rc;
    char buffer[PATH_MAX];

    memset(buffer, 0, sizeof(buffer));
    rc = ioctl(fd_, EVIOCGNAME(sizeof(buffer) - 1), &buffer);
    if (rc < 0) {
        FI_HILOGE("Could not get device name: %{public}s", strerror(errno));
    } else {
        name_.assign(buffer);
    }

    struct input_id inputId;
    rc = ioctl(fd_, EVIOCGID, &inputId);
    if (rc < 0) {
        FI_HILOGE("Could not get device input id: %{public}s", strerror(errno));
    } else {
        bus_ = inputId.bustype;
        product_ = inputId.product;
        vendor_ = inputId.vendor;
        version_ = inputId.version;        
    }

    memset(buffer, 0, sizeof(buffer));
    rc = ioctl(fd_, EVIOCGPHYS(sizeof(buffer) - 1), &buffer);
    if (rc < 0) {
        FI_HILOGE("Could not get location: %{public}s", strerror(errno));
    } else {
        phys_.assign(buffer);
    }

    memset(buffer, 0, sizeof(buffer));
    rc = ioctl(fd_, EVIOCGUNIQ(sizeof(buffer) - 1), &buffer);
    if (rc < 0) {
        FI_HILOGE("Could not get uniq: %{public}s", strerror(errno));
    } else {
        uniq_.assign(buffer);
    }
}

void Device::QuerySupportedEvents()
{
    CALL_DEBUG_ENTER;
    int32_t rc = ioctl(fd_, EVIOCGBIT(0, sizeof(evBitmask_)), evBitmask_);
    if (rc < 0) {
        FI_HILOGE("Could not get events mask: %{public}s", strerror(errno));
    }
    rc = ioctl(fd_, EVIOCGBIT(EV_KEY, sizeof(keyBitmask_)), keyBitmask_);
    if (rc < 0) {
        FI_HILOGE("Could not get key events mask: %{public}s", strerror(errno));
    }
    rc = ioctl(fd_, EVIOCGBIT(EV_ABS, sizeof(absBitmask_)), absBitmask_);
    if (rc < 0) {
        FI_HILOGE("Could not get abs events mask: %{public}s", strerror(errno));
    }
    rc = ioctl(fd_, EVIOCGBIT(EV_REL, sizeof(relBitmask_)), relBitmask_);
    if (rc < 0) {
        FI_HILOGE("Could not get rel events mask: %{public}s", strerror(errno));
    }
    rc = ioctl(fd_, EVIOCGPROP(sizeof(propBitmask_)), propBitmask_);
    if (rc < 0) {
        FI_HILOGE("Could not get properties mask: %{public}s", strerror(errno));
    }
}

void Device::Populate()
{
    CALL_DEBUG_ENTER;
#ifdef OHOS_BUILD_ENABLE_COORDINATION
    if (IsRemote()) {
        networkId_ = MakeNetworkId(GetPhys());
    }
    dhid_ = GenerateDescriptor();
#endif // OHOS_BUILD_ENABLE_COORDINATION
}

void Device::UpdateCapability()
{
    CALL_DEBUG_ENTER;
    CheckPointers();
    CheckKeys();
}

void Device::CheckPointers()
{
    CALL_DEBUG_ENTER;
    bool hasKeys { TestBit(EV_KEY, evBitmask_) };
    bool hasAbs { TestBit(EV_ABS, evBitmask_) };
    bool hasRels { TestBit(EV_REL, evBitmask_) };
    bool hasAbsCoords { TestBit(ABS_X, absBitmask_) &&
                        TestBit(ABS_Y, absBitmask_) };
    bool hasMtCoords { TestBit(ABS_MT_POSITION_X, absBitmask_) &&
                       TestBit(ABS_MT_POSITION_Y, absBitmask_) };
    bool isDirect { TestBit(INPUT_PROP_DIRECT, propBitmask_) };
    bool hasTouch { TestBit(BTN_TOUCH, keyBitmask_) };
    bool hasRelCoords { TestBit(REL_X, relBitmask_) &&
                        TestBit(REL_Y, relBitmask_) };
    bool stylusOrPen { TestBit(BTN_STYLUS, keyBitmask_) ||
                       TestBit(BTN_TOOL_PEN, keyBitmask_) };

    if (hasAbs) {
        if (hasAbsCoords) {
            if (hasKeys && stylusOrPen) {
                caps_.set(INPUT_DEV_CAP_TABLET_TOOL);
                FI_HILOGD("This is tablet tool");
            }
        }
        if (hasMtCoords) {
            if (hasTouch || isDirect) {
                caps_.set(INPUT_DEV_CAP_TOUCH);
                FI_HILOGD("This is touch device");
            }
        }
    } else if (hasRels) {
        if (hasRelCoords) {
            caps_.set(INPUT_DEV_CAP_POINTER);
            FI_HILOGD("This is pointer device");
        }
    }
}

void Device::CheckKeys()
{
    CALL_DEBUG_ENTER;
    if (!TestBit(EV_KEY, evBitmask_)) {
        FI_HILOGD("No EV_KEY capability");
        return;
    }
    for (size_t block { 0U }; block < (sizeof(KEY_BLOCKS) / sizeof(struct range)); ++block) {
        for (size_t key = KEY_BLOCKS[block].start; key < KEY_BLOCKS[block].end; ++key) {
            if (TestBit(key, keyBitmask_)) {
                FI_HILOGD("Found key %{public}zx", key);
                caps_.set(INPUT_DEV_CAP_KEYBOARD);
                return;
            }
        }
    }
}

#ifdef OHOS_BUILD_ENABLE_COORDINATION

bool Device::IsRemote() const
{
    return (GetName().find(INPUT_VIRTUAL_DEVICE_NAME) != std::string::npos);
}

std::string Device::MakeNetworkId(const std::string &phys) const
{
    std::vector<std::string> strList;
    StringSplit(phys, SPLIT_SYMBOL, strList);
    if (strList.size() == 3) {
        return strList[1];
    }
    return EMPTYSTR;
}

std::string Device::GenerateDescriptor() const
{
    const std::string phys = GetPhys();
    std::string descriptor;
    if (IsRemote() && !phys.empty()) {
        FI_HILOGI("physicalPath:%{public}s", phys.c_str());
        std::vector<std::string> strList;
        StringSplit(phys.c_str(), SPLIT_SYMBOL, strList);
        if (strList.size() == 3) {
            descriptor = strList[2];
        }
        return descriptor;
    }

    int32_t vendor = GetVendor();
    const std::string name = GetName();
    const std::string uniq = GetUniq();
    int32_t product = GetProduct();
    std::string rawDescriptor = StringPrintf(":%04x:%04x:", vendor, product);

    // add handling for USB devices to not unique kbs that show up twice
    if (!uniq.empty()) {
        rawDescriptor += "uniqueId:" + uniq;
    }
    if (!phys.empty()) {
        rawDescriptor += "physicalPath:" + phys;
    }
    if (!name.empty()) {
        rawDescriptor += "name:" + std::regex_replace(name, std::regex(" "), "");
    }
    descriptor = DH_ID_PREFIX + Sha256(rawDescriptor);
    FI_HILOGI("Created descriptor raw: %{public}s", rawDescriptor.c_str());
    return descriptor;
}

std::string Device::Sha256(const std::string &in) const
{
    unsigned char out[SHA256_DIGEST_LENGTH * 2 + 1] = {0};
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, in.data(), in.size());
    SHA256_Final(&out[SHA256_DIGEST_LENGTH], &ctx);
    // here we translate sha256 hash to hexadecimal. each 8-bit char will be presented by two characters([0-9a-f])
    constexpr int32_t WIDTH = 4;
    constexpr unsigned char MASK = 0x0F;
    const char* hexCode = "0123456789abcdef";
    constexpr int32_t DOUBLE_TIMES = 2;
    for (int32_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        unsigned char value = out[SHA256_DIGEST_LENGTH + i];
        // uint8_t is 2 digits in hexadecimal.
        out[i * DOUBLE_TIMES] = hexCode[(value >> WIDTH) & MASK];
        out[i * DOUBLE_TIMES + 1] = hexCode[value & MASK];
    }
    out[SHA256_DIGEST_LENGTH * DOUBLE_TIMES] = 0;
    return reinterpret_cast<char*>(out);
}

#endif // OHOS_BUILD_ENABLE_COORDINATION

void Device::RemoveSpace(std::string &str) const
{
    str.erase(remove_if(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c);}), str.end());
}

std::string Device::MakeConfigFileName() const
{
    std::ostringstream ss;
    ss << GetVendor() << "_" << GetProduct() << "_" << GetVersion() << "_" << GetName();
    std::string fname { ss.str() };
    RemoveSpace(fname);

    std::ostringstream sp;
    sp << "/vendor/etc/keymap/" << fname << ".TOML";
    return sp.str();
}

int32_t Device::ReadConfigFile(const std::string &filePath)
{
    CALL_DEBUG_ENTER;
    std::ifstream cfgFile(filePath);
    if (!cfgFile.is_open()) {
        FI_HILOGE("Failed to open config file");
        return FILE_OPEN_FAIL;
    }
    std::string tmp;
    while (std::getline(cfgFile, tmp)) {
        RemoveSpace(tmp);
        size_t pos = tmp.find('#');
        if (pos != tmp.npos && pos != COMMENT_SUBSCRIPT) {
            FI_HILOGE("File format is error");
            cfgFile.close();
            return RET_ERR;
        }
        if (tmp.empty() || tmp.front() == '#') {
            continue;
        }
        pos = tmp.find('=');
        if (pos == (tmp.size() - 1) || pos == tmp.npos) {
            FI_HILOGE("Find config item error");
            cfgFile.close();
            return RET_ERR;
        }
        std::string configItem = tmp.substr(0, pos);
        std::string value = tmp.substr(pos + 1);
        if (ConfigItemSwitch(configItem, value) == RET_ERR) {
            FI_HILOGE("Configuration item error");
            cfgFile.close();
            return RET_ERR;
        }
    }
    cfgFile.close();
    return RET_OK;
}

int32_t Device::ConfigItemSwitch(const std::string &configItem, const std::string &value)
{
    CALL_DEBUG_ENTER;
    if (configItem.empty() || value.empty()) {
        FI_HILOGE("Invalid configuration encountered");
        return RET_ERR;
    }
    if (!Utility::IsInteger(value)) {
        FI_HILOGE("Invalid configuration encountered");
        return RET_ERR;
    }
    if (configItem == CONFIG_ITEM_KEYBOARD_TYPE) {
        keyboardType_ = static_cast<IDevice::KeyboardType>(stoi(value));
    }
    return RET_OK;
}

int32_t Device::ReadTomlFile(const std::string &filePath)
{
    CALL_DEBUG_ENTER;
    char temp[PATH_MAX] {};
    if (realpath(filePath.c_str(), temp) == nullptr) {
        FI_HILOGE("Not real path (\'%{public}s\'): %{public}s", filePath.c_str(), strerror(errno));
        return RET_ERR;
    }
    FI_HILOGD("config file path: %{public}s", temp);

    if (!Utility::DoesFileExist(temp)) {
        FI_HILOGE("File does not exist: %{public}s", temp);
        return RET_ERR;
    }
    if (Utility::GetFileSize(temp) > MAX_FILE_SIZE_ALLOWED) {
        FI_HILOGE("File size is out of range");
        return RET_ERR;
    }
    if (ReadConfigFile(std::string(temp)) != RET_OK) {
        FI_HILOGE("ReadConfigFile failed");
        return RET_ERR;
    }
    return RET_OK;
}

void Device::JudgeKeyboardType()
{
    CALL_DEBUG_ENTER;
    if (TestBit(KEY_HOME, keyBitmask_) && (GetBus() == BUS_BLUETOOTH)) {
        keyboardType_ = IDevice::KEYBOARD_TYPE_REMOTECONTROL;
        FI_HILOGD("The keyboard type is remote control");
    } else if (TestBit(KEY_Q, keyBitmask_)) {
        keyboardType_ = IDevice::KEYBOARD_TYPE_ALPHABETICKEYBOARD;
        FI_HILOGD("The keyboard type is standard");
    } else if (TestBit(KEY_KP1, keyBitmask_)) {
        keyboardType_ = IDevice::KEYBOARD_TYPE_DIGITALKEYBOARD;
        FI_HILOGD("The keyboard type is digital keyboard");
    } else if (TestBit(KEY_LEFTCTRL, keyBitmask_) &&
               TestBit(KEY_RIGHTCTRL, keyBitmask_) &&
               TestBit(KEY_F20, keyBitmask_)) {
        keyboardType_ = IDevice::KEYBOARD_TYPE_HANDWRITINGPEN;
        FI_HILOGD("The keyboard type is handwriting pen");
    } else {
        keyboardType_ = IDevice::KEYBOARD_TYPE_UNKNOWN;
        FI_HILOGD("Undefined keyboard type");
    }
}

void Device::LoadDeviceConfig()
{
    CALL_DEBUG_ENTER;
    if (ReadTomlFile(MakeConfigFileName()) != RET_OK) {
        FI_HILOGE("ReadTomlFile failed");
        keyboardType_ = IDevice::KEYBOARD_TYPE_NONE;
    }
    if (IsKeyboard()) {
        if ((keyboardType_ <= IDevice::KEYBOARD_TYPE_NONE) ||
            (keyboardType_ >= IDevice::KEYBOARD_TYPE_MAX)) {
            JudgeKeyboardType();
        }
    } else {
        keyboardType_ = IDevice::KEYBOARD_TYPE_NONE;
    }
    FI_HILOGD("keyboard type: %{public}d", keyboardType_);
}
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS