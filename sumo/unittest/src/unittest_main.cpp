/****************************************************************************/
/// @file    unittest_main.cpp
/// @author  Matthias Heppner
/// @author  Michael Behrisch
/// @date    Sept 2009
/// @version $Id$
///
// main for the unit tests
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <gtest/gtest.h>


// ===========================================================================
// method definitions
// ===========================================================================
int main(int argc, char **argv) {
  std::cout << "Running unit tests\n";

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS(); 
}
