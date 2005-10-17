/***************************************************************************
                          NBNodeCont.h
              A container for all of the nets nodes
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
// Revision 1.14  2005/10/17 09:00:44  dkrajzew
// got rid of the old MSVC memory leak checker; memory leaks removed
//
// Revision 1.13  2005/10/07 11:38:19  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.12  2005/09/23 06:01:06  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.11  2005/09/15 12:02:26  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.10  2005/07/12 12:32:49  dkrajzew
// code style adapted; guessing of ramps and unregulated near districts implemented; debugging
//
// Revision 1.9  2005/04/27 11:48:27  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.8  2004/11/23 10:21:42  dkrajzew
// debugging
//
// Revision 1.7  2004/08/02 13:11:40  dkrajzew
// made some deprovements or so
//
// Revision 1.6  2004/07/02 09:27:38  dkrajzew
// tls guessing added
//
// Revision 1.5  2004/04/23 12:41:02  dkrajzew
// some further work on vissim-import
//
// Revision 1.4  2004/03/19 13:06:09  dkrajzew
// some further work on vissim-import and geometry computation
//
// Revision 1.3  2004/02/18 05:31:25  dkrajzew
// debug-blas removed
//
// Revision 1.2  2004/02/16 13:59:15  dkrajzew
// some further work on edge geometry
//
// Revision 1.1  2004/01/12 15:26:11  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.25  2003/12/05 14:58:56  dkrajzew
// removed some unused debug macros
//
// Revision 1.24  2003/12/04 13:06:45  dkrajzew
// work on internal lanes
//
// Revision 1.23  2003/11/11 08:33:54  dkrajzew
// consequent position2D instead of two SUMOReals added
//
// Revision 1.22  2003/10/06 07:46:12  dkrajzew
// further work on vissim import (unsignalised vs. signalised streams modality
//  cleared & lane2lane instead of edge2edge-prohibitions implemented
//
// Revision 1.21  2003/09/22 12:40:12  dkrajzew
// further work on vissim-import
//
// Revision 1.20  2003/09/05 15:16:57  dkrajzew
// umlaute conversion; node geometry computation; internal links computation
//
// Revision 1.19  2003/08/18 12:49:59  dkrajzew
// possibility to print node positions added
//
// Revision 1.18  2003/08/14 13:51:51  dkrajzew
// reshifting of networks added
//
// Revision 1.17  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of
//  geometry information
//
// Revision 1.16  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.15  2003/06/05 11:43:35  dkrajzew
// class templates applied; documentation added
//
// Revision 1.14  2003/05/20 09:33:47  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on
//  tl-import; further work on vissim-import
//
// Revision 1.13  2003/04/14 08:34:59  dkrajzew
// some further bugs removed
//
// Revision 1.12  2003/04/10 15:45:19  dkrajzew
// some lost changes reapplied
//
// Revision 1.11  2003/04/04 07:43:04  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of
//  edge sorting (false lane connections) debugged
//
// Revision 1.10  2003/04/01 15:15:53  dkrajzew
// further work on vissim-import
//
// Revision 1.9  2003/03/20 16:23:09  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.8  2003/03/18 13:07:23  dkrajzew
// usage of node position within xml-edge descriptions allowed
//
// Revision 1.7  2003/03/17 14:22:33  dkrajzew
// further debug and windows eol removed
//
// Revision 1.6  2003/03/06 17:18:42  dkrajzew
// debugging during vissim implementation
//
// Revision 1.5  2003/03/03 14:59:10  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.4  2003/02/13 15:51:54  dkrajzew
// functions for merging edges with the same origin and destination added
//
// Revision 1.3  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.2  2002/10/17 13:33:52  dkrajzew
// adding of typed nodes added
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/07/25 08:31:42  dkrajzew
// Report methods transfered from loader to the containers
//
// Revision 1.4  2002/06/11 16:00:42  dkrajzew
// windows eol removed; template class definition inclusion depends now on the
//  EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:12  dkrajzew
// Windows eol removed; minor SUMOReal to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.3  2002/04/09 12:21:25  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/22 10:50:03  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:37:59  traffic
// files for the netbuilder
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
#include <map>
#include <algorithm>
#include <fstream>
#include <utils/options/OptionsCont.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StdDefs.h>
#include <utils/common/ToString.h>
#include <netbuild/NBDistrict.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBJunctionLogicCont.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBJoinedEdgesMap.h>
#include <netbuild/NBOwnTLDef.h>

#include "NBNodeCont.h"

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
NBNodeCont::NBNodeCont()
    : _internalID(1)
{
}


NBNodeCont::~NBNodeCont()
{
    clear();
}


bool
NBNodeCont::insert(const std::string &id, const Position2D &position,
                   NBDistrict *district)
{
    NodeCont::iterator i = _nodes.find(id);
    if(i!=_nodes.end()) {
        if( (*i).second->getPosition().x()==position.x() &&
            (*i).second->getPosition().y()==position.y()) {
            return true;
        }
        return false;
    }
    NBNode *node = new NBNode(id, position, district);
    _nodes[id] = node;
    return true;
}


bool
NBNodeCont::insert(const string &id, const Position2D &position)
{
    NodeCont::iterator i = _nodes.find(id);
    if(i!=_nodes.end()) {
        if( fabs((*i).second->getPosition().x()-position.x())<0.1 &&
            fabs((*i).second->getPosition().y()-position.y())<0.1) {
            return true;
        }
        return false;
    }
    NBNode *node = new NBNode(id, position);
    _nodes[id] = node;
    return true;
}

/*
bool
NBNodeCont::insert(const string &id, SUMOReal x, SUMOReal y,
                   const std::string &type)
{
    NodeCont::iterator i = _nodes.find(id);
    if(i!=_nodes.end()) {
        if( (*i).second->getXCoordinate()==x &&
            (*i).second->getYCoordinate()==y) {
            return true;
        }
        return false;
    }
    NBNode *node = new NBNode(id, x, y, type);
    _nodes[id] = node;
    return true;
}
*/

