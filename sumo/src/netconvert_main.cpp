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
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/HelpPrinter.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/SErrorHandler.h>
#include "netconvert_help.h"

/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#ifdef WIN32
   #include <utils/dev/MemDiff.h>
#endif
#endif

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * functions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * computation methods
 * ----------------------------------------------------------------------- */

    
/** removes dummy edges from junctions */
bool removeDummyEdges(int step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Removing dummy edges " << endl;
    }
    return NBNodeCont::removeDummyEdges(verbose);
}


/** joins edges which connect the same nodes */
bool joinEdges(int step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Joining double connections " << endl;
    }
    return NBNodeCont::recheckEdges(verbose);
}


/** computes the turning edge for each edge */
bool computeTurningDirections(int step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Computing turning directions " << endl;
    }
    return NBEdgeCont::computeTurningDirections(verbose);
}


/** sorts the edges of a node */
bool sortNodesEdges(int step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Sorting nodes' edges " << endl;
    }
    return NBNodeCont::sortNodesEdges(verbose);
}

/** sets the node positions in a way that nodes are lying at zero */
bool normaliseNodePositions(int step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Normalising node positions " << endl;
    }
    return NBNodeCont::normaliseNodePositions(verbose);
}

 /** computes edges 2 edges - relationships
    (step1: computation of approached edges) */
bool computeEdge2Edges(int step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Computing Approached Edges" << endl;
    }
    return NBEdgeCont::computeEdge2Edges();
}

/** computes edges 2 edges - relationships
    (step2: division of lanes to approached edges) */
bool computeEdges2Lanes(int step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Dividing of Lanes on Approached Edges" << endl;
    }
    bool ok = NBNodeCont::computeEdges2Lanes(verbose);
    if(ok) {
        return NBEdgeCont::sortOutgoingLanesConnections(verbose);
    }
    return ok;
}

/** rechecks whether all lanes have a following lane/edge */
bool recheckLanes(int step, bool verbose) {
    if(verbose) {
        cout << "Computing step " << step
            << ": Rechecking of lane endings." << endl;
    }
    return NBEdgeCont::recheckLanes(verbose);
}

/** computes the node-internal priorities of links */
bool computeLinkPriorities(int step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Computing Link Priorities" << endl;
    }
    return NBEdgeCont::computeLinkPriorities(verbose);
}

/** appends the turnarounds */
bool appendTurnarounds(int step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Appending Turnarounds" << endl;
    }
    return NBEdgeCont::appendTurnarounds(verbose);
}

/** computes nodes' logics */
bool computeLogic(int step, OptionsCont *oc)
{
    if(oc->getBool("v")) {
        cout << "Computing step " << step
            << ": Computing node logics" << endl;
    }
    return NBNodeCont::computeLogics(oc->getBool("v"), 0);
}

/* -------------------------------------------------------------------------
 * data processing methods
 * ----------------------------------------------------------------------- */

/** initialises defaults */
bool
initDefaults(OptionsCont *oc)
{
    NBTypeCont::setDefaults(oc->getString("T"),
        oc->getInt("L"), oc->getFloat("S"), oc->getInt("P"));
    return true;
}

/**
 * computes the structures
 * the order of the computation steps is not variable!!!
 */
void
compute(OptionsCont *oc)
{
    bool ok = true;
    bool verbose = oc->getBool("v");
    int step = 1;
    size_t size = NBNodeCont::size();
//    NBEdgeCont::search((NBEdge*) 0x00db2a48);
//    NBNodeCont::searchEdgeInNode("3", "3");
    if(ok) ok = removeDummyEdges(step++, verbose);
//    NBEdgeCont::search((NBEdge*) 0x00db2a48);
//    NBNodeCont::searchEdgeInNode("3", "3");
    if(ok) ok = joinEdges(step++, verbose);
//    NBEdgeCont::search((NBEdge*) 0x00db2a48);
//    NBNodeCont::searchEdgeInNode("3", "3");
    if(ok) ok = computeTurningDirections(step++, verbose);
    if(ok) ok = sortNodesEdges(step++, verbose);
    if(ok) ok = normaliseNodePositions(step++, verbose);
    if(ok) ok = computeEdge2Edges(step++, verbose);
    if(ok) ok = computeEdges2Lanes(step++, verbose);
    if(ok) ok = appendTurnarounds(step++, verbose);
    if(ok) ok = recheckLanes(step++, verbose);
    if(ok) ok = computeLinkPriorities(step++, verbose);
    if(ok) ok = computeLogic(step++, oc);
    if(ok && oc->getBool("v")) NBNode::reportBuild();
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
clearAll(OptionsCont *oc)
{
    NBEdgeCont::clear();
    NBNodeCont::clear();
    NBTypeCont::clear();
    NBJunctionLogicCont::clear();
    NBDistrictCont::clear();
    NBTrafficLightLogicCont::clear();
    NBDistribution::clear();
    delete oc;
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
#endif
#endif
    bool verbose = false;
    OptionsCont *oc = 0;
    try {
        // try to initialise the XML-subsystem
        if(!XMLSubSys::init()) {
            return 1;
        }
        // parse the settings
        oc = NBOptionsIO::getOptions(argc, argv);
        // check if only the help shall be printed
        if(oc->getBool("help")) {
            HelpPrinter::print(help);
            delete oc;
            return 0;
        }
        // initialise the (default) types
        initDefaults(oc);
        // load data
        NBLoader::load(*oc);
        // check whether any errors occured
        if(SErrorHandler::errorOccured()) {
            throw ProcessError();
        }
        // perform the computation
        compute(oc);
        // save network when wished
        save(oc->getString("o"));
        verbose = oc->getBool("v");
        // remove everything from the memory
    } catch (ProcessError) {
        clearAll(oc);
        cout << "Quitting (conversion failed)." << endl;
        return 1; // !!!

    }
    clearAll(oc);
    // report about ending
    if(verbose) {
        cout << "Success." << endl;
    }
    return 0;
}


