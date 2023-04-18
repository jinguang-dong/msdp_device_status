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

    /**
     * @brief 订阅设备状态。
     * @param type 设备状态类型
     * @param event 订阅的事件（进入/退出/进入和退出）
     * @param latency 上报周期
     * @param callback 用于接收设备状态事件变化的回调
     * @return 返回值如果是0表示接口调用成功，返回其他值表示接口调用失败。
     * @since 9
     */
    int32_t SubscribeCallback(Type type, ActivityEvent event, ReportLatencyNs latency,
        sptr<IRemoteDevStaCallback> callback);

    /**
     * @brief 取消订阅设备状态。
     * @param type 设备状态类型
     * @param event 订阅的事件（进入/退出/进入和退出）
     * @param callback 用于接收设备状态事件变化的回调
     * @return 返回值如果是0表示接口调用成功，返回其他值表示接口调用失败。
     * @since 9
     */
    int32_t UnsubscribeCallback(Type type, ActivityEvent event, sptr<IRemoteDevStaCallback> callback);

    /**
     * @brief 获取当前设备状态数据。
     * @param type 设备状态类型。
     * @return 设备状态数据。
     * @since 9
     */
    Data GetDeviceStatusData(const Type type);

    /**
     * @brief 注册死亡监听。
     * @param deathListener 用于接受死亡通知的监听器。
     * @since 9
     */
    void RegisterDeathListener(std::function<void()> deathListener);

    /**
     * @brief 注册键鼠穿越管理事件监听。
     * @return 返回值如果是0表示接口调用成功，返回其他值表示接口调用失败。
     * @since 9
     */
    int32_t RegisterCoordinationListener();

    /**
     * @brief 注销键鼠穿越管理事件监听。
     * @return 返回值如果是0表示接口调用成功，返回其他值表示接口调用失败。
     * @since 9
     */
    int32_t UnregisterCoordinationListener();

    /**
     * @brief 开启/关闭键鼠穿越管理接口。
     * @param userData 用户数据
     * @param enabled 开启/关闭键鼠穿越
     * @return 返回值如果是0表示接口调用成功，返回其他值表示接口调用失败。
     * @since 9
     */
    int32_t EnableCoordination(int32_t userData, bool enabled);

    /**
     * @brief 启动跨设备键鼠穿越。
     * @param userData 用户数据
     * @param remoteNetworkId 键鼠穿越目标设备描述符（networkID）
     * @param startDeviceId 键鼠穿越待穿越输入外设标识符（设备ID句柄）
     * @return 返回值如果是0表示接口调用成功，返回其他值表示接口调用失败。
     * @since 9
     */
    int32_t StartCoordination(int32_t userData, const std::string &remoteNetworkId, int32_t startDeviceId);

    /**
     * @brief 停止跨设备键鼠穿越。
     * @param userData 用户数据
     * @return 返回值如果是0表示接口调用成功，返回其他值表示接口调用失败。
     * @since 9
     */
    int32_t StopCoordination(int32_t userData);

    /**
     * @brief 获取指定设备键鼠穿越状态。
     * @param userData 用户数据
     * @param deviceId 指定设备描述符
     * @return 返回值如果是0表示接口调用成功，返回其他值表示接口调用失败。
     * @since 9
     */
    int32_t GetCoordinationState(int32_t userData, const std::string &deviceId);

    /**
     * @brief 开始拖拽目标。
     * @param dragData 拖拽附加数据
     * @return 返回0表示调用成功，否则，表示调用失败
     * @since 10
     */
    int32_t StartDrag(const DragData &dragData);

    /**
     * @brief 结束拖拽。
     * @param result 标识拖拽调用结果 0-成功,1-失败,2-取消
     * @param hasCustomAnimation 标识是否在拖拽成功时做默认动效，true表示做应用自定义动效，false表示做默认动效
     * @return 返回0表示调用成功，否则，表示调用失败
     * @since 10
     */
    int32_t StopDrag(DragResult result, bool hasCustomAnimation);

    /**
     * @brief 更新拖拽中的光标样式。
     * @param style 指定光标样式。
     * @return 返回值如果是0表示接口调用成功，返回其他值表示接口调用失败。
     * @since 10
     */
    int32_t UpdateDragStyle(DragCursorStyle style);

    /**
     * @brief 获取拖拽目标窗口PID
     * @return 目标窗口PID
     * @since 10
     */
    int32_t GetDragTargetPid();

    /**
     * @brief 获取拖拽目标窗口UdKey
     * @param UdKey 拖拽目标窗口统一数据密钥。
     * @return 返回值如果是-1则是无效值，为大于等于0的值为正确值
     * @since 10
     */
    int32_t GetUdKey(std::string &udKey);

    /**
     * @brief 注册拖拽状态监听。
     * @return 返回值0表示接口调用成功，否则，表示接口调用失败。
     * @since 10
     */
    int32_t AddDraglistener();

    /**
     * @brief 取消注册拖拽状态监听。
     * @return 返回值0表示接口调用成功，否则，表示接口调用失败。
     * @since 10
     */
    int32_t RemoveDraglistener();

    /**
     * @brief 设置拖拽窗口显示或者隐藏
     * @param visible 设置拖拽窗口的是否显示，true表示显示，false表示隐藏。
     * @return 返回值0表示接口调用成功，否则，表示接口调用失败。
     * @since 10
     */
    int32_t SetDragWindowVisible(bool visible);

    /**
     * @brief 获取触控点或鼠标光标相对于阴影缩略图左上角的位置。
     * @param offsetX 要查询的x值。
     * @param offsetY 要查询的y值。
     * @param width 要查询缩略图的宽
     * @param height 要查询缩略图的高
     * @return 返回值0表示接口调用成功，否则，表示接口调用失败。
     * @since 10
     */
    int32_t GetShadowOffset(int32_t& offsetX, int32_t& offsetY, int32_t& width, int32_t& height);

    /**
     * @brief 分配套接字对。
     * @param moduleType 模块类型。
     * @return 返回值0表示接口调用成功，否则，表示接口调用失败。
     * @since 10
     */
    int32_t AllocSocketPair(const int32_t moduleType);

    /**
     * @brief 获取已分配套接字对客户端的套接字描述器。
     * @return 客户端的套接字描述器。
     * @since 10
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
