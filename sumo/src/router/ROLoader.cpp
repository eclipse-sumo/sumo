/****************************************************************************/
/// @file    ROLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Loader for networks and route imports
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <iostream>
#include <string>
#include <iomanip>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <utils/options/OptionsCont.h>
#include <utils/common/ToString.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/xml/SAXWeightsHandler.h>
#include "RONet.h"
#include "RONetHandler.h"
#include "ROLoader.h"
#include "ROEdge.h"
#include "RORDLoader_TripDefs.h"
#include "RORDLoader_SUMOBase.h"
#include "RORDGenerator_Random.h"
#include "RORDGenerator_ODAmounts.h"
#include "ROAbstractRouteDefLoader.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// ROLoader::EdgeFloatTimeLineRetriever_EdgeWeight - methods
// ---------------------------------------------------------------------------
ROLoader::EdgeFloatTimeLineRetriever_EdgeWeight::EdgeFloatTimeLineRetriever_EdgeWeight(
    RONet *net)
        : myNet(net) {}


ROLoader::EdgeFloatTimeLineRetriever_EdgeWeight::~EdgeFloatTimeLineRetriever_EdgeWeight() {}


void
ROLoader::EdgeFloatTimeLineRetriever_EdgeWeight::addEdgeWeight(const std::string &id,
        SUMOReal val,
        SUMOTime beg,
        SUMOTime end) {
    ROEdge *e = myNet->getEdge(id);
    if (e!=0) {
        e->addWeight(val, beg, end);
    } else {
        if (id[0]!=':') {
            MsgHandler::getErrorInstance()->inform("Trying to set a weight for the unknown edge '" + id + "'.");
        }
    }
}



// ---------------------------------------------------------------------------
// ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::SingleWeightRetriever - methods
// ---------------------------------------------------------------------------
ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::SingleWeightRetriever::SingleWeightRetriever(
    Type type, EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight *parent)
        : myType(type), myParent(parent) {}


ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::SingleWeightRetriever::~SingleWeightRetriever() {}


void
ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::SingleWeightRetriever::addEdgeWeight(
    const std::string &id, SUMOReal val, SUMOTime beg, SUMOTime end) {
    myParent->addTypedWeight(myType, id, val, beg, end);
}



// ---------------------------------------------------------------------------
// ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight - methods
// ---------------------------------------------------------------------------
ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight(RONet *net)
        : myNet(net) {
    myAbsoluteRetriever = new SingleWeightRetriever(ABSOLUTE, this);
    myAddRetriever = new SingleWeightRetriever(ADD, this);
    myMultRetriever = new SingleWeightRetriever(MULT, this);
}


ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::~EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight() {
    std::map<ROEdge*, SuppWeights>::iterator i;
    for (i=myWeights.begin(); i!=myWeights.end(); ++i) {
        (*i).first->setSupplementaryWeights(
            (*i).second.absolute, (*i).second.add, (*i).second.mult);
    }
    delete myAbsoluteRetriever;
    delete myAddRetriever;
    delete myMultRetriever;
}


void
ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::addTypedWeight(Type type, const std::string &id,
        SUMOReal val, SUMOTime beg, SUMOTime end) {
    ROEdge *e = myNet->getEdge(id);
    if (e==0) {
        MsgHandler::getErrorInstance()->inform("Trying to set a weight for the unknown edge '" + id + "'.");
        return;
    }
    if (myWeights.find(e)==myWeights.end()) {
        SuppWeights weights;
        weights.absolute = new FloatValueTimeLine();
        weights.add = new FloatValueTimeLine();
        weights.mult = new FloatValueTimeLine();
    }
    switch (type) {
    case ABSOLUTE:
        myWeights[e].absolute->add(beg, end, val);
        break;
    case ADD:
        myWeights[e].add->add(beg, end, val);
        break;
    case MULT:
        myWeights[e].mult->add(beg, end, val);
        break;
    default:
        break;
    }
}


ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::SingleWeightRetriever &
ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::getAbsoluteRetriever() {
    return *myAbsoluteRetriever;
}


ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::SingleWeightRetriever &
ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::getAddRetriever() {
    return *myAddRetriever;
}


ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::SingleWeightRetriever &
ROLoader::EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight::getMultRetriever() {
    return *myMultRetriever;
}



// ---------------------------------------------------------------------------
// ROLoader - methods
// ---------------------------------------------------------------------------
ROLoader::ROLoader(OptionsCont &oc, bool emptyDestinationsAllowed) throw()
        : myOptions(oc), myEmptyDestinationsAllowed(emptyDestinationsAllowed) {}


ROLoader::~ROLoader() {
    destroyHandlers();
}


