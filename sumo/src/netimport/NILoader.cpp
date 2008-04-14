/****************************************************************************/
/// @file    NILoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Perfoms network import
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

#include <string>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/importio/LineReader.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <netimport/NIXMLEdgesHandler.h>
#include <netimport/NIXMLNodesHandler.h>
#include <netimport/NIXMLTypesHandler.h>
#include <netimport/NIXMLConnectionsHandler.h>
#include <netimport/NIElmarNodesHandler.h>
#include <netimport/NIElmarEdgesHandler.h>
#include <netimport/NIElmar2NodesHandler.h>
#include <netimport/NIElmar2EdgesHandler.h>
#include <netimport/NIVisumLoader.h>
#include <netimport/vissim/NIVissimLoader.h>
#include <netimport/NIArcView_Loader.h>
#include <netimport/NIImporter_SUMO.h>
#include <netimport/NITigerLoader.h>
#include <netimport/NIOSMEdgesHandler.h>
#include <netimport/NIOSMNodesHandler.h>
#include <utils/xml/XMLSubSys.h>
#include "NILoader.h"
#include <utils/common/TplConvert.h>
#include <utils/geom/GeoConvHelper.h>

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
NILoader::NILoader(NBNetBuilder &nb)
        : myNetBuilder(nb)
{}


NILoader::~NILoader()
{}


void
NILoader::load(OptionsCont &oc)
{
    // build the projection
    if (!oc.getBool("use-projection")) {
        GeoConvHelper::init("!", Position2D());
    } else if (oc.getBool("proj.simple")) {
        GeoConvHelper::init("-", Position2D());
    } else {
        if (!GeoConvHelper::init(oc.getString("proj"), Position2D(), oc.getBool("proj.inverse"))) {
            throw ProcessError("Could not build projection!");
        }
    }
    // load types first
    NIXMLTypesHandler *handler =
        new NIXMLTypesHandler(myNetBuilder.getTypeCont());
    loadXMLType(handler, oc.getStringVector("xml-type-files"), "types");
    // try to load using different methods
    NIImporter_SUMO::loadNetwork(oc, myNetBuilder);
    loadVisum(oc);
    loadArcView(oc);
    loadVissim(oc);
    loadElmar(oc);
    loadTiger(oc);
    loadOSM(oc);
    loadXML(oc);
    // check the loaded structures
    if (myNetBuilder.getNodeCont().size()==0) {
        throw ProcessError("No nodes loaded.");
    }
    if (myNetBuilder.getEdgeCont().size()==0) {
        throw ProcessError("No edges loaded.");
    }
    // report loaded structures
    WRITE_MESSAGE(" Import done;");
    if (myNetBuilder.getDistrictCont().size()>0) {
        WRITE_MESSAGE("   " + toString(myNetBuilder.getDistrictCont().size()) + " districts loaded.");
    }
    WRITE_MESSAGE("   " + toString(myNetBuilder.getNodeCont().size()) + " nodes loaded.");
    if (myNetBuilder.getTypeCont().size()>0) {
        WRITE_MESSAGE("   " + toString(myNetBuilder.getTypeCont().size()) + " types loaded.");
    }
    WRITE_MESSAGE("   " + toString(myNetBuilder.getEdgeCont().size()) + " edges loaded.");
    if (myNetBuilder.getEdgeCont().getNoEdgeSplits()>0) {
        WRITE_MESSAGE("The split of edges was performed "+ toString(myNetBuilder.getEdgeCont().getNoEdgeSplits()) + " times.");
    }
}


/* -------------------------------------------------------------------------
 * file loading methods
 * ----------------------------------------------------------------------- */
/*
void
NILoader::loadSUMO(OptionsCont &oc)
{
    // load the network
    if (oc.isUsableFileList("sumo-net")) {
        loadXMLType(new NIImporter_SUMO(myNetBuilder.getEdgeCont(), myNetBuilder.getNodeCont()),
                oc.getStringVector("sumo-net"), "sumo-net");
    }
}
*/

