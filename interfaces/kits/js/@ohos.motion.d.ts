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

import { Callback } from "./basic";

/**
 * 识别出用户的手势状态并通知给订阅者.
 *
 * @since 9
 * @sysCap SystemCapability.Sensors.Motion
 * @devices phone, tablet
 * @import import sensor from '@ohos.motion'
 * @permission N/A
 */
declare namespace motion {
    /**
     * 订阅拿起手势的通知.
     * @param 订阅手势状态的类型, {@code type: MotionType.TYPE_PICKUP}.
     * @param callback 用户订阅的Callback函数.
     * @since 9
     */
    function on(type: MotionType.TYPE_PICKUP, callback: Callback<PickupResponse>): void;

    /**
     * 订阅将设备远离的通知.
     * @param 订阅手势状态的类型, {@code type: MotionType.TYPE_TAKE_OFF}.
     * @param callback 用户订阅的Callback函数.
     * @since 9
     */
    function on(type: MotionType.TYPE_TAKE_OFF, callback: Callback<TakeOffResponse>): void;

    /**
     * 订阅靠近监听的通知.
     * @param 订阅的手势状态类型, {@code type: MotionType.TYPE_CLOSE_TO_EAR}.
     * @param callback 用户订阅的Callback函数.
     * @since 9
     */
    function on(type: MotionType.TYPE_CLOSE_TO_EAR, callback: Callback<CloseToEarResponse>): void;

    /**
     * 订阅翻转事件的监听通知.
     * @param 订阅的手势状态类型, {@code type: MotionType.TYPE_FLIP}.
     * @param callback 用户订阅的Callback函数.
     * @since 9
     */
    function on(type: MotionType.TYPE_FLIP, callback: Callback<FlipResponse>): void;

    /**
     * 订阅招一招手势的监听.
     * @param 订阅的手势状态类型, {@code type: MotionType.TYPE_WAVE}.
     * @param callback 用户订阅的Callback函数.
     * @since 9
     */
    function on(type: MotionType.TYPE_WAVE, callback: Callback<WaveResponse>): void;

    /**
     * 订阅摇一摇手势的监听.
     * @param 订阅的手势状态类型, {@code type: MotionType.TYPE_SHAKE}.
     * @param callback 用户订阅的Callback函数.
     * @since 9
     */
    function on(type: MotionType.TYPE_SHAKE, callback: Callback<ShakeResponse>): void;

    /**
     * 订阅旋转手势的监听.
     * @param 订阅的手势状态类型, {@code type: MotionType.TYPE_ROTATE}.
     * @param callback 用户订阅的Callback函数.
     * @since 9
     */
    function on(type: MotionType.TYPE_ROTATE, callback: Callback<RotateResponse>): void;

    /**
     * 订阅口袋模式的监听.
     * @param 订阅的手势状态类型, {@code type: MotionType.TYPE_POCKET}.
     * @param callback 用户订阅的Callback函数.
     * @since 9
     */
    function on(type: MotionType.TYPE_POCKET, callback: Callback<PocketResponse>): void;

    /**
     * 订阅抬落腕事件的监听.
     * @param 订阅的手势状态类型, {@code type: MotionType.TYPE_WRIST_TILT}.
     * @param callback 用户订阅的Callback函数.
     * @since 9
     */
    function on(type: MotionType.TYPE_WRIST_TILT, callback: Callback<WristTiltResponse>): void;
    function on(type: MotionType.TYPE_LEFT_SIDE_SWIPE, callback: Callback<LeftSideSwipeResponse>): void;
    function on(type: MotionType.TYPE_RIGHT_SIDE_SWIPE, callback: Callback<RightSideSwipeResponse>): void;
    function on(type: MotionType.TYPE_UP_SIDE_SWIPE, callback: Callback<UpSideSwipeResponse>): void;
    function on(type: MotionType.TYPE_TWO_FINGERS_PINCH, callback: Callback<TwoFingersPinchResponse>): void;
    function on(type: MotionType.TYPE_THREE_FINGERS_SLIDE, callback: Callback<ThreeFingersSlideResponse>): void;

    /**
     * 取消拿起手势通知的订阅.
     * @param 取消手势状态的类型, {@code type: MotionType.TYPE_PICKUP}.
     * @param callback 用户取消的Callback函数.
     * @since 9
     */
    function off(type: MotionType.TYPE_PICKUP, callback?: Callback<PickupResponse>): void;

    /**
     * 取消设备远离事件的订阅.
     * @param 取消手势状态的类型, {@code type: MotionType.TYPE_TAKE_OFF}.
     * @param callback 用户取消的Callback函数.
     * @since 9
     */
    function off(type: MotionType.TYPE_TAKE_OFF, callback?: Callback<TakeOffResponse>): void;

    /**
     * 取消靠近监听的订阅.
     * @param 取消的手势状态类型, {@code type: MotionType.TYPE_CLOSE_TO_EAR}.
     * @param callback 用户取消的Callback函数.
     * @since 9
     */
    function off(type: MotionType.TYPE_CLOSE_TO_EAR, callback?: Callback<CloseToEarResponse>): void;

    /**
     * 取消翻转事件的订阅.
     * @param 取消的手势状态类型, {@code type: MotionType.TYPE_FLIP}.
     * @param callback 用户取消的Callback函数.
     * @since 9
     */
    function off(type: MotionType.TYPE_FLIP, callback?: Callback<FlipResponse>): void;

    /**
     * 取消招一招手势的订阅.
     * @param 取消的手势状态类型, {@code type: MotionType.TYPE_WAVE}.
     * @param callback 用户取消的Callback函数.
     * @since 9
     */
    function off(type: MotionType.TYPE_WAVE, callback?: Callback<WaveResponse>): void;

