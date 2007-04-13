/****************************************************************************/
/// @file    NILoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// An interface to the loading operations of the
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <proj_api.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/importio/LineReader.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringUtils.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <netimport/xml/NIXMLEdgesHandler.h>
#include <netimport/xml/NIXMLNodesHandler.h>
#include <netimport/xml/NIXMLTypesHandler.h>
#include <netimport/xml/NIXMLConnectionsHandler.h>
#include <netimport/cell/NICellNodesHandler.h>
#include <netimport/cell/NICellEdgesHandler.h>
#include <netimport/elmar/NIElmarNodesHandler.h>
#include <netimport/elmar/NIElmarEdgesHandler.h>
#include <netimport/elmar2/NIElmar2NodesHandler.h>
#include <netimport/elmar2/NIElmar2EdgesHandler.h>
#include <netimport/visum/NIVisumLoader.h>
#include <netimport/vissim/NIVissimLoader.h>
#include <netimport/arcview/NIArcView_Loader.h>
#include <netimport/artemis/NIArtemisLoader.h>
#include <netimport/sumo/NISUMOHandlerNodes.h>
#include <netimport/sumo/NISUMOHandlerEdges.h>
#include <netimport/sumo/NISUMOHandlerDepth.h>
#include <netimport/tiger/NITigerLoader.h>
#include <utils/common/XMLHelpers.h>
#include "NILoader.h"
#include <netbuild/NLLoadFilter.h>
#include <utils/common/TplConvert.h>
#include <utils/geoconv/GeoConvHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method defintions
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
        if (!GeoConvHelper::init(oc.getString("proj"), Position2D())) {
            MsgHandler::getErrorInstance()->inform("Could not build projection!");
            throw ProcessError();
        }
    }
    // load types first
    if (oc.isUsableFileList("xml-type-files")) {
        NIXMLTypesHandler *handler =
            new NIXMLTypesHandler(myNetBuilder.getTypeCont());
        loadXMLType(handler, oc.getString("xml-type-files"), "types");
        myNetBuilder.getTypeCont().report();
    }
    // try to load using different methods
    loadSUMO(oc);
    loadCell(oc);
    loadVisum(oc);
    loadArcView(oc);
    loadArtemis(oc);
    loadVissim(oc);
    loadElmar(oc);
    loadTiger(oc);
    loadXML(oc);
    // check the loaded structures
    if (myNetBuilder.getNodeCont().size()==0) {
        MsgHandler::getErrorInstance()->inform("No nodes loaded.");
        throw ProcessError();
    }
    if (myNetBuilder.getEdgeCont().size()==0) {
        MsgHandler::getErrorInstance()->inform("No edges loaded.");
        throw ProcessError();
    }
}


/* -------------------------------------------------------------------------
 * file loading methods
 * ----------------------------------------------------------------------- */
void
NILoader::loadSUMO(OptionsCont &oc)
{
    // load the network
    if (oc.isUsableFileList("sumo-net")) {
        loadSUMOFiles(oc, LOADFILTER_ALL, oc.getString("sumo-net"), "sumo-net");
    }
}


void
NILoader::loadSUMOFiles(OptionsCont &, LoadFilter what, const string &/*files*/,
                        const string &/*type*/)
{
    // build the handlers to load the data
    std::vector<SUMOSAXHandler*> handlers;
    if (what==LOADFILTER_ALL) {
        handlers.push_back(
            new NISUMOHandlerNodes(myNetBuilder.getNodeCont(), what));
        handlers.push_back(
            new NISUMOHandlerEdges(myNetBuilder.getEdgeCont(),
                                   myNetBuilder.getNodeCont(),what));
        handlers.push_back(
            new NISUMOHandlerDepth(what));
    } else {
        handlers.push_back(new NISUMOHandlerDepth(what));
    }
    //
}


