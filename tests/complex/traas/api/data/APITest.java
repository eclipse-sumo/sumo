/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2020 German Aerospace Center (DLR) and others.
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
/// @date    2013
///
//
/****************************************************************************/
import java.util.LinkedList;
import it.polito.appeal.traci.SumoTraciConnection;
import de.tudresden.sumo.cmd.Simulation;
import de.tudresden.sumo.cmd.Vehicle;
import de.tudresden.sumo.cmd.Vehicletype;
import de.tudresden.sumo.cmd.Edge;
import de.tudresden.sumo.cmd.Lane;
import de.tudresden.sumo.cmd.Route;
import de.tudresden.sumo.cmd.Poi;
import de.tudresden.sumo.cmd.Polygon;
import de.tudresden.sumo.cmd.Person;
import de.tudresden.sumo.cmd.Inductionloop;
import de.tudresden.sumo.cmd.Trafficlight;
import de.tudresden.ws.container.SumoVehicleData;
import de.tudresden.ws.container.SumoStopFlags;
import de.tudresden.ws.container.SumoStringList;
import de.tudresden.ws.container.SumoStage;
import de.tudresden.ws.container.SumoPosition2D;
import de.tudresden.ws.container.SumoRoadPosition;
import de.tudresden.ws.container.SumoTLSController;
import de.tudresden.ws.container.SumoTLSProgram;

