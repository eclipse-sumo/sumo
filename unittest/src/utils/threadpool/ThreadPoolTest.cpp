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
/// @file    ThreadPoolTest.cpp
/// @author  Michael Behrisch
/// @date    2020-09-09
///
// Testing the threadpool implementation,
// based on https://github.com/vukis/Cpp-Utilities/tree/master/ThreadPool
/****************************************************************************/
#include <config.h>

#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <numeric>

#include <utils/common/StopWatch.h>
#include <utils/threadpool/WorkStealingThreadPool.h>


inline void LoadCPUForRandomTime() {
    // Sleeping the thread isn't good as it doesn't tie up the
    // CPU resource in the same way as actual work on a thread would do,
    // The OS is free to schedule work on the CPU while the thread is
    // sleeping. Hence we do some busy work. Note that volatile keyword
    // is necessary to prevent compiler from removing the below code.

    srand(0); // random sequences should be identical

    volatile auto delay = rand() % static_cast<int>(1e5);
    while (delay != 0) {
        delay--;
    }
}

template<typename DurationT>
void LoadCPUFor(DurationT&& duration) {
    for (auto start = std::chrono::steady_clock::now(), now = start;
            now < start + duration;
            now = std::chrono::steady_clock::now()) {
    }
}

#define DO_BENCHMARK_TEST_WITH_DESCRIPTION(description, repeatTimes, test, pool) { \
std::cout << " - Benchmark test ( " << #test << ", description: " << description; \
StopWatch<> stopWatch; \
for (size_t n = 0; n < repeatTimes; ++n) { \
    stopWatch.start(); \
    test(pool); \
    stopWatch.stop(); \
} \
std::cout << " ) => " << stopWatch.getAverage() << " ms" << std::endl; \
}

#define TEST_ASSERT(expr) \
if (!(expr)) { \
    std::ostringstream ss; \
    ss << __FILE__ << ":" <<__LINE__ << " " << #expr; \
    throw std::runtime_error(ss.str()); \
}

#define DO_TEST(test, pool)  { \
std::cout << " - Test ( " << #test << " " << #pool; \
try \
{ \
    test(pool); \
} \
catch (const std::exception& e) \
{ \
    std::cout << " => failed with: " << e.what() << " )" << std::endl; \
    throw; \
} \
std::cout << " => succeed )" << std::endl; \
}


void Test_TaskResultIsAsExpected(WorkStealingThreadPool<>& taskSystem) {
    constexpr size_t taskCount = 10000;

    std::vector<std::future<size_t>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(taskSystem.executeAsync([i](int) {
        return i * i;
    }));

    for (size_t i = 0; i < taskCount; ++i) {
        TEST_ASSERT(i * i == results[i].get());
    }
}

void Test_RandomTaskExecutionTime(WorkStealingThreadPool<>& taskSystem) {
    constexpr size_t taskCount = 10000;

    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(taskSystem.executeAsync([](int) {
        LoadCPUForRandomTime();
    }));

    for (auto& result : results) {
        result.wait();
    }
}

void Test_1nsTaskExecutionTime(WorkStealingThreadPool<>& taskSystem) {
    constexpr size_t taskCount = 10000;
    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(taskSystem.executeAsync([](int) {
        LoadCPUFor(std::chrono::nanoseconds(1));
    }));

    for (auto& result : results) {
        result.wait();
    }
}

void Test_100msTaskExecutionTime(WorkStealingThreadPool<>& taskSystem) {
    constexpr size_t taskCount = 10;
    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(taskSystem.executeAsync([](int) {
        LoadCPUFor(std::chrono::milliseconds(100));
    }));

    for (auto& result : results) {
        result.wait();
    }
}

void Test_EmptyTask(WorkStealingThreadPool<>& taskSystem) {
    constexpr size_t taskCount = 10000;

    std::vector<std::future<void>> results;

    for (size_t i = 0; i < taskCount; ++i)
        results.push_back(taskSystem.executeAsync([](int) {}));

    for (auto& result : results) {
        result.wait();
    }
}

template<class TaskT>
void RepeatTask(WorkStealingThreadPool<>& taskSystem, TaskT&& task, size_t times) {
    std::vector<std::future<void>> results;

    // Here we need not to std::forward just copy task.
    // Because if the universal reference of task has bound to an r-value reference
    // then std::forward will have the same effect as std::move and thus task is not required to contain a valid task.
    // Universal reference must only be std::forward'ed a exactly zero or one times.
    for (size_t i = 0; i < times; ++i) {
        results.push_back(taskSystem.executeAsync(task));
    }

    for (auto& result : results) {
        result.wait();
    }
}

void Test_MultipleTaskProducers(WorkStealingThreadPool<>& taskSystem) {
    constexpr size_t taskCount = 1000;

    std::vector<std::thread> taskProducers{ std::max(1u, std::thread::hardware_concurrency()) };

    for (auto& producer : taskProducers)
        producer = std::thread([&] { RepeatTask(taskSystem, [](int) {
        LoadCPUForRandomTime();
    }, taskCount);
                               });

    for (auto& producer : taskProducers) {
        if (producer.joinable()) {
            producer.join();
        }
    }
}

int main() {
    std::vector<int> pseudoContext(std::thread::hardware_concurrency(), 0);
    WorkStealingThreadPool<int> stealingTaskSystem(true, pseudoContext);
    WorkStealingThreadPool<int> multiQueueTaskSystem(false, pseudoContext);

    std::cout << "==========================================" << std::endl;
    std::cout << "             FUNCTIONAL TESTS             " << std::endl;
    std::cout << "==========================================" << std::endl;
    DO_TEST(Test_TaskResultIsAsExpected, multiQueueTaskSystem);
    DO_TEST(Test_TaskResultIsAsExpected, stealingTaskSystem);
    std::cout << std::endl;

    std::cout << "==========================================" << std::endl;
    std::cout << "            PERFORMANCE TESTS             " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Number of cores: " << std::thread::hardware_concurrency() << std::endl;
    constexpr size_t NumOfRuns = 10;
    std::cout << std::endl;
    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on multiple task queues", NumOfRuns, Test_RandomTaskExecutionTime, multiQueueTaskSystem);
    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on work stealing queue ", NumOfRuns, Test_RandomTaskExecutionTime, stealingTaskSystem);
    std::cout << std::endl;

    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on multiple task queues", NumOfRuns, Test_1nsTaskExecutionTime, multiQueueTaskSystem);
    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on work stealing queue ", NumOfRuns, Test_1nsTaskExecutionTime, stealingTaskSystem);
    std::cout << std::endl;

    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on multiple task queues", NumOfRuns, Test_100msTaskExecutionTime, multiQueueTaskSystem);
    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on work stealing queue ", NumOfRuns, Test_100msTaskExecutionTime, stealingTaskSystem);
    std::cout << std::endl;

    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on multiple task queues", NumOfRuns, Test_EmptyTask, multiQueueTaskSystem);
    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on work stealing queue ", NumOfRuns, Test_EmptyTask, stealingTaskSystem);
    std::cout << std::endl;

    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on multiple task queues", NumOfRuns, Test_MultipleTaskProducers, multiQueueTaskSystem);
    DO_BENCHMARK_TEST_WITH_DESCRIPTION("thread pool based on work stealing queue ", NumOfRuns, Test_MultipleTaskProducers, stealingTaskSystem);
    std::cout << std::endl;

    return 0;
}
