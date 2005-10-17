/***************************************************************************
                          NBNetBuilder.cpp
			  The instance responsible for building networks
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : 20 Nov 2001
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
// Revision 1.24  2005/10/17 09:03:53  dkrajzew
// output patched
//
// Revision 1.23  2005/10/07 11:38:18  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.22  2005/09/23 06:01:06  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.21  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
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
#include <fstream>
#include <iostream>
#include <iomanip>
#include "NBNetBuilder.h"
#include "nodes/NBNodeCont.h"
#include "NBEdgeCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBJunctionLogicCont.h"
#include "NBDistrictCont.h"
#include "NBDistribution.h"
#include "NBRequest.h"
#include "NBTypeCont.h"
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include "NBJoinedEdgesMap.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NBNetBuilder::NBNetBuilder()
    : myHaveBuildNet(false), myTypeCont()
{
}


NBNetBuilder::~NBNetBuilder()
{
}


void
NBNetBuilder::buildLoaded()
{
    myTypeCont.report();
    myEdgeCont.report();
    myNodeCont.report();
    // perform the computation
    OptionsCont &oc = OptionsSubSys::getOptions();
    compute(oc);
    // save network when wished
    save(oc.getString("o"), oc);
    // save plain nodes/edges/connections
    if(oc.isSet("plain-output")) {
        savePlain(oc.getString("plain-output"));
    }
    // save the mapping information when wished
    if(oc.isSet("map-output")) {
        saveMap(oc.getString("map-output"));
    }
}


void
NBNetBuilder::inform(int &step, const std::string &about)
{
    WRITE_MESSAGE(string("Computing step ") + toString<int>(step)+ string(": ") + about);
    step++;
}


bool
NBNetBuilder::removeDummyEdges(int &step)
{
    // Removes edges that are connecting the same node
    inform(step, "Removing dummy edges ");
    return myNodeCont.removeDummyEdges(myDistrictCont, myEdgeCont, myTLLCont);
}


bool
NBNetBuilder::joinEdges(int &step)
{
    inform(step, "Joining double connections");
    return myNodeCont.recheckEdges(myDistrictCont, myTLLCont, myEdgeCont);
}


bool
NBNetBuilder::removeUnwishedNodes(int &step, OptionsCont &oc)
{
    if(oc.getBool("no-node-removal")) {
        return true;
    }
    inform(step, "Removing empty nodes and geometry nodes.");
    return myNodeCont.removeUnwishedNodes(myDistrictCont, myEdgeCont, myTLLCont);
}


bool
NBNetBuilder::removeUnwishedEdges(int &step, OptionsCont &oc)
{
    if(!OptionsSubSys::getOptions().isSet("keep-edges")) {
        return true;
    }
    inform(step, "Removing unwished edges.");
    return myEdgeCont.removeUnwishedEdges(myDistrictCont, oc);
}


bool
NBNetBuilder::guessRamps(int &step, OptionsCont &oc)
{
    if(!oc.getBool("guess-ramps")&&!oc.getBool("guess-obscure-ramps")) {
        return true;
    }
    inform(step, "Guessing and setting on-/off-ramps");
    return myNodeCont.guessRamps(oc, myEdgeCont, myDistrictCont);
}


bool
NBNetBuilder::guessTLs(int &step, OptionsCont &oc)
{
    inform(step, "Guessing and setting TLs");
    if(oc.isSet("explicite-tls")) {
        StringTokenizer st(oc.getString("explicite-tls"), ";");
        while(st.hasNext()) {
            myNodeCont.setAsTLControlled(st.next(), myTLLCont);
        }
    }
    return myNodeCont.guessTLs(oc, myTLLCont);
}


bool
NBNetBuilder::computeTurningDirections(int &step)
{
    inform(step, "Computing turning directions");
    return myEdgeCont.computeTurningDirections();
}


bool
NBNetBuilder::sortNodesEdges(int &step)
{
    inform(step, "Sorting nodes' edges");
    return myNodeCont.sortNodesEdges(myTypeCont);
}


bool
NBNetBuilder::normaliseNodePositions(int &step)
{
    inform(step, "Normalising node positions");
    bool ok = myNodeCont.normaliseNodePositions();
    if(ok) {
        ok = myEdgeCont.normaliseEdgePositions(myNodeCont);
    }
    return ok;
}


bool
NBNetBuilder::computeEdge2Edges(int &step)
{
    inform(step, "Computing Approached Edges");
    return myEdgeCont.computeEdge2Edges();
}


bool
NBNetBuilder::computeLanes2Edges(int &step)
{
    inform(step, "Computing Approaching Lanes");
    return myEdgeCont.computeLanes2Edges();
}


bool
NBNetBuilder::computeLanes2Lanes(int &step)
{
    inform(step, "Dividing of Lanes on Approached Lanes");
    bool ok = myNodeCont.computeLanes2Lanes();
    if(ok) {
        return myEdgeCont.sortOutgoingLanesConnections();
    }
    return ok;
}

bool
NBNetBuilder::recheckLanes(int &step)
{
    inform(step, "Rechecking of lane endings");
    return myEdgeCont.recheckLanes();
}


bool
NBNetBuilder::computeNodeShapes(int &step, OptionsCont &oc)
{
    inform(step, "Computing node shapes");
    return myNodeCont.computeNodeShapes(oc);
}


bool
NBNetBuilder::computeEdgeShapes(int &step)
{
    inform(step, "Computing edge shapes");
    return myEdgeCont.computeEdgeShapes();
}


/** computes the node-internal priorities of links */
/*bool
computeLinkPriorities(int &step, bool verbose)
{
    if(verbose) {
        cout << "Computing step " << step
            << ": Computing Link Priorities" << endl;
    }
    return myEdgeCont.computeLinkPriorities(verbose);
}
*/