void
ROLoader::loadNet(RONet &toFill, ROAbstractEdgeBuilder &eb) {
    std::string file = myOptions.getString("net-file");
    if (file=="") {
        throw ProcessError("Missing definition of network to load!");
    }
    if (!FileHelpers::exists(file)) {
        throw ProcessError("The network file '" + file + "' could not be found.");
    }
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading net...");
    RONetHandler handler(toFill, eb);
    handler.setFileName(file);
    if (!XMLSubSys::runParser(handler, file)) {
        MsgHandler::getMessageInstance()->endProcessMsg("failed.");
        throw ProcessError();
    } else {
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
}


unsigned int
ROLoader::openRoutes(RONet &net) {
    // build loader
    // load sumo-routes when wished
    bool ok = openTypedRoutes("sumo-input", net);
    // load the XML-trip definitions when wished
    ok &= openTypedRoutes("trip-defs", net);
    // load the sumo-alternative file when wished
    ok &= openTypedRoutes("alternatives", net);
    // load the amount definitions if wished
    ok &= openTypedRoutes("flows", net);
    // build generators
    if (myOptions.isSet("R")) {
        myHandler.push_back(new RORDGenerator_Random(net,
                            myOptions.getInt("begin"), myOptions.getInt("end"), myOptions.getBool("prune-random")));
    }
    // check
    if (ok&&myHandler.size()==0) {
        throw ProcessError("No route input specified.");
    }
    // skip routes prior to the begin time
    if (ok&&!myOptions.getBool("unsorted")) {
        MsgHandler::getMessageInstance()->inform("Skipping...");
        for (RouteLoaderCont::iterator i=myHandler.begin(); ok&&i!=myHandler.end(); i++) {
            ok &= (*i)->readRoutesAtLeastUntil(myOptions.getInt("begin"), true);
        }
        MsgHandler::getMessageInstance()->inform("Skipped until: " + toString<SUMOTime>(getMinTimeStep()));
    }
    // check whether everything's ok
    if (!ok) {
        destroyHandlers();
        throw ProcessError();
    }
    return (unsigned int) myHandler.size();
}


void
ROLoader::processRoutesStepWise(SUMOTime start, SUMOTime end,
                                RONet &net, SUMOAbstractRouter<ROEdge,ROVehicle> &router) {
    SUMOTime absNo = end - start;
    // skip routes that begin before the simulation's begin
    // loop till the end
    bool endReached = false;
    bool errorOccured = false;
    SUMOTime time = myHandler.size()!=0 ? getMinTimeStep() : start;
    SUMOTime firstStep = time;
    SUMOTime lastStep = time;
    for (; time<end&&!errorOccured&&!endReached; time++) {
        writeStats(time, start, absNo);
        makeSingleStep(time, net, router);
        // check whether further data exist
        endReached = !net.furtherStored();
        lastStep = time;
        for (RouteLoaderCont::iterator i=myHandler.begin(); endReached&&i!=myHandler.end(); i++) {
            if (!(*i)->ended()) {
                endReached = false;
            }
        }
        errorOccured =
            MsgHandler::getErrorInstance()->wasInformed()
            &&
            !myOptions.getBool("continue-on-unbuild");
    }
    MsgHandler::getMessageInstance()->inform("Routes found between time steps " + toString<int>(firstStep) + " and " + toString<int>(lastStep) + ".");
}


bool
ROLoader::makeSingleStep(SUMOTime end, RONet &net, SUMOAbstractRouter<ROEdge,ROVehicle> &router) {
    RouteLoaderCont::iterator i;
    // go through all handlers
    if (myHandler.size()!= 0) {
        for (i=myHandler.begin(); i!=myHandler.end(); i++) {
            // load routes until the time point is reached
            if ((*i)->readRoutesAtLeastUntil(end, false)) {
                // save the routes
                net.saveAndRemoveRoutesUntil(myOptions, router, end);
            } else {
                return false;
            }
        }
        return MsgHandler::getErrorInstance()->wasInformed();
    } else {
        return false;
    }
}


SUMOTime
ROLoader::getMinTimeStep() const throw() {
    RouteLoaderCont::const_iterator i=myHandler.begin();
    SUMOTime ret = (*i)->getLastReadTimeStep();
    ++i;
    for (; i!=myHandler.end(); i++) {
        SUMOTime akt = (*i)->getLastReadTimeStep();
        if (akt<ret) {
            ret = akt;
        }
    }
    return ret;
}


void
ROLoader::processAllRoutes(SUMOTime start, SUMOTime end,
                           RONet &net,
                           SUMOAbstractRouter<ROEdge,ROVehicle> &router) {
    long absNo = end - start;
    bool ok = true;
    for (RouteLoaderCont::iterator i=myHandler.begin(); ok&&i!=myHandler.end(); i++) {
        ok &= (*i)->readRoutesAtLeastUntil(INT_MAX, false);
    }
    // save the routes
    SUMOTime time = start;
    for (; time<end; time++) {
        writeStats(time, start, absNo);
        net.saveAndRemoveRoutesUntil(myOptions, router, time);
    }
}


bool
ROLoader::openTypedRoutes(const std::string &optionName,
                          RONet &net) throw() {
    // check whether the current loader is known
    //  (not all routers import all route formats)
    if (!myOptions.exists(optionName)) {
        return true;
    }
    // check whether the current loader is wished
    //  and the file(s) can be used
    if (!myOptions.isUsableFileList(optionName)) {
        return !myOptions.isSet(optionName);
    }
    bool ok = true;
    vector<string> files = myOptions.getStringVector(optionName);
    for (vector<string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
        // build the instance when everything's all right
        try {
            ROAbstractRouteDefLoader *instance =
                buildNamedHandler(optionName, *fileIt, net);
            myHandler.push_back(instance);
        } catch (ProcessError &) {
            MsgHandler::getErrorInstance()->inform("The loader for " + optionName + " from file '" + *fileIt + "' could not be initialised.");
            ok = false;
        }
    }
    return ok;
}


ROAbstractRouteDefLoader*
ROLoader::buildNamedHandler(const std::string &optionName,
                            const std::string &file,
                            RONet &net) throw(ProcessError) {
    if (optionName=="sumo-input") {
        return new RORDLoader_SUMOBase(net,
                                       myOptions.getInt("begin"), myOptions.getInt("end"),
                                       myOptions.getFloat("gBeta"), myOptions.getFloat("gA"),
                                       myOptions.getInt("max-alternatives"), myOptions.getBool("repair"),
                                       file);
    }
    if (optionName=="trip-defs") {
        return new RORDLoader_TripDefs(net,
                                       myOptions.getInt("begin"), myOptions.getInt("end"),
                                       myEmptyDestinationsAllowed, file);
    }
    if (optionName=="alternatives") {
        return new RORDLoader_SUMOBase(net,
                                       myOptions.getInt("begin"), myOptions.getInt("end"),
                                       myOptions.getFloat("gBeta"), myOptions.getFloat("gA"),
                                       myOptions.getInt("max-alternatives"), myOptions.getBool("repair"),
                                       file);
    }
    if (optionName=="flows") {
        return new RORDGenerator_ODAmounts(net,
                                           myOptions.getInt("begin"), myOptions.getInt("end"),
                                           myEmptyDestinationsAllowed, myOptions.getBool("randomize-flows"), file);
    }
    return 0;
}


bool
ROLoader::loadWeights(RONet &net, const std::string &file,
                      bool useLanes) {
    // check whether the file exists
    if (!FileHelpers::exists(file)) {
        MsgHandler::getErrorInstance()->inform("The weights file '" + file + "' does not exist!");
        return false;
    }
    // build and prepare the weights handler
    EdgeFloatTimeLineRetriever_EdgeWeight retriever(&net);
    SAXWeightsHandler::ToRetrieveDefinition *def = new SAXWeightsHandler::ToRetrieveDefinition("traveltime", !useLanes, retriever);
    SAXWeightsHandler handler(def, file);
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading precomputed net weights...");
    // build and prepare the parser
    if (XMLSubSys::runParser(handler, file)) {
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
        return true;
    } else {
        MsgHandler::getMessageInstance()->endProcessMsg("failed.");
        return false;
    }
}


void
ROLoader::loadSupplementaryWeights(RONet& net) {
    string filename = myOptions.getString("supplementary-weights");
    if (! FileHelpers::exists(filename)) {
        throw ProcessError("Could not open the supplementary-weights file '" + filename + "'.");
    }
    EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight retriever(&net);
    std::vector<SAXWeightsHandler::ToRetrieveDefinition*> defs;
    defs.push_back(new SAXWeightsHandler::ToRetrieveDefinition("absolute", true, retriever.getAbsoluteRetriever()));
    defs.push_back(new SAXWeightsHandler::ToRetrieveDefinition("summand", true, retriever.getAddRetriever()));
    defs.push_back(new SAXWeightsHandler::ToRetrieveDefinition("factor", true, retriever.getMultRetriever()));
    SAXWeightsHandler handler(defs, filename);
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading precomputed supplementary net-weights.");
    if (XMLSubSys::runParser(handler, filename)) {
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    } else {
        throw ProcessError("failed.");

    }
}


void
ROLoader::writeStats(SUMOTime time, SUMOTime start, int absNo) throw() {
    if (myOptions.getBool("verbose")) {
        SUMOReal perc = (SUMOReal)(time-start) / (SUMOReal) absNo;
        cout.setf(ios::fixed , ios::floatfield) ;    // use decimal format
        cout.setf(ios::showpoint) ;    // print decimal point
        cout << setprecision(OUTPUT_ACCURACY);
        MsgHandler::getMessageInstance()->progressMsg("Reading time step: " + toString(time) + "  (" + toString(time-start) + "/" + toString(absNo) + " = " + toString(perc * 100) + "% done)       ");
    }
}



void
ROLoader::destroyHandlers() throw() {
    for (RouteLoaderCont::const_iterator i=myHandler.begin(); i!=myHandler.end(); ++i) {
        delete *i;
    }
    myHandler.clear();
}


/****************************************************************************/

