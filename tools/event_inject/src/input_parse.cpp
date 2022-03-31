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

#include "input_parse.h"

#include "cJSON.h"

#include "msg_head.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "GetDeviceNode" };
} // namespace

std::string Pos::ToString() const
{
    std::ostringstream oss;
    oss << "pos(" << xPos << "," << yPos << ")";
    return oss.str();
}

std::string DeviceEvent::ToString() const
{
    std::ostringstream oss;
    oss << "{eventType:" << eventType
        << ",event:[";
    for (const auto &item : event) {
        oss << item << ",";
    }
    oss << "],keyValue:" << keyValue
        << ",blockTime:" << blockTime
        << ",ringEvents:[";
    for (const auto &item : ringEvents) {
        oss << item << ",";
    }
    oss << "],direction:" << direction
        << ",distance:" << distance
        << ",xPos:" << xPos
        << ",yPos:" << yPos
        << ",tiltX:" << tiltX
        << ",tiltY:" << tiltY
        << ",pressure:" << pressure
        << ",trackingId:" << trackingId
        << ",reportType:" << reportType
        << ",keyStatus:" << keyStatus
        << ",posXY:";
    for (const auto &item : posXY) {
        oss << item.ToString() << ",";
    }
    oss << "}" << std::endl;
    return oss.str();
}

std::string DeviceItem::ToString() const
{
    std::ostringstream oss;
    oss << "{deviceName:" << deviceName
        << ",deviceIndex:" << deviceIndex
        << ",events:[";
    for (const auto &item : events) {
        oss << item.ToString() << ",";
    }
    oss << "]" << std::endl;
    return oss.str();
}

bool GetJsonData(cJSON *json, std::string key, std::string& val)
{
    CHKPF(json);
    if (cJSON_HasObjectItem(json, key.c_str())) {
        cJSON* rawVal = cJSON_GetObjectItem(json, key.c_str());
        CHKPF(rawVal);
        val = rawVal->valuestring;
        return true;
    }
    return false;
}

bool GetJsonData(cJSON *json, std::string key, int32_t& val)
{
    CHKPF(json);
    if (cJSON_HasObjectItem(json, key.c_str())) {
        cJSON* rawVal = cJSON_GetObjectItem(json, key.c_str());
        CHKPF(rawVal);
        val = rawVal->valueint;
        return true;
    }
    return false;
}

bool GetJsonData(cJSON *json, std::string key, int16_t& val)
{
    CHKPF(json);
    if (cJSON_HasObjectItem(json, key.c_str())) {
        cJSON* rawVal = cJSON_GetObjectItem(json, key.c_str());
        CHKPF(rawVal);
        val = rawVal->valueint;
        return true;
    }
    return false;
}

bool GetJsonData(cJSON *json, std::string key, int64_t& val)
{
    CHKPF(json);
    if (cJSON_HasObjectItem(json, key.c_str())) {
        cJSON* rawVal = cJSON_GetObjectItem(json, key.c_str());
        CHKPF(rawVal);
        val = rawVal->valueint;
        return true;
    }
    return false;
}

bool GetJsonData(cJSON *json, std::string key, std::vector<int32_t>& vals)
{
    CHKPF(json);
    if (cJSON_HasObjectItem(json, key.c_str())) {
        cJSON* rawVal = cJSON_GetObjectItem(json, key.c_str());
        CHKPF(rawVal);
        if (cJSON_IsArray(rawVal)) {
            int32_t rawValSize = cJSON_GetArraySize(rawVal);
            for (int32_t i = 0; i < rawValSize; ++i) {
                cJSON* val = cJSON_GetArrayItem(rawVal, i);
                CHKPF(val);
                vals.push_back(val->valueint);
            }
            return true;
        }
    }
    return false;
}

DeviceEvent InputParse::ParseEvents(const std::string& info) const
{
    cJSON* eventInfo = cJSON_Parse(info.c_str());
    if (eventInfo == nullptr) {
        return  {};
    }
    int32_t eventSize = cJSON_GetArraySize(eventInfo);
    DeviceEvent event;
    for (int32_t i = 0; i < eventSize; ++i) {
        cJSON* eventArray = cJSON_GetArrayItem(eventInfo, i);
        if (eventArray == nullptr) {
            MMI_HILOGW("event is null");
            cJSON_Delete(eventInfo);
            return event;
        }
        if (cJSON_IsArray(eventArray)) {
            Pos pos;
            cJSON* xPos = cJSON_GetArrayItem(eventArray, 0);
            if (xPos == nullptr) {
                MMI_HILOGW("yPos is null");
                cJSON_Delete(eventInfo);
                return event;
            }
            pos.xPos = xPos->valueint;
            cJSON* yPos = cJSON_GetArrayItem(eventArray, 1);
            if (yPos == nullptr) {
                MMI_HILOGW("yPos is null");
                cJSON_Delete(eventInfo);
                return event;
            }
            pos.yPos = yPos->valueint;
            event.posXY.push_back(pos);
        }
    }
    cJSON_Delete(eventInfo);
    return event;
}

