/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    testlibsumo_main.cpp
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Testing libsumo for C++
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <iostream>
#include <libsumo/Simulation.h>
#include <utils/common/ToString.h>



// ===========================================================================
// main function
// ===========================================================================
int
main(int argc, char** argv) {
//    std::vector<std::string> options;
//    for (int i = 1; i < argc; i++) {
//        options.push_back(argv[i]);
//    }
//	libsumo::Simulation::load(options);
//    std::cout << "Simulation loaded\n";
#ifdef _DEBUG
  std::cout<<"routing as if just dont care"<<std::endl;
#endif
  std::vector<std::string> options;
    for (int i = 1; i < argc; i++) {
        options.push_back(argv[i]);
    }
    libsumo::Simulation::load(options);
    std::cout << "Simulation loaded\n";
  std::vector<libsumo::TraCIStage> result = libsumo::Simulation::findIntermodalRoute("A1A0", "B0B1", "public", 0, 3, 0.5, 0.75, 444, 400,0,"ped");
  
  for (const auto& stage : result) {
        std::cout << " type=" << stage.type << " line=" << stage.line << " travelTime=" << stage.travelTime << " cost=" << stage.cost << " destination: "<< stage.destStop<<"\n";
    }
}


/****************************************************************************/