Position2D
NBNodeCont::insert(const string &id) // !!! really needed
{
    pair<SUMOReal, SUMOReal> ret(-1.0, -1.0);
    NodeCont::iterator i = _nodes.find(id);
    if(i!=_nodes.end()) {
        return (*i).second->getPosition();
    } else {
        NBNode *node = new NBNode(id, Position2D(-1.0, -1.0));
        _nodes[id] = node;
    }
    return Position2D(-1, -1);
}


bool
NBNodeCont::insert(NBNode *node)
{
    string id = node->getID();
    NodeCont::iterator i = _nodes.find(id);
    if(i!=_nodes.end()) {
        if( (*i).second->getPosition().x()==node->getPosition().x() &&
            (*i).second->getPosition().y()==node->getPosition().y() ) {
            return true;
        }
        return false;
    }
    _nodes[id] = node;
    return true;
}


NBNode *
NBNodeCont::retrieve(const string &id)
{
    NodeCont::iterator i = _nodes.find(id);
    if(i==_nodes.end()) {
        return 0;
    }
    return (*i).second;
}


NBNode *
NBNodeCont::retrieve(const Position2D &position)
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        NBNode *node = (*i).second;
        if( fabs(node->getPosition().x()-position.x())<0.1
            &&
            fabs(node->getPosition().y()-position.y())<0.1) {

            return node;
        }
    }
    return 0;
}


bool
NBNodeCont::erase(NBNode *node)
{
    NodeCont::iterator i = _nodes.find(node->getID());
    if(i==_nodes.end()) {
        return false;
    }
    _nodes.erase(i);
    delete node;
    return true;
}



bool
NBNodeCont::normaliseNodePositions()
{
    // compute the boundary
    Boundary boundary;
    NodeCont::iterator i;
    for(i=_nodes.begin(); i!=_nodes.end(); i++) {
        boundary.add((*i).second->getPosition());
    }
    // reformat
    SUMOReal xmin = boundary.xmin() * -1;
    SUMOReal ymin = boundary.ymin() * -1;
    for(i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->resetby(xmin, ymin);
    }
    myNetworkOffset = Position2D(xmin, ymin);
    return true;
}


bool
NBNodeCont::reshiftNodePositions(SUMOReal xoff, SUMOReal yoff, SUMOReal rot)
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->reshiftPosition(xoff, yoff, rot);
    }
    return true;
}


bool
NBNodeCont::computeLanes2Lanes()
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->computeLanes2Lanes();
    }
    return true;
}


// computes the "wheel" of incoming and outgoing edges for every node
bool
NBNodeCont::computeLogics(const NBEdgeCont &ec, NBJunctionLogicCont &jc,
                          OptionsCont &oc)
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->computeLogic(ec, jc, oc);
    }
    return true;
}


bool
NBNodeCont::sortNodesEdges(const NBTypeCont &tc)
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->sortNodesEdges(tc);
    }
    return true;
}


void
NBNodeCont::writeXMLNumber(ostream &into)
{
    into << "   <node_count>" << _nodes.size() << "</node_count>" << endl;
}


std::vector<std::string>
NBNodeCont::getInternalNamesList()
{
    std::vector<std::string> ret;
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        std::vector<std::string> nodes =
            (*i).second->getInternalNamesList();
        copy(nodes.begin(), nodes.end(),
            back_inserter(ret));
    }
    return ret;
}



void
NBNodeCont::writeXMLInternalLinks(ostream &into)
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->writeXMLInternalLinks(into);
    }
    into << endl;
}


void
NBNodeCont::writeXMLInternalEdgePos(ostream &into)
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->writeXMLInternalEdgePos(into);
    }
    into << endl;
}


