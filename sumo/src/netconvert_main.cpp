/***************************************************************************
                          main.cpp
			  The main procedure for the conversion /
			  building of networks
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
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
// Revision 1.14  2003/06/24 08:06:36  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.13  2003/06/19 07:07:52  dkrajzew
// false order of calling XML- and Options-subsystems patched
//
// Revision 1.12  2003/06/18 11:26:15  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.11  2003/06/05 14:41:53  dkrajzew
// further parameter contraining the area of connectors to join (VIssim) added
//
// Revision 1.10  2003/05/20 09:54:45  dkrajzew
// configuration files are no longer set as default
//
// Revision 1.9  2003/04/10 16:13:51  dkrajzew
// recent changes
//
// Revision 1.8  2003/04/09 15:53:16  dkrajzew
// netconvert-changes: further work on Vissim-import, documentation added
//
// Revision 1.7  2003/04/04 08:41:48  dkrajzew
// help screen updated; min-decel usage added
//
// Revision 1.6  2003/03/18 13:19:38  dkrajzew
// memory leak debugging functions included (MSVC)
//
// Revision 1.5  2003/03/06 17:19:11  dkrajzew
// debugging
//
// Revision 1.4  2003/03/03 15:08:48  dkrajzew
// vissim requires more steps on building
//
// Revision 1.3  2003/02/07 10:37:30  dkrajzew
// files updated
//
// Revision 1.1  2002/10/16 14:51:08  dkrajzew
// Moved from ROOT/sumo to ROOT/src; added further help and main files for netconvert, router, od2trips and gui version
//
// Revision 1.9  2002/07/25 08:24:10  dkrajzew
// Visum and Cell import added
//
// Revision 1.8  2002/07/02 12:51:51  dkrajzew
// removal of options on help printing added
//
// Revision 1.7  2002/07/02 08:54:50  dkrajzew
// XMLSubSys implemented; return values fixed; better flow and a better documentation
//
// Revision 1.6  2002/06/17 15:15:28  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.5  2002/06/11 15:56:07  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
// Revision 1.3  2002/05/14 04:39:47  dkrajzew
// new computation steps
//
// Revision 1.2  2002/04/26 10:04:04  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/09 14:18:26  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.4  2002/04/09 12:19:51  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.3  2002/03/22 10:40:25  dkrajzew
// Memory leak finder added
//
// Revision 1.2  2002/03/20 08:28:02  dkrajzew
// Help output added
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// -------------------------------------------
// moved to an own project
// -------------------------------------------
// Revision 1.1  2001/12/06 13:03:15  traffic
// added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <string>
#include <fstream>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBJunctionLogicCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBDistrictCont.h>
#include <netbuild/NBDistribution.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBOptionsIO.h>
#include <netbuild/NBLoader.h>
#include <netbuild/NBRequest.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/HelpPrinter.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include "netconvert_help.h"


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#ifdef WIN32
   #include <utils/dev/MemDiff.h>
   #include <crtdbg.h>
#endif
#endif

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * functions
 * ======================================================================= */
void
inform(int step, const std::string &about)
{
    MsgHandler::getMessageInstance()->inform(
        string("Computing step ") + toString<int>(step)
        + string(": ") + about);
}

/* -------------------------------------------------------------------------
 * computation methods
 * ----------------------------------------------------------------------- */


/** removes dummy edges from junctions */
/*
bool setInit(int step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Setting structures to initial " << endl;
    }
    NBEdgeCont::setInit(verbose);
    return true;
}
*/

/** removes dummy edges from junctions */
bool
removeDummyEdges(int step)
{
    inform(step, "Removing dummy edges ");
    return NBNodeCont::removeDummyEdges();
}


/** joins edges which connect the same nodes */
bool
joinEdges(int step)
{
    inform(step, "Joining double connections");
    return NBNodeCont::recheckEdges();
}


/** computes the turning direction for each edge */
bool
computeTurningDirections(int step)
{
    inform(step, "Computing turning directions");
    return NBEdgeCont::computeTurningDirections();
}


/** sorts the edges of a node */
bool
sortNodesEdges(int step)
{
    inform(step, "Sorting nodes' edges, computing shape");
    return NBNodeCont::sortNodesEdges();
}


/** sets the node positions in a way that nodes are lying at zero */
bool
normaliseNodePositions(int step)
{
    inform(step, "Normalising node positions");
    return NBNodeCont::normaliseNodePositions();
}


/** computes edges 2 edges - relationships
    (step1: computation of approached edges) */
bool
computeEdge2Edges(int step)
{
    inform(step, "Computing Approached Edges");
    return NBEdgeCont::computeEdge2Edges();
}


/** computes edges 2 edges - relationships
    (step2: computation of which lanes approach the edges) */
bool
computeLanes2Edges(int step)
{
    inform(step, "Computing Approaching Lanes");
    return NBEdgeCont::computeLanes2Edges();
}


/** computes edges 2 edges - relationships
    (step3: division of lanes to approached edges) */
bool
computeLanes2Lanes(int step)
{
    inform(step, "Dividing of Lanes on Approached Lanes");
    bool ok = NBNodeCont::computeLanes2Lanes();
    if(ok) {
        return NBEdgeCont::sortOutgoingLanesConnections();
    }
    return ok;
}

