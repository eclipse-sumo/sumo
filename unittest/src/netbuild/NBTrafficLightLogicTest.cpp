/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBTrafficLightLogicTest.cpp
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2009-10-24
/// @version $Id$
///
// Tests the class NBTrafficLightLogic
/****************************************************************************/

#include <gtest/gtest.h>
#include <netbuild/NBTrafficLightLogic.h>
#include "../utils/iodevices/OutputDeviceMock.h"


/* Test the method 'getDuration'*/
TEST(NBTrafficLightLogic, test_method_getDuration) {
    NBTrafficLightLogic tll("1", "11", 2);
    EXPECT_EQ(0, tll.getDuration());

    tll.addStep(3, "gr");
    tll.addStep(5, "rG");
    EXPECT_EQ(8, tll.getDuration());
}