void
NBNodeCont::writeXMLInternalSuccInfos(ostream &into)
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->writeXMLInternalSuccInfos(into);
    }
    into << endl;
}


void
NBNodeCont::writeXML(ostream &into)
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->writeXML(into);
    }
    into << endl;
}


int
NBNodeCont::size()
{
    return(_nodes.size());
}


int
NBNodeCont::getNo()
{
    return _nodes.size();
}


void
NBNodeCont::clear()
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        delete((*i).second);
    }
    _nodes.clear();
}


void
NBNodeCont::report()
{
    WRITE_MESSAGE(string("   ") + toString<int>(getNo()) + string(" nodes loaded."));
}


bool
NBNodeCont::recheckEdges(NBDistrictCont &dc, NBTrafficLightLogicCont &tlc,
                         NBEdgeCont &ec)
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        // count the edges to other nodes outgoing from the current
        //  node
        std::map<NBNode*, EdgeVector> connectionCount;
        const EdgeVector &outgoing = (*i).second->getOutgoingEdges();
        for(EdgeVector::const_iterator j=outgoing.begin(); j!=outgoing.end(); j++) {
            NBEdge *e = (*j);
            NBNode *connected = e->getToNode();
            if(connectionCount.find(connected)==connectionCount.end()) {
                connectionCount[connected] = EdgeVector();
                connectionCount[connected].push_back(e);
            } else {
                connectionCount[connected].push_back(e);
            }
        }
        // check whether more than a single edge connect another node
        //  and join them
        std::map<NBNode*, EdgeVector>::iterator k;
        for(k=connectionCount.begin(); k!=connectionCount.end(); k++) {
            // possibly we do not have anything to join...
            if((*k).second.size()<2) {
                continue;
            }
            // for the edges that seem to be a single street,
            //  check whether the geometry is similar
            const EdgeVector &ev = (*k).second;
            typedef std::vector<EdgeVector> EdgeVV;
            EdgeVV geometryCombinations;
            for(EdgeVector::const_iterator l=ev.begin(); l!=ev.end(); ++l) {
                // append the first one simply to the list of really joinable edges
                if(geometryCombinations.size()==0) {
                    EdgeVector tmp;
                    tmp.push_back(*l);
                    geometryCombinations.push_back(tmp);
                    continue;
                }
                // check the lists of really joinable edges
                bool wasPushed = false;
                for(EdgeVV::iterator m=geometryCombinations.begin(); !wasPushed&&m!=geometryCombinations.end(); ++m) {
                    for(EdgeVector::iterator n=(*m).begin(); !wasPushed&&n!=(*m).end(); ++n) {
                        if((*n)->isNearEnough2BeJoined2(*l)) {
                            (*m).push_back(*l);
                            wasPushed = true;
                        }
                    }
                }
                if(!wasPushed) {
                    EdgeVector tmp;
                    tmp.push_back(*l);
                    geometryCombinations.push_back(tmp);
                }
            }
            /*
            // recheck combinations
            //  (hate this)
            bool hasChanged = true;
            while(hasChanged) {
                hasChanged = false;
                for(EdgeVV::iterator m=geometryCombinations.begin(); !hasChanged&&m!=geometryCombinations.end(); ++m) {
                    for(EdgeVV::iterator n=m+1; !hasChanged&&n!=geometryCombinations.end(); ++n) {
                        for(EdgeVector::iterator o=(*m).begin(); !hasChanged&&o!=(*m).end(); o++) {
                            for(EdgeVector::iterator p=(*n).begin(); !hasChanged&&p!=(*n).end(); p++) {
                                if((*o)->isNearEnough2BeJoined2(*p)) {
                                    copy((*m).begin(), (*m).end(), back_inserter(*n));
                                    geometryCombinations.erase(m);
                                    hasChanged = true;
                                }
                            }
                        }
                    }
                }
            }
            // now join finally
            for(EdgeVV::iterator m=geometryCombinations.begin(); m!=geometryCombinations.end(); ++m) {
                if((*m).size()>1) {
                    NBEdgeCont::joinSameNodeConnectingEdges(*m);
                }
            }
            */
        }
        for(k=connectionCount.begin(); k!=connectionCount.end(); k++) {
            // join edges
            if((*k).second.size()>1) {
                ec.joinSameNodeConnectingEdges(dc, tlc, (*k).second);
            }
        }
    }
    return true;
}



bool
NBNodeCont::removeDummyEdges(NBDistrictCont &dc, NBEdgeCont &ec,
                             NBTrafficLightLogicCont &tc)
{
    size_t no = 0;
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        no += (*i).second->eraseDummies(dc, ec, tc);
    }
    if(no!=0) {
        WRITE_WARNING(toString<int>(no) + string(" dummy edges removed."));
    }
    return true;
}