DeviceEvent InputParse::ParseEventsObj(const std::string& info) const
{
    cJSON* eventInfo = cJSON_Parse(info.c_str());
    if (eventInfo == nullptr) {
        return  {};
    }
    DeviceEvent event;
    GetJsonData(eventInfo, "eventType", event.eventType);
    GetJsonData(eventInfo, "event", event.event);
    GetJsonData(eventInfo, "keyValue", event.keyValue);
    GetJsonData(eventInfo, "blockTime", event.blockTime);
    GetJsonData(eventInfo, "ringEvents", event.ringEvents);
    GetJsonData(eventInfo, "direction", event.direction);
    GetJsonData(eventInfo, "distance", event.distance);
    GetJsonData(eventInfo, "xPos", event.xPos);
    GetJsonData(eventInfo, "yPos", event.yPos);
    GetJsonData(eventInfo, "tiltX", event.tiltX);
    GetJsonData(eventInfo, "tiltY", event.tiltY);
    GetJsonData(eventInfo, "pressure", event.pressure);
    GetJsonData(eventInfo, "trackingId", event.trackingId);
    GetJsonData(eventInfo, "reportType", event.reportType);
    GetJsonData(eventInfo, "keyStatus", event.keyStatus);
    return event;
}

std::vector<DeviceEvent> InputParse::ParseData(const std::string& info) const
{
    cJSON* events = cJSON_Parse(info.c_str());
    if (events == nullptr) {
        return {};
    }
    int32_t eventsSize = cJSON_GetArraySize(events);
    std::vector<DeviceEvent> eventData;
    for (int32_t i = 0; i < eventsSize; ++i) {
        cJSON* eventInfo = cJSON_GetArrayItem(events, i);
        if (eventInfo == nullptr) {
            MMI_HILOGW("eventInfo is null");
            cJSON_Delete(events);
            return eventData;
        }
        DeviceEvent event;
        std::string eventInfoStr = cJSON_Print(eventInfo);
        if (cJSON_IsArray(eventInfo)) {
            event = ParseEvents(eventInfoStr);
        } else {
            event = ParseEventsObj(eventInfoStr);
        }
        eventData.push_back(event);
    }
    cJSON_Delete(events);
    return eventData;
}

DeviceItems InputParse::DataInit(const std::string& fileData, bool logType)
{
    CALL_LOG_ENTER;
    DeviceItems deviceItems;
    cJSON* arrays = cJSON_Parse(fileData.c_str());
    if (arrays == nullptr) {
        MMI_HILOGW("arrays is null");
        return {};
    }
    int32_t arraysSize = cJSON_GetArraySize(arrays);
    for (int32_t i = 0; i < arraysSize; ++i) {
        cJSON* deviceInfo = cJSON_GetArrayItem(arrays, i);
        if (deviceInfo == nullptr) {
            MMI_HILOGW("deviceInfo is null");
            cJSON_Delete(arrays);
            return deviceItems;
        }
        cJSON* deviceName = cJSON_GetObjectItem(deviceInfo, "deviceName");
        if (deviceName == nullptr) {
            MMI_HILOGW("deviceInfo is null");
            cJSON_Delete(arrays);
            return deviceItems;
        }
        DeviceItem deviceItem = {.deviceName = deviceName->valuestring, .deviceIndex = 0};
        GetJsonData(deviceInfo, "deviceIndex", deviceItem.deviceIndex);
        cJSON *events = nullptr;
        if (cJSON_HasObjectItem(deviceInfo, "events")) {
            events = cJSON_GetObjectItem(deviceInfo, "events");
        } else if (cJSON_HasObjectItem(deviceInfo, "singleEvent")) {
            events = cJSON_GetObjectItem(deviceInfo, "singleEvent");
        }
        if (events == nullptr) {
            MMI_HILOGW("events is null");
            cJSON_Delete(arrays);
            return deviceItems;
        }
        std::string eventsStr = cJSON_Print(events);
        deviceItem.events = ParseData(eventsStr);
        deviceItems.push_back(deviceItem);
        if (logType) {
            MMI_HILOGW("deviceItem[%{public}d]: %{public}s", i, deviceItem.ToString().c_str());
        }
    }
    cJSON_Delete(arrays);
    return deviceItems;
}
} // namespace MMI
} // namespace OHOS