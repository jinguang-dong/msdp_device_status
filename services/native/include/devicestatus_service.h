/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef DEVICESTATUS_SERVICE_H
#define DEVICESTATUS_SERVICE_H

#include <memory>

#include <iremote_object.h>
#include <system_ability.h>

#include "delegate_tasks.h"
#include "device_manager.h"
#include "devicestatus_delayed_sp_singleton.h"
#include "devicestatus_dumper.h"
#include "devicestatus_manager.h"
#include "devicestatus_srv_stub.h"
#include "drag_data.h"
#include "drag_manager.h"
#include "i_context.h"
#ifdef OHOS_BUILD_ENABLE_MOTION_DRAG
#include "motion_drag.h"
#endif // OHOS_BUILD_ENABLE_MOTION_DRAG
#include "stationary_callback.h"
#include "stationary_data.h"
#include "stream_server.h"
#include "timer_manager.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
enum class ServiceRunningState {STATE_NOT_START, STATE_RUNNING, STATE_EXIT};
class DeviceStatusService final : public IContext,
                                  public StreamServer,
                                  public SystemAbility,
                                  public DeviceStatusSrvStub {
    DECLARE_SYSTEM_ABILITY(DeviceStatusService)
    DECLARE_DELAYED_SP_SINGLETON(DeviceStatusService);
public:
    virtual void OnDump() override;
    virtual void OnStart() override;
    virtual void OnStop() override;

    IDelegateTasks& GetDelegateTasks() override;
    IDeviceManager& GetDeviceManager() override;
    ITimerManager& GetTimerManager() override;
    IDragManager& GetDragManager() override;

    void Subscribe(Type type, ActivityEvent event, ReportLatencyNs latency,
        sptr<IRemoteDevStaCallback> callback) override;
    void Unsubscribe(Type type, ActivityEvent event, sptr<IRemoteDevStaCallback> callback) override;
    Data GetCache(const Type& type) override;
    bool IsServiceReady() const;
    std::shared_ptr<DeviceStatusManager> GetDeviceStatusManager() const;
    int32_t Dump(int32_t fd, const std::vector<std::u16string>& args) override;
    void ReportSensorSysEvent(int32_t type, bool enable);
    int32_t RegisterCoordinationListener() override;
    int32_t UnregisterCoordinationListener() override;
    int32_t PrepareCoordination(int32_t userData) override;
    int32_t UnprepareCoordination(int32_t userData) override;
    int32_t ActivateCoordination(int32_t userData, const std::string &remoteNetworkId, int32_t startDeviceId) override;
    int32_t DeactivateCoordination(int32_t userData, bool isUnchained) override;
    int32_t GetCoordinationState(int32_t userData, const std::string &networkId) override;
    int32_t StartDrag(const DragData &dragData) override;
    int32_t StopDrag(const DragDropResult &dropResult) override;
    int32_t UpdateDragStyle(DragCursorStyle style) override;
    int32_t GetDragTargetPid() override;
    int32_t GetUdKey(std::string &udKey) override;
    int32_t AddDraglistener() override;
    int32_t RemoveDraglistener() override;
    int32_t SetDragWindowVisible(bool visible) override;
    int32_t GetShadowOffset(int32_t& offsetX, int32_t& offsetY, int32_t& width, int32_t& height) override;
    int32_t UpdateShadowPic(const ShadowInfo &shadowInfo) override;
    int32_t GetDragData(DragData &dragData) override;
    int32_t AllocSocketFd(const std::string &programName, int32_t moduleType,
        int32_t &toReturnClientFd, int32_t &tokenType) override;
    void OnConnected(SessionPtr s) override;
    void OnDisconnected(SessionPtr s) override;
    int32_t AddEpoll(EpollEventType type, int32_t fd) override;
    int32_t DelEpoll(EpollEventType type, int32_t fd);
    bool IsRunning() const override;
    int32_t AddHotAreaListener() override;

private:
    bool Init();
    int32_t InitDelegateTasks();
    int32_t InitTimerMgr();
    void OnThread();
    void OnDelegateTask(const epoll_event &ev);
    void OnTimeout(const epoll_event &ev);
    void OnDeviceMgr(const epoll_event &ev);
    int32_t EnableDevMgr(int32_t nRetries);
    void DisableDevMgr();
    int32_t OnAddHotAreaListener(int32_t pid);

#ifdef OHOS_BUILD_ENABLE_COORDINATION
    int32_t OnRegisterCoordinationListener(int32_t pid);
    int32_t OnUnregisterCoordinationListener(int32_t pid);
    int32_t OnPrepareCoordination(int32_t pid, int32_t userData);
    int32_t OnUnprepareCoordination(int32_t pid, int32_t userData);
    int32_t OnActivateCoordination(int32_t pid, int32_t userData, const std::string &remoteNetworkId,
        int32_t startDeviceId);
    int32_t OnDeactivateCoordination(int32_t pid, int32_t userData, bool isUnchained);
    int32_t OnGetCoordinationState(int32_t pid, int32_t userData, const std::string &networkId);
#endif // OHOS_BUILD_ENABLE_COORDINATION

private:
    std::atomic<ServiceRunningState> state_ { ServiceRunningState::STATE_NOT_START };
    std::thread worker_;
    DelegateTasks delegateTasks_;
    DeviceManager devMgr_;
    TimerManager timerMgr_;
    std::atomic<bool> ready_ { false };
    std::shared_ptr<DeviceStatusManager> devicestatusManager_ { nullptr };
    DragManager dragMgr_;
#ifdef OHOS_BUILD_ENABLE_MOTION_DRAG
    MotionDrag motionDrag_;
#endif // OHOS_BUILD_ENABLE_MOTION_DRAG
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_SERVICE_H