void
NBNodeCont::searchEdgeInNode(const NBEdgeCont &ec,
                             string nodeid, string edgeid)
{
    NBNode *n = retrieve(nodeid);
    NBEdge *e = ec.retrieve(edgeid);
    if(n==0||e==0) {
        return;
    }

    if( find(n->_outgoingEdges->begin(), n->_outgoingEdges->end(), e)
        !=n->_outgoingEdges->end()) {
        int checkdummy = 0;
    }

    if( find(n->_incomingEdges->begin(), n->_incomingEdges->end(), e)
        !=n->_incomingEdges->end()) {
        int checkdummy = 0;
    }

}

std::string
NBNodeCont::getFreeID()
{
    return "SUMOGenerated" + toString<int>(getNo());
}


Position2D
NBNodeCont::getNetworkOffset() const
{
    return myNetworkOffset;
}

bool
NBNodeCont::computeNodeShapes(OptionsCont &oc)
{
    ofstream *ngd = 0;
    if(oc.isSet("node-geometry-dump")) {
        ngd = new ofstream(oc.getString("node-geometry-dump").c_str());
        (*ngd) << "<pois>" << endl;
    }
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->computeNodeShape(ngd);
    }
    if(ngd!=0) {
        (*ngd) << "</pois>" << endl;
    }
    delete ngd;
    return true;
}


void
NBNodeCont::printNodePositions()
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        string ni = (*i).second->getID();
        ni += string(":")
            + toString<SUMOReal>((*i).second->getPosition().x())
            + string(", ")
            + toString<SUMOReal>((*i).second->getPosition().y());
        MsgHandler::getMessageInstance()->inform(ni);
    }
}


bool
NBNodeCont::removeUnwishedNodes(NBDistrictCont &dc, NBEdgeCont &ec,
                                NBTrafficLightLogicCont &tlc)
{
    size_t no = 0;
    std::vector<NBNode*> toRemove;
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        NBNode *current = (*i).second;
        bool remove = false;
        std::vector<std::pair<NBEdge*, NBEdge*> > toJoin;
        // check for completely empty nodes
        if( current->getOutgoingEdges().size()==0
            &&
            current->getIncomingEdges().size()==0) {

            // remove if empty
            remove = true;
        }
        // check for nodes which are only geometry nodes
        if( (current->getOutgoingEdges().size()==1
             &&
             current->getIncomingEdges().size()==1)
            ||
            (current->getOutgoingEdges().size()==2
             &&
             current->getIncomingEdges().size()==2) ) {

            // ok, one in, one out or two in, two out
            //  -> ask the node whether to join
            remove = current->checkIsRemovable();
            if(remove) {
                toJoin = current->getEdgesToJoin();
            }
        }
        // remove the node and join the geometries when wished
        if(!remove) {
            continue;
        }
        for(std::vector<std::pair<NBEdge*, NBEdge*> >::iterator j=toJoin.begin(); j!=toJoin.end(); j++) {
            NBEdge *begin = (*j).first;
            NBEdge *continuation = (*j).second;
            begin->append(continuation);
            continuation->getToNode()->replaceIncoming(continuation, begin, 0);
            tlc.replaceRemoved(continuation, -1, begin, -1);
            gJoinedEdges.appended(begin->getID(), continuation->getID());
            ec.erase(dc, continuation);
        }
        toRemove.push_back(current);
        no++;
    }
    // erase
    for(std::vector<NBNode*>::iterator j=toRemove.begin(); j!=toRemove.end(); j++) {
        erase(*j);
    }
    WRITE_MESSAGE(string("   ") + toString<int>(no) + string(" nodes removed."));
    return true;
}



bool
NBNodeCont::mayNeedOnRamp(OptionsCont &oc, NBNode *cur) const
{
    if(cur->getIncomingEdges().size()==2&&cur->getOutgoingEdges().size()==1) {
        // may be an on-ramp
        NBEdge *pot_highway = cur->getIncomingEdges()[0];
        NBEdge *pot_ramp = cur->getIncomingEdges()[1];
        NBEdge *cont = cur->getOutgoingEdges()[0];
        if(pot_highway->getSpeed()<pot_ramp->getSpeed()) {
            swap(pot_highway, pot_ramp);
        } else if(pot_highway->getSpeed()==pot_ramp->getSpeed()
            &&
            pot_highway->getNoLanes()<pot_ramp->getNoLanes()) {

            swap(pot_highway, pot_ramp);
        }
        // check conditions
            // is it really a highway?
        if(pot_highway->getSpeed()<oc.getFloat("ramp-guess.min-highway-speed")
            ||
            cont->getSpeed()<oc.getFloat("ramp-guess.min-highway-speed")) {
            return false;
        }
            // is it really a ramp?
        if(oc.getFloat("ramp-guess.max-ramp-speed")>0
            &&
            oc.getFloat("ramp-guess.max-ramp-speed")<pot_ramp->getSpeed()) {
            return false;
        }
        // ok, may be
        return true;
    }
    return false;
}


