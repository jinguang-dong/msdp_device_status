/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

//! Implementation of epoll event loop.

#![allow(dead_code)]
#![allow(unused_variables)]

use std::collections::BTreeMap;
use std::ffi::{ c_void, c_char, c_int, CString };
use std::future::Future;
use std::mem::MaybeUninit;
use std::os::fd::RawFd;
use std::pin::Pin;
use std::sync::{ Arc, Mutex };
use std::sync::atomic::{ AtomicBool, Ordering };
use std::task::{ Context, Poll, Waker };
use fusion_data_rust::FusionResult;
use fusion_utils_rust::{ call_debug_enter, last_error };
use hilog_rust::{ debug, info, error, hilog, HiLogLabel, LogType };

/// Indicating data other than high-priority data can be read.
pub const LIBC_EPOLLIN: u32 = libc::EPOLLIN as u32;
const LIBC_EPOLLONESHOT: u32 = libc::EPOLLONESHOT as u32;
/// Indicating an error has occurred.
pub const LIBC_EPOLLERR: u32 = libc::EPOLLERR as u32;
/// Indicating a hangup has occurred.
pub const LIBC_EPOLLHUP: u32 = libc::EPOLLHUP as u32;
const LIBC_EPOLLALL: u32 = LIBC_EPOLLIN | LIBC_EPOLLERR | LIBC_EPOLLHUP;
const MAX_EPOLL_EVENTS: c_int = 128;
const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xD002220,
    tag: "Scheduler",
};

/// Abstraction of epoll handler.
pub trait IEpollHandler: Send + Sync {
    /// Return file descriptor of this epoll handler.
    fn fd(&self) -> RawFd;
    /// Dispatch epoll events to this epoll handler.
    fn dispatch(&self, events: u32);
}

struct EpollEvent {
    fd: RawFd,
    events: u32,
}

struct EpollHandler {
    raw: Arc<dyn IEpollHandler>,
    handle: ylong_runtime::task::JoinHandle<()>,
    waker: Option<Waker>,
    events: u32,
}

impl EpollHandler {
    fn new(raw: Arc<dyn IEpollHandler>, handle: ylong_runtime::task::JoinHandle<()>) -> Self
    {
        Self {
            raw,
            handle,
            waker: Default::default(),
            events: Default::default(),
        }
    }

    #[inline]
    fn fd(&self) -> RawFd
    {
        self.raw.fd()
    }

    #[inline]
    fn raw_handler(&self) -> Arc<dyn IEpollHandler>
    {
        self.raw.clone()
    }

    #[inline]
    fn set_waker(&mut self, waker: &Waker)
    {
        self.waker.replace(waker.clone());
    }

    #[inline]
    fn take_events(&mut self) -> u32
    {
        let events = self.events;
        self.events = Default::default();
        events
    }
}

impl Drop for EpollHandler {
    fn drop(&mut self)
    {
        self.handle.cancel();
    }
}

struct Driver {
    epoll: Arc<Epoll>,
    is_running: Arc<AtomicBool>,
}

impl Driver {
    fn new(epoll: Arc<Epoll>, is_running: Arc<AtomicBool>) -> Self
    {
        Self { epoll, is_running }
    }

    #[inline]
    fn is_running(&self) -> bool
    {
        self.is_running.load(Ordering::Relaxed)
    }

    fn run(&self)
    {
        call_debug_enter!("Driver::run");
        while self.is_running() {
            if let Some(epoll_events) = self.epoll.epoll_wait() {
                if !self.is_running() {
                    info!(LOG_LABEL, "Driver stopped running");
                    break;
                }
                self.epoll.wake(&epoll_events);
            }
        }
    }
}

struct Epoll {
    epoll_fd: RawFd,
    handlers: Mutex<BTreeMap<RawFd, EpollHandler>>,
}

impl Epoll {
    fn new() -> Self
    {
        let epoll_fd = unsafe { libc::epoll_create1(libc::EPOLL_CLOEXEC) };
        assert_ne!(epoll_fd, -1, "epoll_create1 fail: {}", last_error());
        Self {
            epoll_fd,
            handlers: Mutex::default(),
        }
    }

    #[inline]
    fn fd(&self) -> RawFd
    {
        self.epoll_fd
    }

    fn epoll_add(&self, fd: RawFd) -> FusionResult<()>
    {
        call_debug_enter!("Epoll::epoll_add");
        let mut ev = unsafe {
            MaybeUninit::<libc::epoll_event>::zeroed().assume_init()
        };
        ev.events = LIBC_EPOLLIN | LIBC_EPOLLONESHOT | LIBC_EPOLLHUP | LIBC_EPOLLERR;
        ev.u64 = fd as u64;

        let ret = unsafe {
            libc::epoll_ctl(self.epoll_fd, libc::EPOLL_CTL_ADD, fd, &mut ev)
        };
        if ret != 0 {
            error!(LOG_LABEL, "epoll_ctl_add({},{}) fail: {}",
                   @public(self.epoll_fd), @public(fd), @public(last_error()));
            Err(-1)
        } else {
            Ok(())
        }
    }

