#include <map>
#include <string>
#include <cassert>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Boundery.h>
#include <utils/common/SErrorHandler.h>
#include <utils/convert/ToString.h>
#include "NIVissimConnection.h"
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include "NIVissimConnection.h"
#include "NIVissimDisturbance.h"
#include "NIVissimNodeDef.h"
#include "NIVissimEdge.h"
#include "NIVissimTL.h"

using namespace std;


NIVissimTL::SignalDictType NIVissimTL::NIVissimTLSignal::myDict;

NIVissimTL::NIVissimTLSignal::NIVissimTLSignal(int lsaid, int id,
                                               const std::string &name,
                                               const IntVector &groupids,
                                               int edgeid,
                                               int laneno,
                                               double position,
                                               const IntVector &vehicleTypes)
    : myLSA(lsaid), myID(id), myName(name), myGroupIDs(groupids),
    myEdgeID(edgeid), myLane(laneno), myPosition(position),
    myVehicleTypes(vehicleTypes)
{
}


NIVissimTL::NIVissimTLSignal::~NIVissimTLSignal()
{
}

bool
NIVissimTL::NIVissimTLSignal::isWithin(const Position2DVector &poly) const
{
    return poly.around(getPosition());
}


Position2D
NIVissimTL::NIVissimTLSignal::getPosition() const
{
    return NIVissimAbstractEdge::dictionary(myEdgeID)->getGeomPosition(myPosition);
}


bool
NIVissimTL::NIVissimTLSignal::dictionary(int lsaid, int id,
        NIVissimTL::NIVissimTLSignal *o)
{
    SignalDictType::iterator i = myDict.find(lsaid);
    if(i==myDict.end()) {
        myDict[lsaid] = SSignalDictType();
        i = myDict.find(lsaid);
    }
    SSignalDictType::iterator j = (*i).second.find(id);
    if(j==(*i).second.end()) {
        myDict[lsaid][id] = o;
        return true;
    }
    return false;
}


NIVissimTL::NIVissimTLSignal*
NIVissimTL::NIVissimTLSignal::dictionary(int lsaid, int id)
{
    SignalDictType::iterator i = myDict.find(lsaid);
    if(i==myDict.end()) {
        return 0;
    }
    SSignalDictType::iterator j = (*i).second.find(id);
    if(j==(*i).second.end()) {
        return 0;
    }
    return (*j).second;
}


void
NIVissimTL::NIVissimTLSignal::clearDict()
{
    for(SignalDictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        for(SSignalDictType::iterator j=(*i).second.begin(); j!=(*i).second.end(); j++) {
            delete (*j).second;
        }
    }
    myDict.clear();
}


NIVissimTL::SSignalDictType
NIVissimTL::NIVissimTLSignal::getSignalsFor(int tlid)
{
    SignalDictType::iterator i = myDict.find(tlid);
    if(i==myDict.end()) {
        return SSignalDictType();
    }
    return (*i).second;
}


void
NIVissimTL::NIVissimTLSignal::addTo(NBNode *node) const
{
    NIVissimConnection *c = NIVissimConnection::dictionary(myEdgeID);
    ConnectionVector assignedConnections;
    if(c==0) {
        // What to do if on an edge? -> close all outgoing connections
        string eid = toString<int>(myEdgeID);
        NBEdge *edge =
            NBEdgeCont::retrieve(eid);
        if(edge==0) {
            edge = NBEdgeCont::retrievePossiblySplitted(eid, myPosition);
        }
        assert(edge!=0);
        // Check whether it is already known, which edges are approached
        //  by which lanes
        EdgeVector conn;
        if(edge->lanesWereAssigned()) {
            conn = edge->getEdgesFromLane(myLane-1);
        } else {
            conn = edge->getConnected();
        }
        //
        for(EdgeVector::const_iterator i=conn.begin(); i!=conn.end(); i++) {
            assignedConnections.push_back(
                Connection(edge, *i)
                );
        }
    } else {
        assignedConnections.push_back(
            Connection(
                NBEdgeCont::retrieve(toString<int>(c->getFromEdgeID()) ),
                NBEdgeCont::retrieve(toString<int>(c->getToEdgeID()) ) )
                );
    }
    // add to the group
    assert(myGroupIDs.size()!=0);
    if(myGroupIDs.size()==1) {
        node->addToSignalGroup(
            toString<int>(*(myGroupIDs.begin())),
            assignedConnections);
    } else {
        node->addToSignalGroup(
            toString<int>(*(myGroupIDs.begin())),
            assignedConnections);
        // !!!
    }
}