bool
NBNetBuilder::appendTurnarounds(int &step, OptionsCont &oc)
{
    if(!oc.getBool("append-turnarounds")) {
        return true;
    }
    inform(step, "Appending Turnarounds");
    return myEdgeCont.appendTurnarounds();
}


bool
NBNetBuilder::setTLControllingInformation(int &step)
{
    inform(step, "Computing tls logics");
    return myTLLCont.setTLControllingInformation(myEdgeCont);
}


bool
NBNetBuilder::computeLogic(int &step, OptionsCont &oc)
{
    inform(step, "Computing node logics");
    return myNodeCont.computeLogics(myEdgeCont, myJunctionLogicCont, oc);
}


bool
NBNetBuilder::computeTLLogic(int &step, OptionsCont &oc)
{
    inform(step, "Computing traffic light logics");
    return myTLLCont.computeLogics(myEdgeCont, oc);
}


bool
NBNetBuilder::reshiftRotateNet(int &step, OptionsCont &oc)
{
    if(oc.isDefault("x-offset-to-apply")) {
        return true;
    }
    inform(step, "Transposing network");
    SUMOReal xoff = oc.getFloat("x-offset-to-apply");
    SUMOReal yoff = oc.getFloat("y-offset-to-apply");
    SUMOReal rot = oc.getFloat("rotation-to-apply");
    inform(step, "Normalising node positions");
    bool ok = myNodeCont.reshiftNodePositions(xoff, yoff, rot);
    if(ok) {
        ok = myEdgeCont.reshiftEdgePositions(xoff, yoff, rot);
    }
    return ok;
}




void
NBNetBuilder::compute(OptionsCont &oc)
{
    if(myHaveBuildNet) {
        return;
    }
    bool ok = true;
    int step = 1;
//    if(ok) ok = setInit(step);
    //
    if(ok) ok = removeDummyEdges(step);
    gJoinedEdges.init(myEdgeCont);
    if(ok) ok = joinEdges(step);
    if(ok) ok = removeUnwishedNodes(step, oc);
    if(ok&&oc.getBool("keep-edges.postload")) {
        if(ok) ok = removeUnwishedEdges(step, oc);
        if(ok) ok = removeUnwishedNodes(step, oc);
    }
    if(ok) ok = guessRamps(step, oc);
    if(ok) ok = guessTLs(step, oc);
    if(ok) ok = computeTurningDirections(step);
    if(ok) ok = sortNodesEdges(step);
    if(ok) ok = normaliseNodePositions(step);
    if(ok) ok = computeEdge2Edges(step);
    if(ok) ok = computeLanes2Edges(step);
    if(ok) ok = computeLanes2Lanes(step);
    if(ok) ok = appendTurnarounds(step, oc);
    if(ok) ok = recheckLanes(step);
    if(ok) ok = computeNodeShapes(step, oc);
    if(ok) ok = computeEdgeShapes(step);
//    if(ok) ok = computeLinkPriorities(step++);
    if(ok) ok = setTLControllingInformation(step);
    if(ok) ok = computeLogic(step, oc);
    if(ok) ok = computeTLLogic(step, oc);

    if(ok) ok = reshiftRotateNet(step, oc);

    NBNode::reportBuild();
    NBRequest::reportWarnings();
    checkPrint(oc);
    if(!ok) {
        throw ProcessError();
    }
    myHaveBuildNet = true;
}