    fn epoll_del(&self, fd: RawFd) -> FusionResult<()>
    {
        call_debug_enter!("Epoll::epoll_del");
        let ret = unsafe {
            libc::epoll_ctl(self.epoll_fd, libc::EPOLL_CTL_DEL, fd, std::ptr::null_mut())
        };
        if ret != 0 {
            error!(LOG_LABEL, "epoll_ctl_remove({},{}) fail: {}",
                   @public(self.epoll_fd), @public(fd), @public(last_error()));
            Err(-1)
        } else {
            Ok(())
        }
    }

    fn epoll_wait(&self) -> Option<Vec<EpollEvent>>
    {
        call_debug_enter!("Epoll::epoll_wait");
        let mut events: Vec<libc::epoll_event> = Vec::with_capacity(MAX_EPOLL_EVENTS as usize);
        let ret = unsafe {
            libc::epoll_wait(self.epoll_fd, events.as_mut_ptr(), MAX_EPOLL_EVENTS, -1)
        };
        if ret < 0 {
            error!(LOG_LABEL, "epoll_wait({}) fail: {}", @public(self.epoll_fd), @public(last_error()));
            return None;
        }
        let num_of_events = ret as usize;
        let epoll_events = unsafe {
            std::slice::from_raw_parts(events.as_ptr(), num_of_events)
        };
        let epoll_events: Vec<EpollEvent> = epoll_events.iter().map(|e| {
            EpollEvent {
                fd: e.u64 as RawFd,
                events: e.events,
            }
        }).collect();
        Some(epoll_events)
    }

    fn epoll_reset(&self, fd: RawFd) -> FusionResult<()>
    {
        call_debug_enter!("Epoll::epoll_reset");
        let mut ev = unsafe { MaybeUninit::<libc::epoll_event>::zeroed().assume_init() };
        ev.events = LIBC_EPOLLIN | LIBC_EPOLLONESHOT | LIBC_EPOLLHUP | LIBC_EPOLLERR;
        ev.u64 = fd as u64;

        let ret = unsafe {
            libc::epoll_ctl(self.epoll_fd, libc::EPOLL_CTL_MOD, fd, &mut ev)
        };
        if ret != 0 {
            error!(LOG_LABEL, "in reset_fd, epoll_ctl_mod({},{}) fail: {}",
                   @public(self.epoll_fd), @public(fd), @public(last_error()));
            Err(-1)
        } else {
            Ok(())
        }
    }

    fn add_epoll_handler(&self, fd: RawFd, epoll_handler: EpollHandler) -> FusionResult<Arc<dyn IEpollHandler>>
    {
        call_debug_enter!("Epoll::add_epoll_handler");
        let mut guard = self.handlers.lock().unwrap();
        if guard.contains_key(&fd) {
            error!(LOG_LABEL, "Epoll handler ({}) has been added", @public(fd));
            return Err(-1);
        }
        debug!(LOG_LABEL, "Add epoll handler ({})", @public(fd));
        let raw = epoll_handler.raw_handler();
        guard.insert(fd, epoll_handler);
        let _ = self.epoll_add(fd);
        Ok(raw)
    }

    fn remove_epoll_handler(&self, fd: RawFd) -> FusionResult<Arc<dyn IEpollHandler>>
    {
        call_debug_enter!("Epoll::remove_epoll_handler");
        let mut guard = self.handlers.lock().unwrap();
        let _ = self.epoll_del(fd);
        if let Some(h) = guard.remove(&fd) {
            debug!(LOG_LABEL, "Remove epoll handler ({})", @public(fd));
            Ok(h.raw_handler())
        } else {
            error!(LOG_LABEL, "No epoll handler ({})", @public(fd));
            Err(-1)
        }
    }

    fn wake(&self, events: &[EpollEvent])
    {
        call_debug_enter!("Epoll::wake");
        let mut guard = self.handlers.lock().unwrap();
        for e in events {
            if let Some(handler) = guard.get_mut(&e.fd) {
                debug!(LOG_LABEL, "Wake epoll handler ({})", @public(e.fd));
                handler.events = e.events;
                if let Some(waker) = &handler.waker {
                    waker.wake_by_ref();
                }
            } else {
                error!(LOG_LABEL, "No epoll handler ({})", @public(e.fd));
            }
        }
    }

    fn dispatch_inner(&self, fd: RawFd, waker: &Waker) -> Option<(Arc<dyn IEpollHandler>, u32)>
    {
        call_debug_enter!("Epoll::dispatch_inner");
        let mut guard = self.handlers.lock().unwrap();
        if let Some(handler) = guard.get_mut(&fd) {
            handler.set_waker(waker);
            let events = handler.take_events() & LIBC_EPOLLALL;
            if events != 0 {
                Some((handler.raw_handler(), events))
            } else {
                debug!(LOG_LABEL, "No epoll event");
                None
            }
        } else {
            error!(LOG_LABEL, "No epoll handler with ({})", @public(fd));
            None
        }
    }

    fn dispatch(&self, fd: RawFd, waker: &Waker)
    {
        call_debug_enter!("Epoll::dispatch");
        if let Some((handler, events)) = self.dispatch_inner(fd, waker) {
            handler.dispatch(events);
            let _ = self.epoll_reset(fd);
        }
    }
}