void
NBNodeCont::buildOnRamp(OptionsCont &oc, NBNode *cur,
                        NBEdgeCont &ec, NBDistrictCont &dc,
                        std::vector<NBEdge*> &incremented)
{
    NBEdge *pot_highway = cur->getIncomingEdges()[0];
    NBEdge *pot_ramp = cur->getIncomingEdges()[1];
    NBEdge *cont = cur->getOutgoingEdges()[0];
    if(pot_highway->getSpeed()<pot_ramp->getSpeed()) {
        swap(pot_highway, pot_ramp);
    } else if(pot_highway->getSpeed()==pot_ramp->getSpeed()
        &&
        pot_highway->getNoLanes()<pot_ramp->getNoLanes()) {

        swap(pot_highway, pot_ramp);
    }
    if(cont->getGeometry().length()<=oc.getFloat("ramp-guess.ramp-length")) {
        // the edge is shorter than the wished ramp
            //  append a lane only
        if(find(incremented.begin(), incremented.end(), cont)==incremented.end()) {
            cont->incLaneNo(pot_ramp->getNoLanes());
            incremented.push_back(cont);
            if(!pot_highway->addLane2LaneConnections(0, cont, pot_ramp->getNoLanes(),
                MIN2(cont->getNoLanes()-pot_ramp->getNoLanes(), pot_highway->getNoLanes()), true)) {

                MsgHandler::getErrorInstance()->inform("Could not set connection!!!");
                throw ProcessError();
            }
            if(!pot_ramp->addLane2LaneConnections(0, cont, 0, pot_ramp->getNoLanes(), true)) {
                MsgHandler::getErrorInstance()->inform("Could not set connection!!!");
                throw ProcessError();
            }
            cont->invalidateConnections(true);
        }
        Position2DVector p = pot_ramp->getGeometry();
        p.pop_back();
        p.push_back(cont->getLaneShape(0).at(0));
        pot_ramp->setGeometry(p);
    } else {
        NBNode *rn =
            new NBNode(cont->getID() + "-AddedOnRampNode",
                cont->getGeometry().positionAtLengthPosition(
                oc.getFloat("ramp-guess.ramp-length")));
        if(!insert(rn)) {
            MsgHandler::getErrorInstance()->inform(
                "Ups - could not build on-ramp for edge '"
                + pot_highway->getID() + "' (node could not be build)!");
            throw ProcessError();
        }
        string name = cont->getID();
        bool ok = ec.splitAt(dc, cont, rn,
            cont->getID()+"-AddedOnRampEdge", cont->getID(),
            cont->getNoLanes()+pot_ramp->getNoLanes(), cont->getNoLanes());
        if(!ok) {
            MsgHandler::getErrorInstance()->inform(
                "Ups - could not build on-ramp for edge '"
                + pot_highway->getID() + "'!");
                return;
        } else {
            NBEdge *added_ramp = ec.retrieve(name+"-AddedOnRampEdge");
            NBEdge *added = ec.retrieve(name);
            incremented.push_back(added_ramp);
            if(added_ramp->getNoLanes()!=added->getNoLanes()) {
                int off = added_ramp->getNoLanes()-added->getNoLanes();
                if(!added_ramp->addLane2LaneConnections(off, added, 0, added->getNoLanes(), true)) {
                    MsgHandler::getErrorInstance()->inform("Could not set connection!!!");
                    throw ProcessError();
                }
            } else {
                if(!added_ramp->addLane2LaneConnections(0, added, 0, added_ramp->getNoLanes(), true)) {
                    MsgHandler::getErrorInstance()->inform("Could not set connection!!!");
                    throw ProcessError();
                }
            }
            if(!pot_highway->addLane2LaneConnections(0, added_ramp, pot_ramp->getNoLanes(),
                MIN2(added_ramp->getNoLanes()-pot_ramp->getNoLanes(), pot_highway->getNoLanes()), false)) {

                MsgHandler::getErrorInstance()->inform("Could not set connection!!!");
                throw ProcessError();
            }
            if(!pot_ramp->addLane2LaneConnections(0, added_ramp, 0, pot_ramp->getNoLanes(), true)) {
                MsgHandler::getErrorInstance()->inform("Could not set connection!!!");
                throw ProcessError();
            }
            Position2DVector p = pot_ramp->getGeometry();
            p.pop_back();
            p.push_back(added_ramp->getLaneShape(0).at(0));
            pot_ramp->setGeometry(p);
        }
    }
}