NIVissimTL::GroupDictType NIVissimTL::NIVissimTLSignalGroup::myDict;

NIVissimTL::NIVissimTLSignalGroup::NIVissimTLSignalGroup(
        int lsaid, int id,
        const std::string &name,
        bool isGreenBegin, const DoubleVector &times,
        double tredyellow, double tyellow)
    : myLSA(lsaid), myID(id), myName(name), myTimes(times),
    myFirstIsRed(!isGreenBegin), myTRedYellow(tredyellow),
    myTYellow(tyellow)
{
}


NIVissimTL::NIVissimTLSignalGroup::~NIVissimTLSignalGroup()
{
}

/*
void
NIVissimTL::NIVissimTLSignalGroup::setStaticGreen()
{
    myFirstIsRed = false;
}


void
NIVissimTL::NIVissimTLSignalGroup::setStaticRed()
{
    myFirstIsRed = true;
}


void
NIVissimTL::NIVissimTLSignalGroup::addTimes(const DoubleVector &times)
{
    myTimes = times;
}


void
NIVissimTL::NIVissimTLSignalGroup::addYellowTimes(double tredyellow,
                                                  double tyellow)
{
    myTRedYellow = tredyellow;
    myTYellow = tyellow;
}

*/

bool
NIVissimTL::NIVissimTLSignalGroup::dictionary(int lsaid, int id,
        NIVissimTL::NIVissimTLSignalGroup *o)
{
    GroupDictType::iterator i = myDict.find(lsaid);
    if(i==myDict.end()) {
        myDict[lsaid] = SGroupDictType();
        i = myDict.find(lsaid);
    }
    SGroupDictType::iterator j = (*i).second.find(id);
    if(j==(*i).second.end()) {
        myDict[lsaid][id] = o;
        return true;
    }
    return false;
/*
    GroupDictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
    */
}


NIVissimTL::NIVissimTLSignalGroup*
NIVissimTL::NIVissimTLSignalGroup::dictionary(int lsaid, int id)
{
    GroupDictType::iterator i = myDict.find(lsaid);
    if(i==myDict.end()) {
        return 0;
    }
    SGroupDictType::iterator j = (*i).second.find(id);
    if(j==(*i).second.end()) {
        return 0;
    }
    return (*j).second;
}

void
NIVissimTL::NIVissimTLSignalGroup::clearDict()
{
    for(GroupDictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        for(SGroupDictType::iterator j=(*i).second.begin(); j!=(*i).second.end(); j++) {
            delete (*j).second;
        }
    }
    myDict.clear();
}


NIVissimTL::SGroupDictType
NIVissimTL::NIVissimTLSignalGroup::getGroupsFor(int tlid)
{
    GroupDictType::iterator i = myDict.find(tlid);
    if(i==myDict.end()) {
        return SGroupDictType();
    }
    return (*i).second;
}


void
NIVissimTL::NIVissimTLSignalGroup::addTo(NBNode *node) const
{
    // get the color at the begin
    NBNode::TLColor color = myFirstIsRed
        ? NBNode::TLCOLOR_RED : NBNode::TLCOLOR_GREEN;
    string id = toString<int>(myID);
    node->addSignalGroup(id);
    for(DoubleVector::const_iterator i=myTimes.begin(); i!=myTimes.end(); i++) {
        node->addSignalGroupPhaseBegin(id, *i, color);
        color = color==NBNode::TLCOLOR_RED
            ? NBNode::TLCOLOR_GREEN : NBNode::TLCOLOR_RED;
    }
    node->setSignalYellowTimes(id, myTRedYellow, myTYellow);
}








NIVissimTL::DictType NIVissimTL::myDict;