/** rechecks whether all lanes have a following lane/edge */
bool
recheckLanes(int step)
{
    inform(step, "Rechecking of lane endings");
    return NBEdgeCont::recheckLanes();
}


/** computes the node-internal priorities of links */
/*bool
computeLinkPriorities(int step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Computing Link Priorities" << endl;
    }
    return NBEdgeCont::computeLinkPriorities(verbose);
}
*/

/** appends the turnarounds */
bool
appendTurnarounds(int step)
{
    inform(step, "Appending Turnarounds");
    return NBEdgeCont::appendTurnarounds();
}


/** computes nodes' logics */
bool
computeLogic(int step, OptionsCont &oc)
{
    inform(step, "Computing node logics");
    return NBNodeCont::computeLogics(oc);
}


/** computes nodes' tl-logics */
bool
computeTLLogic(int step, OptionsCont &oc)
{
    inform(step, "Computing traffic light logics");
    return NBTrafficLightLogicCont::computeLogics(oc);
}


/* -------------------------------------------------------------------------
 * data processing methods
 * ----------------------------------------------------------------------- */
/**
 * computes the structures
 * the order of the computation steps is not variable!!!
 */
void
compute(OptionsCont &oc)
{
    bool ok = true;
    int step = 1;
//    if(ok) ok = setInit(step++);
    if(ok) ok = removeDummyEdges(step++);
//    if(ok) ok = joinEdges(step++);
    if(ok) ok = computeTurningDirections(step++);
    if(ok) ok = sortNodesEdges(step++);
    if(ok) ok = normaliseNodePositions(step++);
    if(ok) ok = computeEdge2Edges(step++);
    if(ok) ok = computeLanes2Edges(step++);
    if(ok) ok = computeLanes2Lanes(step++);
    if(ok) ok = appendTurnarounds(step++);
    if(ok) ok = recheckLanes(step++);
//    if(ok) ok = computeLinkPriorities(step++);
    if(ok) ok = computeLogic(step++, oc);
    if(ok) ok = computeTLLogic(step++, oc);

    NBNode::reportBuild();
    NBRequest::reportWarnings();
    if(!ok) throw ProcessError();
}

/** saves the net (not the junction logics) */
bool
save(string path)
{
    // try to build the output file
    ofstream res(path.c_str());
    if(!res.good()) {
        return false;
    }
    // print the computed values
    res << "<net>" << endl << endl;
    res.setf( ios::fixed, ios::floatfield );
    // write the ocunt of some elements
        // write the list of edges
    NBEdgeCont::writeXMLEdgeList(res);
        // write the number of nodes
    NBNodeCont::writeXMLNumber(res);
    res << endl;
    // write the districts
    NBDistrictCont::writeXML(res);
    // write edges with lanes and connected edges
    NBEdgeCont::writeXMLStep1(res);
    // write the logics
    NBJunctionLogicCont::writeXML(res);
    NBTrafficLightLogicCont::writeXML(res);
    // write the nodes
    NBNodeCont::writeXML(res);
    // write the successors of lanes
    NBEdgeCont::writeXMLStep2(res);
    // write the positions of edges
    NBEdgeCont::writeXMLStep3(res);
    res << "</net>" << endl;
    return true;
}

/** clears all structures */
void
clearAll()
{
    NBEdgeCont::clear();
    NBNodeCont::clear();
    NBTypeCont::clear();
    NBJunctionLogicCont::clear();
    NBDistrictCont::clear();
    NBTrafficLightLogicCont::clear();
    NBDistribution::clear();
    XMLSubSys::close();
}

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int main(int argc, char **argv)
{
#ifdef _DEBUG
#ifdef WIN32
    CMemDiff state1;
    // uncomment next line and insert the context of an undeleted
    //  allocation to break within it (MSVC++ only)
    // _CrtSetBreakAlloc(814107);
#endif
#endif

    int ret = 0;
    try {
        if(!SystemFrame::init(false, argc, argv,
            NBOptionsIO::fillOptions, 0, help)) {
            throw ProcessError();
        }
        // retrieve the options
        OptionsCont &oc = OptionsSubSys::getOptions();
        // initialise the (default) types
        NBTypeCont::setDefaults(
            oc.getString("T"),
            oc.getInt("L"),
            oc.getFloat("S"),
            oc.getInt("P"));
        // load data
        NBLoader::load(oc);
        // check whether any errors occured
        if(MsgHandler::getErrorInstance()->wasInformed()) {
            throw ProcessError();
        }
        NBTypeCont::report();
        NBEdgeCont::report();
        NBNodeCont::report();
        // perform the computation
        compute(oc);
        // save network when wished
        save(oc.getString("o"));
        // remove everything from the memory
    } catch (...) {
        clearAll();
        MsgHandler::getErrorInstance()->inform("Quitting (conversion failed).");
        ret = 1;
    }
    clearAll();
    SystemFrame::close();
    // report about ending
    if(ret=0) {
        MsgHandler::getMessageInstance()->inform("Success.");
    }
    return ret;
}