void
NBNodeCont::buildOffRamp(OptionsCont &oc, NBNode *cur,
                         NBEdgeCont &ec, NBDistrictCont &dc,
                         std::vector<NBEdge*> &incremented)
{
    NBEdge *pot_highway = cur->getOutgoingEdges()[0];
    NBEdge *pot_ramp = cur->getOutgoingEdges()[1];
    NBEdge *prev = cur->getIncomingEdges()[0];
    if(pot_highway->getSpeed()<pot_ramp->getSpeed()) {
        swap(pot_highway, pot_ramp);
    } else if(pot_highway->getSpeed()==pot_ramp->getSpeed()
        &&
        pot_highway->getNoLanes()<pot_ramp->getNoLanes()) {

        swap(pot_highway, pot_ramp);
    }
    // ok, append on-ramp
    if(prev->getGeometry().length()<=oc.getFloat("ramp-guess.ramp-length")) {
        // the edge is shorter than the wished ramp
        //  append a lane only
        if(find(incremented.begin(), incremented.end(), prev)==incremented.end()) {
            incremented.push_back(prev);
            prev->incLaneNo(pot_ramp->getNoLanes());
            prev->invalidateConnections(true);
            if(!prev->addLane2LaneConnections(pot_ramp->getNoLanes(), pot_highway, 0,
                MIN2(prev->getNoLanes()-1, pot_highway->getNoLanes()), true)) {

                MsgHandler::getErrorInstance()->inform("Could not set connection!!!");
                throw ProcessError();
            }
            if(!prev->addLane2LaneConnections(0, pot_ramp, 0, pot_ramp->getNoLanes(), false)) {
                MsgHandler::getErrorInstance()->inform("Could not set connection!!!");
                throw ProcessError();
            }
        }
        Position2DVector p = pot_ramp->getGeometry();
        p.pop_front();
        p.push_front(prev->getLaneShape(0).at(-1));
        pot_ramp->setGeometry(p);
    } else {
        NBNode *rn =
            new NBNode(prev->getID() + "-AddedOffRampNode",
                prev->getGeometry().positionAtLengthPosition(
                    prev->getGeometry().length()-oc.getFloat("ramp-guess.ramp-length")));
        if(!insert(rn)) {
            MsgHandler::getErrorInstance()->inform(
                "Ups - could not build off-ramp for edge '"
                + pot_highway->getID() + "' (node could not be build)!");
            throw ProcessError();
        }
        string name = prev->getID();
        bool ok = ec.splitAt(dc, prev, rn,
            prev->getID(), prev->getID()+"-AddedOffRampEdge",
            prev->getNoLanes(), prev->getNoLanes()+pot_ramp->getNoLanes());
        if(!ok) {
            MsgHandler::getErrorInstance()->inform(
                "Ups - could not build on-ramp for edge '"
                + pot_highway->getID() + "'!");
            return;
        } else {
            NBEdge *added_ramp = ec.retrieve(name+"-AddedOffRampEdge");
            NBEdge *added = ec.retrieve(name);
            if(added_ramp->getNoLanes()!=added->getNoLanes()) {
                incremented.push_back(added_ramp);
                int off = added_ramp->getNoLanes()-added->getNoLanes();
                if(!added->addLane2LaneConnections(0, added_ramp, off, added->getNoLanes(), true)) {
                    MsgHandler::getErrorInstance()->inform("Could not set connection!!!");
                    throw ProcessError();
                }
            } else {
                if(!added->addLane2LaneConnections(0, added_ramp, 0, added_ramp->getNoLanes(), true)) {
                    MsgHandler::getErrorInstance()->inform("Could not set connection!!!");
                    throw ProcessError();
                }
            }
            if(!added_ramp->addLane2LaneConnections(pot_ramp->getNoLanes(), pot_highway, 0,
                MIN2(added_ramp->getNoLanes()-pot_ramp->getNoLanes(), pot_highway->getNoLanes()), true)) {

                MsgHandler::getErrorInstance()->inform("Could not set connection!!!");
                throw ProcessError();
            }
            if(!added_ramp->addLane2LaneConnections(0, pot_ramp, 0, pot_ramp->getNoLanes(), false)) {
                MsgHandler::getErrorInstance()->inform("Could not set connection!!!");
                throw ProcessError();
            }
            Position2DVector p = pot_ramp->getGeometry();
            p.pop_front();
            p.push_front(added_ramp->getLaneShape(0).at(-1));
            pot_ramp->setGeometry(p);
        }
    }
}


bool
NBNodeCont::mayNeedOffRamp(OptionsCont &oc, NBNode *cur) const
{
    if(cur->getIncomingEdges().size()==1&&cur->getOutgoingEdges().size()==2) {
        // may be an off-ramp
        NBEdge *pot_highway = cur->getOutgoingEdges()[0];
        NBEdge *pot_ramp = cur->getOutgoingEdges()[1];
        NBEdge *prev = cur->getIncomingEdges()[0];
        if(pot_highway->getSpeed()<pot_ramp->getSpeed()) {
            swap(pot_highway, pot_ramp);
        } else if(pot_highway->getSpeed()==pot_ramp->getSpeed()
            &&
            pot_highway->getNoLanes()<pot_ramp->getNoLanes()) {

            swap(pot_highway, pot_ramp);
        }
        // check conditions
            // is it really a highway?
        if(pot_highway->getSpeed()<oc.getFloat("ramp-guess.min-highway-speed")
            ||
            prev->getSpeed()<oc.getFloat("ramp-guess.min-highway-speed")) {
            return false;
        }
            // is it really a ramp?
        if(oc.getFloat("ramp-guess.max-ramp-speed")>0
            &&
            oc.getFloat("ramp-guess.max-ramp-speed")<pot_ramp->getSpeed()) {
            return false;
        }
        return true;
    }
    return false;
}


