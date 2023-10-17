/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    testmantle_main.cpp
/// @author  Michael Behrisch
/// @date    2023-10-17
///
// Testing Mantle API
/****************************************************************************/
#ifdef _MSC_VER
// Avoid some noisy warnings with Visual Studio
#pragma warning(disable:4820 4514 5045 4710 4668)
#endif

#include <iostream>
#include <libsumo/libsumo.h>

#include <units.h>

#include "MantleAPI/Common/position.h"
#include "MantleAPI/Map/i_coord_converter.h"
#include "MantleAPI/Traffic/entity_properties.h"
#include "MantleAPI/Traffic/i_entity.h"
#include "MantleAPI/Traffic/i_entity_repository.h"
#include "test_utils.h"


class SUMOEnvironment : public mantle_api::MockEnvironment {
public:
    void CreateMap(const std::string& map_file_path, const mantle_api::MapDetails& /* map_details */) {
        libsumo::Simulation::load({map_file_path});
    }
};


// ===========================================================================
// main function
// ===========================================================================
int
main(int argc, char** argv) {
    std::vector<std::string> options;
    for (int i = 1; i < argc; i++) {
        options.push_back(argv[i]);
    }

    using namespace units::literals;

    mantle_api::Position inert_pos{};
    inert_pos = mantle_api::OpenDriveRoadPosition{"0", 0_m, 0_m};
    SUMOEnvironment env{};
    env.CreateMap(options.empty() ? "dummy_map_path" : options[0], {});

    mantle_api::VehicleProperties vehicle_properties;
    vehicle_properties.is_host = true;
    vehicle_properties.model = "G12";

    auto& repo = env.GetEntityRepository();
    auto& host_vehicle = repo.Create("host", vehicle_properties);
    const auto* const converter = env.GetConverter();
    auto world_pos = converter->Convert(inert_pos);
    host_vehicle.SetPosition(world_pos);
    host_vehicle.SetVisibility(mantle_api::EntityVisibilityConfig{true, false, true, {"radar"}});


    /*
    std::cout << "Simulation loaded\n";
    if (options.size() == 0 || (options[0] != "sumo" && options[0] != "sumo-gui")) {
        options.insert(options.begin(), "sumo");
    }
    libsumo::Simulation::start(options);
    std::cout << "Simulation started\n";
    libsumo::Simulation::close();
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
