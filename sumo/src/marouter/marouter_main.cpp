/****************************************************************************/
/// @file    marouter_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Thu, 06 Jun 2002
/// @version $Id$
///
// Main for MAROUTER
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <utils/common/TplConvert.h>
#include <iostream>
#include <string>
#include <limits.h>
#include <ctime>
#include <vector>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/ToString.h>
#include <utils/distribution/Distribution_Points.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/iodevices/OutputDevice_String.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/vehicle/RouteCostCalculator.h>
#include <utils/vehicle/DijkstraRouterTT.h>
#include <utils/vehicle/DijkstraRouterEffort.h>
#include <utils/vehicle/AStarRouter.h>
#include <utils/vehicle/CHRouter.h>
#include <utils/vehicle/CHRouterWrapper.h>
#include <utils/xml/XMLSubSys.h>
#include <od/ODCell.h>
#include <od/ODDistrict.h>
#include <od/ODDistrictCont.h>
#include <od/ODDistrictHandler.h>
#include <od/ODMatrix.h>
#include <router/ROEdge.h>
#include <router/ROLoader.h>
#include <router/RONet.h>
#include <router/RORoute.h>
#include <router/RORoutable.h>

#include "ROMAFrame.h"
#include "ROMAAssignments.h"
#include "ROMAEdgeBuilder.h"
#include "ROMARouteHandler.h"
#include "ROMAEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// functions
// ===========================================================================
/* -------------------------------------------------------------------------
 * data processing methods
 * ----------------------------------------------------------------------- */
/**
 * loads the net
 * The net is in this meaning made up by the net itself and the dynamic
 * weights which may be supplied in a separate file
 */
void
initNet(RONet& net, ROLoader& loader, OptionsCont& oc) {
    // load the net
    ROMAEdgeBuilder builder;
    ROEdge::setGlobalOptions(oc.getBool("weights.interpolate"));
    loader.loadNet(net, builder);
    // initialize the travel times
    /* const SUMOTime begin = string2time(oc.getString("begin"));
    const SUMOTime end = string2time(oc.getString("end"));
    for (std::map<std::string, ROEdge*>::const_iterator i = net.getEdgeMap().begin(); i != net.getEdgeMap().end(); ++i) {
        (*i).second->addTravelTime(STEPS2TIME(begin), STEPS2TIME(end), (*i).second->getLength() / (*i).second->getSpeed());
    }*/
    // load the weights when wished/available
    if (oc.isSet("weight-files")) {
        loader.loadWeights(net, "weight-files", oc.getString("weight-attribute"), false, oc.getBool("weights.expand"));
    }
    if (oc.isSet("lane-weight-files")) {
        loader.loadWeights(net, "lane-weight-files", oc.getString("weight-attribute"), true, oc.getBool("weights.expand"));
    }
}

SUMOReal
getTravelTime(const ROEdge* const edge, const ROVehicle* const /* veh */, SUMOReal /* time */) {
    return edge->getLength() / edge->getSpeed();
}


/**
 * Computes all pair shortest paths, saving them
 */
void
computeAllPairs(RONet& net, OptionsCont& oc) {
    std::ofstream outFile(oc.getString("all-pairs-output").c_str(), std::ios::binary);
    // build the router
    typedef DijkstraRouterTT<ROEdge, ROVehicle, noProhibitions<ROEdge, ROVehicle> > Dijkstra;
    Dijkstra router(ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &getTravelTime);
    ConstROEdgeVector into;
    const int numInternalEdges = net.getInternalEdgeNumber();
    const int numTotalEdges = (int)net.getEdgeNo();
    for (int i = numInternalEdges; i < numTotalEdges; i++) {
        const Dijkstra::EdgeInfo& ei = router.getEdgeInfo(i);
        if (ei.edge->getFunc() != ROEdge::ET_INTERNAL) {
            router.compute(ei.edge, 0, 0, 0, into);
            for (int j = numInternalEdges; j < numTotalEdges; j++) {
                FileHelpers::writeFloat(outFile, router.getEdgeInfo(j).traveltime);
            }
        }
    }
}


