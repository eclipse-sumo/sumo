/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSEventControlTest.cpp
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2009-11-23
/// @version $Id$
///
// Tests the class MSEventControl
/****************************************************************************/

#include <gtest/gtest.h>
#include <microsim/MSEventControl.h>
#include "../utils/common/CommandMock.h"


/* Test the method 'execute'. Tests if the execute method from the Command Class is called.*/

TEST(MSEventControl, test_method_execute) {

    MSEventControl eventControl;
    CommandMock* mock = new CommandMock();
    eventControl.setCurrentTimeStep(4);
    eventControl.addEvent(mock);

    EXPECT_FALSE(mock->isExecuteCalled());
    eventControl.execute(5);
    EXPECT_TRUE(mock->isExecuteCalled());
}
