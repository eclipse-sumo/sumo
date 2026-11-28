/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    MSEventControlTest.cpp
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2009-11-23
///
// Tests the class MSEventControl
/****************************************************************************/
#include <config.h>

#include <atomic>
#include <functional>
#include <thread>
#include <gtest/gtest.h>
#include <microsim/MSEventControl.h>
#include <utils/common/CommandMock.h>


/* Test the method 'execute'. Tests if the execute method from the Command Class is called.*/

TEST(MSEventControl, test_method_execute) {

    MSEventControl eventControl;
    CommandMock* mock = new CommandMock();
    eventControl.addEvent(mock);

    EXPECT_FALSE(mock->isExecuteCalled());
    eventControl.execute(5);
    EXPECT_TRUE(mock->isExecuteCalled());
}


namespace {

class EventCallback : public Command {
public:
    explicit EventCallback(std::function<SUMOTime(SUMOTime)> callback) :
        myCallback(std::move(callback)) {
    }

    SUMOTime execute(SUMOTime time) override {
        return myCallback(time);
    }

private:
    std::function<SUMOTime(SUMOTime)> myCallback;
};

}


TEST(MSEventControl, concurrentEnqueue) {
    MSEventControl eventControl;
    constexpr int producerCount = 4;
    constexpr int commandsPerProducer = 1000;
    std::atomic<int> ready{0};
    int executed = 0;
    std::vector<std::thread> producers;
    for (int i = 0; i < producerCount; ++i) {
        producers.emplace_back([&] {
            ++ready;
            while (ready.load() < producerCount) {
                std::this_thread::yield();
            }
            for (int j = 0; j < commandsPerProducer; ++j) {
                eventControl.addEvent(new EventCallback([&executed](SUMOTime) {
                    ++executed;
                    return 0;
                }), 0);
            }
        });
    }
    for (std::thread& producer : producers) {
        producer.join();
    }

    eventControl.execute(0);
    EXPECT_EQ(producerCount * commandsPerProducer, executed);
    EXPECT_TRUE(eventControl.isEmpty());
}


TEST(MSEventControl, executeCanEnqueue) {
    MSEventControl eventControl;
    int executed = 0;
    eventControl.addEvent(new EventCallback([&](SUMOTime time) {
        ++executed;
        if (time == 0) {
            eventControl.addEvent(new EventCallback([&executed](SUMOTime) {
                ++executed;
                return 0;
            }), time);
            return DELTA_T;
        }
        return SUMOTime{0};
    }), 0);

    eventControl.execute(0);
    EXPECT_EQ(2, executed);
    EXPECT_FALSE(eventControl.isEmpty());
    eventControl.execute(DELTA_T);
    EXPECT_EQ(3, executed);
    EXPECT_TRUE(eventControl.isEmpty());
}
