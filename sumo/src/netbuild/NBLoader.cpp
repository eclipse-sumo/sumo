/***************************************************************************
                          NBLoader.cpp
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
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
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
#include <sax2/XMLReaderFactory.hpp>
#include <sax2/DefaultHandler.hpp>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/importio/LineReader.h>
#include <utils/common/FileHelpers.h>
#include "NBTypeCont.h"
#include "NBNodeCont.h"
#include "NBEdgeCont.h"
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <netimport/xml/NBXMLEdgesHandler.h>
#include <netimport/xml/NBXMLNodesHandler.h>
#include <netimport/xml/NBXMLTypesHandler.h>
#include <netimport/cell/NBCellNodesHandler.h>
#include <netimport/cell/NBCellEdgesHandler.h>
#include <netimport/visum/NBVisumLoader.h>
#include <netimport/sumo/NBSUMOHandlerNodes.h>
#include <netimport/sumo/NBSUMOHandlerEdges.h>
#include <netimport/sumo/NBSUMOHandlerDepth.h>
#include "NBLoader.h"
#include "NLLoadFilter.h"
#include <utils/convert/TplConvert.h>

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * static members
 * ======================================================================= */
bool NBLoader::_verbose;

/* =========================================================================
 * method defintions
 * ======================================================================= */
void NBLoader::load(OptionsCont &oc) {
    // get the report options
    bool warn = oc.getBool("w");
    _verbose = oc.getBool("v");
    // get the format to use
    //string type = oc.getString("used-file-format");
    // try to load using different methods
    loadSUMO(oc, warn);
    loadXML(oc, warn);
    loadCell(oc, warn);
    loadVisum(oc, warn);
    // check the loaded structures
    if(NBNodeCont::size()==0) {
        cout << "Error: No nodes loaded." << endl;
        throw ProcessError();
    }
    if(NBEdgeCont::size()==0) {
        cout << "Error: No edges loaded." << endl;
        throw ProcessError();
    }
    // end loading
    reportBegin("Closing the load process... ");
//    NBNodeCont::close();
    reportEnd();
}


/* -------------------------------------------------------------------------
 * file loading methods
 * ----------------------------------------------------------------------- */
void
NBLoader::loadSUMO(OptionsCont &oc, bool warn)
{
    // load the network
    if(oc.isUsableFileList("sumo-net")) {
        loadSUMOFiles(oc, LOADFILTER_ALL, oc.getString("sumo-net"),
            "sumo-net");
    }
    // load the junction logics only when they shall not be recomputed
    if(oc.isUsableFileList("sumo-logics") && !oc.getBool("recompute-junction-logics") ) {
        loadSUMOFiles(oc, LOADFILTER_LOGICS, oc.getString("sumo-logics"),
            "sumo-logics");
    }
}


void
NBLoader::loadSUMOFiles(OptionsCont &oc, LoadFilter what, const string &files,
                        const string &type)
{
    // build the handlers to load the data
    bool verbose = oc.getBool("v");
    std::vector<SUMOSAXHandler*> handlers;
    if(what==LOADFILTER_ALL) {
        handlers.push_back(new NBSUMOHandlerNodes(what, true, verbose));
        handlers.push_back(new NBSUMOHandlerEdges(what, true, verbose));
        handlers.push_back(new NBSUMOHandlerDepth(what, true, verbose));
    } else {
        handlers.push_back(new NBSUMOHandlerDepth(what, true, verbose));
    }
    //
}


void
NBLoader::loadXML(OptionsCont &oc, bool warn) {
    // load types
    if(oc.isUsableFileList("t")) {
        NBXMLTypesHandler *handler = new NBXMLTypesHandler(warn, _verbose);
        loadXMLType(handler, oc.getString("t"), "types");
        NBTypeCont::report(_verbose);
    } else if(_verbose||warn) {
        if(oc.isSet("e")&&oc.isSet("n")) {
            cout << "No types defined, using defaults..." << endl;
        }
    }

    // load nodes
    if(oc.isUsableFileList("n")) {
        NBXMLNodesHandler *handler = new NBXMLNodesHandler(warn, _verbose);
        loadXMLType(handler, oc.getString("n"), "nodes");
        NBNodeCont::report(_verbose);
    }

    // load the edges
    if(oc.isUsableFileList("e")) {
        NBXMLEdgesHandler *handler = new NBXMLEdgesHandler(warn, _verbose);
        loadXMLType(handler, oc.getString("e"), "edges");
        NBEdgeCont::report(_verbose);
    }
}

/** loads a single user-specified file */
void
NBLoader::loadXMLType(SUMOSAXHandler *handler, const std::string &files,
                      const string &type)
{
    // build parser
    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
    parser->setFeature(XMLString::transcode("http://xml.org/sax/features/validation"), false);
    parser->setContentHandler(handler);
    parser->setErrorHandler(handler);
    // start the parsing
    try {
        StringTokenizer st(files, ";");
        while(st.hasNext()) {
            string file = st.next();
            handler->setFileName(file);
            loadXMLFile(*parser, file, type);
        }
    } catch (const XMLException& toCatch) {
        cout << "Error: " << TplConvert<XMLCh>::_2str(toCatch.getMessage()) << endl;
        cout << "  The " << type << " could not be loaded from '" <<
            handler->getFileName() << "'." << endl;
        delete handler;
        throw ProcessError();
    }
    delete parser;
    delete handler;
}


void
NBLoader::loadXMLFile(SAX2XMLReader &parser, const std::string &file,
                      const string &type)
{
    if(_verbose) {
        cout << "Parsing the " << type << " from '"
            << file << "'..." << endl;
    }
    parser.parse(file.c_str());
}

void
NBLoader::loadCell(OptionsCont &oc, bool warn) {
    LineReader lr;
    // load nodes
    if(oc.isSet("cell-node-file")) {
        reportBegin("Loading nodes... ");
        string file = oc.getString("cell-node-file");
        NBCellNodesHandler handler(file, warn, _verbose);
        if(!useLineReader(lr, file, handler)) {
            throw ProcessError();
        }
        reportEnd();
        NBNodeCont::report(_verbose);
    }
    // load edges
    if(oc.isSet("cell-edge-file")) {
        reportBegin("Loading edges... ");
        string file = oc.getString("cell-edge-file");
        // parse the file
        NBCellEdgesHandler handler(file, warn, _verbose,
            NBCapacity2Lanes(oc.getFloat("N")));
        if(!useLineReader(lr, file, handler)) {
            throw ProcessError();
        }
        reportEnd();
        NBEdgeCont::report(_verbose);
    }
}

bool
NBLoader::useLineReader(LineReader &lr, const std::string &file, LineHandler &lh) {
    // check opening
    if(!lr.setFileName(file)) {
        cout << "The file '" << file << "' could not be opened." << endl;
        return false;
    }
    lr.readAll(lh);
    return true;
}

void
NBLoader::loadVisum(OptionsCont &oc, bool warn) {
    // load the visum types
    if(oc.isSet("visum")) {
        NBVisumLoader loader(oc.getString("visum"),
            NBCapacity2Lanes(oc.getFloat("N")));
        loader.load(oc);
    }
}

void
NBLoader::reportBegin(const std::string &msg)
{
    if(_verbose) {
        cout << msg;
    }
}

void
NBLoader::reportEnd()
{
    if(_verbose) {
        cout << "done." << endl;
    }
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBLoader.icc"
//#endif

// Local Variables:
// mode:C++
// End:


