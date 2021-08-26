/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2020-2021 German Aerospace Center (DLR) and others.
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
/// @file    WorkStealingThreadPool.h
/// @author  Michael Behrisch
/// @date    2020-09-09
///
// Threadpool implementation,
// based on https://github.com/vukis/Cpp-Utilities/tree/master/ThreadPool
/****************************************************************************/
#pragma once
#include <config.h>

#include "TaskQueue.h"
#include <algorithm>
#include <thread>

template<typename CONTEXT = int>
class WorkStealingThreadPool {
public:

    explicit WorkStealingThreadPool(const bool workSteal, const std::vector<CONTEXT>& context)
        : myQueues{ context.size() }, myTryoutCount(workSteal ? 1 : 0) {
        size_t index = 0;
        for (const CONTEXT& c : context) {
            if (workSteal) {
                myThreads.emplace_back([this, index, c] { workStealRun(index, c); });
            } else {
                myThreads.emplace_back([this, index, c] { run(index, c); });
            }
            index++;
        }
    }

    ~WorkStealingThreadPool() {
        for (auto& queue : myQueues) {
            queue.setEnabled(false);
        }
        for (auto& thread : myThreads) {
            thread.join();
        }
    }

    template<typename TaskT>
    auto executeAsync(TaskT&& task, int idx = -1) -> std::future<decltype(task(std::declval<CONTEXT>()))> {
        const auto index = idx == -1 ? myQueueIndex++ : idx;
        if (myTryoutCount > 0) {
            for (size_t n = 0; n != myQueues.size() * myTryoutCount; ++n) {
                // Here we need not to std::forward just copy task.
                // Because if the universal reference of task has bound to an r-value reference
                // then std::forward will have the same effect as std::move and thus task is not required to contain a valid task.
                // Universal reference must only be std::forward'ed a exactly zero or one times.
                bool success = false;
                auto result = myQueues[(index + n) % myQueues.size()].tryPush(task, success);

                if (success) {
                    return result;
                }
            }
        }
        return myQueues[index % myQueues.size()].push(std::forward<TaskT>(task));
    }

    void waitAll() {
        std::vector<std::future<void>> results;
        for (int n = 0; n != (int)myQueues.size(); ++n) {
            results.push_back(executeAsync([](CONTEXT) {}, n));
        }
        for (auto& r : results) {
            r.wait();
        }
    }

private:
    void run(size_t queueIndex, const CONTEXT& context) {
        while (myQueues[queueIndex].isEnabled()) {
            typename TaskQueue<CONTEXT>::TaskPtrType task;
            if (myQueues[queueIndex].waitAndPop(task)) {
                task->exec(context);
            }
        }
    }

    void workStealRun(size_t queueIndex, const CONTEXT& context) {
        while (myQueues[queueIndex].isEnabled()) {
            typename TaskQueue<CONTEXT>::TaskPtrType task;
            for (size_t n = 0; n != myQueues.size()*myTryoutCount; ++n) {
                if (myQueues[(queueIndex + n) % myQueues.size()].tryPop(task)) {
                    break;
                }
            }
            if (!task && !myQueues[queueIndex].waitAndPop(task)) {
                return;
            }
            task->exec(context);
        }
    }

private:
    std::vector<TaskQueue<CONTEXT> > myQueues;
    std::atomic<size_t>    myQueueIndex{ 0 };
    const size_t myTryoutCount;
    std::vector<std::thread> myThreads;
};
