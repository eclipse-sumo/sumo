/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    test_lib.cpp
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Testing libsumo for C++
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <libsumo/Simulation.h>


// ===========================================================================
// main function
// ===========================================================================
int
main(int argc, char** argv) {
	libsumo::Simulation::load(std::vector<std::string>());
}


/****************************************************************************/
