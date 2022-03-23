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
#include "mmi_client.h"

#include <cinttypes>
#include <condition_variable>

#include "mmi_log.h"
#include "proto.h"
#include "util.h"

#include "mmi_fd_listener.h"
#include "multimodal_event_handler.h"
#include "multimodal_input_connect_manager.h"

namespace OHOS {
namespace MMI {
namespace {
std::mutex mtx;
std::condition_variable cv;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "MMIClient" };
} // namespace

using namespace AppExecFwk;
MMIClient::MMIClient() {}

MMIClient::~MMIClient()
{
    CALL_LOG_ENTER;
}

bool MMIClient::SendMessage(const NetPacket &pkt) const
{
    return SendMsg(pkt);
}

bool MMIClient::GetCurrentConnectedStatus() const
{
    return GetConnectedStatus();
}

bool MMIClient::Start()
{
    CALL_LOG_ENTER;
    msgHandler_.Init();
    EventManager.SetClientHandle(GetSharedPtr());
    auto callback = std::bind(&ClientMsgHandler::OnMsgHandler, &msgHandler_,
        std::placeholders::_1, std::placeholders::_2);
    if (!(StartClient(callback))) {
        MMI_LOGE("Client startup failed");
        Stop();
        return false;
    }
    if (!StartEventRunner()) {
        MMI_LOGE("Start runner failed");
        Stop();
        return false;
    }
    return true;
}

bool MMIClient::StartEventRunner()
{
    CALL_LOG_ENTER;
    int32_t pid = GetPid();
    uint64_t tid = GetThisThreadId();
    MMI_LOGI("pid:%{public}d threadId:%{public}" PRIu64, pid, tid);

    MMI_LOGI("step 1");
    constexpr int32_t WAIT_FOR_TIMEOUT = 3;
    std::unique_lock <std::mutex> lck(mtx);
    ehThread_ = std::thread(std::bind(&MMIClient::OnEventHandlerThread, this));
    ehThread_.detach();
    if (cv.wait_for(lck, std::chrono::seconds(WAIT_FOR_TIMEOUT)) == std::cv_status::timeout) {
        MMI_LOGE("EventThandler thread start timeout");
        Stop();
        return false;
    }
    MMI_LOGI("step 4");

    recvThread_ = std::thread(std::bind(&MMIClient::OnRecvThread, this));
    recvThread_.detach();
    if (cv.wait_for(lck, std::chrono::seconds(WAIT_FOR_TIMEOUT)) == std::cv_status::timeout) {
        MMI_LOGE("Recv thread start timeout");
        Stop();
        return false;
    }
    MMI_LOGI("step 7");
    return true;
}

void MMIClient::OnEventHandlerThread()
{
    CALL_LOG_ENTER;
    SetThreadName("mmi_client_EventHdr");
    int32_t pid = GetPid();
    uint64_t tid = GetThisThreadId();
    MMI_LOGI("pid:%{public}d threadId:%{public}" PRIu64, pid, tid);

    MMI_LOGI("step 2");
    auto eventHandler = MEventHandler;
    CHKPV(eventHandler);
    auto eventRunner = eventHandler->GetEventRunner();
    CHKPV(eventRunner);
    cv.notify_one();
    MMI_LOGI("step 3");
    eventRunner->Run();
}

void MMIClient::OnRecvThread()
{
    CALL_LOG_ENTER;
    SetThreadName("mmi_client_RecvEventHdr");
    int32_t pid = GetPid();
    uint64_t tid = GetThisThreadId();
    MMI_LOGI("pid:%{public}d threadId:%{public}" PRIu64, pid, tid);

    MMI_LOGI("step 5");
    auto runner = EventRunner::Create(false);
    CHKPV(runner);
    recvEventHandler_ = std::make_shared<MMIEventHandler>(runner, GetSharedPtr());
    CHKPV(recvEventHandler_);
    if (isConnected_ && fd_ >= 0) {
        if (!AddFdListener(fd_)) {
            MMI_LOGE("add fd listener return false");
            return;
        }
    } else {
        if (!recvEventHandler_->SendEvent(MMI_EVENT_HANDLER_ID_RECONNECT, 0, CLIENT_RECONNECT_COOLING_TIME)) {
            MMI_LOGE("send reconnect event return false.");
            return;
        }
    }
    cv.notify_one();
    MMI_LOGI("step 6");
    runner->Run();
}

bool MMIClient::AddFdListener(int32_t fd)
{
    CALL_LOG_ENTER;
    if (fd < 0) {
        MMI_LOGE("Invalid fd:%{public}d", fd);
        return false;
    }
    CHKPF(recvEventHandler_);
    auto fdListener = std::make_shared<MMIFdListener>(GetSharedPtr());
    CHKPF(fdListener);
    auto errCode = recvEventHandler_->AddFileDescriptorListener(fd, FILE_DESCRIPTOR_INPUT_EVENT, fdListener);
    if (errCode != ERR_OK) {
        MMI_LOGE("add fd listener error,fd:%{public}d code:%{public}u str:%{public}s", fd, errCode,
            recvEventHandler_->GetErrorStr(errCode).c_str());
        return false;
    }
    isRunning_ = true;
    int32_t pid = GetPid();
    uint64_t tid = GetThisThreadId();
    MMI_LOGI("serverFd:%{public}d was listening,mask:%{public}u pid:%{public}d threadId:%{public}" PRIu64,
        fd, FILE_DESCRIPTOR_INPUT_EVENT, pid, tid);
    return true;
}

bool MMIClient::DelFdListener(int32_t fd)
{
    CALL_LOG_ENTER;
    CHKPF(recvEventHandler_);
    if (fd < 0) {
        MMI_LOGE("Invalid fd:%{public}d", fd);
        return false;
    }
    recvEventHandler_->RemoveFileDescriptorListener(fd);
    isRunning_ = false;
    return true;
}

void MMIClient::OnRecvMsg(const char *buf, size_t size)
{
    CHKPV(buf);
    if (size == 0) {
        MMI_LOGE("Invalid input param size");
        return;
    }
    OnRecv(buf, size);
}

int32_t MMIClient::Reconnect()
{
    CALL_LOG_ENTER;
    return ConnectTo();
}

void MMIClient::OnDisconnect()
{
    OnDisconnected();
}

void MMIClient::RegisterConnectedFunction(ConnectCallback fun)
{
    funConnected_ = fun;
}

void MMIClient::RegisterDisconnectedFunction(ConnectCallback fun)
{
    funDisconnected_ = fun;
}

void MMIClient::VirtualKeyIn(RawInputEvent virtualKeyEvent)
{
    NetPacket pkt(MmiMessageId::ON_VIRTUAL_KEY);
    pkt << virtualKeyEvent;
    SendMsg(pkt);
}

void MMIClient::OnDisconnected()
{
    MMI_LOGD("Disconnected from server, fd:%{public}d", fd_);
    isConnected_ = false;
    if (funDisconnected_) {
        funDisconnected_(*this);
    }
    if (!DelFdListener(fd_)) {
        MMI_LOGE("delete fd listener failed.");
    }
    Close();
    if (!isExit && recvEventHandler_ != nullptr) {
        MMI_LOGD("start reconnecting to the server");
        if (!recvEventHandler_->SendEvent(MMI_EVENT_HANDLER_ID_RECONNECT, 0, CLIENT_RECONNECT_COOLING_TIME)) {
            MMI_LOGE("send reconnect event return false.");
        }
    }
}

void MMIClient::OnConnected()
{
    MMI_LOGD("Connection to server succeeded, fd:%{public}d", GetFd());
    isConnected_ = true;
    if (funConnected_) {
        funConnected_(*this);
    }
    if (!isExit && !isRunning_ && fd_ >= 0 && recvEventHandler_ != nullptr) {
        if (!AddFdListener(fd_)) {
            MMI_LOGE("Add fd listener failed");
        }
    }
}

int32_t MMIClient::Socket()
{
    CALL_LOG_ENTER;
    int32_t ret = MultimodalInputConnectManager::GetInstance()->
                        AllocSocketPair(IMultimodalInputConnect::CONNECT_MODULE_TYPE_MMI_CLIENT);
    if (ret != RET_OK) {
        MMI_LOGE("UDSSocket::Socket, call MultimodalInputConnectManager::AllocSocketPair return %{public}d", ret);
        return -1;
    }
    fd_ = MultimodalInputConnectManager::GetInstance()->GetClientSocketFdOfAllocedSocketPair();
    if (fd_ == IMultimodalInputConnect::INVALID_SOCKET_FD) {
        MMI_LOGE("UDSSocket::Socket, call MultimodalInputConnectManager::GetClientSocketFdOfAllocedSocketPair"
                 " return invalid fd");
    } else {
        MMI_LOGD("UDSSocket::Socket, call MultimodalInputConnectManager::GetClientSocketFdOfAllocedSocketPair"
                 " return fd:%{public}d", fd_);
    }
    return fd_;
}

void MMIClient::Stop()
{
    CALL_LOG_ENTER;
    UDSClient::Stop();
    if (recvEventHandler_) {
        recvEventHandler_->SendSyncEvent(MMI_EVENT_HANDLER_ID_STOP, 0, EventHandler::Priority::IMMEDIATE);
    }
    MEventHandler->SendSyncEvent(MMI_EVENT_HANDLER_ID_STOP, 0, EventHandler::Priority::IMMEDIATE);
}
} // namespace MMI
} // namespace OHOS