void
NILoader::loadXML(OptionsCont &oc)
{
    // load nodes
    loadXMLType(new NIXMLNodesHandler(myNetBuilder.getNodeCont(),
                                      myNetBuilder.getTLLogicCont(), oc),
                oc.getStringVector("xml-node-files"), "nodes");
    // load the edges
    loadXMLType(new NIXMLEdgesHandler(myNetBuilder.getNodeCont(),
                                      myNetBuilder.getEdgeCont(),
                                      myNetBuilder.getTypeCont(),
                                      myNetBuilder.getDistrictCont(), oc),
                oc.getStringVector("xml-edge-files"), "edges");
    // load the connections
    loadXMLType(new NIXMLConnectionsHandler(myNetBuilder.getEdgeCont()),
                oc.getStringVector("xml-connection-files"), "connections");
}


/** loads a single user-specified file */
void
NILoader::loadXMLType(SUMOSAXHandler *handler, const vector<string> &files,
                      const string &type)
{
    // build parser
    SAX2XMLReader* parser = XMLSubSys::getSAXReader(*handler);
    string exceptMsg = "";
    // start the parsing
    try {
        for (vector<string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
            if (!FileHelpers::exists(*file)) {
                MsgHandler::getErrorInstance()->inform("Could not open " + type + "-file '" + *file + "'.");
                exceptMsg = "Process Error";
                continue;
            }
            handler->setFileName(*file);
            MsgHandler::getMessageInstance()->beginProcessMsg("Parsing " + type + " from '" + *file + "'...");
            parser->parse(file->c_str());
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
    } catch (const XMLException& toCatch) {
        exceptMsg = TplConvert<XMLCh>::_2str(toCatch.getMessage())
                    + "\n  The " + type  + " could not be loaded from '" + handler->getFileName() + "'.";
    } catch (const ProcessError& toCatch) {
        exceptMsg = string(toCatch.what()) + "\n  The " + type  + " could not be loaded from '" + handler->getFileName() + "'.";
    } catch (...) {
        exceptMsg = "The " + type  + " could not be loaded from '" + handler->getFileName() + "'.";
    }
    delete parser;
    delete handler;
    if (exceptMsg != "") {
        throw ProcessError(exceptMsg);
    }
}


bool
NILoader::useLineReader(LineReader &lr, const std::string &file,
                        LineHandler &lh)
{
    // check opening
    if (!lr.setFile(file)) {
        MsgHandler::getErrorInstance()->inform("The file '" + file + "' could not be opened.");
        return false;
    }
    lr.readAll(lh);
    return true;
}


void
NILoader::loadVisum(OptionsCont &oc)
{
    if (!oc.isSet("visum")) {
        return;
    }
    // load the visum network
    NIVisumLoader loader(myNetBuilder, oc.getString("visum"),
                         NBCapacity2Lanes(oc.getFloat("capacity-norm")),
                         oc.getBool("visum.use-net-prio"));
    loader.load();
}


void
NILoader::loadArcView(OptionsCont &oc)
{
    if (!oc.isSet("arcview")) {
        return;
    }
    // check whether the correct set of entries is given
    //  and compute both file names
    string dbf_file = oc.getString("arcview") + ".dbf";
    string shp_file = oc.getString("arcview") + ".shp";
    string shx_file = oc.getString("arcview") + ".shx";
    // check whether the files do exist
    if (!FileHelpers::exists(dbf_file)) {
        MsgHandler::getErrorInstance()->inform("File not found: " + dbf_file);
    }
    if (!FileHelpers::exists(shp_file)) {
        MsgHandler::getErrorInstance()->inform("File not found: " + shp_file);
    }
    if (!FileHelpers::exists(shx_file)) {
        MsgHandler::getErrorInstance()->inform("File not found: " + shx_file);
    }
    if (MsgHandler::getErrorInstance()->wasInformed()) {
        return;
    }
    // load the arcview files
    NIArcView_Loader loader(oc,
                            myNetBuilder.getNodeCont(), myNetBuilder.getEdgeCont(), myNetBuilder.getTypeCont(),
                            dbf_file, shp_file, oc.getBool("speed-in-kmh"), !oc.getBool("navtech.rechecklanes"));
    loader.load(oc);
}


void
NILoader::loadVissim(OptionsCont &oc)
{
    if (!oc.isSet("vissim")) {
        return;
    }
    // load the visum network
    NIVissimLoader loader(myNetBuilder, oc.getString("vissim"));
    loader.load(oc);
}


void
NILoader::loadElmar(OptionsCont &oc)
{
    if (!oc.isSet("elmar")&&!oc.isSet("elmar2")) {
        return;
    }
    // check which one to use
    std::string opt;
    bool unsplitted;
    if (oc.isSet("elmar")) {
        opt = "elmar";
        unsplitted = false;
    } else {
        opt = "elmar2";
        unsplitted = true;
    }

    LineReader lr;
    // load nodes
    std::map<std::string, Position2DVector> myGeoms;
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading nodes...");
    if (!unsplitted) {
        string file = oc.getString(opt) + "_nodes.txt";
        NIElmarNodesHandler handler1(myNetBuilder.getNodeCont(), file);
        if (!useLineReader(lr, file, handler1)) {
            throw ProcessError();
        }
    } else {
        string file = oc.getString(opt) + "_nodes_unsplitted.txt";
        NIElmar2NodesHandler handler1(myNetBuilder.getNodeCont(), file, myGeoms);
        if (!useLineReader(lr, file, handler1)) {
            throw ProcessError();
        }
    }
    MsgHandler::getMessageInstance()->endProcessMsg("done.");

    // load edges
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading edges...");
    if (!unsplitted) {
        std::string file = oc.getString(opt) + "_links.txt";
        // parse the file
        NIElmarEdgesHandler handler2(myNetBuilder.getNodeCont(),
                                     myNetBuilder.getEdgeCont(), file, !oc.getBool("navtech.rechecklanes"));
        if (!useLineReader(lr, file, handler2)) {
            throw ProcessError();
        }
    } else {
        std::string file = oc.getString(opt) + "_links_unsplitted.txt";
        // parse the file
        NIElmar2EdgesHandler handler2(myNetBuilder.getNodeCont(),
                                      myNetBuilder.getEdgeCont(), file, myGeoms, !oc.getBool("navtech.rechecklanes"));
        if (!useLineReader(lr, file, handler2)) {
            throw ProcessError();
        }
    }
    myNetBuilder.getEdgeCont().recheckLaneSpread();
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
}


void
NILoader::loadTiger(OptionsCont &oc)
{
    if (!oc.isSet("tiger")) {
        return;
    }
    NITigerLoader l(myNetBuilder.getEdgeCont(), myNetBuilder.getNodeCont(),
                    oc.getString("tiger"));
    l.load(oc);
}


void
NILoader::loadOSM(OptionsCont &oc)
{
    if (!oc.isSet("osm-files")) {
        return;
    }
    std::map<int, NIOSMNode*> tmpNodes;
    // load nodes
    loadXMLType(new NIOSMNodesHandler(tmpNodes, oc),
                oc.getStringVector("osm-files"), "nodes");
    // load the edges
    loadXMLType(new NIOSMEdgesHandler(tmpNodes,
                                      myNetBuilder.getNodeCont(), myNetBuilder.getEdgeCont(),
                                      myNetBuilder.getTypeCont(), myNetBuilder.getDistrictCont(), oc),
                oc.getStringVector("osm-files"), "edges");
    for (std::map<int, NIOSMNode*>::const_iterator i=tmpNodes.begin(); i!=tmpNodes.end(); ++i) {
        delete(*i).second;
    }
}



/****************************************************************************/
