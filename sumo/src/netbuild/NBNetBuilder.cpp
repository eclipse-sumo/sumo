#include <string>
#include <fstream>
#include <iostream>
#include "NBNetBuilder.h"
#include "NBNodeCont.h"
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
#include <utils/convert/ToString.h>

using namespace std;

NBNetBuilder::NBNetBuilder()
{
}


NBNetBuilder::~NBNetBuilder()
{
}


void 
NBNetBuilder::buildLoaded()
{
    NBTypeCont::report();
    NBEdgeCont::report();
    NBNodeCont::report();
    // perform the computation
    OptionsCont &oc = OptionsSubSys::getOptions();
    compute(oc);
    // save network when wished
    save(oc.getString("o"));
}


void
NBNetBuilder::inform(int step, const std::string &about)
{
    MsgHandler::getMessageInstance()->inform(
        string("Computing step ") + toString<int>(step)
        + string(": ") + about);
}

bool
NBNetBuilder::removeDummyEdges(int step)
{
    inform(step, "Removing dummy edges ");
    return NBNodeCont::removeDummyEdges();
}


bool
NBNetBuilder::joinEdges(int step)
{
    inform(step, "Joining double connections");
    return NBNodeCont::recheckEdges();
}


bool
NBNetBuilder::computeTurningDirections(int step)
{
    inform(step, "Computing turning directions");
    return NBEdgeCont::computeTurningDirections();
}


bool
NBNetBuilder::sortNodesEdges(int step)
{
    inform(step, "Sorting nodes' edges, computing shape");
    return NBNodeCont::sortNodesEdges();
}


bool
NBNetBuilder::normaliseNodePositions(int step)
{
    inform(step, "Normalising node positions");
    bool ok = NBNodeCont::normaliseNodePositions();
    if(ok) {
        ok = NBEdgeCont::normaliseEdgePositions();
    }
    return ok;
}


bool
NBNetBuilder::computeEdge2Edges(int step)
{
    inform(step, "Computing Approached Edges");
    return NBEdgeCont::computeEdge2Edges();
}


bool
NBNetBuilder::computeLanes2Edges(int step)
{
    inform(step, "Computing Approaching Lanes");
    return NBEdgeCont::computeLanes2Edges();
}


bool
NBNetBuilder::computeLanes2Lanes(int step)
{
    inform(step, "Dividing of Lanes on Approached Lanes");
    bool ok = NBNodeCont::computeLanes2Lanes();
    if(ok) {
        return NBEdgeCont::sortOutgoingLanesConnections();
    }
    return ok;
}

bool
NBNetBuilder::recheckLanes(int step)
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

bool
NBNetBuilder::appendTurnarounds(int step)
{
    inform(step, "Appending Turnarounds");
    return NBEdgeCont::appendTurnarounds();
}


bool
NBNetBuilder::computeLogic(int step, OptionsCont &oc)
{
    inform(step, "Computing node logics");
    return NBNodeCont::computeLogics(oc);
}


bool
NBNetBuilder::computeTLLogic(int step, OptionsCont &oc)
{
    inform(step, "Computing traffic light logics");
    return NBTrafficLightLogicCont::computeLogics(oc);
}


void
NBNetBuilder::compute(OptionsCont &oc)
{
    bool ok = true;
    int step = 1;
//    if(ok) ok = setInit(step++);
    if(ok) ok = removeDummyEdges(step++);
    if(ok) ok = joinEdges(step++);
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


bool
NBNetBuilder::save(string path)
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