void
NBNetBuilder::checkPrint(OptionsCont &oc)
{
    if(oc.getBool("print-node-positions")) {
        myNodeCont.printNodePositions();
    }
}


bool
NBNetBuilder::save(ostream &res, OptionsCont &oc)
{
    // print the computed values
    res << "<net>" << endl << endl;
    res.setf( ios::fixed, ios::floatfield );
    res << setprecision( 2 );
    // write the numbers of some elements
    std::vector<std::string> ids;
    if(oc.getBool("add-internal-links")) {
        ids = myNodeCont.getInternalNamesList();
    }
    myEdgeCont.writeXMLEdgeList(res, ids);
    if(oc.getBool("add-internal-links")) {
        myNodeCont.writeXMLInternalLinks(res);
    }

        // write the number of nodes
    myNodeCont.writeXMLNumber(res);
    res << endl;
    // write the districts
    myDistrictCont.writeXML(res);
    // write edges with lanes and connected edges
    myEdgeCont.writeXMLStep1(res);
    // write the logics
    myJunctionLogicCont.writeXML(res);
    myTLLCont.writeXML(res);
    // write the nodes
    myNodeCont.writeXML(res);
    // write the successors of lanes
    myEdgeCont.writeXMLStep2(res);
    if(oc.getBool("add-internal-links")) {
        myNodeCont.writeXMLInternalSuccInfos(res);
    }
    // write the positions of edges
    myEdgeCont.writeXMLStep3(res);
    if(oc.getBool("add-internal-links")) {
        myNodeCont.writeXMLInternalEdgePos(res);
    }
    res << "</net>" << endl;
    return true;
}


bool
NBNetBuilder::save(string path, OptionsCont &oc)
{
    // try to build the output file
    ofstream res(path.c_str());
    if(!res.good()) {
        return false;
    }
    return save(res, oc);
}


void
NBNetBuilder::savePlain(const std::string &filename)
{
    // try to build the output file
    myNodeCont.savePlain(filename + string(".nod.xml"));
    myEdgeCont.savePlain(filename + string(".edg.xml"));
//    NBConnectionCont::svaePlain(filename);
}


bool
NBNetBuilder::saveMap(string path)
{
    // try to build the output file
    ofstream res(path.c_str());
    if(!res.good()) {
        return false;
    }
    // write map
    res << gJoinedEdges;
	return true;
}


