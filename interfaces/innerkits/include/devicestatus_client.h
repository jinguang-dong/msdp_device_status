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

#ifndef DEVICESTATUS_CLIENT_H
#define DEVICESTATUS_CLIENT_H

#include <functional>
#include <map>

#include <singleton.h>

#include "devicestatus_common.h"
#include "devicestatus_data_utils.h"
#include "drag_data.h"
#include "i_coordination_listener.h"
#include "idevicestatus.h"
#include "idevicestatus_callback.h"

namespace OHOS {
namespace Msdp {
namespace DeviceStatus {
class DeviceStatusClient final : public DelayedRefSingleton<DeviceStatusClient> {
    DECLARE_DELAYED_REF_SINGLETON(DeviceStatusClient)

public:
    std::map<Type, int32_t> GetTypeMap()
    {
        return typeMap_;
    }
    DISALLOW_COPY_AND_MOVE(DeviceStatusClient);

    /*
    * @name: SubscribeCallback
    * @function: 订阅设备状态回调
    * @parameter: type 设备状态类型
    * @parameter: event 对应类型的激活状态
    * @parameter: latency 上报周期
    * @parameter: callback 用于接收设备状态变化的回调
    * @return: 订阅结果
    */
    int32_t SubscribeCallback(Type type, ActivityEvent event, ReportLatencyNs latency,
        sptr<IRemoteDevStaCallback> callback);

    /*
    * @name: UnsubscribeCallback
    * @function: 取消订阅设备状态回调
    * @parameter: type 设备状态类型
    * @parameter: event 对应类型的激活状态
    * @parameter: callback 用于接收设备状态变化的回调
    * @return: 取消订阅结果
    */
    int32_t UnsubscribeCallback(Type type, ActivityEvent event, sptr<IRemoteDevStaCallback> callback);

    /*
    * @name: GetDeviceStatusData
    * @function: 获取最近一次的设备状态数据
    * @parameter: type 设备状态类型
    * @return: 设备状态数据
    */
    Data GetDeviceStatusData(const Type type);

    /*
    * @name: RegisterDeathListener
    * @function: 注册死亡监听
    * @parameter: deathListener 用于接受死亡通知的监听器
    */
    void RegisterDeathListener(std::function<void()> deathListener);


    /*
    * @name: RegisterCoordinationListener
    * @function: 注册键鼠穿越监听
    */
    int32_t RegisterCoordinationListener();

    /*
    * @name: UnregisterCoordinationListener
    * @function: 取消注册键鼠穿越监听
    */
    int32_t UnregisterCoordinationListener();

    /*
    * @name: EnableCoordination
    * @function: 使能键鼠穿越能力
    * @parameter: userData 用户数据
    * @parameter: enabled 使能状态
    * @return: 使能结果
    */
    int32_t EnableCoordination(int32_t userData, bool enabled);

    /*
    * @name: StartCoordination
    * @function: 开始键鼠穿越
    * @parameter: remoteNetworkId 远端网络ID
    * @parameter: startDeviceId 源设备ID
    * @return: 开始结果
    */
    int32_t StartCoordination(int32_t userData, const std::string &remoteNetworkId, int32_t startDeviceId);

    /*
    * @name: StopCoordination
    * @function: 停止键鼠穿越
    * @parameter: userData 用户数据
    * @return: 停止结果
    */
    int32_t StopCoordination(int32_t userData);

    /*
    * @name: GetCoordinationState
    * @function: 获取键鼠穿越状态
    * @parameter: userData 用户数据
    * @parameter: deviceId 设备ID
    * @return: 键鼠穿越状态
    */
    int32_t GetCoordinationState(int32_t userData, const std::string &deviceId);


    /*
    * @name: StartDrag
    * @function: 开始拖拽
    * @parameter: dragData 拖拽数据
    * @return: 开始拖拽结果
    */
    int32_t StartDrag(const DragData &dragData);

    /*
    * @name: StopDrag
    * @function: 停止拖拽
    * @parameter: result 拖拽结果
    * @parameter: hasCustomAnimation 是否有自定义动画
    * @return: 停止拖拽结果
    */
    int32_t StopDrag(DragResult result, bool hasCustomAnimation);

    /*
    * @name: UpdateDragStyle
    * @function: 更新拖拽样式
    * @parameter: style 拖拽样式
    * @return: 更新结果
    */
    int32_t UpdateDragStyle(DragCursorStyle style);

    /*
    * @name: GetDragTargetPid
    * @function: 获取拖拽目标进程号
    * @return: 进程号
    */
    int32_t GetDragTargetPid();

    /*
    * @name: GetUdKey
    * @function: 获取UdKey
    * @parameter: udKey udKey
    * @return: Udkey
    */
    int32_t GetUdKey(std::string &udKey);

    /*
    * @name: AddDraglistener
    * @function: 添加拖拽监听
    * @return: 添加结果
    */
    int32_t AddDraglistener();

    /*
    * @name: RemoveDraglistener
    * @function: 移除拖拽监听
    * @return: 移除结果
    */
    int32_t RemoveDraglistener();

    /*
    * @name: SetDragWindowVisible
    * @function: 设置拖拽窗口可见性
    * @parameter: visible 是否可见
    * @return: 设置结果
    */
    int32_t SetDragWindowVisible(bool visible);

    /*
    * @name: GetShadowOffset
    * @function: 获取阴影偏移量
    * @parameter: offsetX X轴偏移量
    * @parameter: offsetY Y轴偏移量
    * @parameter: width 宽
    * @parameter: height 高
    * @return: 阴影偏移量
    */
    int32_t GetShadowOffset(int32_t& offsetX, int32_t& offsetY, int32_t& width, int32_t& height);


    /*
    * @name: AllocSocketPair
    * @function: 分配套接字对
    * @parameter: moduleType 模块类型
    * @return: 分配结果
    */
    int32_t AllocSocketPair(const int32_t moduleType);

    /*
    * @name: GetClientSocketFdOfAllocedSocketPair
    * @function: 获取已分配套接字对客户端的套接字描述器
    * @return: 客户端的套接字描述器
    */
    int32_t GetClientSocketFdOfAllocedSocketPair() const;

private:
    class DeviceStatusDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        DeviceStatusDeathRecipient() = default;
        ~DeviceStatusDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remote);
    private:
        DISALLOW_COPY_AND_MOVE(DeviceStatusDeathRecipient);
    };

    ErrCode Connect();
    sptr<Idevicestatus> devicestatusProxy_ {nullptr};
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ {nullptr};
    void ResetProxy(const wptr<IRemoteObject>& remote);
    std::mutex mutex_;
        int32_t tokenType_ { -1 };
    int32_t socketFd_ { -1 };
    std::map<Type, int32_t> typeMap_ = {};
    std::function<void()> deathListener_ { nullptr };
};
} // namespace DeviceStatus
} // namespace Msdp
} // namespace OHOS
#endif // DEVICESTATUS_CLIENT_H
