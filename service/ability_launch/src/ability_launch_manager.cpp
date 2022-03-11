/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ability_launch_manager.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "ability_manager_client.h"
#include "file_ex.h"
#include "ohos/aafwk/base/string_wrapper.h"

#include "mmi_log.h"
#include "timer_manager.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr int32_t MAX_PREKEYS_NUM = 4;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "AbilityLaunchManager" };
} // namespace

AbilityLaunchManager::AbilityLaunchManager()
{
    std::string configFile = GetConfigFilePath();
    ResolveConfig(configFile);
    Print();
}

std::string AbilityLaunchManager::GenerateKey(const ShortcutKey& key)
{
    std::set<int32_t> preKeys = key.preKeys;
    std::stringstream oss;
    for(const auto preKey: preKeys) {
        oss << preKey << ",";
    }
    oss << key.finalKey << ",";
    oss << key.triggerType;
    return std::string(oss.str());
}

std::string AbilityLaunchManager::GetConfigFilePath() const
{
    std::string defaultConfig = "/product/multimodalinput/ability_launch_config.json";
    return FileExists(defaultConfig) ? defaultConfig : "/system/etc/multimodalinput/ability_launch_config.json";
}

void AbilityLaunchManager::ResolveConfig(const std::string configFile)
{
    if (!FileExists(configFile)) {
        MMI_LOGE("config file %{public}s not exist", configFile.c_str());
        return;
    }
    MMI_LOGD("config file path:%{public}s", configFile.c_str());
    std::ifstream reader(configFile);
    if (!reader.is_open()) {
        MMI_LOGE("config file open failed");
        return;
    }
    json configJson;
    reader >> configJson;
    reader.close();
    if (configJson.empty()) {
        MMI_LOGE("config file is empty");
        return;
    }
    json shortkeys = configJson["Shortkeys"];
    if (!shortkeys.is_array() || shortkeys.empty()) {
        MMI_LOGE("shortkeys in config file is empty");
        return;
    }
    for (size_t i = 0; i < shortkeys.size(); i++) {
        ShortcutKey shortcutKey;
        if (!ConvertToShortcutKey(shortkeys[i], shortcutKey)) {
            continue;
        }
        std::string key = GenerateKey(shortcutKey);
        auto res = shortcutKeys_.find(key);
        if (res == shortcutKeys_.end()) {
            auto iter = shortcutKeys_.emplace(key, shortcutKey);
            if (!iter.second) {
                MMI_LOGE("Duplicate shortcutKey:%{public}s", key.c_str());
            }
        }
    }
}

bool AbilityLaunchManager::ConvertToShortcutKey(const json &jsonData, ShortcutKey &shortcutKey)
{
    json preKey = jsonData["preKey"];
    if (!preKey.is_array()) {
        MMI_LOGE("preKey number must be array");
        return false;
    }
    if (preKey.size() > MAX_PREKEYS_NUM) {
        MMI_LOGE("preKey number must less and equal four");
        return false;
    }

    for (size_t i = 0; i < preKey.size(); i++) {
        if (!preKey[i].is_number() || preKey[i] < 0) {
            MMI_LOGE("preKey must be number and bigger or equal to 0");
            return false;
        }
        auto ret = shortcutKey.preKeys.emplace(preKey[i]);
        if (!ret.second) {
            MMI_LOGE("preKey must be unduplicated");
            return false;
        }
    }
    if (!jsonData["finalKey"].is_number()) {
        MMI_LOGE("finalKey must be number");
        return false;
    }
    shortcutKey.finalKey = jsonData["finalKey"];
    if ((!jsonData["trigger"].is_string())
        || ((jsonData["trigger"].get<std::string>() != "key_up")
        && (jsonData["trigger"].get<std::string>() != "key_down"))) {
        MMI_LOGE("trigger must be one of [key_up, key_down]");
        return false;
    }
    if (jsonData["trigger"].get<std::string>() == "key_up") {
        shortcutKey.triggerType = KeyEvent::KEY_ACTION_UP;
    } else {
        shortcutKey.triggerType = KeyEvent::KEY_ACTION_DOWN;
    }
    if ((!jsonData["keyDownDuration"].is_number()) || (jsonData["keyDownDuration"] < 0)) {
        MMI_LOGE("keyDownDuration must be number and bigger and equal zero");
        return false;
    }
    shortcutKey.keyDownDuration = jsonData["keyDownDuration"];
    if (!PackageAbility(jsonData["ability"], shortcutKey.ability)) {
        MMI_LOGE("package ability failed");
        return false;
    }
    return true;
}

