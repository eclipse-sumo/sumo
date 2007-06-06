/****************************************************************************/
/// @file    NLBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Container for MSNet during its building
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
#include <microsim/output/meandata/MSMeanData_Net_Utils.h>
#include <microsim/MSRouteLoaderControl.h>
#include <microsim/MSRouteLoader.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/TplConvert.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/XMLHelpers.h>
#include <microsim/output/MSDetector2File.h>
#include <microsim/output/MSDetectorControl.h>
#include <sumo_only/SUMOFrame.h>
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
        : m_pOptions(oc), myEdgeBuilder(eb), myJunctionBuilder(jb),
        myDetectorBuilder(db), myTriggerBuilder(tb), myShapeBuilder(sb),
        myNet(net), myXMLHandler(xmlHandler)
{}


NLBuilder::~NLBuilder()
{}



bool
NLBuilder::build()
{
    SAX2XMLReader* parser = XMLHelpers::getSAXReader(myXMLHandler);
    bool ok = load("net", LOADFILTER_ALL, m_pOptions.getString("net-file"), *parser);
    // try to build the net
    if (ok) {
        ok = buildNet();
    }
    // load the previous state if wished
    if (ok&&m_pOptions.isSet("load-state")) {
        BinaryInputDevice strm(m_pOptions.getString("load-state"));
        if (!strm.good()) {
            MsgHandler::getErrorInstance()->inform("Could not read state from '" + m_pOptions.getString("load-state") + "'!");
            ok = false;
        } else {
            MsgHandler::getMessageInstance()->beginProcessMsg("Loading state from '" + m_pOptions.getString("load-state") + "'...");
            myNet.loadState(strm, (long) 0xfffffff);
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
    }
    // load weights if wished
    if (ok&&m_pOptions.isSet("weight-files")) {
        if (!FileHelpers::checkFileList("weights", m_pOptions.getString("weight-files"))) {
            ok = false;
        }
        // start parsing; for each file in the list
        StringTokenizer st(m_pOptions.getString("weight-files"), ';');
        while (st.hasNext()&&ok) {
            string tmp = st.next();
            // report about loading when wished
            WRITE_MESSAGE("Loading weights from '" + tmp + "'...");
            // check whether the file exists
            if (!FileHelpers::exists(tmp)) {
                // report error if not
                MsgHandler::getErrorInstance()->inform("The weights file '" + tmp + "' does not exist!");
                ok = false;
            } else {
                EdgeFloatTimeLineRetriever_EdgeWeight retriever(&myNet);
                WeightsHandler::ToRetrieveDefinition *def = new WeightsHandler::ToRetrieveDefinition("edge", "traveltime", true, retriever);
                WeightsHandler wh(def, tmp);
                // parse the file
                XMLHelpers::runParser(wh, tmp);
                ok = !(MsgHandler::getErrorInstance()->wasInformed());
            }
        }
    }
    // load routes
    if (m_pOptions.isSet("route-files")&&ok&&m_pOptions.getInt("route-steps")<=0) {
        ok = load("routes", LOADFILTER_DYNAMIC, m_pOptions.getString("route-files"),
                  *parser);
    }
    // load additional net elements (sources, detectors, ...)
    if (m_pOptions.isSet("additional-files")&&ok) {
        ok = load("additional elements",
                  (NLLoadFilter)((int) LOADFILTER_NETADD|(int) LOADFILTER_DYNAMIC),
                  m_pOptions.getString("additional-files"), *parser);
    }
    subreport("Loading done.", "Loading failed.");
    delete parser;
    return ok&&!MsgHandler::getErrorInstance()->wasInformed();
}


bool
NLBuilder::buildNet(GNEImageProcWindow &)
{
    return buildNet();
}


bool
NLBuilder::buildNet()
{
    myJunctionBuilder.closeJunctions(myDetectorBuilder, myXMLHandler.getContinuations());
    MSEdgeControl *edges = myEdgeBuilder.build();
    std::vector<OutputDevice*> streams = SUMOFrame::buildStreams(m_pOptions);
    MSMeanData_Net_Cont meanData = MSMeanData_Net_Utils::buildList(
                                       myNet.getDetectorControl().getDet2File(), *edges,
                                       m_pOptions.getIntVector("dump-intervals"),
                                       m_pOptions.getString("dump-basename"),
                                       m_pOptions.getIntVector("lanedump-intervals"),
                                       m_pOptions.getString("lanedump-basename"),
                                       m_pOptions.getIntVector("dump-begins"), m_pOptions.getIntVector("dump-ends"));
    myNet.closeBuilding(
        edges,
        myJunctionBuilder.build(),
        buildRouteLoaderControl(m_pOptions),
        myJunctionBuilder.buildTLLogics(),
        streams, meanData,
        m_pOptions.getIntVector("save-state.times"),
        m_pOptions.getString("save-state.prefix"));
    return true;
}

bool
NLBuilder::load(const std::string &mmlWhat,
                NLLoadFilter what,
                const string &files,
                SAX2XMLReader &parser)
{
    // initialise the handler for the current type of data
    myXMLHandler.setWanted(what);
    // check whether the list of files does not contain ';'s only
    if (!FileHelpers::checkFileList(mmlWhat, files)) {
        return false;
    }
    // start parsing
    parser.setContentHandler(&myXMLHandler);
    parser.setErrorHandler(&myXMLHandler);
    parse(mmlWhat, files, parser);
    // report about loaded structures
    subreport(
        "Loading of " + mmlWhat + " done.",
        "Loading of " + mmlWhat + " failed.");
    return !MsgHandler::getErrorInstance()->wasInformed();
}


bool
NLBuilder::parse(const std::string &mmlWhat,
                 const string &files,
                 SAX2XMLReader &parser)
{
    // for each file in the list
    StringTokenizer st(files, ';');
    bool ok = true;
    while (st.hasNext()&&ok) {
        string tmp = st.next();
        // report about loading when wished
        WRITE_MESSAGE("Loading " + mmlWhat + " from '" + tmp + "'...");
        // check whether the file exists
        if (!FileHelpers::exists(tmp)) {
            // report error if not
            MsgHandler::getErrorInstance()->inform("The " + mmlWhat + " file '" + tmp + "' does not exist!");
            return false;
        } else {
            // parse the file
            myXMLHandler.setFileName(tmp);
            parser.parse(tmp.c_str());
            ok = !(MsgHandler::getErrorInstance()->wasInformed());
        }
    }
    return ok;
}


void
NLBuilder::subreport(const std::string &ok, const std::string &wrong)
{
    if (!MsgHandler::getErrorInstance()->wasInformed()) {
        WRITE_MESSAGE(ok.c_str());
    } else {
        WRITE_MESSAGE(wrong.c_str());
    }
}


MSRouteLoaderControl *
NLBuilder::buildRouteLoaderControl(const OptionsCont &oc)
{
    // build the loaders
    MSRouteLoaderControl::LoaderVector loaders;
    // check whether a list is existing
    if (oc.isSet("r")&&oc.getInt("route-steps")>0) {
        // extract the list
        StringTokenizer st(oc.getString("r"), ';');
        // check whether all files can be opened
        bool ok = true;
        while (st.hasNext()) {
            string name = st.next();
            if (!FileHelpers::exists(name)) {
                MsgHandler::getErrorInstance()->inform("The route file '" + name + "' does not exist.");
                ok = false;
            }
        }
        if (!ok) {
            throw ProcessError();
        }
        // open files for reading
        st.reinit();
        while (st.hasNext()) {
            string file = st.next();
            loaders.push_back(myNet.buildRouteLoader(file, oc.getInt("incremental-dua-base"), oc.getInt("incremental-dua-step")));
        }
    }
    // build the route control
    return new MSRouteLoaderControl(myNet, oc.getInt("route-steps"), loaders);
}



/****************************************************************************/