/**
 * Writes the travel times for a single interval
 */
void
writeInterval(OutputDevice& dev, const SUMOTime begin, const SUMOTime end, const RONet& net, const ROVehicle* const veh) {
    dev.openTag(SUMO_TAG_INTERVAL).writeAttr(SUMO_ATTR_BEGIN, time2string(begin)).writeAttr(SUMO_ATTR_END, time2string(end));
    for (std::map<std::string, ROEdge*>::const_iterator i = net.getEdgeMap().begin(); i != net.getEdgeMap().end(); ++i) {
        ROMAEdge* edge = static_cast<ROMAEdge*>(i->second);
        if (edge->getFunc() == ROEdge::ET_NORMAL) {
            dev.openTag(SUMO_TAG_EDGE).writeAttr(SUMO_ATTR_ID, edge->getID());
            const SUMOReal traveltime = edge->getTravelTime(veh, STEPS2TIME(begin));
            const SUMOReal flow = edge->getFlow(STEPS2TIME(begin));
            dev.writeAttr("traveltime", traveltime);
            dev.writeAttr("speed", edge->getLength() / traveltime);
            dev.writeAttr("entered", flow);
            dev.writeAttr("flowCapacityRatio", 100. * flow / ROMAAssignments::getCapacity(edge));
            dev.closeTag();
        }
    }
    dev.closeTag();
}


/**
 * Computes the routes saving them
 */
