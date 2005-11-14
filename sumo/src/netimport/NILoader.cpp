/***************************************************************************
                          NILoader.cpp
                          An interface to the loading operations of the
                          netconverter
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.13  2005/11/14 09:53:49  dkrajzew
// "speed-in-km" is now called "speed-in-kmh"; removed two files definition for arcview
//
// Revision 1.12  2005/10/17 09:18:43  dkrajzew
// got rid of the old MSVC memory leak checker
//
// Revision 1.11  2005/10/07 11:41:37  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/23 06:13:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/07/12 12:35:21  dkrajzew
// elmar2 importer included; debugging
//
// Revision 1.7  2005/04/27 12:24:24  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.6  2004/11/23 10:23:34  dkrajzew
// debugging
//
// Revision 1.5  2004/08/02 12:46:23  dkrajzew
// got rid of the shapelib-interface; conversion of geocoordinates added
//
// Revision 1.4  2004/07/02 09:34:37  dkrajzew
// elmar and tiger import added
//
// Revision 1.3  2004/01/26 07:06:32  dkrajzew
// removed some dead code; usage of xmlhelpers added
//
// Revision 1.2  2004/01/12 15:37:10  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.1  2003/07/17 12:15:39  dkrajzew
// NBLoader moved to netimport; NBNetBuilder performs now all the building steps
//
// Revision 1.9  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.8  2003/06/19 07:17:21  dkrajzew
// new message subsystem paradigm applied
//
// Revision 1.7  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a messageing or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.6  2003/03/20 16:23:09  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.5  2003/03/12 16:47:53  dkrajzew
// extension for artemis-import
//
// Revision 1.4  2003/02/13 15:54:07  dkrajzew
// xml-loaders now use new options
//
// Revision 1.3  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.2  2002/10/17 13:32:55  dkrajzew
// handling of connection specification files added
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.8  2002/07/25 08:36:43  dkrajzew
// Visum7.5 and Cell import added
//
// Revision 1.7  2002/06/21 10:13:27  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.6  2002/06/17 15:19:29  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.5  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/10 06:56:14  dkrajzew
// Conversion of strings (XML and c-strings) to numerical values generalized; options now recognize false numerical input
//
// Revision 1.3  2002/05/14 04:42:55  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:11  dkrajzew
// Windows eol removed; minor SUMOReal to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.3  2002/03/22 10:50:03  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.2  2002/03/15 09:17:11  traffic
// A non-verbose mode is now possible and the handling of map logics is removed
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <parsers/SAXParser.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/importio/LineReader.h>
#include <utils/common/FileHelpers.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
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

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method defintions
 * ======================================================================= */
NILoader::NILoader(NBNetBuilder &nb)
    : myNetBuilder(nb)
{
}


NILoader::~NILoader()
{
}


