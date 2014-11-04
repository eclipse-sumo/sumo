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
#include <router/ROLoader.h>
#include <router/RONet.h>
#include <router/ROEdge.h>
#include <utils/vehicle/RouteCostCalculator.h>
#include <utils/vehicle/DijkstraRouterTT.h>
#include <utils/vehicle/DijkstraRouterEffort.h>
#include <utils/vehicle/AStarRouter.h>
#include <utils/vehicle/BulkStarRouter.h>
#include <utils/vehicle/CHRouter.h>
#include <utils/vehicle/CHRouterWrapper.h>
#include <router/ROFrame.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <od2trips/ODCell.h>
#include <od2trips/ODDistrict.h>
#include <od2trips/ODDistrictCont.h>
#include <od2trips/ODDistrictHandler.h>
#include <od2trips/ODMatrix.h>
#include <utils/distribution/Distribution_Points.h>

#include "ROMAFrame.h"
#include "ROMAAssignments.h"
#include "ROMAEdgeBuilder.h"

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
    ROMAEdgeBuilder builder(oc.getBool("weights.expand"), oc.getBool("weights.interpolate"));
    loader.loadNet(net, builder);
    // load the weights when wished/available
    if (oc.isSet("weight-files")) {
        loader.loadWeights(net, "weight-files", oc.getString("weight-attribute"), false);
    }
    if (oc.isSet("lane-weight-files")) {
        loader.loadWeights(net, "lane-weight-files", oc.getString("weight-attribute"), true);
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
    typedef DijkstraRouterTT<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> > Dijkstra;
    Dijkstra router(net.getEdgeNo(), oc.getBool("ignore-errors"), &getTravelTime);
    std::vector<const ROEdge*> into;
    const int numInternalEdges = net.getInternalEdgeNumber();
    const int numTotalEdges = (int)net.getEdgeNo();
    for (int i = numInternalEdges; i < numTotalEdges; i++) {
        const Dijkstra::EdgeInfo& ei = router.getEdgeInfo(i);
        if (ei.edge->getType() != ROEdge::ET_INTERNAL) {
            router.compute(ei.edge, 0, 0, 0, into);
            for (int j = numInternalEdges; j < numTotalEdges; j++) {
                FileHelpers::writeFloat(outFile, router.getEdgeInfo(j).traveltime);
            }
        }
    }
}

/**
 * Computes the routes saving them
 */
