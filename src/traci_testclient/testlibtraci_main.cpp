/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    testlibtraci_main.cpp
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
///
// Testing libtraci for C++
/****************************************************************************/
#ifdef _MSC_VER
// Avoid some noisy warnings with Visual Studio
#pragma warning(disable:4820 4514 5045 4668 4710)
#endif

#include <iostream>
#include <libsumo/libtraci.h>


// ===========================================================================
// main function
// ===========================================================================
int
main(int argc, char** argv) {
    std::vector<std::string> options;
    for (int i = 1; i < argc; i++) {
        options.push_back(argv[i]);
    }
    try {
        libtraci::Simulation::start(options);
        // libtraci::Simulation::start(options, -1, libsumo::DEFAULT_NUM_RETRIES, "default", true);
        std::cout << "Simulation started\n";
        for (int i = 0; i < 50; i++) {
            libtraci::Simulation::step();
        }
        libtraci::Simulation::close();
    } catch (const std::runtime_error& e) {
        std::cerr << "Could not start simulation: " << e.what() << "\n";
    }
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