void
NBNetBuilder::insertNetBuildOptions(OptionsCont &oc)
{
    // register building defaults
    oc.doRegister("type", 'T', new Option_String("Unknown"));
    oc.doRegister("lanenumber", 'L', new Option_Integer(1));
    oc.doRegister("speed", 'S', new Option_Float((SUMOReal) 13.9));
    oc.doRegister("priority", 'P', new Option_Integer(1));
    // register computation variables
    oc.doRegister("min-decel", 'D', new Option_Float(3.0));
    // register the report options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());
    // extended
    oc.doRegister("print-node-positions", new Option_Bool(false));
    // register the data processing options
    oc.doRegister("recompute-junction-logics", new Option_Bool(false));
    oc.doRegister("omit-corrupt-edges", new Option_Bool(false));
    oc.doRegister("flip-y", new Option_Bool(false));
    oc.doRegister("all-logics", new Option_Bool(false));
    oc.doRegister("use-laneno-as-priority", new Option_Bool(false));
    oc.doRegister("keep-small-tyellow", new Option_Bool(false));
    oc.doRegister("traffic-light-green", new Option_Integer());
    oc.doRegister("traffic-light-yellow", new Option_Integer());
    oc.doRegister("x-offset-to-apply", new Option_Float(0));
    oc.doRegister("y-offset-to-apply", new Option_Float(0));
    oc.doRegister("rotation-to-apply", new Option_Float(0));
    oc.doRegister("no-node-removal", new Option_Bool(false));
    oc.doRegister("append-turnarounds", new Option_Bool(false));
    oc.doRegister("add-internal-links", 'I', new Option_Bool(false));

    oc.doRegister("map-output", 'M', new Option_FileName());

    // tls setting options
        // tls-guessing
    oc.doRegister("guess-tls", new Option_Bool(false));
    oc.doRegister("tls-guess.no-incoming-min", new Option_Integer(2));
    oc.doRegister("tls-guess.no-incoming-max", new Option_Integer(5));
    oc.doRegister("tls-guess.no-outgoing-min", new Option_Integer(1));
    oc.doRegister("tls-guess.no-outgoing-max", new Option_Integer(5));
    oc.doRegister("tls-guess.min-incoming-speed", new Option_Float((SUMOReal) (40/3.6)));
    oc.doRegister("tls-guess.max-incoming-speed", new Option_Float((SUMOReal)(69/3.6)));
    oc.doRegister("tls-guess.min-outgoing-speed", new Option_Float((SUMOReal)(40/3.6)));
    oc.doRegister("tls-guess.max-outgoing-speed", new Option_Float((SUMOReal)(69/3.6)));
    oc.doRegister("tls-guess.district-nodes", new Option_Bool(false));
        // tls-shifts
    oc.doRegister("tl-logics.half-offset", new Option_String());
    oc.doRegister("tl-logics.quarter-offset", new Option_String());
        // explicite tls
    oc.doRegister("explicite-tls", new Option_String());
    oc.doRegister("explicite-no-tls", new Option_String());

    // edge
    oc.doRegister("edges-min-speed", new Option_Float());
    oc.doRegister("keep-edges", new Option_String());
    oc.doRegister("keep-edges.input-file", new Option_FileName());
    oc.doRegister("keep-edges.postload", new Option_Bool(false));

    // unregulated nodes options
    oc.doRegister("keep-unregulated", new Option_Bool(false));
    oc.doRegister("keep-unregulated.nodes", new Option_String());
    oc.doRegister("keep-unregulated.district-nodes", new Option_Bool(false));

    // ramp guessing options
    oc.doRegister("guess-ramps", new Option_Bool(false));
    oc.doRegister("ramp-guess.max-ramp-speed", new Option_Float(-1));
    oc.doRegister("ramp-guess.min-highway-speed", new Option_Float((SUMOReal) (80/3.6)));
    oc.doRegister("ramp-guess.ramp-length", new Option_Float(100));

    oc.doRegister("guess-obscure-ramps", new Option_Bool(false));
    oc.doRegister("obscure-ramps.add-ramp", new Option_Bool(false));
    oc.doRegister("obscure-ramps.min-highway-speed", new Option_Float((SUMOReal) (100/3.6)));


    oc.doRegister("plain-output", new Option_FileName());
    oc.doRegister("node-geometry-dump", new Option_FileName());

    oc.doRegister("netbuild.debug", new Option_Integer(0));

}



void
NBNetBuilder::preCheckOptions(OptionsCont &oc)
{
    // we possibly have to load the edges to keep
    if(oc.isSet("keep-edges.input-file")) {
        ifstream strm(oc.getString("keep-edges.input-file").c_str());
        if(!strm.good()) {
            MsgHandler::getErrorInstance()->inform(
                "Could not load names of edges too keep from '"
                + oc.getString("keep-edges.input-file") + "'.");
            throw ProcessError();
        }
        std::ostringstream oss;
        bool first = true;
        while(strm.good()) {
            if(!first) {
                oss << ';';
            }
            string name;
            strm >> name;
            oss << name;
            first = false;
        }
        oc.set("keep-edges", oss.str());
    }
}


bool
NBNetBuilder::netBuild() const
{
    return myHaveBuildNet;
}


NBEdgeCont &
NBNetBuilder::getEdgeCont()
{
    return myEdgeCont;
}


NBNodeCont &
NBNetBuilder::getNodeCont()
{
    return myNodeCont;
}


NBTypeCont &
NBNetBuilder::getTypeCont()
{
    return myTypeCont;
}


NBTrafficLightLogicCont &
NBNetBuilder::getTLLogicCont()
{
    return myTLLCont;
}


NBJunctionLogicCont &
NBNetBuilder::getJunctionLogicCont()
{
    return myJunctionLogicCont;
}


NBDistrictCont &
NBNetBuilder::getDistrictCont()
{
    return myDistrictCont;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