void
NBNodeCont::checkHighwayRampOrder(NBEdge *&pot_highway, NBEdge *&pot_ramp)
{
    if(pot_highway->getSpeed()<pot_ramp->getSpeed()) {
        swap(pot_highway, pot_ramp);
    } else if(pot_highway->getSpeed()==pot_ramp->getSpeed()
        &&
        pot_highway->getNoLanes()<pot_ramp->getNoLanes()) {

        swap(pot_highway, pot_ramp);
    }
}


bool
NBNodeCont::guessRamps(OptionsCont &oc, NBEdgeCont &ec,
                       NBDistrictCont &dc)
{
    std::vector<NBEdge*> incremented;
    // check whether obsure highway connections shall be checked
    if(oc.getBool("guess-obscure-ramps")) {
        for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
            NBNode *cur = (*i).second;
            const EdgeVector &inc = (*i).second->getIncomingEdges();
            const EdgeVector &out = (*i).second->getOutgoingEdges();
            if(inc.size()!=2||out.size()!=2) {
                continue;
            }
            {
                bool hadInHighway = false;
                for(EdgeVector::const_iterator j=inc.begin(); j!=inc.end(); ++j) {
                    if((*j)->getSpeed()>oc.getFloat("obscure-ramps.min-highway-speed")
                        &&
                       (*j)->getBasicType()!=NBEdge::EDGEFUNCTION_SOURCE
                        &&
                       (*j)->getBasicType()!=NBEdge::EDGEFUNCTION_SINK) {

                        hadInHighway = true;
                    }
                }
                if(!hadInHighway) {
                    continue;
                }
            }
            {
                bool hadOutHighway = false;
                for(EdgeVector::const_iterator j=out.begin(); j!=out.end(); ++j) {
                    if((*j)->getSpeed()>oc.getFloat("obscure-ramps.min-highway-speed")
                        &&
                       (*j)->getBasicType()!=NBEdge::EDGEFUNCTION_SOURCE
                        &&
                       (*j)->getBasicType()!=NBEdge::EDGEFUNCTION_SINK) {

                        hadOutHighway = true;
                    }
                }
                if(!hadOutHighway) {
                    continue;
                }
            }
            // ok, something is strange:
            //  we do have a highway, here with an off- and an on-ramp on the same node!?
            // try to place the incoming before...
            //  ... determine a possible position, first
            NBEdge *inc_highway = inc[0];
            NBEdge *inc_ramp = inc[1];
            NBEdge *out_highway = out[0];
            NBEdge *out_ramp = out[1];
            checkHighwayRampOrder(inc_highway, inc_ramp);
            checkHighwayRampOrder(out_highway, out_ramp);

            if(100>GeomHelper::distance(inc_highway->getToNode()->getPosition(), inc_ramp->getGeometry().at(-1))) {
                Position2DVector tmp = inc_ramp->getGeometry();
                tmp.eraseAt(-1);
                inc_ramp->setGeometry(tmp);
            }
            SUMOReal pos =
                inc_highway->getGeometry().nearest_position_on_line_to_point(
                    inc_ramp->getGeometry().at(-1));
            if(pos<0) {
                continue;
            }
            Position2D p = inc_highway->getGeometry().positionAtLengthPosition(pos);
            NBNode *rn =
                new NBNode(inc_highway->getID() + "-AddedAntiObscureNode", p);
            if(!insert(rn)) {
                MsgHandler::getErrorInstance()->inform(
                    "Ups - could not build anti-obscure node '"
                    + inc_highway->getID() + "'!");
                throw ProcessError();
            }
            string name = inc_highway->getID();
            bool ok = ec.splitAt(dc, inc_highway, rn,
                inc_highway->getID(), inc_highway->getID()+"-AddedInBetweenEdge",
                inc_highway->getNoLanes(), inc_highway->getNoLanes());
            if(!ok) {
                MsgHandler::getErrorInstance()->inform(
                    "Ups - could not build anti-obscure edge '"
                    + inc_highway->getID() + "'!");
                throw ProcessError();
            } else {
                NBEdge *added_cont = ec.retrieve(name+"-AddedInBetweenEdge");
                NBEdge *added = ec.retrieve(name);
                added_cont->getToNode()->removeIncoming(out_ramp);
                added->getToNode()->addIncomingEdge(out_ramp);
            }
        }
    }
    // check whether on-off ramps shall be guessed
    if(oc.getBool("guess-ramps")) {
        for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
            NBNode *cur = (*i).second;
            if(mayNeedOnRamp(oc, cur)) {
                buildOnRamp(oc, cur, ec, dc, incremented);
            }
            if(mayNeedOffRamp(oc, cur)) {
                buildOffRamp(oc, cur, ec, dc, incremented);
            }
        }
    }
    return true;
}