void
NILoader::load(OptionsCont &oc)
{
    // get the report options
    // get the format to use
    //string type = oc.getString("used-file-format");
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
    if(myNetBuilder.getNodeCont().size()==0) {
        MsgHandler::getErrorInstance()->inform("No nodes loaded.");
        throw ProcessError();
    }
    if(myNetBuilder.getEdgeCont().size()==0) {
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
    try {
        if(oc.isUsableFileList("sumo-net")) {
            loadSUMOFiles(oc, LOADFILTER_ALL, oc.getString("sumo-net"),
                "sumo-net");
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.msg());
    }
}


void
NILoader::loadSUMOFiles(OptionsCont &oc, LoadFilter what, const string &files,
                        const string &type)
{
    // build the handlers to load the data
    std::vector<SUMOSAXHandler*> handlers;
    if(what==LOADFILTER_ALL) {
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
NILoader::loadXML(OptionsCont &oc) {
    // load types
    try {
        if(oc.isUsableFileList("xml-type-files")) {
			NIXMLTypesHandler *handler =
				new NIXMLTypesHandler(myNetBuilder.getTypeCont());
            loadXMLType(handler, oc.getString("xml-type-files"), "types");
            myNetBuilder.getTypeCont().report();
        } else {
            if(oc.isSet("e")&&oc.isSet("n")) {
                WRITE_WARNING("No types defined, using defaults...");
            }
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.msg());
    }

    // load nodes
    try {
        if(oc.isUsableFileList("xml-node-files")) {
            NIXMLNodesHandler *handler =
                new NIXMLNodesHandler(myNetBuilder.getNodeCont(),
                    myNetBuilder.getTLLogicCont(), oc);
            loadXMLType(handler, oc.getString("xml-node-files"), "nodes");
            myNetBuilder.getNodeCont().report();
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.msg());
    }

    // load the edges
    try {
        if(oc.isUsableFileList("xml-edge-files")) {
            NIXMLEdgesHandler *handler =
                new NIXMLEdgesHandler(myNetBuilder.getNodeCont(),
					myNetBuilder.getEdgeCont(),
					myNetBuilder.getTypeCont(), oc);
            loadXMLType(handler, oc.getString("xml-edge-files"), "edges");
            myNetBuilder.getEdgeCont().report();
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.msg());
    }

    // load the connections
    try {
        if(oc.isUsableFileList("xml-connection-files")) {
            NIXMLConnectionsHandler *handler =
                new NIXMLConnectionsHandler(myNetBuilder.getEdgeCont());
            loadXMLType(handler, oc.getString("xml-connection-files"), "connections");
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.msg());
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
        while(st.hasNext()) {
            string file = st.next();
            handler->setFileName(file);
            loadXMLFile(*parser, file, type);
        }
    } catch (const XMLException& toCatch) {
        MsgHandler::getErrorInstance()->inform(
            TplConvert<XMLCh>::_2str(toCatch.getMessage()));
        MsgHandler::getErrorInstance()->inform(
            string("  The ") + type  + string(" could not be loaded from '")
            + handler->getFileName() + string("'."));
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
    WRITE_MESSAGE(string("Parsing the ") + type + string(" from '")+ string(file) + string("'..."));
    parser.parse(file.c_str());
}


void
NILoader::loadCell(OptionsCont &oc) {
    LineReader lr;
    // load nodes
    if(oc.isSet("cell-node-file")) {
        WRITE_MESSAGE("Loading nodes... ");
        string file = oc.getString("cell-node-file");
        NICellNodesHandler handler(myNetBuilder.getNodeCont(), file);
        if(!useLineReader(lr, file, handler)) {
            throw ProcessError();
        }
        WRITE_MESSAGE("done.");
        myNetBuilder.getNodeCont().report();
    }
    // load edges
    if(oc.isSet("cell-edge-file")) {
        WRITE_MESSAGE("Loading edges... ");
        string file = oc.getString("cell-edge-file");
        // parse the file
        NICellEdgesHandler handler(myNetBuilder.getNodeCont(),
            myNetBuilder.getEdgeCont(),
            myNetBuilder.getTypeCont(),
            file, NBCapacity2Lanes(oc.getFloat("N")));
        if(!useLineReader(lr, file, handler)) {
            throw ProcessError();
        }
        WRITE_MESSAGE("done.");
        myNetBuilder.getEdgeCont().report();
    }
}


bool
NILoader::useLineReader(LineReader &lr, const std::string &file,
                        LineHandler &lh) {
    // check opening
    if(!lr.setFileName(file)) {
        MsgHandler::getErrorInstance()->inform(
            string("The file '") + file + string("' could not be opened."));
        return false;
    }
    lr.readAll(lh);
    return true;
}


void
NILoader::loadVisum(OptionsCont &oc) {
    if(!oc.isSet("visum")) {
        return;
    }
    // load the visum network
    NIVisumLoader loader(myNetBuilder, oc.getString("visum"),
        NBCapacity2Lanes(oc.getFloat("N")));
    loader.load(oc);
}


void
NILoader::loadArcView(OptionsCont &oc)
{
    if(!oc.isSet("arcview")) {
        return;
    }
    // check whether the correct set of entries is given
    //  and compute both file names
    string dbf_file = oc.getString("arcview") + string(".dbf");
    string shp_file = oc.getString("arcview") + string(".shp");
    string shx_file = oc.getString("arcview") + string(".shx");
    // check whether the files do exist
    if(!FileHelpers::exists(dbf_file)) {
        MsgHandler::getErrorInstance()->inform(string("File not found: ") + dbf_file);
    }
    if(!FileHelpers::exists(shp_file)) {
        MsgHandler::getErrorInstance()->inform(string("File not found: ") + shp_file);
    }
    if(!FileHelpers::exists(shx_file)) {
        MsgHandler::getErrorInstance()->inform(string("File not found: ") + shx_file);
    }
    if(MsgHandler::getErrorInstance()->wasInformed()) {
        return;
    }
    // load the arcview files
    NIArcView_Loader loader(myNetBuilder.getNodeCont(),
        myNetBuilder.getEdgeCont(),
        dbf_file, shp_file, oc.getBool("speed-in-kmh"));
    loader.load(oc);
}


void
NILoader::loadVissim(OptionsCont &oc) {
    if(!oc.isSet("vissim")) {
        return;
    }
    // load the visum network
    NIVissimLoader loader(myNetBuilder, oc.getString("vissim"));
    loader.load(oc);
}


void
NILoader::loadArtemis(OptionsCont &oc) {
    if(!oc.isSet("artemis")) {
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
    if(!oc.isSet("elmar")&&!oc.isSet("elmar2")) {
        return;
    }
    std::string opt;
    bool unsplitted;
    if(oc.isSet("elmar")) {
        opt = "elmar";
        unsplitted = false;
    } else {
        opt = "elmar2";
        unsplitted = true;
    }

    LineReader lr;
    // load min/max
    string unsplitted_nodes_file
        = oc.getString(opt) + string("_nodes_unsplitted.txt");
    lr.setFileName(unsplitted_nodes_file);
    if(!lr.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("Could not open '") + unsplitted_nodes_file
            + string("'."));
        throw ProcessError();
    }
    SUMOReal xmin, xmax, ymin, ymax;
    try {
        string line = lr.readLine();
        line = lr.readLine();
        xmin = TplConvert<char>::_2SUMOReal(lr.readLine().c_str());
        xmax = TplConvert<char>::_2SUMOReal(lr.readLine().c_str());
        ymin = TplConvert<char>::_2SUMOReal(lr.readLine().c_str());
        ymax = TplConvert<char>::_2SUMOReal(lr.readLine().c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform(
            string("Error on reading min/max definitions from '")
            + oc.getString("elmar") + string("_nodes_unsplitted.txt")
            + string("'."));
        throw ProcessError();
    }
    // load nodes
    std::map<std::string, Position2DVector> myGeoms;
    WRITE_MESSAGE("Loading nodes... ");
    if(!unsplitted) {
        string file = oc.getString(opt) + string("_nodes.txt");
        NIElmarNodesHandler handler1(myNetBuilder.getNodeCont(), file,
            (xmin+xmax)/(SUMOReal) 2.0, (ymin+ymax)/(SUMOReal) 2.0);
        if(!useLineReader(lr, file, handler1)) {
            throw ProcessError();
        }
    } else {
        string file = oc.getString(opt) + string("_nodes_unsplitted.txt");
        NIElmar2NodesHandler handler1(myNetBuilder.getNodeCont(), file,
            (xmin+xmax)/(SUMOReal) 2.0, (ymin+ymax)/(SUMOReal) 2.0, myGeoms);
        if(!useLineReader(lr, file, handler1)) {
            throw ProcessError();
        }
    }
    WRITE_MESSAGE("done.");
    myNetBuilder.getNodeCont().report();

    // load edges
    WRITE_MESSAGE("Loading edges... ");
    if(!unsplitted) {
        std::string file = oc.getString(opt) + string("_links.txt");
        // parse the file
        NIElmarEdgesHandler handler2(myNetBuilder.getNodeCont(),
            myNetBuilder.getEdgeCont(), file);
        if(!useLineReader(lr, file, handler2)) {
            throw ProcessError();
        }
    } else {
        std::string file = oc.getString(opt) + string("_links_unsplitted.txt");
        // parse the file
        NIElmar2EdgesHandler handler2(myNetBuilder.getNodeCont(),
            myNetBuilder.getEdgeCont(), file, myGeoms);
        if(!useLineReader(lr, file, handler2)) {
            throw ProcessError();
        }
    }
    WRITE_MESSAGE("done.");
    myNetBuilder.getEdgeCont().report();
}


void
NILoader::loadTiger(OptionsCont &oc)
{
    if(!oc.isSet("tiger")) {
        return;
    }
    NITigerLoader l(myNetBuilder.getEdgeCont(),
        myNetBuilder.getNodeCont(), oc.getString("tiger"));
    l.load(oc);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