void
computeRoutes(RONet& net, OptionsCont& oc, ODMatrix& matrix) {
    // build the router
    SUMOAbstractRouter<ROEdge, ROVehicle>* router;
    const std::string measure = oc.getString("weight-attribute");
    const std::string routingAlgorithm = oc.getString("routing-algorithm");
    if (measure == "traveltime") {
        if (routingAlgorithm == "dijkstra") {
            if (net.hasRestrictions()) {
                if (oc.getInt("paths") > 1) {
                    router = new DijkstraRouterTT<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                        net.getEdgeNo(), oc.getBool("ignore-errors"), &ROMAAssignments::getPenalizedTT);
                } else {
                    router = new DijkstraRouterTT<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                        net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic);
                }
            } else {
                if (oc.getInt("paths") > 1) {
                    router = new DijkstraRouterTT<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                        net.getEdgeNo(), oc.getBool("ignore-errors"), &ROMAAssignments::getPenalizedTT);
                } else {
                    router = new DijkstraRouterTT<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                        net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic);
                }
            }
        } else if (routingAlgorithm == "astar") {
            if (net.hasRestrictions()) {
                if (oc.getInt("paths") > 1) {
                    router = new AStarRouter<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                        net.getEdgeNo(), oc.getBool("ignore-errors"), &ROMAAssignments::getPenalizedTT);
                } else {
                    router = new AStarRouter<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                        net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic);
                }
            } else {
                if (oc.getInt("paths") > 1) {
                    router = new AStarRouter<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                        net.getEdgeNo(), oc.getBool("ignore-errors"), &ROMAAssignments::getPenalizedTT);
                } else {
                    router = new AStarRouter<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                        net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic);
                }
            }
        } else if (routingAlgorithm == "bulkstar") {
            if (net.hasRestrictions()) {
                router = new BulkStarRouter<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic, &ROEdge::getMinimumTravelTime);
            } else {
                router = new BulkStarRouter<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic, &ROEdge::getMinimumTravelTime);
            }
        } else if (routingAlgorithm == "CH") {
            const SUMOTime weightPeriod = (oc.isSet("weight-files") ?
                                           string2time(oc.getString("weight-period")) :
                                           std::numeric_limits<int>::max());
            if (net.hasRestrictions()) {
                router = new CHRouter<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic, SVC_IGNORING, weightPeriod, true);
            } else {
                router = new CHRouter<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic, SVC_IGNORING, weightPeriod, false);
            }
        } else if (routingAlgorithm == "CHWrapper") {
            const SUMOTime begin = string2time(oc.getString("begin"));
            const SUMOTime weightPeriod = (oc.isSet("weight-files") ?
                                           string2time(oc.getString("weight-period")) :
                                           std::numeric_limits<int>::max());

            router = new CHRouterWrapper<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic, begin, weightPeriod);
        } else {
            throw ProcessError("Unknown routing Algorithm '" + routingAlgorithm + "'!");
        }

    } else {
        DijkstraRouterEffort<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >::Operation op;
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
        } else if (measure == "noise") {
            op = &ROEdge::getNoiseEffort;
        } else {
            throw ProcessError("Unknown measure (weight attribute '" + measure + "')!");
        }
        if (net.hasRestrictions()) {
            if (oc.getInt("paths") > 1) {
                router = new DijkstraRouterEffort<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROMAAssignments::getPenalizedEffort, &ROMAAssignments::getTravelTime);
            } else {
                router = new DijkstraRouterEffort<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), op, &ROEdge::getTravelTimeStatic);
            }
        } else {
            if (oc.getInt("paths") > 1) {
                router = new DijkstraRouterEffort<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROMAAssignments::getPenalizedEffort, &ROMAAssignments::getTravelTime);
            } else {
                router = new DijkstraRouterEffort<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), op, &ROEdge::getTravelTimeStatic);
            }
        }
    }
    // prepare the output
    net.openOutput(oc.isSet("output-file") ? oc.getString("output-file") : "",
                   oc.isSet("flow-output") ? oc.getString("flow-output") : "", "");
    // process route definitions
    try {
        if (oc.isSet("timeline")) {
            matrix.applyCurve(matrix.parseTimeLine(oc.getStringVector("timeline"), oc.getBool("timeline.day-in-hours")));
        }
        ROVehicle defaultVehicle(SUMOVehicleParameter(), 0, net.getVehicleTypeSecure(DEFAULT_VTYPE_ID), &net);
        ROMAAssignments a(string2time(oc.getString("begin")), string2time(oc.getString("end")), net, matrix, *router);
        const std::string assignMethod = oc.getString("assignment-method");
        if (assignMethod == "incremental") {
            a.incremental(oc.getInt("max-iterations"));
        } else if (assignMethod == "SUE") {
            a.sue(oc.getInt("max-iterations"), oc.getInt("max-inner-iterations"),
                  oc.getInt("paths"), oc.getFloat("paths.penalty"), oc.getFloat("tolerance"), oc.getString("route-choice-method"));
        }
        // update path costs and output
        bool haveOutput = false;
        OutputDevice* dev = net.getRouteOutput();
        if (dev != 0) {
            for (std::vector<ODCell*>::const_iterator i = matrix.getCells().begin(); i != matrix.getCells().end(); ++i) {
                const ODCell* const c = *i;
                dev->openTag(SUMO_TAG_ROUTE_DISTRIBUTION).writeAttr(SUMO_ATTR_ID, c->origin + "_" + c->destination + "_" + time2string(c->begin) + "_" + time2string(c->end));
                for (std::vector<RORoute*>::const_iterator j = c->pathsVector.begin(); j != c->pathsVector.end(); ++j) {
                    (*j)->setCosts(router->recomputeCosts((*j)->getEdgeVector(), &defaultVehicle, string2time(oc.getString("begin"))));
                    (*j)->writeXMLDefinition(*dev, 0, true, false);
                }
                dev->closeTag();
            }
            haveOutput = true;
        }
        dev = net.getRouteOutput(true);
        if (dev != 0) {
            int num = 0;
            for (std::vector<ODCell*>::const_iterator i = matrix.getCells().begin(); i != matrix.getCells().end(); ++i) {
                const ODCell* const c = *i;
                dev->openTag(SUMO_TAG_FLOW).writeAttr(SUMO_ATTR_ID, oc.getString("prefix") + toString(num++));
                dev->writeAttr(SUMO_ATTR_BEGIN, time2string(c->begin)).writeAttr(SUMO_ATTR_END, time2string(c->end));
                dev->writeAttr(SUMO_ATTR_NUMBER, int(c->vehicleNumber));
                matrix.writeDefaultAttrs(*dev, oc.getBool("ignore-vehicle-type"), c);
                dev->openTag(SUMO_TAG_ROUTE_DISTRIBUTION);
                for (std::vector<RORoute*>::const_iterator j = c->pathsVector.begin(); j != c->pathsVector.end(); ++j) {
                    (*j)->setCosts(router->recomputeCosts((*j)->getEdgeVector(), &defaultVehicle, string2time(oc.getString("begin"))));
                    (*j)->writeXMLDefinition(*dev, 0, true, false);
                }
                dev->closeTag();
                dev->closeTag();
            }
            haveOutput = true;
        }
        if (!haveOutput) {
            throw ProcessError("No output file given.");
        }
        // end the processing
        net.cleanup(router);
    } catch (ProcessError&) {
        net.cleanup(router);
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
    oc.setApplicationName("marouter", "SUMO marouter Version " + getBuildName(VERSION_STRING));
    int ret = 0;
    RONet* net = 0;
    try {
        XMLSubSys::init();
        ROMAFrame::fillOptions();
        OptionsIO::getOptions(true, argc, argv);
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
            if (!oc.isSet("additional-files")) {
                delete net;
                SystemFrame::close();
                if (ret == 0) {
                    std::cout << "Success." << std::endl;
                }
                return ret;
            }
        }
        // load districts
        ODDistrictCont districts;
        districts.loadDistricts(oc.getString("additional-files"));
        if (districts.size() == 0) {
            throw ProcessError("No districts loaded.");
        }
        // load the matrix
        ODMatrix matrix(districts);
        matrix.loadMatrix(oc);
        if (matrix.getNoLoaded() == 0) {
            throw ProcessError("No vehicles loaded.");
        }
        if (MsgHandler::getErrorInstance()->wasInformed() && !oc.getBool("ignore-errors")) {
            throw ProcessError("Loading failed.");
        }
        MsgHandler::getErrorInstance()->clear();
        WRITE_MESSAGE(toString(matrix.getNoLoaded()) + " vehicles loaded.");

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

