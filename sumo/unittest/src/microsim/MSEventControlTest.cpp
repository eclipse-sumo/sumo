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
#include <microsim/MSEventControl.h>
#include "../utils/common/CommandMock.h"


/* Test the method 'execute'. Tests if the execute method from the Command Class is called.*/

TEST(MSEventControl, test_method_execute) {
    
    MSEventControl eventControl;
    CommandMock *mock = new CommandMock(); 
    eventControl.setCurrentTimeStep(4);
    eventControl.addEvent(mock,1,MSEventControl::ADAPT_AFTER_EXECUTION);
    
    EXPECT_FALSE(mock->isExecuteCalled());
    eventControl.execute(5);
    EXPECT_TRUE(mock->isExecuteCalled());
}



