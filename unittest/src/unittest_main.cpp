/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    unittest_main.cpp
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @date    Sept 2009
/// @version $Id$
///
// main for the unit tests
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <gtest/gtest.h>


// ===========================================================================
// method definitions
// ===========================================================================
int main(int argc, char** argv) {
    std::cout << "Running unit tests\n";

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
