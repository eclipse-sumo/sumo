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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include <gtest/gtest.h>
#include <netbuild/NBTrafficLightLogic.h>
#include "../utils/iodevices/OutputDeviceMock.h"


/* Test the method 'getDuration'*/
TEST(NBTrafficLightLogic, test_method_getDuration) {
    NBTrafficLightLogic tll("1","11",2);
    EXPECT_EQ(0, tll.getDuration());

    tll.addStep(3,"gr");
    tll.addStep(5,"rG");
    EXPECT_EQ(8, tll.getDuration());
}
