/****************************************************************************/
/// @file    NLBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The main interface for loading a microsim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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

#include "NLBuilder.h"
#include <microsim/MSNet.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSGlobals.h>
#include <iostream>
#include <vector>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <string>
#include <map>
#include "NLHandler.h"
#include "NLEdgeControlBuilder.h"
#include "NLJunctionControlBuilder.h"
#include "NLDetectorBuilder.h"
#include "NLTriggerBuilder.h"
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleTransfer.h>
#include <microsim/trigger/MSTriggerControl.h>
#include <microsim/output/MSMeanData_Net_Utils.h>
#include <microsim/MSRouteLoaderControl.h>
#include <microsim/MSRouteLoader.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/TplConvert.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/SysUtils.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSFrame.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include "NLGeomShapeBuilder.h"

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
// NLBuilder::EdgeFloatTimeLineRetriever_EdgeWeight - methods
// ---------------------------------------------------------------------------
NLBuilder::EdgeFloatTimeLineRetriever_EdgeWeight::EdgeFloatTimeLineRetriever_EdgeWeight(
    MSNet *net)
        : myNet(net)
{}


NLBuilder::EdgeFloatTimeLineRetriever_EdgeWeight::~EdgeFloatTimeLineRetriever_EdgeWeight()
{}


void
NLBuilder::EdgeFloatTimeLineRetriever_EdgeWeight::addEdgeWeight(const std::string &id,
        SUMOReal val,
        SUMOTime beg,
        SUMOTime end)
{
    MSEdge *e = MSEdge::dictionary(id);
    if (e!=0) {
        e->addWeight(val, beg, end);
    } else {
        MsgHandler::getErrorInstance()->inform("Trying to set a weight for the unknown edge '" + id + "'.");
    }
}



// ---------------------------------------------------------------------------
// NLBuilder - methods
// ---------------------------------------------------------------------------
NLBuilder::NLBuilder(const OptionsCont &oc,
                     MSNet &net,
                     NLEdgeControlBuilder &eb,
                     NLJunctionControlBuilder &jb,
                     NLDetectorBuilder &db,
                     NLTriggerBuilder &tb,
                     NLGeomShapeBuilder &sb,
                     NLHandler &xmlHandler)
        : myOptions(oc), myEdgeBuilder(eb), myJunctionBuilder(jb),
        myDetectorBuilder(db), myTriggerBuilder(tb), myShapeBuilder(sb),
        myNet(net), myXMLHandler(xmlHandler)
{}


NLBuilder::~NLBuilder()
{}


bool
NLBuilder::build()
{
    SAX2XMLReader* parser = XMLSubSys::getSAXReader(myXMLHandler);
    // try to build the net
    if (!load("net-file", *parser)) {
        delete parser;
        return false;
    }
    buildNet();
#ifdef HAVE_MESOSIM
    // load the previous state if wished
    if (myOptions.isSet("load-state")) {
        long before = SysUtils::getCurrentMillis();
        BinaryInputDevice strm(myOptions.getString("load-state"));
        if (!strm.good()) {
            MsgHandler::getErrorInstance()->inform("Could not read state from '" + myOptions.getString("load-state") + "'!");
        } else {
            MsgHandler::getMessageInstance()->beginProcessMsg("Loading state from '" + myOptions.getString("load-state") + "'...");
            myNet.loadState(strm);
        }
        if (MsgHandler::getErrorInstance()->wasInformed()) {
            delete parser;
            return false;
        }
        MsgHandler::getMessageInstance()->endProcessMsg("done (" + toString(SysUtils::getCurrentMillis()-before) + "ms).");
    }
#endif
    // load weights if wished
    if (myOptions.isSet("weight-files")) {
        if (!myOptions.isUsableFileList("weight-files")) {
            delete parser;
            return false;
        }
        // start parsing; for each file in the list
        StringTokenizer st(myOptions.getString("weight-files"), ';');
        while (st.hasNext()) {
            string tmp = st.next();
            // report about loading when wished
            WRITE_MESSAGE("Loading weights from '" + tmp + "'...");
            // check whether the file exists
            if (!FileHelpers::exists(tmp)) {
                // report error if not
                MsgHandler::getErrorInstance()->inform("The weights file '" + tmp + "' does not exist!");
                return false;
            } else {
                EdgeFloatTimeLineRetriever_EdgeWeight retriever(&myNet);
                SAXWeightsHandler::ToRetrieveDefinition *def = new SAXWeightsHandler::ToRetrieveDefinition("traveltime", true, retriever);
                SAXWeightsHandler wh(def, tmp);
                // parse the file
                if (!XMLSubSys::runParser(wh, tmp)) {
                    return false;
                }
            }
        }
    }
    // load routes
    if (myOptions.isSet("route-files")&&myOptions.getInt("route-steps")<=0) {
        if (!load("route-files", *parser)) {
            delete parser;
            return false;
        }
    }
    // load additional net elements (sources, detectors, ...)
    if (myOptions.isSet("additional-files")) {
        if (!load("additional-files", *parser)) {
            delete parser;
            return false;
        }
    }
    delete parser;
    WRITE_MESSAGE("Loading done.");
    return true;
}


