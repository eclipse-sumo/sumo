/***************************************************************************
                          NLNetBuilder.cpp
              Container for MSNet during its building
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.19  2004/07/02 09:37:31  dkrajzew
// work on class derivation (for online-routing mainly)
//
// Revision 1.18  2004/04/02 11:23:52  dkrajzew
// extended traffic lights are now no longer templates; MSNet now handles all simulation-wide output
//
// Revision 1.17  2004/01/26 07:07:36  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics; different detector visualistaion in dependence to his controller
//
// Revision 1.16  2004/01/12 14:46:21  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.15  2003/12/11 06:19:04  dkrajzew
// network loading and initialisation improved
//
// Revision 1.14  2003/12/05 10:26:10  dkrajzew
// handling of internal links when theyre not wished improved
//
// Revision 1.13  2003/12/04 13:18:23  dkrajzew
// handling of internal links added
//
// Revision 1.12  2003/11/24 10:17:03  dkrajzew
// removed some dead code
//
// Revision 1.11  2003/10/22 15:47:44  dkrajzew
// we have to distinct between two teleporter versions now
//
// Revision 1.10  2003/08/20 11:51:27  dkrajzew
// option for suppressing output of empty edges within the raw-output added
//
// Revision 1.9  2003/07/07 08:35:10  dkrajzew
// changes due to loading of geometry applied from the gui-version
//  (no major drawbacks in loading speed)
//
// Revision 1.8  2003/06/18 11:18:05  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
// this handler puts it further to output instances.
// changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.7  2003/05/21 15:18:21  dkrajzew
// yellow traffic lights implemented
//
// Revision 1.6  2003/05/20 09:45:35  dkrajzew
// some make-up done (splitting large methods; patching comments)
//
// Revision 1.5  2003/04/01 15:18:12  dkrajzew
// minor changes
//
// Revision 1.4  2003/03/19 08:07:34  dkrajzew
// bugs and syntactical faults arised during compilation on Linux removed
//
// Revision 1.3  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.2  2002/10/17 10:32:41  dkrajzew
// sources and detectors joined with triggers to additional-files; usage of
//  standard SUMOSAXHandler instead of NLSAXHandler;
// loading of triggers implemented
//
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition
//  parseable in one step
//
// Revision 1.13  2002/07/31 17:34:51  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.15  2002/07/22 12:44:32  dkrajzew
// Source loading structures added
//
// Revision 1.14  2002/07/11 07:31:54  dkrajzew
// Option_FileName invented to allow relative path names within the
//  configuration files
//
// Revision 1.13  2002/07/02 08:19:08  dkrajzew
// Test for non-empty file option where a list of files is expected moved
//  to FileHelpers
//
// Revision 1.12  2002/06/21 10:53:49  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.11  2002/06/11 15:07:08  dkrajzew
// error checking fixed
//
// Revision 1.10  2002/06/11 14:39:27  dkrajzew
// windows eol removed
//
// Revision 1.9  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.8  2002/06/10 08:36:07  dkrajzew
// Conversion of strings generalized
//
// Revision 1.7  2002/06/07 14:39:59  dkrajzew
// errors occured while building larger nets and adaption of new netconverting
//  methods debugged
//
// Revision 1.6  2002/05/14 04:54:25  dkrajzew
// Unexisting files are now catched independent to the Xerces-error mechanism;
//  error report generation moved to XMLConvert
//
// Revision 1.5  2002/04/24 10:32:05  dkrajzew
// Unfound files are now only reported once
//
// Revision 1.4  2002/04/17 11:18:47  dkrajzew
// windows-newlines removed
//
// Revision 1.3  2002/04/16 06:06:29  dkrajzew
// Build report debugged
//
// Revision 1.2  2002/04/15 07:07:56  dkrajzew
// new loading paradigm implemented
//
// Revision 2.0  2002/02/14 14:43:24  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:45  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:09  traffic
// moved from netbuild
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NLNetBuilder.h"
#include <microsim/MSNet.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSGlobals.h>
#include <iostream>
#include <vector>
#include <parsers/SAXParser.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <string>
#include <map>
#include "NLContainer.h"
#include "NLNetHandler.h"
#include "NLEdgeControlBuilder.h"
#include "NLJunctionControlBuilder.h"
#include "NLDetectorBuilder.h"
#include "NLTriggerBuilder.h"
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleTransfer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/convert/TplConvert.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/XMLHelpers.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NLNetBuilder::NLNetBuilder(const OptionsCont &oc,
                           NLEdgeControlBuilder &eb,
                           NLJunctionControlBuilder &jb)
    : m_pOptions(oc), myEdgeBuilder(eb), myJunctionBuilder(jb)
{
}


NLNetBuilder::~NLNetBuilder()
{
}



MSNet *
NLNetBuilder::buildNet(MSVehicleControl *vc)
{
    // pre-initialise the network
     // set whether empty edges shall be printed on dump
    MSGlobals::myOmitEmptyEdgesOnDump =
        !m_pOptions.getBool("dump-empty-edges");
     // set whether internal lanes shall be used
    MSGlobals::myUsingInternalLanes =
        m_pOptions.getBool("use-internal-links");
     // preinit network
    MSNet::preInitMSNet(
        m_pOptions.getInt("b"),
        vc,
        m_pOptions.getUIntVector("dump-intervals"),
        m_pOptions.getString("dump-basename"));

    // we need a specialised detector builder
    NLDetectorBuilder db;
    NLTriggerBuilder tb;
    // initialise loading buffer ...
    NLContainer *container =
        new NLContainer(myEdgeBuilder, myJunctionBuilder);
    // ... and the parser
    // get the matching handler
    NLNetHandler handler("", *container, db, tb,
        m_pOptions.getFloat("actuating-detector-pos"),
        m_pOptions.getFloat("agent-detector-len"));
    SAX2XMLReader* parser = XMLHelpers::getSAXReader(handler);
    MSNet *net = 0;
    bool ok = load(handler, *parser);
    subreport("Loading done.", "Loading failed.");
    // try to build a net
    if(!MsgHandler::getErrorInstance()->wasInformed()) {
        net = container->buildMSNet(db, m_pOptions);
    }
    delete parser;
    delete container;
    return net;
}


bool
NLNetBuilder::load(NLNetHandler &handler, SAX2XMLReader &parser)
{
    // load the net
    bool ok = load(LOADFILTER_ALL, m_pOptions.getString("n"),
        handler, parser);
    // load the junctions
    if(m_pOptions.isSet("j")&&ok) {
        ok = load(LOADFILTER_LOGICS, m_pOptions.getString("j"),
            handler, parser);
    }
    // load the junctions
    if(m_pOptions.isSet("r")&&ok&&m_pOptions.getInt("route-steps")<=0) {
        ok = load(LOADFILTER_DYNAMIC, m_pOptions.getString("r"),
            handler, parser);
    }
    // load additional net elements (sources, detectors, ...)
    if(m_pOptions.isSet("a")&&ok) {
        ok = load(LOADFILTER_NETADD, m_pOptions.getString("a"),
            handler, parser);
    }
    return ok;
}


bool
NLNetBuilder::load(LoadFilter what, const string &files, NLNetHandler &handler,
                   SAX2XMLReader &parser)
{
    // initialise the handler for the current type of data
    handler.setWanted(what);
    // check whether the list of files does not contain ';'s only
    if(!FileHelpers::checkFileList(files)) {
        MsgHandler::getErrorInstance()->inform(
            string("No ") + getDataName(what) + string(" found!"));
        MsgHandler::getErrorInstance()->inform(
            "Check your program parameter.");
        return false;
    }
    // report about loading when wished
    MsgHandler::getMessageInstance()->inform(
        string("Loading ") + getDataName(what) + string("..."));
    // start parsing
    parser.setContentHandler(&handler);
    parser.setErrorHandler(&handler);
    parse(files, handler, parser);
    // report about loaded structures
    subreport(
        "Loading of " + getDataName(what) + " done.",
        "Loading of " + getDataName(what) + " failed.");
    return !MsgHandler::getErrorInstance()->wasInformed();
}


bool
NLNetBuilder::parse(const string &files, NLNetHandler &handler,
                    SAX2XMLReader &parser)
{
    // for each file in the list
    StringTokenizer st(files, ';');
    bool ok = true;
    while(st.hasNext()&&ok) {
        // check whether the file exists
        string tmp = st.next();
        if(!FileHelpers::exists(tmp)) {
            // report error if not
            MsgHandler::getErrorInstance()->inform(
                string("The file '") + tmp + string("' does not exist!"));
            return false;
        } else {
            // parse the file
            handler.setFileName(tmp);
            parser.parse(tmp.c_str());
            ok = !(MsgHandler::getErrorInstance()->wasInformed());
        }
    }
    return ok;
}


string
NLNetBuilder::getDataName(LoadFilter forWhat)
{
    switch(forWhat) {
    case LOADFILTER_ALL:
        return "net";
        break;
    case LOADFILTER_LOGICS:
        return "junction logics";
        break;
    case LOADFILTER_NETADD:
        return "additional net elements";
        break;
    case LOADFILTER_DYNAMIC:
        return "vehicle routes";
        break;
    default:
        break;
    }
    throw exception();
}


void
NLNetBuilder::subreport(const std::string &ok, const std::string &wrong)
{
    if(!MsgHandler::getErrorInstance()->wasInformed()) {
        MsgHandler::getMessageInstance()->inform(ok.c_str());
    } else {
        MsgHandler::getMessageInstance()->inform(wrong.c_str());
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
//