bool AbilityLaunchManager::PackageAbility(const json &jsonAbility, Ability &ability)
{
    if (!jsonAbility.is_object()) {
        MMI_LOGE("ability must be object");
        return false;
    }
    if (!jsonAbility["entities"].is_array()) {
        MMI_LOGE("entities must be array");
        return false;
    }
    if (!jsonAbility["params"].is_array()) {
        MMI_LOGE("params must be array");
        return false;
    }
    ability.bundleName = jsonAbility["bundleName"];
    ability.abilityName = jsonAbility["abilityName"];
    ability.action = jsonAbility["action"];
    ability.type = jsonAbility["type"];
    ability.deviceId = jsonAbility["deviceId"];
    ability.uri = jsonAbility["uri"];
    for (size_t i = 0; i < jsonAbility["entities"].size(); i++) {
        ability.entities.push_back(jsonAbility["entities"][i]);
    }
    json params = jsonAbility["params"];
    for (size_t i = 0; i < params.size(); i++) {
        if (!params[i].is_object()) {
            MMI_LOGE("param must be object");
            return false;
        }
        auto ret = ability.params.emplace(params[i]["key"], params[i]["value"]);
        if (!ret.second) {
            MMI_LOGW("Emplace to failed");
        }
    }
    return true;
}

void AbilityLaunchManager::Print()
{
    uint32_t count = shortcutKeys_.size();
    MMI_LOGD("shortcutKey count:%{public}u", count);
    for (const auto &item : shortcutKeys_) {
        auto &shortcutKey = item.second;
        for (auto prekey: shortcutKey.preKeys) {
            MMI_LOGD("preKey:%{public}d", prekey);
        }
        MMI_LOGD("finalKey:%{public}d,keyDownDuration:%{public}d,triggerType:%{public}d,"
            " bundleName:%{public}s,abilityName:%{public}s", shortcutKey.finalKey,
            shortcutKey.keyDownDuration, shortcutKey.triggerType,
            shortcutKey.ability.bundleName.c_str(), shortcutKey.ability.abilityName.c_str());
    }
}

bool AbilityLaunchManager::CheckLaunchAbility(const std::shared_ptr<KeyEvent> key)
{
    CALL_LOG_ENTER;
    if (IsKeyMatch(lastMatchedKey_, key)) {
        MMI_LOGE("The same key is waiting timeout, skip");
        return true;
    }
    if (lastMatchedKey_.timerId >= 0) {
        MMI_LOGE("remove timer:%{public}d", lastMatchedKey_.timerId);
        TimerMgr->RemoveTimer(lastMatchedKey_.timerId);
    }
    ResetLastMatchedKey();
    for (auto& item: shortcutKeys_) {
        ShortcutKey &shortcutKey = item.second;
        if (!IsKeyMatch(shortcutKey, key)) {
            MMI_LOGD("not matched, next");
            continue;
        }
        shortcutKey.Print();
        if(shortcutKey.triggerType == KeyEvent::KEY_ACTION_DOWN) {
            return HandleKeyDown(shortcutKey);
        } else if (shortcutKey.triggerType == KeyEvent::KEY_ACTION_UP) {
            return HandleKeyUp(key, shortcutKey);
        } else {
            return HandleKeyCancel(shortcutKey);
        }
    }
    return false;
}

bool AbilityLaunchManager::IsKeyMatch(const ShortcutKey &shortcutKey, const std::shared_ptr<KeyEvent> &key)
{
    CALL_LOG_ENTER;
    if ((key->GetKeyCode() != shortcutKey.finalKey) || (shortcutKey.triggerType != key->GetKeyAction())) {
        return false;
    }
    if ((shortcutKey.preKeys.size() + 1) != key->GetKeyItems().size()) {
        return false;
    }
    for (const auto &item : key->GetKeyItems()) {
        int32_t keyCode = item.GetKeyCode();
        if (SkipFinalKey(keyCode, key)) {
            continue;
        }
        if (shortcutKey.preKeys.find(keyCode) == shortcutKey.preKeys.end()) {
            return false;
        }
    }
    MMI_LOGD("leave, key matched");
    return true;
}

