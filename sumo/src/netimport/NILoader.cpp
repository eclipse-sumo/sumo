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
// Windows eol removed; minor double to int conversions removed;
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
//
/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * included modules
 * ======================================================================= */
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
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <netimport/xml/NIXMLEdgesHandler.h>
#include <netimport/xml/NIXMLNodesHandler.h>
#include <netimport/xml/NIXMLTypesHandler.h>
#include <netimport/xml/NIXMLConnectionsHandler.h>
#include <netimport/cell/NICellNodesHandler.h>
#include <netimport/cell/NICellEdgesHandler.h>
#include <netimport/visum/NIVisumLoader.h>
#include <netimport/vissim/NIVissimLoader.h>
#include <netimport/arcview/NIArcView_Loader.h>
#include <netimport/artemis/NIArtemisLoader.h>
#include <netimport/sumo/NISUMOHandlerNodes.h>
#include <netimport/sumo/NISUMOHandlerEdges.h>
#include <netimport/sumo/NISUMOHandlerDepth.h>
#include <utils/common/XMLHelpers.h>
#include "NILoader.h"
#include <netbuild/NLLoadFilter.h>
#include <utils/convert/TplConvert.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method defintions
 * ======================================================================= */
void NILoader::load(OptionsCont &oc) {
    // get the report options
    // get the format to use
    //string type = oc.getString("used-file-format");
    // try to load using different methods
    loadSUMO(oc);
    loadXML(oc);
    loadCell(oc);
    loadVisum(oc);
    loadArcView(oc);
    loadArtemis(oc);
    loadVissim(oc);
    // check the loaded structures
    if(NBNodeCont::size()==0) {
        MsgHandler::getErrorInstance()->inform("No nodes loaded.");
        throw ProcessError();
    }
    if(NBEdgeCont::size()==0) {
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
    if(oc.isUsableFileList("sumo-net")) {
        loadSUMOFiles(oc, LOADFILTER_ALL, oc.getString("sumo-net"),
            "sumo-net");
    }
}


void
NILoader::loadSUMOFiles(OptionsCont &oc, LoadFilter what, const string &files,
                        const string &type)
{
    // build the handlers to load the data
    std::vector<SUMOSAXHandler*> handlers;
    if(what==LOADFILTER_ALL) {
        handlers.push_back(new NISUMOHandlerNodes(what));
        handlers.push_back(new NISUMOHandlerEdges(what));
        handlers.push_back(new NISUMOHandlerDepth(what));
    } else {
        handlers.push_back(new NISUMOHandlerDepth(what));
    }
    //
}


void
NILoader::loadXML(OptionsCont &oc) {
    // load types
    if(oc.isUsableFileList("t")) {
        NIXMLTypesHandler *handler = new NIXMLTypesHandler();
        loadXMLType(handler, oc.getString("t"), "types");
        NBTypeCont::report();
    } else {
        if(oc.isSet("e")&&oc.isSet("n")) {
            MsgHandler::getWarningInstance()->inform(
                "No types defined, using defaults...");
        }
    }

    // load nodes
    if(oc.isUsableFileList("n")) {
        NIXMLNodesHandler *handler =
            new NIXMLNodesHandler(oc);
        loadXMLType(handler, oc.getString("n"), "nodes");
        NBNodeCont::report();
    }

    // load the edges
    if(oc.isUsableFileList("e")) {
        NIXMLEdgesHandler *handler =
            new NIXMLEdgesHandler(oc);
        loadXMLType(handler, oc.getString("e"), "edges");
        NBEdgeCont::report();
    }

    // load the connections
    if(oc.isUsableFileList("x")) {
        NIXMLConnectionsHandler *handler =
            new NIXMLConnectionsHandler();
        loadXMLType(handler, oc.getString("x"), "connections");
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
    MsgHandler::getMessageInstance()->inform(
        string("Parsing the ") + type + string(" from '")
        + string(file) + string("'..."));
    parser.parse(file.c_str());
}


void
NILoader::loadCell(OptionsCont &oc) {
    LineReader lr;
    // load nodes
    if(oc.isSet("cell-node-file")) {
        MsgHandler::getMessageInstance()->inform("Loading nodes... ");
        string file = oc.getString("cell-node-file");
        NICellNodesHandler handler(file);
        if(!useLineReader(lr, file, handler)) {
            throw ProcessError();
        }
        MsgHandler::getMessageInstance()->inform("done.");
        NBNodeCont::report();
    }
    // load edges
    if(oc.isSet("cell-edge-file")) {
        MsgHandler::getMessageInstance()->inform("Loading edges... ");
        string file = oc.getString("cell-edge-file");
        // parse the file
        NICellEdgesHandler handler(file,
            NBCapacity2Lanes(oc.getFloat("N")));
        if(!useLineReader(lr, file, handler)) {
            throw ProcessError();
        }
        MsgHandler::getMessageInstance()->inform("done.");
        NBEdgeCont::report();
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
    NIVisumLoader loader(oc.getString("visum"),
        NBCapacity2Lanes(oc.getFloat("N")));
    loader.load(oc);
}


void
NILoader::loadArcView(OptionsCont &oc) {
    if(!oc.isSet("arcview")&&!oc.isSet("arcview-dbf")&&!oc.isSet("arcview-shp")) {
        return;
    }
    // check whether the correct set of entries is given
    //  and compute both file names
    string dbf_file;
    string shp_file;
        // check whether both the combines and explicite name giving were used
    if(oc.isSet("arcview")) {
        if(oc.isSet("arcview-dbf")||oc.isSet("arcview-shp")) {
            MsgHandler::getErrorInstance()->inform(
                string("It is not possible to load multiple files."));
            MsgHandler::getErrorInstance()->inform(
                string(" Use EITHER \"--arcview\" OR \"--arcview-dbf\"/\"--arcview-shp\""));
            return;
        }
        dbf_file = oc.getString("arcview") + string("_dbf.txt");
        shp_file = oc.getString("arcview") + string("_shp.txt");
    }
        // check whether only one of the files was given (when explicite
        //  file names for bith structures are given)
    if(!oc.isSet("arcview")) {
        if(!oc.isSet("arcview-dbf")||!oc.isSet("arcview-shp")) {
            MsgHandler::getErrorInstance()->inform(
                string("You must give two files to parse ArcView-data."));
            MsgHandler::getErrorInstance()->inform(
                string(" (\"--arcview-dbf\"/\"--arcview-shp\")"));
            return;
        }
        dbf_file = oc.getString("arcview-dbf");
        shp_file = oc.getString("arcview-shp");
    }
    // check whether both files do exist
    if(!FileHelpers::exists(dbf_file)) {
        MsgHandler::getErrorInstance()->inform(
            string("File not found: ") + dbf_file);
    }
    if(!FileHelpers::exists(shp_file)) {
        MsgHandler::getErrorInstance()->inform(
            string("File not found: ") + shp_file);
    }
    if(MsgHandler::getErrorInstance()->wasInformed()) {
        return;
    }
    // load the arcview files
    NIArcView_Loader loader(dbf_file, shp_file);
    loader.load(oc);
}


void
NILoader::loadVissim(OptionsCont &oc) {
    if(!oc.isSet("vissim")) {
        return;
    }
    // load the visum network
    NIVissimLoader loader(oc.getString("vissim"));
    loader.load(oc);
}


void
NILoader::loadArtemis(OptionsCont &oc) {
    if(!oc.isSet("artemis")) {
        return;
    }
    // load the visum network
    NIArtemisLoader loader(oc.getString("artemis"));
    loader.load(oc);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