void
computeRoutes(RONet& net, OptionsCont& oc, ODMatrix& matrix) {
    // build the router
    SUMOAbstractRouter<ROEdge, ROVehicle>* router = 0;
    const std::string measure = oc.getString("weight-attribute");
    const std::string routingAlgorithm = oc.getString("routing-algorithm");
    const SUMOTime begin = string2time(oc.getString("begin"));
    const SUMOTime end = string2time(oc.getString("end"));
    if (measure == "traveltime") {
        if (routingAlgorithm == "dijkstra") {
            if (net.hasPermissions()) {
                if (oc.getInt("paths") > 1) {
                    router = new DijkstraRouterTT<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                        ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROMAAssignments::getPenalizedTT);
                } else {
                    router = new DijkstraRouterTT<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                        ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic);
                }
            } else {
                if (oc.getInt("paths") > 1) {
                    router = new DijkstraRouterTT<ROEdge, ROVehicle, noProhibitions<ROEdge, ROVehicle> >(
                        ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROMAAssignments::getPenalizedTT);
                } else {
                    router = new DijkstraRouterTT<ROEdge, ROVehicle, noProhibitions<ROEdge, ROVehicle> >(
                        ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic);
                }
            }
        } else if (routingAlgorithm == "astar") {
            if (net.hasPermissions()) {
                if (oc.getInt("paths") > 1) {
                    router = new AStarRouter<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                        ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROMAAssignments::getPenalizedTT);
                } else {
                    router = new AStarRouter<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                        ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic);
                }
            } else {
                if (oc.getInt("paths") > 1) {
                    router = new AStarRouter<ROEdge, ROVehicle, noProhibitions<ROEdge, ROVehicle> >(
                        ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROMAAssignments::getPenalizedTT);
                } else {
                    router = new AStarRouter<ROEdge, ROVehicle, noProhibitions<ROEdge, ROVehicle> >(
                        ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic);
                }
            }
        } else if (routingAlgorithm == "CH") {
            const SUMOTime weightPeriod = (oc.isSet("weight-files") ?
                                           string2time(oc.getString("weight-period")) :
                                           std::numeric_limits<int>::max());
            if (net.hasPermissions()) {
                router = new CHRouter<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                    ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic, SVC_IGNORING, weightPeriod, true);
            } else {
                router = new CHRouter<ROEdge, ROVehicle, noProhibitions<ROEdge, ROVehicle> >(
                    ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic, SVC_IGNORING, weightPeriod, false);
            }
        } else if (routingAlgorithm == "CHWrapper") {
            const SUMOTime weightPeriod = (oc.isSet("weight-files") ?
                                           string2time(oc.getString("weight-period")) :
                                           std::numeric_limits<int>::max());
            router = new CHRouterWrapper<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic,
                begin, end, weightPeriod, oc.getInt("routing-threads"));
        } else {
            throw ProcessError("Unknown routing Algorithm '" + routingAlgorithm + "'!");
        }

    } else {
        DijkstraRouterEffort<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >::Operation op;
        if (measure == "CO") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::CO>;
        } else if (measure == "CO2") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::CO2>;
        } else if (measure == "PMx") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::PM_X>;
        } else if (measure == "HC") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::HC>;
        } else if (measure == "NOx") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::NO_X>;
        } else if (measure == "fuel") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::FUEL>;
        } else if (measure == "electricity") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::ELEC>;
        } else if (measure == "noise") {
            op = &ROEdge::getNoiseEffort;
        } else {
            throw ProcessError("Unknown measure (weight attribute '" + measure + "')!");
        }
        if (net.hasPermissions()) {
            if (oc.getInt("paths") > 1) {
                router = new DijkstraRouterEffort<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                    ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROMAAssignments::getPenalizedEffort, &ROMAAssignments::getTravelTime);
            } else {
                router = new DijkstraRouterEffort<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                    ROEdge::getAllEdges(), oc.getBool("ignore-errors"), op, &ROEdge::getTravelTimeStatic);
            }
        } else {
            if (oc.getInt("paths") > 1) {
                router = new DijkstraRouterEffort<ROEdge, ROVehicle, noProhibitions<ROEdge, ROVehicle> >(
                    ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROMAAssignments::getPenalizedEffort, &ROMAAssignments::getTravelTime);
            } else {
                router = new DijkstraRouterEffort<ROEdge, ROVehicle, noProhibitions<ROEdge, ROVehicle> >(
                    ROEdge::getAllEdges(), oc.getBool("ignore-errors"), op, &ROEdge::getTravelTimeStatic);
            }
        }
    }
    try {
        const RORouterProvider provider(router, 0, 0);
        // prepare the output
        net.openOutput(oc);
        // process route definitions
        if (oc.isSet("timeline")) {
            matrix.applyCurve(matrix.parseTimeLine(oc.getStringVector("timeline"), oc.getBool("timeline.day-in-hours")));
        }
        matrix.sortByBeginTime();
        ROVehicle defaultVehicle(SUMOVehicleParameter(), 0, net.getVehicleTypeSecure(DEFAULT_VTYPE_ID), &net);
        ROMAAssignments a(begin, end, oc.getBool("additive-traffic"), oc.getFloat("weight-adaption"), net, matrix, *router);
        a.resetFlows();
#ifdef HAVE_FOX
        const int maxNumThreads = oc.getInt("routing-threads");
        while ((int)net.getThreadPool().size() < maxNumThreads) {
            new RONet::WorkerThread(net.getThreadPool(), provider);
        }
#endif
        const std::string assignMethod = oc.getString("assignment-method");
        if (assignMethod == "incremental") {
            a.incremental(oc.getInt("max-iterations"), oc.getBool("verbose"));
        } else if (assignMethod == "SUE") {
            a.sue(oc.getInt("max-iterations"), oc.getInt("max-inner-iterations"),
                  oc.getInt("paths"), oc.getFloat("paths.penalty"), oc.getFloat("tolerance"), oc.getString("route-choice-method"));
        }
        // update path costs and output
        bool haveOutput = false;
        OutputDevice* dev = net.getRouteOutput();
        if (dev != 0) {
            std::vector<std::string> tazParamKeys;
            if (oc.isSet("taz-param")) {
                tazParamKeys = oc.getStringVector("taz-param");
            }
            std::map<SUMOTime, std::string> sortedOut;
            SUMOTime lastEnd = -1;
            int num = 0;
            for (std::vector<ODCell*>::const_iterator i = matrix.getCells().begin(); i != matrix.getCells().end(); ++i) {
                const ODCell* const c = *i;
                if (lastEnd >= 0 && lastEnd <= c->begin) {
                    for (std::map<SUMOTime, std::string>::const_iterator desc = sortedOut.begin(); desc != sortedOut.end(); ++desc) {
                        dev->writePreformattedTag(desc->second);
                    }
                    sortedOut.clear();
                }
                if (c->departures.empty()) {
                    OutputDevice_String od(dev->isBinary(), 1);
                    od.openTag(SUMO_TAG_FLOW).writeAttr(SUMO_ATTR_ID, oc.getString("prefix") + toString(num++));
                    od.writeAttr(SUMO_ATTR_BEGIN, time2string(c->begin)).writeAttr(SUMO_ATTR_END, time2string(c->end));
                    od.writeAttr(SUMO_ATTR_NUMBER, int(c->vehicleNumber));
                    matrix.writeDefaultAttrs(od, oc.getBool("ignore-vehicle-type"), c);
                    od.openTag(SUMO_TAG_ROUTE_DISTRIBUTION);
                    for (std::vector<RORoute*>::const_iterator j = c->pathsVector.begin(); j != c->pathsVector.end(); ++j) {
                        (*j)->setCosts(router->recomputeCosts((*j)->getEdgeVector(), &defaultVehicle, string2time(oc.getString("begin"))));
                        (*j)->writeXMLDefinition(od, 0, true, false);
                    }
                    od.closeTag();
                    od.closeTag();
                    sortedOut[c->begin] += od.getString();
                } else {
                    for (std::map<SUMOTime, std::vector<std::string> >::const_iterator deps = c->departures.begin(); deps != c->departures.end(); ++deps) {
                        const std::string routeDistId = c->origin + "_" + c->destination + "_" + time2string(c->begin) + "_" + time2string(c->end);
                        for (std::vector<std::string>::const_iterator id = deps->second.begin(); id != deps->second.end(); ++id) {
                            OutputDevice_String od(dev->isBinary(), 1);
                            od.openTag(SUMO_TAG_VEHICLE).writeAttr(SUMO_ATTR_ID, *id).writeAttr(SUMO_ATTR_DEPART, time2string(deps->first));
                            matrix.writeDefaultAttrs(od, oc.getBool("ignore-vehicle-type"), c);
                            od.openTag(SUMO_TAG_ROUTE_DISTRIBUTION);
                            for (std::vector<RORoute*>::const_iterator j = c->pathsVector.begin(); j != c->pathsVector.end(); ++j) {
                                (*j)->setCosts(router->recomputeCosts((*j)->getEdgeVector(), &defaultVehicle, string2time(oc.getString("begin"))));
                                (*j)->writeXMLDefinition(od, 0, true, false);
                            }
                            od.closeTag();
                            if (!tazParamKeys.empty()) {
                                od.openTag(SUMO_TAG_PARAM).writeAttr(SUMO_ATTR_KEY, tazParamKeys[0]).writeAttr(SUMO_ATTR_VALUE, c->origin).closeTag();
                                if (tazParamKeys.size() > 1) {
                                    od.openTag(SUMO_TAG_PARAM).writeAttr(SUMO_ATTR_KEY, tazParamKeys[1]).writeAttr(SUMO_ATTR_VALUE, c->destination).closeTag();
                                }
                            }
                            od.closeTag();
                            sortedOut[deps->first] += od.getString();
                        }
                    }
                }
                for (std::vector<RORoute*>::const_iterator j = c->pathsVector.begin(); j != c->pathsVector.end(); ++j) {
                    delete *j;
                }
                if (c->end > lastEnd) {
                    lastEnd = c->end;
                }
            }
            for (std::map<SUMOTime, std::string>::const_iterator desc = sortedOut.begin(); desc != sortedOut.end(); ++desc) {
                dev->writePreformattedTag(desc->second);
            }
            haveOutput = true;
        }
        if (OutputDevice::createDeviceByOption("netload-output", "meandata")) {
            if (oc.getBool("additive-traffic")) {
                writeInterval(OutputDevice::getDeviceByOption("netload-output"), begin, end, net, a.getDefaultVehicle());
            } else {
                SUMOTime lastCell = 0;
                for (std::vector<ODCell*>::const_iterator i = matrix.getCells().begin(); i != matrix.getCells().end(); ++i) {
                    if ((*i)->end > lastCell) {
                        lastCell = (*i)->end;
                    }
                }
                const SUMOTime interval = string2time(OptionsCont::getOptions().getString("aggregation-interval"));
                for (SUMOTime start = begin; start < MIN2(end, lastCell); start += interval) {
                    writeInterval(OutputDevice::getDeviceByOption("netload-output"), start, start + interval, net, a.getDefaultVehicle());
                }
            }
            haveOutput = true;
        }
        if (!haveOutput) {
            throw ProcessError("No output file given.");
        }
        // end the processing
        net.cleanup();
    } catch (ProcessError&) {
        for (std::vector<ODCell*>::const_iterator i = matrix.getCells().begin(); i != matrix.getCells().end(); ++i) {
            for (std::vector<RORoute*>::const_iterator j = (*i)->pathsVector.begin(); j != (*i)->pathsVector.end(); ++j) {
                delete *j;
            }
        }
        net.cleanup();
        throw;
    }
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char** argv) {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.setApplicationDescription("Import O/D-matrices for macroscopic traffic assignment");
    oc.setApplicationName("marouter", "SUMO marouter Version " VERSION_STRING);
    int ret = 0;
    RONet* net = 0;
    try {
        XMLSubSys::init();
        ROMAFrame::fillOptions();
        OptionsIO::setArgs(argc, argv);
        OptionsIO::getOptions();
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        XMLSubSys::setValidation(oc.getString("xml-validation"), oc.getString("xml-validation.net"));
        MsgHandler::initOutputOptions();
        if (!ROMAFrame::checkOptions()) {
            throw ProcessError();
        }
        RandHelper::initRandGlobal();
        // load data
        ROLoader loader(oc, false, false);
        net = new RONet();
        initNet(*net, loader, oc);
        if (oc.isSet("all-pairs-output")) {
            computeAllPairs(*net, oc);
            if (net->getDistricts().empty()) {
                delete net;
                SystemFrame::close();
                if (ret == 0) {
                    std::cout << "Success." << std::endl;
                }
                return ret;
            }
        }
        if (net->getDistricts().empty()) {
            throw ProcessError("No districts loaded.");
        }
        // load districts
        ODDistrictCont districts;
        districts.makeDistricts(net->getDistricts());
        // load the matrix
        ODMatrix matrix(districts);
        matrix.loadMatrix(oc);
        ROMARouteHandler handler(matrix);
        matrix.loadRoutes(oc, handler);
        if (matrix.getNumLoaded() == 0) {
            throw ProcessError("No vehicles loaded.");
        }
        if (MsgHandler::getErrorInstance()->wasInformed() && !oc.getBool("ignore-errors")) {
            throw ProcessError("Loading failed.");
        }
        MsgHandler::getErrorInstance()->clear();
        WRITE_MESSAGE(toString(matrix.getNumLoaded()) + " vehicles loaded.");

        // build routes and parse the incremental rates if the incremental method is choosen.
        try {
            computeRoutes(*net, oc, matrix);
        } catch (XERCES_CPP_NAMESPACE::SAXParseException& e) {
            WRITE_ERROR(toString(e.getLineNumber()));
            ret = 1;
        } catch (XERCES_CPP_NAMESPACE::SAXException& e) {
            WRITE_ERROR(TplConvert::_2str(e.getMessage()));
            ret = 1;
        }
        if (MsgHandler::getErrorInstance()->wasInformed() || ret != 0) {
            throw ProcessError();
        }
    } catch (const ProcessError& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    }

    delete net;
    SystemFrame::close();
    if (ret == 0) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}



/****************************************************************************/