void
NLBuilder::buildNet()
{
    myJunctionBuilder.closeJunctions(myDetectorBuilder, myXMLHandler.getContinuations());
    MSEdgeControl *edges = myEdgeBuilder.build();
    MSFrame::buildStreams();
    std::vector<MSMeanData_Net*> meanData = 
        MSMeanData_Net_Utils::buildList(myNet.getDetectorControl(), *edges,
            myOptions.getIntVector("dump-intervals"), myOptions.getString("dump-basename"),
            myOptions.getIntVector("lanedump-intervals"), myOptions.getString("lanedump-basename"),
            myOptions.getIntVector("dump-begins"), myOptions.getIntVector("dump-ends"),
            !myOptions.getBool("exclude-empty-edges"), !myOptions.getBool("exclude-empty-lanes"));
    vector<int> stateDumpTimes;
    string stateDumpFiles;
#ifdef HAVE_MESOSIM
    stateDumpTimes = myOptions.getIntVector("save-state.times");
    stateDumpFiles = myOptions.getString("save-state.prefix");
#endif
    myNet.closeBuilding(edges, myJunctionBuilder.build(),
        buildRouteLoaderControl(myOptions), myJunctionBuilder.buildTLLogics(),
        meanData, stateDumpTimes, stateDumpFiles);
}


bool
NLBuilder::load(const std::string &mmlWhat, SAX2XMLReader &parser)
{
    // start parsing
    parser.setContentHandler(&myXMLHandler);
    parser.setErrorHandler(&myXMLHandler);
    if (!OptionsCont::getOptions().isUsableFileList(mmlWhat)) {
        return false;
    }
    long before = SysUtils::getCurrentMillis();
    vector<string> files = OptionsCont::getOptions().getStringVector(mmlWhat);
    for (vector<string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
        WRITE_MESSAGE("Loading " + mmlWhat + " from '" + *fileIt + "'...");
        myXMLHandler.setFileName(*fileIt);
        parser.parse(fileIt->c_str());
        if (MsgHandler::getErrorInstance()->wasInformed()) {
            WRITE_MESSAGE("Loading of " + mmlWhat + " failed.");
            return false;
        }
    }
    WRITE_MESSAGE("Loading of " + mmlWhat + " done (" + toString(SysUtils::getCurrentMillis()-before) + "ms).");
    return true;
}


MSRouteLoaderControl *
NLBuilder::buildRouteLoaderControl(const OptionsCont &oc)
{
    // build the loaders
    MSRouteLoaderControl::LoaderVector loaders;
    // check whether a list is existing
    if (oc.isSet("route-files")&&oc.getInt("route-steps")>0) {
        vector<string> files = oc.getStringVector("route-files");
        for (vector<string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
            if (!FileHelpers::exists(*fileIt)) {
                throw ProcessError("The route file '" + *fileIt + "' does not exist.");
            }
        }
        // open files for reading
        for (vector<string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
            loaders.push_back(myNet.buildRouteLoader(*fileIt, oc.getInt("incremental-dua-base"), oc.getInt("incremental-dua-step")));
        }
    }
    // build the route control
    return new MSRouteLoaderControl(myNet, oc.getInt("route-steps"), loaders);
}


/****************************************************************************/

