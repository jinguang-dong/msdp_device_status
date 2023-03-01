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

import { AsyncCallback } from './basic';
import { Callback } from './basic';
import image from './@ohos.multimedia.image';

/**
 * 拖拽管理模块
 *
 * @since 10
 * @syscap SystemCapability.Msdp.DeviceStatus.Drag
 * @systemapi Hide this for inner system use.
 */
declare namespace drag {
  /**
   * 键鼠穿越状态通知消息
   * 
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   */
  enum CoordinationMsg {
    /**
     * 键鼠穿越准备
     *
     * @since 10
     */
    COORDINATION_PREPARE,

    /**
     * 键鼠穿越取消准备
     *
     * @since 10
     */
    COORDINATION_UNPREPARE,

    /**
     * 键鼠穿越启动
     *
     * @since 10
     */
    COORDINATION_ACTIVATE,

    /**
     * 键鼠穿越启动成功
     *
     * @since 10
     */
    COORDINATION_ACTIVATE_SUCCESS,

    /**
     * 键鼠穿越启动失败
     *
     * @since 10
     */
    COORDINATION_ACTIVATE_FAIL,

    /**
     * 键鼠穿越停止成功
     *
     * @since 10
     */
    COORDINATION_DEACTIVATE_SUCCESS,

    /**
     * 键鼠穿越停止失败
     *
     * @since 10
     */
    COORDINATION_DEACTIVATE_FAIL,
  }

  /**
   * 拖拽状态通知消息
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   */
  enum DragState {
    /**
     * 拖拽异常
     *
     * @since 10
     */
    MSG_DRAG_STATE_ERROR,

    /**
     * 拖拽开始
     *
     * @since 10
     */
    MSG_DRAG_STATE_START,

    /**
     * 拖拽结束
     *
     * @since 10
     */
    MSG_DRAG_STATE_STOP,

    /**
     * 拖拽取消
     *
     * @since 10
     */
    MSG_DRAG_STATE_CANCEL
  }

  /**
   * 拖拽事件来源
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   */
  enum DragSource {
    /**
     * 鼠标
     *
     * @since 10
     */
    MOUSE,

    /**
     * 触摸屏
     *
     * @since 10
     */
    TOUCH
  }

  /**
   * 通知消息类型
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   */
  enum NoticeMsgType {
    /**
     * 拖拽方式
     *
     * @since 10
     */
    DRAG_STYLE,

    /**
     * 拖拽文本
     *
     * @since 10
     */
    DRAG_TEXT,

    /**
     * 拖拽穿越
     *
     * @since 10
     */
    DRAG_CORSS,
  }

  /**
   * 拖拽结果
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   */
  enum DragResult {
    /**
     * 复制
     *
     * @since 10
     */
    COPY,

    /**
     * 移动
     *
     * @since 10
     */
    MOVE,

    /**
     * 取消
     *
     * @since 10
     */
    CANCEL,

    /**
     * 失败
     *
     * @since 10
     */
    FAIL
  }

  /**
   * 拖拽选项
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   */
  interface DragOption {
    /**
     * 阴影缩略图
     *
     * @since 10
     */
    pixelMap: image.PixelMap;

    /**
     * 阴影缩略图显示位置的X坐标
     *
     * @since 10
     */
    x: number;

    /**
     * 阴影缩略图显示位置的Y坐标
     *
     * @since 10
     */
    y: number;

    /**
     * 拖拽事件来源
     *
     * @since 10
     */
    source: DragSource;

    /**
     * 拖拽目标数量
     *
     * @since 10
     */
    dragNum: number;

    /**
     * 拖出应用pid
     *
     * @since 10
     */
    startDragPid: number;
  }

  /**
   * 通知消息
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   */
  interface NoticeMsg {
    /**
     * 通知消息类型
     *
     * @since 10
     */
    msgType: NoticeMsgType;

    /**
     * 是否允许拖入
     *
     * @since 10
     */
    allowDragIn: boolean;

    /**
     * 文字角标
     *
     * @since 10
     */
    text: string;
  }

  /**
   * 准备键鼠穿越
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param callback 异步返回执行结果
   * @throws {BusinessError} 401 - Parameter error.
   */
  function prepare(callback: AsyncCallback<void>): void;