bool
NBNodeCont::guessTLs(OptionsCont &oc, NBTrafficLightLogicCont &tlc)
{
    // loop#1 checking whether the node shall tls controlled,
    //  because it is assigned to a district
    if(oc.getBool("tls-guess.district-nodes")) {
        for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
            NBNode *cur = (*i).second;
            if((*i).second->isNearDistrict()) {
                setAsTLControlled((*i).first, tlc);
            }
        }
    }
    // maybe no tls shall be guessed
    if(!oc.getBool("guess-tls")) {
        return true;
    }
    // build list of definitely not tls-controlled junctions
    std::vector<NBNode*> ncontrolled;
    if(oc.isSet("explicite-no-tls")) {
        StringTokenizer st(oc.getString("explicite-no-tls"), ";");
        while(st.hasNext()) {
            string name = st.next();
            NBNode *n = NBNodeCont::retrieve(name);
            if(n==0) {
                MsgHandler::getErrorInstance()->inform(
                    string(" The node '") + name + string("' to set as not-controlled is not known."));
                throw ProcessError();
            }
            ncontrolled.push_back(n);
        }
    }

    // loop#2: checking whether the node shall be controlled by a tls due
    //  to the number of lane & their speeds
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        NBNode *cur = (*i).second;
        //  do nothing if already is tl-controlled
        if(cur->isTLControlled()) {
            continue;
        }
        // do nothing if in the list of explicite non-controlled junctions
        if(find(ncontrolled.begin(), ncontrolled.end(), cur)!=ncontrolled.end()) {
            continue;
        }

        // check whether the node has the right amount of incoming edges
        //  to be controlled by a tl
        if( (int) cur->getIncomingEdges().size()<oc.getInt("tls-guess.no-incoming-min")
            ||
            (int) cur->getIncomingEdges().size()>oc.getInt("tls-guess.no-incoming-max") ) {

            // nope...
            continue;
        }

        // check whether the node has the right amount of outgoing edges
        //  to be controlled by a tl
        if( (int) cur->getOutgoingEdges().size()<oc.getInt("tls-guess.no-outgoing-min")
            ||
            (int) cur->getOutgoingEdges().size()>oc.getInt("tls-guess.no-outgoing-max") ) {

            // nope...
            continue;
        }

        // check whether the edges have the correct speed
        //  to be controlled by a tl
        if( NBContHelper::getMinSpeed(cur->getIncomingEdges())<oc.getFloat("tls-guess.min-incoming-speed")
            ||
            NBContHelper::getMaxSpeed(cur->getIncomingEdges())>oc.getFloat("tls-guess.max-incoming-speed")
            ||
            NBContHelper::getMinSpeed(cur->getOutgoingEdges())<oc.getFloat("tls-guess.min-outgoing-speed")
            ||
            NBContHelper::getMaxSpeed(cur->getOutgoingEdges())>oc.getFloat("tls-guess.max-outgoing-speed") ) {

            // nope...
            continue;
        }

        // hmmm, should be tls-controlled (probably)
        setAsTLControlled((*i).first, tlc);

    }
    return true;
}


void
NBNodeCont::setAsTLControlled(const std::string &name,
                              NBTrafficLightLogicCont &tlc)
{
    NBNode *node = retrieve(name);
    if(node==0) {
        MsgHandler::getWarningInstance()->inform(
            string("Building a tl-logic for node '") + name
            + string("' is not possible."));
        MsgHandler::getWarningInstance()->inform(
            string(" The node '") + name + string("' is not known."));
        return;
    }
    NBTrafficLightDefinition *tlDef =
        new NBOwnTLDef(name, node);
    if(!tlc.insert(name, tlDef)) {
        // actually, nothing should fail here
        MsgHandler::getWarningInstance()->inform(
            string("Building a tl-logic for node '") + name
            + string("' twice is not possible."));
        delete tlDef;
        return;
    }
}


bool
NBNodeCont::savePlain(const std::string &file)
{
    // try to build the output file
    ofstream res(file.c_str());
    if(!res.good()) {
        return false;
    }
    res << "<nodes>" << endl;
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        NBNode *n = (*i).second;
        res << "   <node id=\"" << n->getID() << "\" x=\""
            << n->getPosition().x() << "\" y=\"" << n->getPosition().y()
            << "\"/>" << endl;
    }
    res << "</nodes>" << endl;
    return res.good();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

