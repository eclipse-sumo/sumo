/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
    std::vector<std::string> options;
    for (int i = 1; i < argc; i++) {
        options.push_back(argv[i]);
    }
    libsumo::Simulation::load(options);
    std::cout << "Simulation loaded\n";
    /*
      std::vector<libsumo::TraCIStage> result = libsumo::Simulation::findIntermodalRoute("64455492", "-22913705", "public", 21600, 3, -1, -1, 0, 0,0,"ped");
      double cost = 0;
      double time = 0;
      for (const auto& stage : result)
      {
            std::cout << " type=" << stage.type << " line=" << stage.line << " travelTime=" << stage.travelTime << " cost=" << stage.cost << " destination: "<< stage.destStop<<"\n";
            std::cout << "Descr:\n" << stage.description<< std::endl<<std::endl;
            cost += stage.cost;
            time += stage.travelTime;
      }
        std::cout<<"end cost: "<<cost<<std::endl;
      std::cout<<"end time: "<<time<<std::endl;
    */
}


/****************************************************************************/