public class APITest {
    public static void main(String[] args) {
        String sumo_bin = "sumo";
        String config_file = "data/config.sumocfg";
        double step_length = 1.0;

        if (args.length > 0) {
            sumo_bin = args[0];
        }
        if (args.length > 1) {
            config_file = args[1];
        }

        try {
            SumoTraciConnection conn = new SumoTraciConnection(sumo_bin, config_file);
            conn.addOption("step-length", step_length + "");
            conn.addOption("start", "true"); //start sumo immediately

            //start Traci Server
            conn.runServer();
            conn.setOrder(1);

            SumoStage stage = (SumoStage)conn.do_job_get(Simulation.findRoute("gneE0", "gneE2", "car", 0, 0));
            System.out.println("findRoute result stage:");
            for (String s : stage.edges) {
                System.out.println("  " + s);
            }
            LinkedList<SumoStage> stages = (LinkedList<SumoStage>)conn.do_job_get(Simulation.findIntermodalRoute("gneE0", "gneE2", "", 0, 0,
                                           1.5, 1, 50, 50, 0, "", "", ""));
            System.out.println("findIntermodalRoute result stages:");
            for (SumoStage s2 : stages) {
                for (String s : s2.edges) {
                    System.out.println("  " + s);
                }

            }
            SumoStringList walk = new SumoStringList();
            walk.add("gneE2");
            walk.add("gneE3");
            for (int i = 0; i < 36; i++) {

                conn.do_timestep();
                conn.do_job_set(Vehicle.addFull("v" + i, "r1", "car", "now", "0", "0", "max", "current", "max", "current", "", "", "", 0, 0));
                double timeSeconds = (double)conn.do_job_get(Simulation.getTime());
                int tlsPhase = (int)conn.do_job_get(Trafficlight.getPhase("gneJ1"));
                String tlsPhaseName = (String)conn.do_job_get(Trafficlight.getPhaseName("gneJ1"));
                System.out.println(String.format("Step %s, tlsPhase %s (%s)", timeSeconds, tlsPhase, tlsPhaseName));

                SumoVehicleData vehData = (SumoVehicleData)conn.do_job_get(Inductionloop.getVehicleData("loop1"));
                for (SumoVehicleData.VehicleData d : vehData.ll) {
                    System.out.println(String.format("  veh=%s len=%s entry=%s leave=%s type=%s", d.vehID, d.length, d.entry_time, d.leave_time, d.typeID));
                }
                if (i == 10) {
                    SumoStopFlags flags = new SumoStopFlags(false, false, false, false, false, false, false);
                    conn.do_job_set(Vehicle.setStop("v0", "gneE0", 200, (byte)0, 10, flags, 10, -1));
                }
                conn.do_job_set(Person.add("p" + i, "gneE2", 10.0, 0, "DEFAULT_PEDTYPE"));
                conn.do_job_set(Person.appendWalkingStage("p" + i, walk, 50.0, -1, -1.0, ""));
            }
            SumoStringList vehIDs = (SumoStringList)conn.do_job_get(Vehicle.getIDList());
            String vehIDsStr = "";
            for (String id : vehIDs) {
                vehIDsStr += (id + " ");
            }
            System.out.println("VehicleIDs: " + vehIDsStr);

            conn.do_job_set(Vehicle.setParameter("v0", "vehParam", "vehValue"));
            System.out.println("vehicle.getParameter: " + (String)conn.do_job_get(Vehicle.getParameter("v0", "vehParam")));

            conn.do_job_set(Vehicle.setAdaptedTraveltime("v0", 100, 200, "gneE0", 200));
            System.out.println("Vehicletype.getAdaptedTraveltime: " + (double)conn.do_job_get(Vehicle.getAdaptedTraveltime("v0", 150, "gneE0")));

            System.out.println("Vehicle.getRoute: " + conn.do_job_get(Vehicle.getRoute("v0")));
            SumoStringList route = new SumoStringList();
            route.add("gneE0");
            conn.do_job_set(Vehicle.setRoute("v0", route));
            System.out.println("Vehicle.getRoute: " + conn.do_job_get(Vehicle.getRoute("v0")));
            conn.do_job_set(Vehicle.setRouteID("v0", "r1"));
            System.out.println("Vehicle.getRoute: " + conn.do_job_get(Vehicle.getRoute("v0")));

            conn.do_job_set(Vehicletype.setParameter("car", "vehtypeParam", "vehtypeValue"));
            System.out.println("Vehicletype.getParameter: " + (String)conn.do_job_get(Vehicletype.getParameter("car", "vehtypeParam")));

            SumoStringList personIDs = (SumoStringList)conn.do_job_get(Person.getIDList());
            String personIDsStr = "";
            for (String id : personIDs) {
                personIDsStr += (id + " ");
            }
            System.out.println("PersonIDs: " + personIDsStr);

            SumoStage personStage = (SumoStage)conn.do_job_get(Person.getStage("p0", 0));
            System.out.println("Person.getStage: " + personStage.type);

            conn.do_job_set(Edge.setParameter("gneE0", "edgeParam", "edgeValue"));
            System.out.println("Edge.getParameter: " + (String)conn.do_job_get(Edge.getParameter("gneE0", "edgeParam")));

            conn.do_job_set(Edge.adaptTraveltime("gneE0", 123, 2000, 4000));
            System.out.println("Edge.getAdaptatedTraveltime: " + (double)conn.do_job_get(Edge.getAdaptedTraveltime("gneE0", 3000)));
            System.out.println("Edge.getAdaptatedTraveltime: " + (double)conn.do_job_get(Edge.getAdaptedTraveltime("gneE0", 1000)));
            System.out.println("Edge.getAdaptatedTraveltime: " + (double)conn.do_job_get(Edge.getAdaptedTraveltime("gneE0", 5000)));

            conn.do_job_set(Edge.setEffort("gneE0", 246, 2000, 4000));
            System.out.println("Edge.getEffort: " + (double)conn.do_job_get(Edge.getEffort("gneE0", 3000)));
            System.out.println("Edge.getEffort: " + (double)conn.do_job_get(Edge.getEffort("gneE0", 1000)));
            System.out.println("Edge.getEffort: " + (double)conn.do_job_get(Edge.getEffort("gneE0", 5000)));

            conn.do_job_set(Lane.setParameter("gneE0_1", "laneParam", "laneValue"));
            System.out.println("Lane.getParameter: " + (String)conn.do_job_get(Lane.getParameter("gneE0_1", "laneParam")));

            conn.do_job_set(Route.setParameter("r1", "routeParam", "routeValue"));
            System.out.println("Route.getParameter: " + (String)conn.do_job_get(Route.getParameter("r1", "routeParam")));

            conn.do_job_set(Poi.setParameter("t0", "poiParam", "poiValue"));
            System.out.println("Poi.getParameter: " + (String)conn.do_job_get(Poi.getParameter("t0", "poiParam")));

            SumoStringList controlledJunctions = (SumoStringList)conn.do_job_get(Trafficlight.getControlledJunctions("gneJ1"));
            System.out.println("Trafficlight.getControlledJunctions: " + controlledJunctions);

            conn.do_job_set(Trafficlight.setParameter("gneJ1", "tlsParam", "tlsValue"));
            System.out.println("Trafficlight.getParameter: " + (String)conn.do_job_get(Trafficlight.getParameter("gneJ1", "tlsParam")));

            conn.do_job_set(Trafficlight.setPhaseDuration("gneJ1", 15));

            SumoTLSController tlsc = (SumoTLSController)conn.do_job_get(Trafficlight.getCompleteRedYellowGreenDefinition("gneJ1"));
            System.out.println("Trafficlight.getCompleteRedYellowGreenDefinition: " + tlsc.programs);

            SumoTLSProgram tlsp = (SumoTLSProgram)tlsc.programs.values().toArray()[0];
            conn.do_job_set(Trafficlight.setCompleteRedYellowGreenDefinition("gneJ1", tlsp));

            System.out.println("Trafficlight.getPhaseDuration: " + (double)conn.do_job_get(Trafficlight.getPhaseDuration("gneJ1")));
            System.out.println("Trafficlight.getNextSwitch: " + (double)conn.do_job_get(Trafficlight.getNextSwitch("gneJ1")));

            SumoRoadPosition roadPos = (SumoRoadPosition)conn.do_job_get(Simulation.convertRoad(505.38, -42.08, false, "passenger"));
            System.out.println("Simulation.convertRoad: " + roadPos.edgeID + ", " + roadPos.pos + ", " + roadPos.laneIndex);

            SumoPosition2D geoPos = (SumoPosition2D)conn.do_job_get(Simulation.convertGeo(505.38, -42.08, false));
            System.out.println("Simulation.convertGeo: " + geoPos);

            System.out.println("Lane.getLinks: " + conn.do_job_get(Lane.getLinks(":gneJ1_6_0")));

            conn.close();

        } catch (Exception ex) {
            ex.printStackTrace();
        }

        try {
            SumoTraciConnection conn = new SumoTraciConnection(sumo_bin, config_file);
            conn.addOption("step-length", step_length + "");
            conn.addOption("start", "true"); //start sumo immediately

            //start Traci Server
            conn.runServer();
            conn.setOrder(1);

            // expecting exception here since we use get instead of set
            conn.do_job_get(Simulation.saveState("file-state-now"));
        } catch (Exception tex) {
            System.err.println(tex.getMessage());
        }
    }

}