bool AbilityLaunchManager::SkipFinalKey(const int32_t keyCode, const std::shared_ptr<KeyEvent> &key) {
    return keyCode == key->GetKeyCode();
}

bool AbilityLaunchManager::HandleKeyDown(ShortcutKey &shortcutKey)
{
    CALL_LOG_ENTER;
    if (shortcutKey.keyDownDuration == 0) {
        MMI_LOGD("Start launch ability immediately");
        LaunchAbility(shortcutKey);
        return true;
    }
    shortcutKey.timerId = TimerMgr->AddTimer(shortcutKey.keyDownDuration, 1, [this, shortcutKey] () {
        MMI_LOGD("Timer callback");
        LaunchAbility(shortcutKey);
    });
    if (shortcutKey.timerId < 0) {
        MMI_LOGE("Timer add failed");
        return false;
    }
    MMI_LOGD("add timer success, timeid:%{public}d", shortcutKey.timerId);
    lastMatchedKey_ = shortcutKey;
    return true;
}

bool AbilityLaunchManager::HandleKeyUp(const std::shared_ptr<KeyEvent> &keyEvent, const ShortcutKey &shortcutKey)
{
    CALL_LOG_ENTER;
    if (shortcutKey.keyDownDuration == 0) {
        MMI_LOGD("Start launch ability immediately");
        LaunchAbility(shortcutKey);
        return true;
    }
    const KeyEvent::KeyItem* keyItem = keyEvent->GetKeyItem();
    CHKPF(keyItem);
    auto upTime = keyEvent->GetActionTime();
    auto downTime = keyItem->GetDownTime();
    MMI_LOGD("UpTime:%{public}" PRId64 ",downTime:%{public}" PRId64 ",keyDownDuration:%{public}d",
        upTime, downTime, shortcutKey.keyDownDuration);
    if (upTime - downTime >= static_cast<int64_t>(shortcutKey.keyDownDuration) * 1000) {
        MMI_LOGD("Skip, upTime - downTime >= duration");
        return false;
    }
    MMI_LOGD("Start launch ability immediately");
    LaunchAbility(shortcutKey);
    return true;
}

bool AbilityLaunchManager::HandleKeyCancel(ShortcutKey &shortcutKey)
{
    CALL_LOG_ENTER;
    if (shortcutKey.timerId < 0) {
        MMI_LOGE("Skip, timerid less than 0");
    }
    auto timerId = shortcutKey.timerId;
    shortcutKey.timerId = -1;
    TimerMgr->RemoveTimer(timerId);
    MMI_LOGD("timerId: %{public}d", timerId);
    return false;
}

void AbilityLaunchManager::LaunchAbility(ShortcutKey key)
{
    AAFwk::Want want;
    want.SetElementName(key.ability.deviceId, key.ability.bundleName, key.ability.abilityName);
    want.SetAction(key.ability.action);
    want.SetUri(key.ability.uri);
    want.SetType(key.ability.uri);
    for (const auto &entity : key.ability.entities) {
        want.AddEntity(entity);
    }
    AAFwk::WantParams wParams;
    for (const auto &item : key.ability.params) {
        wParams.SetParam(item.first, AAFwk::String::Box(item.second));
    }
    want.SetParams(wParams);
    MMI_LOGD("Start launch ability, bundleName:%{public}s", key.ability.bundleName.c_str());
    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    if (err != ERR_OK) {
        MMI_LOGE("LaunchAbility failed, bundleName:%{public}s,err:%{public}d", key.ability.bundleName.c_str(), err);
    }
    ResetLastMatchedKey();
    MMI_LOGD("End launch ability, bundleName:%{public}s", key.ability.bundleName.c_str());
}

void ShortcutKey::Print() const
{
    for (const auto &prekey: preKeys) {
        MMI_LOGI("eventkey matched, preKey:%{public}d", prekey);
    }
    MMI_LOGD("eventkey matched, finalKey:%{public}d,bundleName:%{public}s",
        finalKey, ability.bundleName.c_str());
}
} // namespace MMI
} // namespace OHOS