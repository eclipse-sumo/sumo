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
// Revision 1.18  2003/08/14 13:51:51  dkrajzew
// reshifting of networks added
//
// Revision 1.17  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.16  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.15  2003/06/05 11:43:35  dkrajzew
// class templates applied; documentation added
//
// Revision 1.14  2003/05/20 09:33:47  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on tl-import; further work on vissim-import
//
// Revision 1.13  2003/04/14 08:34:59  dkrajzew
// some further bugs removed
//
// Revision 1.12  2003/04/10 15:45:19  dkrajzew
// some lost changes reapplied
//
// Revision 1.11  2003/04/04 07:43:04  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge sorting (false lane connections) debugged
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
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:12  dkrajzew
// Windows eol removed; minor double to int conversions removed;
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <map>
#include <algorithm>
//#include <strstream>
#include <utils/options/OptionsCont.h>
#include <utils/geom/Boundery.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include "NBDistrict.h"
#include "NBNodeCont.h"
#include "NBEdgeCont.h"
#include "NBJunctionLogicCont.h"


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static members
 * ======================================================================= */
NBNodeCont::NodeCont    NBNodeCont::_nodes;
int                     NBNodeCont::_internalID = 1;
Position2D              NBNodeCont::myNetworkOffset;


/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
NBNodeCont::insert(const std::string &id, double x, double y,
                   NBDistrict *district)
{
    NodeCont::iterator i = _nodes.find(id);
    if(i!=_nodes.end()) {
        if( (*i).second->getXCoordinate()==x &&
            (*i).second->getYCoordinate()==y) {
            return true;
        }
        return false;
    }
    NBNode *node = new NBNode(id, x, y, district);
    _nodes[id] = node;
    return true;
}


bool
NBNodeCont::insert(const string &id, double x, double y)
{
    NodeCont::iterator i = _nodes.find(id);
    if(i!=_nodes.end()) {
        if( (*i).second->getXCoordinate()==x &&
            (*i).second->getYCoordinate()==y) {
            return true;
        }
        return false;
    }
    NBNode *node = new NBNode(id, x, y);
    _nodes[id] = node;
    return true;
}

/*
bool
NBNodeCont::insert(const string &id, double x, double y,
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

pair<double, double>
NBNodeCont::insert(const string &id) // !!! really needed
{
    pair<double, double> ret(-1.0, -1.0);
    NodeCont::iterator i = _nodes.find(id);
    if(i!=_nodes.end()) {
        ret.first = (*i).second->getXCoordinate();
        ret.second = (*i).second->getYCoordinate();
    } else {
        NBNode *node = new NBNode(id, -1.0, -1.0);
        _nodes[id] = node;
    }
    return ret;
}


bool
NBNodeCont::insert(NBNode *node)
{
    string id = node->getID();
    NodeCont::iterator i = _nodes.find(id);
    if(i!=_nodes.end()) {
        if( (*i).second->getXCoordinate()==node->getXCoordinate() &&
            (*i).second->getYCoordinate()==node->getYCoordinate() ) {
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
NBNodeCont::retrieve(double x, double y)
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        NBNode *node = (*i).second;
        if(node->getXCoordinate()==x && node->getYCoordinate()==y)
        return node;
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
}



bool
NBNodeCont::normaliseNodePositions()
{
    // compute the boundery
    Boundery boundery;
    NodeCont::iterator i;
    for(i=_nodes.begin(); i!=_nodes.end(); i++) {
        boundery.add((*i).second->getXCoordinate(),
            (*i).second->getYCoordinate());
    }
    // reformat
    double xmin = boundery.xmin() * -1;
    double ymin = boundery.ymin() * -1;
    for(i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->resetby(xmin, ymin);
    }
    myNetworkOffset = Position2D(xmin, ymin);
    return true;
}


bool
NBNodeCont::reshiftNodePositions(double xoff, double yoff, double rot)
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
NBNodeCont::computeLogics(OptionsCont &oc)
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->computeLogic(oc);
    }
    return true;
}


bool
NBNodeCont::sortNodesEdges()
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->sortNodesEdges();
    }
    return true;
}


void
NBNodeCont::writeXMLNumber(ostream &into)
{
    into << "   <node_count>" << _nodes.size() << "</node_count>" << endl;
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
    MsgHandler::getMessageInstance()->inform(
        string("   ") + toString<int>(getNo()) + string(" nodes loaded."));
}


bool
NBNodeCont::recheckEdges()
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
        for(std::map<NBNode*, EdgeVector>::iterator k=connectionCount.begin(); k!=connectionCount.end(); k++) {
            // join edges
            if((*k).second.size()>1) {
                NBEdgeCont::joinSameNodeConnectingEdges((*k).second);
            }
        }
    }
    return true;
}



bool
NBNodeCont::removeDummyEdges()
{
	size_t no = 0;
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        no += (*i).second->eraseDummies();
    }
	if(no!=0) {
        MsgHandler::getWarningInstance()->inform(
		    toString<int>(no) + string(" dummy edges removed."));
	}
    return true;
}


void
NBNodeCont::searchEdgeInNode(string nodeid, string edgeid)
{
    NBNode *n = retrieve(nodeid);
    NBEdge *e = NBEdgeCont::retrieve(edgeid);
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
NBNodeCont::getNetworkOffset()
{
    return myNetworkOffset;
}

bool
NBNodeCont::computeNodeShapes()
{
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        (*i).second->computeNodeShape();
    }
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBNodeCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:

