/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2020-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    TaskQueue.h
/// @author  Michael Behrisch
/// @date    2020-09-09
///
// Threadpool implementation,
// based on https://github.com/vukis/Cpp-Utilities/tree/master/ThreadPool
/****************************************************************************/
#pragma once
#include <config.h>

#include <condition_variable>
#include <functional>
#include <queue>
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355 5204 5220) // mask warnings in MSVCs ppl-stdlib
#endif
#include <future>
#ifdef _MSC_VER
#pragma warning(pop)
#endif


template <typename C>
class TaskBase {
public:
    virtual ~TaskBase() = default;
    virtual void exec(const C& context) = 0;
};

template <typename T, typename C>
class Task : public TaskBase<C> {
public:
    Task(T&& t) : task(std::move(t)) {}
    void exec(const C& context) override {
        task(context);
    }

    T task;
};

template <typename C>
class TaskQueue {
    using LockType = std::unique_lock<std::mutex>;

public:
    using TaskPtrType = std::unique_ptr<TaskBase<C> >;
    TaskQueue() = default;
    ~TaskQueue() = default;

    void setEnabled(bool enabled) {
        {
            LockType lock{ myMutex };
            myEnabled = enabled;
        }
        if (!enabled) {
            myReady.notify_all();
        }
    }

    bool isEnabled() const {
        LockType lock{ myMutex };
        return myEnabled;
    }

    bool waitAndPop(TaskPtrType& task) {
        LockType lock{ myMutex };
        myReady.wait(lock, [this] { return !myEnabled || !myQueue.empty(); });
        if (myEnabled && !myQueue.empty()) {
            task = std::move(myQueue.front());
            myQueue.pop();
            return true;
        }
        return false;
    }

    template <typename TaskT>
    auto push(TaskT&& task) -> std::future<decltype(task(std::declval<C>()))> {
        using PkgTask = std::packaged_task<decltype(task(std::declval<C>()))(C)>;
        auto job = std::unique_ptr<Task<PkgTask, C>>(new Task<PkgTask, C>(PkgTask(std::forward<TaskT>(task))));
        auto future = job->task.get_future();
        {
            LockType lock{ myMutex };
            myQueue.emplace(std::move(job));
        }

        myReady.notify_one();
        return future;
    }

    bool tryPop(TaskPtrType& task) {
        LockType lock{ myMutex, std::try_to_lock };
        if (!lock || !myEnabled || myQueue.empty()) {
            return false;
        }
        task = std::move(myQueue.front());
        myQueue.pop();
        return true;
    }

    template <typename TaskT>
    auto tryPush(TaskT&& task, bool& success) -> std::future<decltype(task(std::declval<C>()))> {
        std::future<decltype(task(std::declval<C>()))> future;
        success = false;
        {
            LockType lock{ myMutex, std::try_to_lock };
            if (!lock) {
                return future;
            }
            using PkgTask = std::packaged_task<decltype(task(std::declval<C>()))(C)>;
            auto job = std::unique_ptr<Task<PkgTask, C>>(new Task<PkgTask, C>(PkgTask(std::forward<TaskT>(task))));
            future = job->task.get_future();
            success = true;
            myQueue.emplace(std::move(job));
        }

        myReady.notify_one();
        return future;
    }

private:
    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;

    std::queue<TaskPtrType> myQueue;
    bool myEnabled = true;
    mutable std::mutex myMutex;
    std::condition_variable myReady;
};
