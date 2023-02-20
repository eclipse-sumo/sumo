/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2023 German Aerospace Center (DLR) and others.
// TraaS module
// Copyright (C) 2013-2017 Dresden University of Technology
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
/// @file    APITest.java
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @date    2013
///
//
/****************************************************************************/
import org.eclipse.sumo.libtraci.*;

public class APITest {

    public static void main(String[] args) {
        if (System.getProperty("os.name").startsWith("Windows") && Simulation.class.toString().contains("libsumo")) {
            System.loadLibrary("iconv-2");
            System.loadLibrary("intl-8");
            System.loadLibrary("proj_9_0");
        }
        System.loadLibrary("libtracijni");
        String sumo_bin = "sumo";
        String config_file = "data/config.sumocfg";
        double step_length = 1;
        if (args.length > 0) {
            sumo_bin = args[0];
        }
        if (args.length > 1) {
            config_file = args[1];
        }

        Simulation.start(new StringVector(new String[] {sumo_bin,
                                          "-c", config_file,
                                          "--start"
                                                       }));


        double deltaT = Simulation.getDeltaT();
        System.out.println("deltaT:" + deltaT);
        TraCIStage stage = Simulation.findRoute("gneE0", "gneE2", "car", 0, 0);
        System.out.println("findRoute result stage:");
        for (String s : stage.getEdges()) {
            System.out.println("  " + s);
        }
        TraCIStageVector stages = Simulation.findIntermodalRoute("gneE0", "gneE2", "", 0, 0,
                                  1.5, 1, 50, 50, 0, "", "", "");
        System.out.println("findIntermodalRoute result stages:");
        for (TraCIStage s2 : stages) {
            for (String s : s2.getEdges()) {
                System.out.println("  " + s);
            }

        }
        StringVector walk = new StringVector();
        walk.add("gneE2");
        walk.add("gneE3");
        for (int i = 0; i < 36; i++) {

            Simulation.step();
            Vehicle.add("v" + i, "r1", "car", "now", "0", "0", "max", "current", "max", "current", "", "", "", 0, 0);
            double timeSeconds = Simulation.getTime();
            int tlsPhase = TrafficLight.getPhase("gneJ1");
            String tlsPhaseName = TrafficLight.getPhaseName("gneJ1");
            System.out.println(String.format("Step %s, tlsPhase %s (%s)", timeSeconds, tlsPhase, tlsPhaseName));

            TraCIVehicleDataVector vehData = InductionLoop.getVehicleData("loop1");
            for (TraCIVehicleData d : vehData) {
                System.out.println(String.format("  veh=%s len=%s entry=%s leave=%s type=%s",
                                                 d.getId(), d.getLength(), d.getEntryTime(), d.getLeaveTime(), d.getTypeID()));
            }
            if (i == 10) {
                Vehicle.setStop("v0", "gneE0", 200, 0, 10, 0, 10, -1);
            }
            Person.add("p" + i, "gneE2", 10.0, libtraci.getDEPARTFLAG_NOW(), "DEFAULT_PEDTYPE");
            Person.appendWalkingStage("p" + i, walk, 50.0, -1, -1.0, "");
        }
        StringVector vehIDs = Vehicle.getIDList();
        String vehIDsStr = "";
        for (String id : vehIDs) {
            vehIDsStr += (id + " ");
        }
        System.out.println("VehicleIDs: " + vehIDsStr);

        Vehicle.setParameter("v0", "vehParam", "vehValue");
        System.out.println("vehicle.getParameter: " + Vehicle.getParameter("v0", "vehParam"));

        Vehicle.setAdaptedTraveltime("v0", "gneE0", 200, 100, 200);
        System.out.println("VehicleType.getAdaptedTraveltime: " + Vehicle.getAdaptedTraveltime("v0", 150, "gneE0"));

        System.out.println("Vehicle.getRoute: " + Vehicle.getRoute("v0"));
        StringVector route = new StringVector();
        route.add("gneE0");
        Vehicle.setRoute("v0", route);
        System.out.println("Vehicle.getRoute: " + Vehicle.getRoute("v0"));
        Vehicle.setRouteID("v0", "r1");
        System.out.println("Vehicle.getRoute: " + Vehicle.getRoute("v0"));

        TraCINextTLSVector ntlsV = Vehicle.getNextTLS("v0");
        for (TraCINextTLSData t : ntlsV) {
            System.out.println("Vehicle.getNextTLS: id=" + t.getId() + " dist=" + t.getDist() + " state=" + t.getState());
        }

        VehicleType.setParameter("car", "vehtypeParam", "vehtypeValue");
        System.out.println("VehicleType.getParameter: " + VehicleType.getParameter("car", "vehtypeParam"));

        StringVector personIDs = Person.getIDList();
        String personIDsStr = "";
        for (String id : personIDs) {
            personIDsStr += (id + " ");
        }
        System.out.println("PersonIDs: " + personIDsStr);

        TraCIStage personStage = Person.getStage("p0", 0);
        System.out.println("Person.getStage: " + personStage.getType());

        Edge.setParameter("gneE0", "edgeParam", "edgeValue");
        System.out.println("Edge.getParameter: " + Edge.getParameter("gneE0", "edgeParam"));

        Edge.adaptTraveltime("gneE0", 123, 2000, 4000);
        System.out.println("Edge.getAdaptatedTraveltime: " + Edge.getAdaptedTraveltime("gneE0", 3000));
        System.out.println("Edge.getAdaptatedTraveltime: " + Edge.getAdaptedTraveltime("gneE0", 1000));
        System.out.println("Edge.getAdaptatedTraveltime: " + Edge.getAdaptedTraveltime("gneE0", 5000));

        Edge.setEffort("gneE0", 246, 2000, 4000);
        System.out.println("Edge.getEffort: " + Edge.getEffort("gneE0", 3000));
        System.out.println("Edge.getEffort: " + Edge.getEffort("gneE0", 1000));
        System.out.println("Edge.getEffort: " + Edge.getEffort("gneE0", 5000));

        Lane.setParameter("gneE0_1", "laneParam", "laneValue");
        System.out.println("Lane.getParameter: " + Lane.getParameter("gneE0_1", "laneParam"));

        Route.setParameter("r1", "routeParam", "routeValue");
        System.out.println("Route.getParameter: " + Route.getParameter("r1", "routeParam"));

        POI.setParameter("t0", "poiParam", "poiValue");
        System.out.println("Poi.getParameter: " + POI.getParameter("t0", "poiParam"));

        StringVector controlledJunctions = TrafficLight.getControlledJunctions("gneJ1");
        System.out.println("TrafficLight.getControlledJunctions: " + controlledJunctions);

        TrafficLight.setParameter("gneJ1", "tlsParam", "tlsValue");
        System.out.println("TrafficLight.getParameter: " + TrafficLight.getParameter("gneJ1", "tlsParam"));

        TrafficLight.setPhaseDuration("gneJ1", 15);

        TraCILogicVector tlsc = TrafficLight.getCompleteRedYellowGreenDefinition("gneJ1");
        for (TraCILogic tls : tlsc) {
            System.out.println("TrafficLight.getCompleteRedYellowGreenDefinition: " + tls.getProgramID());
        }

        TraCILogic tlsp = tlsc.get(0);
        TrafficLight.setCompleteRedYellowGreenDefinition("gneJ1", tlsp);

        System.out.println("TrafficLight.getPhaseDuration: " + (double)TrafficLight.getPhaseDuration("gneJ1"));
        System.out.println("TrafficLight.getNextSwitch: " + (double)TrafficLight.getNextSwitch("gneJ1"));

        TraCIRoadPosition roadPos = Simulation.convertRoad(505.38, -42.08, false, "passenger");
        System.out.println("Simulation.convertRoad: " + roadPos.getEdgeID() + ", " + roadPos.getPos() + ", " + roadPos.getLaneIndex());

        TraCIPosition geoPos = Simulation.convertGeo(505.38, -42.08, false);
        System.out.println("Simulation.convertGeo: " + geoPos.getX() + ", " + geoPos.getY());

        for (TraCIConnection c : Lane.getLinks(":gneJ1_6_0")) {
            System.out.println("Lane.getLinks to=" + c.getApproachedLane() + " via=" + c.getApproachedInternal() + " prio=" + c.getHasPrio() + " open=" + c.getIsOpen());
        }

        Simulation.close();

        Simulation.start(new StringVector(new String[] {sumo_bin,
                                          "-c", config_file,
                                          "--start"
                                                       }));

        Simulation.saveState("file-state-now");
        Simulation.close();
    }

}
