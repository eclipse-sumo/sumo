/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    MFXWorkerThreadTest.cpp
/// @author  Michael Behrisch
/// @date    Oct 2010
///
// Tests the class MFXWorkerThread
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <gtest/gtest.h>
#include <utils/common/StdDefs.h>
#include <utils/foxtools/MFXWorkerThread.h>

class TestTask : public MFXWorkerThread::Task {
public:
    void run(MFXWorkerThread* /* context */) {
    }
};

// ===========================================================================
// test definitions
// ===========================================================================
/* Test the initialization.*/
TEST(MFXWorkerThread, test_init) {
    MFXWorkerThread::Pool g(4);
}

/* Test retrieving all tasks.*/
TEST(MFXWorkerThread, test_get_all) {
    MFXWorkerThread::Pool g(4);
    MFXWorkerThread::Task* task1 = new TestTask();
    MFXWorkerThread::Task* task2 = new TestTask();
    MFXWorkerThread::Task* task3 = new TestTask();
    MFXWorkerThread::Task* task4 = new TestTask();
    g.add(task1);
    g.add(task2);
    g.add(task3);
    g.add(task4);
    g.waitAll();
}
