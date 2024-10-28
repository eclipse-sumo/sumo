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
/// @file    testlibsumo_main.cpp
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
///
// Testing libsumo for C++
/****************************************************************************/
#ifdef _MSC_VER
// Avoid some noisy warnings with Visual Studio
#pragma warning(disable:4820 4514 5045 4710 4668)
#endif

#include <iostream>
#include <libsumo/libsumo.h>


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
    if (options.size() == 0 || (options[0] != "sumo" && options[0] != "sumo-gui")) {
        options.insert(options.begin(), "sumo");
    }
    libsumo::Simulation::start(options);
    std::cout << "Simulation started\n";
    libsumo::Simulation::close();
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