  /**
   * 准备键鼠穿越
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @throws {BusinessError} 401 - Parameter error.
   */
  function prepare(): Promise<void>;

  /**
   * 取消准备键鼠穿越
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param callback 异步返回执行结果
   * @throws {BusinessError} 401 - Parameter error.
   */
  function unprepare(callback: AsyncCallback<void>): void;

  /**
   * 取消准备键鼠穿越
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @throws {BusinessError} 401 - Parameter error.
   */
  function unprepare(): Promise<void>;

  /**
   * 启动键鼠穿越
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param targetNetworkId 键鼠穿越目标设备描述符
   * @param inputDeviceId 待穿越输入设备标识符
   * @param callback 异步返回执行结果
   * @throws {BusinessError} 401 - Parameter error.
   * @throws {BusinessError} **00001 - 操作失败
   */
  function activate(targetNetworkId: string, inputDeviceId: number, callback: AsyncCallback<void>): void;

  /**
   * 启动键鼠穿越
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param targetNetworkId 键鼠穿越目标设备描述符
   * @param inputDeviceId 待穿越输入设备标识符
   * @throws {BusinessError} 401 - Parameter error.
   * @throws {BusinessError} **00001 - 操作失败
   */
  function activate(targetNetworkId: string, inputDeviceId: number): Promise<void>;

  /**
   * 停止键鼠穿越
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param callback 异步返回执行结果
   * @param isUnchained 是否关闭跨设备链路
   * @throws {BusinessError} 401 - Parameter error.
   */
  function deactivate(callback: AsyncCallback<void>, isUnchained?: boolean): void;

  /**
   * 停止键鼠穿越
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param isUnchained 是否关闭跨设备链路
   */
  function deactivate(isUnchained?: boolean): Promise<void>;

  /**
   * 获取键鼠穿越开关状态
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param networkId 键鼠穿越目标设备描述符
   * @param callback 异步返回目标设备的键鼠穿越开关状态
   * @throws {BusinessError} 401 - Parameter error.
   */
  function getCrossingSwitchState(networkId: string, callback: AsyncCallback<boolean>): void;

  /**
   * 获取键鼠穿越开关的状态
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param networkId 键鼠穿越目标设备描述符
   * @throws {BusinessError} 401 - Parameter error.
   */
  function getCrossingSwitchState(networkId: string): Promise<boolean>;

  /**
   * 注册阴影缩略图动效托管
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param startCallback 开始拖拽回调函数
   * @param noticeCallback 通知拖拽状态回调函数
   * @param endCallback 结束拖拽回调函数
   */
  function registerThumbnailDraw(startCallback: Callback<DragOption>,
    noticeCallback: Callback<NoticeMsg>,
    endCallback: Callback<{ endDragPid: number, dragResult: DragResult }>): void;

  /**
   * 取消注册阴影缩略图动效托管
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param callback 取消注册结果的回调函数
   */
  function unregisterThumbnailDraw(callback: Callback<void>): void;

  /**
   * 监听键鼠穿越状态
   * 
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param type 监听类型
   * @param callback 异步返回键鼠穿越状态消息
   * @throws {BusinessError} 401 - Parameter error.
   */
  function on(type: 'coordination', callback: Callback<{ networkId: string, msg: CoordinationMsg }>): void;

  /**
   * 取消监听键鼠穿越状态
   * 
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param type 监听类型
   * @param callback 待取消监听的回调函数，参数如果为空，取消所有监听回调函数
   * @throws {BusinessError} 401 - Parameter error.
   */
  function off(type: 'coordination', callback?: Callback<void>): void;

  /**
   * 监听拖拽状态
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param type 监听类型
   * @param callback 接收拖拽状态的通知消息
   */
  function on(type: 'drag', callback: Callback<DragState>): void;

  /**
   * 取消监听拖拽状态
   *
   * @since 10
   * @syscap SystemCapability.Msdp.DeviceStatus.Drag
   * @systemapi Hide this for inner system use.
   * @param type 监听类型
   * @param callback 待取消监听的回调函数，参数如果为空，取消所有监听回调函数
   */
  function off(type: 'drag', callback?: Callback<DragState>): void;
}

export default drag;