void
NILoader::loadXML(OptionsCont &oc)
{
    // load nodes
    if (oc.isUsableFileList("xml-node-files")) {
        NIXMLNodesHandler *handler =
            new NIXMLNodesHandler(myNetBuilder.getNodeCont(),
                                  myNetBuilder.getTLLogicCont(), oc);
        loadXMLType(handler, oc.getString("xml-node-files"), "nodes");
        myNetBuilder.getNodeCont().report();
    }

    // load the edges
    if (oc.isUsableFileList("xml-edge-files")) {
        NIXMLEdgesHandler *handler =
            new NIXMLEdgesHandler(myNetBuilder.getNodeCont(),
                                  myNetBuilder.getEdgeCont(), myNetBuilder.getTypeCont(),
                                  myNetBuilder.getDistrictCont(), oc);
        loadXMLType(handler, oc.getString("xml-edge-files"), "edges");
        myNetBuilder.getEdgeCont().report();
    }

    // load the connections
    if (oc.isUsableFileList("xml-connection-files")) {
        NIXMLConnectionsHandler *handler =
            new NIXMLConnectionsHandler(myNetBuilder.getEdgeCont());
        loadXMLType(handler, oc.getString("xml-connection-files"), "connections");
    }
}


/** loads a single user-specified file */
void
NILoader::loadXMLType(SUMOSAXHandler *handler, const std::string &files,
                      const string &type)
{
    // build parser
    SAX2XMLReader* parser = XMLHelpers::getSAXReader(*handler);
    // start the parsing
    try {
        StringTokenizer st(files, ";");
        while (st.hasNext()) {
            string file = st.next();
            handler->setFileName(file);
            loadXMLFile(*parser, file, type);
        }
    } catch (const XMLException& toCatch) {
        MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(toCatch.getMessage())
                                               + "\n  The " + type  + " could not be loaded from '" + handler->getFileName() + "'.");
        delete handler;
        throw ProcessError();
    }
    delete parser;
    delete handler;
}


void
NILoader::loadXMLFile(SAX2XMLReader &parser, const std::string &file,
                      const string &type)
{
    WRITE_MESSAGE("Parsing " + type + " from '" + file + "'...");
    parser.parse(file.c_str());
}


void
NILoader::loadCell(OptionsCont &oc)
{
    LineReader lr;
    // load nodes
    if (oc.isSet("cell-node-file")) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Loading nodes... ");
        string file = oc.getString("cell-node-file");
        NICellNodesHandler handler(myNetBuilder.getNodeCont(), file);
        if (!useLineReader(lr, file, handler)) {
            throw ProcessError();
        }
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
        myNetBuilder.getNodeCont().report();
    }
    // load edges
    if (oc.isSet("cell-edge-file")) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Loading edges... ");
        string file = oc.getString("cell-edge-file");
        // parse the file
        NICellEdgesHandler handler(myNetBuilder.getNodeCont(),
                                   myNetBuilder.getEdgeCont(),
                                   myNetBuilder.getTypeCont(),
                                   file, NBCapacity2Lanes(oc.getFloat("N")));
        if (!useLineReader(lr, file, handler)) {
            throw ProcessError();
        }
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
        myNetBuilder.getEdgeCont().report();
    }
}


bool
NILoader::useLineReader(LineReader &lr, const std::string &file,
                        LineHandler &lh)
{
    // check opening
    if (!lr.setFileName(file)) {
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
    loader.load(oc);
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
NILoader::loadArtemis(OptionsCont &oc)
{
    if (!oc.isSet("artemis")) {
        return;
    }
    // load the visum network
    NIArtemisLoader loader(oc.getString("artemis"),
                           myNetBuilder.getDistrictCont(),
                           myNetBuilder.getNodeCont(), myNetBuilder.getEdgeCont(),
                           myNetBuilder.getTLLogicCont());
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
    myNetBuilder.getNodeCont().report();

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
    myNetBuilder.getEdgeCont().report();
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



/****************************************************************************/