impl Default for Epoll {
    fn default() -> Self
    {
        Self::new()
    }
}

impl Drop for Epoll {
    fn drop(&mut self)
    {
        let ret = unsafe { libc::close(self.epoll_fd) };
        if ret != 0 {
            error!(LOG_LABEL, "close({}) fail: {}", @public(self.epoll_fd), @public(last_error()));
        }
    }
}

struct EpollHandlerFuture {
    fd: RawFd,
    epoll: Arc<Epoll>,
}

impl EpollHandlerFuture {
    fn new(fd: RawFd, epoll: Arc<Epoll>) -> Self
    {
        Self { fd, epoll }
    }
}

impl Future for EpollHandlerFuture {
    type Output = ();

    fn poll(self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Self::Output>
    {
        call_debug_enter!("EpollHandlerFuture::poll");
        self.epoll.dispatch(self.fd, cx.waker());
        Poll::Pending
    }
}

struct EpollWaker {
    fds: [RawFd; 2],
}

impl EpollWaker {
    fn new() -> Self
    {
        let mut fds: [c_int; 2] = [-1; 2];

        let ret = unsafe {
            libc::pipe2(fds.as_mut_ptr(), libc::O_CLOEXEC | libc::O_NONBLOCK)
        };
        if ret != 0 {
            error!(LOG_LABEL, "pipe2 fail: {}", @public(last_error()));
        }
        Self { fds }
    }

    fn wake(&self)
    {
        call_debug_enter!("EpollWaker::wake");
        let data: i32 = 0;

        let ret = unsafe {
            libc::write(self.fds[1],
                        std::ptr::addr_of!(data) as *const c_void,
                        std::mem::size_of_val(&data))
        };
        if ret == -1 {
            error!(LOG_LABEL, "write fail: {}", @public(last_error()));
        }
    }
}

impl IEpollHandler for EpollWaker {
    fn fd(&self) -> RawFd
    {
        self.fds[0]
    }

    fn dispatch(&self, events: u32)
    {
        if (events & LIBC_EPOLLIN) == LIBC_EPOLLIN {
            let data: i32 = 0;

            let ret = unsafe {
                libc::read(self.fd(),
                           std::ptr::addr_of!(data) as *mut c_void,
                           std::mem::size_of_val(&data))
            };
            if ret == -1 {
                error!(LOG_LABEL, "read fail: {}", @public(last_error()));
            }
        }
    }
}

impl Default for EpollWaker {
    fn default() -> Self
    {
        Self::new()
    }
}

impl Drop for EpollWaker {
    fn drop(&mut self)
    {
        for fd in &mut self.fds {
            if *fd != -1 {
                let ret = unsafe { libc::close(*fd) };
                if ret != 0 {
                    error!(LOG_LABEL, "close({}) fail: {}", @public(*fd), @public(last_error()));
                }
            }
        }
    }
}

/// Bookkeeping of epoll handling.
pub struct Scheduler {
    epoll: Arc<Epoll>,
    epoll_waker: Arc<EpollWaker>,
    is_running: Arc<AtomicBool>,
    join_handle: Option<std::thread::JoinHandle<()>>,
}

impl Scheduler {
    pub(crate) fn new() -> Self
    {
        call_debug_enter!("Scheduler::new");
        let epoll: Arc<Epoll> = Arc::default();
        let is_running = Arc::new(AtomicBool::new(true));
        let driver = Driver::new(epoll.clone(), is_running.clone());
        let join_handle = std::thread::spawn(move || {
            driver.run();
        });
        let scheduler = Self {
            epoll,
            epoll_waker: Arc::default(),
            is_running,
            join_handle: Some(join_handle),
        };
        let _ = scheduler.add_epoll_handler(scheduler.epoll_waker.clone());
        scheduler
    }

    pub(crate) fn add_epoll_handler(&self, handler: Arc<dyn IEpollHandler>) -> FusionResult<Arc<dyn IEpollHandler>>
    {
        call_debug_enter!("Scheduler::add_epoll_handler");
        let fd: RawFd = handler.fd();
        let join_handle = ylong_runtime::spawn(
            EpollHandlerFuture::new(fd, self.epoll.clone())
        );
        self.epoll.add_epoll_handler(fd, EpollHandler::new(handler, join_handle))
    }

    pub(crate) fn remove_epoll_handler(&self, handler: Arc<dyn IEpollHandler>) -> FusionResult<Arc<dyn IEpollHandler>>
    {
        call_debug_enter!("Scheduler::remove_epoll_handler");
        self.epoll.remove_epoll_handler(handler.fd())
    }
}

impl Default for Scheduler {
    fn default() -> Self
    {
        Self::new()
    }
}

impl Drop for Scheduler {
    fn drop(&mut self)
    {
        call_debug_enter!("Scheduler::drop");
        self.is_running.store(false, Ordering::Relaxed);
        self.epoll_waker.wake();
        if let Some(join_handle) = self.join_handle.take() {
            let _ = join_handle.join();
        }
    }
}