NIVissimTL::NIVissimTL(int id, const std::string &type,
                       const std::string &name, double absdur,
                       double offset)
    : myID(id), myName(name), myAbsDuration(absdur), myOffset(offset),
    myCurrentGroup(0), myNodeID(-1), myType(type)

{
}


NIVissimTL::~NIVissimTL()
{
}





bool
NIVissimTL::dictionary(int id, const std::string &type,
                       const std::string &name, double absdur,
                       double offset)
{
    NIVissimTL *o = new NIVissimTL(id, type, name, absdur, offset);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}

bool
NIVissimTL::dictionary(int id, NIVissimTL *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimTL *
NIVissimTL::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}

/*
void
NIVissimTL::assignNodes()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i).second->assignNode();
    }
}


void
NIVissimTL::assignNode()
{
    // collect participating edge names
    NIVissimExtendedEdgePointVector edges;
    for(SignalVector::iterator i=mySignals.begin(); i!=mySignals.end(); i++) {
        edges.push_back(new NIVissimExtendedEdgePoint(
            (*i)->myEdgeID,
            IntVector(), (*i)->myPosition, IntVector()));
    }
    // try to set the according node
    myNodeID = NIVissimNodeDef::searchAndSetMatchingTLParent(myID, edges);
    if(myNodeID<0) {
        SErrorHandler::add(
            string("No node matching edges of tl '") + toString<int>(myID)
            + string("' could be found."));
    }
}

*/


IntVector
NIVissimTL::getWithin(const AbstractPoly &poly, double offset)
{
    IntVector ret;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        if((*i).second->crosses(poly, offset)) {
            ret.push_back((*i).second->myID);
        }
    }
    return ret;
}


void
NIVissimTL::computeBounding()
{
    Boundery *bound = new Boundery();
    SSignalDictType signals = NIVissimTLSignal::getSignalsFor(myID);
    for(SSignalDictType::const_iterator j=signals.begin(); j!=signals.end(); j++) {
        bound->add((*j).second->getPosition());
    }
    myBoundery = bound;
    cout << "TL " << myID << ":" << *myBoundery << endl;
}


void
NIVissimTL::buildNodeClusters()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i).second->buildNodeCluster();
    }
}



int
NIVissimTL::buildNodeCluster()
{
    // the traffic light may already be assigned to a node
    if(myNodeID>=0) {
        return myNodeID;
    }
    int id = -1;
//    while(changed) {
        IntVector connectors = NIVissimConnection::getWithin(*myBoundery);
        IntVector disturbances = NIVissimDisturbance::getWithin(*myBoundery);
        myNodeID = NIVissimNodeCluster::dictionary(myID, myID, connectors,
            disturbances);
        IntVector::iterator i;
        for(i=connectors.begin(); i!=connectors.end(); i++) {
            NIVissimConnection::dictionary(*i)->inCluster(myNodeID);
        }
        for(i=disturbances.begin(); i!=disturbances.end(); i++) {
            NIVissimDisturbance::dictionary(*i)->inCluster(myNodeID);
        }
//    }
    return myNodeID;
}


void
NIVissimTL::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}





bool
NIVissimTL::dict_SetSignals()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimTL *tl = (*i).second;
        if(tl->myNodeID<0) {
            continue;
        }
        NBNode *node = NBNodeCont::retrieve(toString<int>(tl->myNodeID));
        node->setType(NBNode::TYPE_TRAFFIC_LIGHT);
        // add each group to the node's container
        SGroupDictType sgs = NIVissimTLSignalGroup::getGroupsFor(tl->getID());
        for(SGroupDictType::const_iterator j=sgs.begin(); j!=sgs.end(); j++) {
            (*j).second->addTo(node);
        }
        // add the signal group signals to the node
        SSignalDictType signals = NIVissimTLSignal::getSignalsFor(tl->getID());
        for(SSignalDictType::const_iterator k=signals.begin(); k!=signals.end(); k++) {
            (*k).second->addTo(node);
        }
    }
    return true;
}


std::string
NIVissimTL::getType() const
{
    return myType;
}


int
NIVissimTL::getID() const
{
    return myID;
}


void
NIVissimTL::setNodeID(int id)
{
    myNodeID = id;
}