    /**
     * 取消摇一摇手势的订阅.
     * @param 取消的手势状态类型, {@code type: MotionType.TYPE_SHAKE}.
     * @param callback 用户取消的Callback函数.
     * @since 9
     */
    function off(type: MotionType.TYPE_SHAKE, callback?: Callback<ShakeResponse>): void;

    /**
     * 取消旋转手势的订阅.
     * @param 取消的手势状态类型, {@code type: MotionType.TYPE_ROTATE}.
     * @param callback 用户取消的Callback函数.
     * @since 9
     */
    function off(type: MotionType.TYPE_ROTATE, callback?: Callback<RotateResponse>): void;

    /**
     * 取消口袋模式的订阅.
     * @param 取消的手势状态类型, {@code type: MotionType.TYPE_POCKET}.
     * @param callback 用户取消的Callback函数.
     * @since 9
     */
    function off(type: MotionType.TYPE_POCKET, callback?: Callback<PocketResponse>): void;

    /**
     * 取消抬落腕事件的订阅.
     * @param 取消的手势状态类型, {@code type: MotionType.TYPE_WRIST_TILT}.
     * @param callback 用户取消的Callback函数.
     * @since 9
     */
    function off(type: MotionType.TYPE_WRIST_TILT, callback?: Callback<WristTiltResponse>): void;
    function off(type: MotionType.TYPE_LEFT_SIDE_SWIPE, callback?: Callback<LeftSideSwipeResponse>): void;
    function off(type: MotionType.TYPE_RIGHT_SIDE_SWIPE, callback?: Callback<RightSideSwipeResponse>): void;
    function off(type: MotionType.TYPE_UP_SIDE_SWIPE, callback?: Callback<UpSideSwipeResponse>): void;
    function off(type: MotionType.TYPE_TWO_FINGERS_PINCH, callback?: Callback<TwoFingersPinchResponse>): void;
    function off(type: MotionType.TYPE_THREE_FINGERS_SLIDE, callback?: Callback<ThreeFingersSlideResponse>): void;

    /**
     * 手势状态类型。
     * @devices phone, tablet
     * @sysCap SystemCapability.Msdp.Motion
     */
    export enum MotionType {
        TYPE_PICKUP = "pickUp",
        TYPE_TAKE_OFF = "takeOff",
        TYPE_CLOSE_TO_EAR = "closeToEar",
        TYPE_FLIP = "flip",
        TYPE_WAVE = "wave",
        TYPE_SHAKE = "shame",
        TYPE_ROTATE = "rotate",
        TYPE_POCKET = "pocket",
        TYPE_WRIST_TILT = "wristTitle",
        TYPE_LEFT_SIDE_SWIPE = "leftSideSwipe",
        TYPE_RIGHT_SIDE_SWIPE = "rightSideSwipe",
        TYPE_UP_SIDE_SWIPE = "upSideSwipe",
        TYPE_TWO_FINGERS_PINCH = "twoFingersPinch",
        TYPE_THREE_FINGERS_SLIDE = "threeFingersSlide"
    }

    /**
     * 手势状态事件的基本数据结构
     * @devices phone, tablet
     * @sysCap SystemCapability.Msdp.Motion
     */
    export interface MotionResponse {
        motionValue: boolean
    }

    /**
     * 拿起事件的数据
     * @devices phone, tablet
     * @sysCap SystemCapability.Msdp.Motion
     */
    export interface PickupResponse extends MotionResponse {}

    /**
     * 翻转事件的数据
     * @devices phone, tablet
     * @sysCap SystemCapability.Msdp.Motion
     */
    export interface FlipResponse extends MotionResponse {}

    /**
     * 靠近耳朵事件的数据
     * @devices phone, tablet
     * @sysCap SystemCapability.Msdp.Motion
     */
    export interface CloseToEarResponse extends MotionResponse {}

    /**
     * 摇一摇事件的数据
     * @devices phone, tablet
     * @sysCap SystemCapability.Msdp.Motion
     */
    export interface ShakeResponse extends MotionResponse {}

    /**
     * 旋转事件的数据
     * @devices phone, tablet
     * @sysCap SystemCapability.Msdp.Motion
     */
    export interface RotateResponse extends MotionResponse {}

    /**
     * 口袋模式事件的数据
     * @devices phone, tablet
     * @sysCap SystemCapability.Msdp.Motion
     */
    export interface PocketResponse extends MotionResponse {}

    /**
     * 远离事件的数据
     * @devices phone, tablet
     * @sysCap SystemCapability.Msdp.Motion
     */
    export interface TakeOffResponse extends MotionResponse {}

    /**
     * 抬落腕事件的数据
     * @devices phone, tablet
     * @sysCap SystemCapability.Msdp.Motion
     */
    export interface WristTiltResponse extends MotionResponse {}

    /**
     * 招一招事件的数据
     * @devices phone, tablet
     * @sysCap SystemCapability.Msdp.Motion
     */
    export interface WaveResponse extends MotionResponse {}
    export interface LeftSideSwipeResponse extends MotionResponse {
        status: number,
        action: number,
        move:   number,
    }
    export interface RightSideSwipeResponse extends MotionResponse {
        status: number,
        action: number,
        move:   number,
    }

    export interface UpSideSwipeResponse extends MotionResponse {
        status: number,
        action: number,
        move:   number,
    }
    export interface ThreeFingersSlideResponse extends MotionResponse {
        status: number,
        action: number,
        move:   number,
    }

    export interface TwoFingersPinchResponse extends MotionResponse {
        status: number,
        action: number,
        move:   number,
    }
}

export default motion;

