/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2020-2020 German Aerospace Center (DLR) and others.
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
/// @file    TestUtilities.h
/// @author  Michael Behrisch
/// @date    2020-09-09
///
// Testing the threadpool implementation,
// based on https://github.com/vukis/Cpp-Utilities/tree/master/ThreadPool
/****************************************************************************/
#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <numeric>

inline void LoadCPUForRandomTime()
{
    // Sleeping the thread isn't good as it doesn't tie up the
    // CPU resource in the same way as actual work on a thread would do,
    // The OS is free to schedule work on the CPU while the thread is
    // sleeping. Hence we do some busy work. Note that volatile keyword
    // is necessary to prevent compiler from removing the below code.

    srand(0); // random sequences should be indentical

    volatile auto delay = rand() % static_cast<int>(1e5);
    while (delay != 0) {
        delay--;
    };
}

template<typename DurationT>
void LoadCPUFor(DurationT&& duration)
{
    // Sleeping the thread isn't good as it doesn't tie up the
    // CPU resource in the same way as actual work on a thread would do,
    // The OS is free to schedule work on the CPU while the thread is
    // sleeping. Hence we do some busy work. Note that volatile keyword
    // is necessary to prevent compiler from removing the below code.

    for (auto start = std::chrono::steady_clock::now(), now = start;
        now < start + duration;
        now = std::chrono::steady_clock::now())
    {
    }
}

template<
    typename TimeT = std::chrono::milliseconds,
    typename ClockT = std::chrono::steady_clock>
    class StopWatch
{
public:

    using TimeRep = TimeT;

    StopWatch()
    {
        Start();
    }

    void Start()
    {
        m_start = m_end = ClockT::now();
    }

    int Stop()
    {
        m_end = ClockT::now();
        return Elapsed();
    }

    int Elapsed() const
    {
        auto delta = std::chrono::duration_cast<TimeT>(m_end - m_start);
        m_history.push_back(delta);
        return (int)delta.count();
    }

    const std::vector<TimeT>& GetHistory() const
    {
        return m_history;
    }

private:
    std::chrono::time_point<ClockT> m_start;
    std::chrono::time_point<ClockT> m_end;
    mutable std::vector<TimeT>      m_history;
};

using StopWatchMs = StopWatch<>;

#define DO_BENCHMARK_TEST(repeatTimes, test) { \
std::cout << " - Benchmark test ( " << #test; \
StopWatchMs stopWatch; \
for (size_t n = 0; n < repeatTimes; ++n) { \
    stopWatch.Start(); \
    test(); \
    stopWatch.Stop(); \
} \
const auto history = stopWatch.GetHistory(); \
std::cout << " ) => " << (std::accumulate(history.begin(), history.end(), StopWatchMs::TimeRep{}) / repeatTimes).count() << " ms" << std::endl; \
}

#define DO_BENCHMARK_TEST_WITH_DESCRIPTION(description, repeatTimes, test, pool) { \
std::cout << " - Benchmark test ( " << #test << ", description: " << description; \
StopWatchMs stopWatch; \
for (size_t n = 0; n < repeatTimes; ++n) { \
    stopWatch.Start(); \
    test(pool); \
    stopWatch.Stop(); \
} \
const auto history = stopWatch.GetHistory(); \
std::cout << " ) => " << (std::accumulate(history.begin(), history.end(), StopWatchMs::TimeRep{}) / repeatTimes).count() << " ms" << std::endl; \
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
