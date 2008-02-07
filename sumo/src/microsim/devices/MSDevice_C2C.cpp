/****************************************************************************/
/// @file    MSDevice_C2C.cpp
/// @author  Michael Behrisch, Daniel Krajzewicz
/// @date    Tue, 04 Dec 2007
/// @version $Id$
///
// C2C communication and rerouting device
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDevice_C2C.h"
#include <microsim/MSNet.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSLane.h>
#include <microsim/MSRoute.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/SUMODijkstraRouter.h>
#include "MSCells.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;



// ===========================================================================
// static member variables
// ===========================================================================
int MSDevice_C2C::myVehicleIndex = 0;
MSCells* MSDevice_C2C::myCells = 0;

// ===========================================================================
// method definitions
// ===========================================================================
void
MSDevice_C2C::insertOptions() throw()
{
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addOptionSubTopic("C2C");

    oc.doRegister("device.c2x.probability", new Option_Float(0.));//!!! describe
    oc.addDescription("device.c2x.probability", "C2C", "The probability for a vehicle to have c2c");

    oc.doRegister("device.c2x.knownveh", new Option_String());//!!! describe
    oc.addDescription("device.c2x.knownveh", "C2C", "Assign a device to named vehicles");

    oc.doRegister("device.c2x.deterministic", new Option_Bool(false)); //!!! describe
    oc.addDescription("device.c2x.deterministic", "C2C", "The devices are set deterministoc using a fraction of 1000");

    oc.doRegister("device.c2x.range", new Option_Float(100.));//!!! describe
    oc.addDescription("device.c2x.range", "C2C", "The range of the c2c device");

    oc.doRegister("device.c2x.keep-duration", new Option_Integer(30 * 60));//!!! describe
    oc.addDescription("device.c2x.keep-duration", "C2C", "Duration of keeping messages");

    oc.doRegister("device.c2x.insert-info-factor", new Option_Float((SUMOReal) 1.2));//!!! describe
    oc.addDescription("device.c2x.insert-info-factor", "C2C", "Factor for adding messages");

    oc.doRegister("c2x.cluster-info", new Option_FileName());//!!! describe
    oc.addDescription("c2x.cluster-info", "C2C", "Save cluster information into FILE");

    oc.doRegister("c2x.edge-near-info", new Option_FileName());//!!! describe
    oc.addDescription("c2x.edge-near-info", "C2C", "Save 'connected' edges into FILE");

    oc.doRegister("c2x.saved-info", new Option_FileName());//!!! describe
    oc.addDescription("c2x.saved-info", "C2C", "");

    oc.doRegister("c2x.saved-info-freq", new Option_FileName());//!!! describe
    oc.addDescription("c2x.saved-info-freq", "C2C", "");

    oc.doRegister("c2x.transmitted-info", new Option_FileName());//!!! describe
    oc.addDescription("c2x.transmitted-info", "C2C", "Save transmitted information into FILE");

    oc.doRegister("c2x.vehicle-in-range", new Option_FileName());//!!! describe
    oc.addDescription("c2x.vehicle-in-range", "C2C", "Save names of connected vehicles into FILE");

    myVehicleIndex = 0;
}


void
MSDevice_C2C::buildVehicleDevices(MSVehicle &v, std::vector<MSDevice*> &into) throw()
{
    OptionsCont &oc = OptionsCont::getOptions();
    if (oc.getFloat("device.c2x.probability")==0&&!oc.isSet("device.c2x.knownveh")) {
        // no c2c communication is modelled
        return;
    }
    // c2c communication
    bool haveByNumber = false;
    if (oc.getBool("device.c2x.deterministic")) {
        haveByNumber = ((myVehicleIndex%1000) < (int)(oc.getFloat("device.c2x.probability")*1000.));
    } else {
        haveByNumber = RandHelper::rand()<=oc.getFloat("device.c2x.probability");
    }
    bool haveByName = oc.isSet("device.c2x.knownveh") && OptionsCont::getOptions().isInStringVector("device.c2x.knownveh", v.getID());
    if (haveByNumber||haveByName) {
        MSDevice_C2C* device = new MSDevice_C2C(v);
        into.push_back(device);
        if (myCells == 0) {
            myCells = new MSCells(MSGlobals::gLANRange);
        }
        myCells->add(device);
    }
    myVehicleIndex++;
}


// Compute Car2Car-Communication
void
MSDevice_C2C::computeCar2Car(SUMOTime t)
{
    std::vector<MSDevice_C2C*> connected;
    std::vector<MSDevice_C2C*> clusterHeaders;
    myCells->update();

    for (MSCells::CellsIterator cell = myCells->begin(); cell!=myCells->end(); ++cell) {
        for (vector<MSDevice_C2C*>::const_iterator device = (*cell)->begin(); device!=(*cell)->end(); ++device) {
            (*device)->updateInfos(t);
            (*device)->addNeighbors(*cell, t);
            (*device)->addNeighbors(myCells->getNeighbor(cell, 0, 1), t);
            (*device)->addNeighbors(myCells->getNeighbor(cell, 1, 0), t);
            (*device)->addNeighbors(myCells->getNeighbor(cell, 1, 1), t);
            (*device)->addNeighbors(myCells->getNeighbor(cell, -1, 1), t);
            (*device)->cleanUpConnections(t);
            (*device)->setClusterId(-1);
            if ((*device)->getConnections().size()!=0) {
                connected.push_back(*device);
            }
        }
    }
    // build the clusters
    int clusterId = 1;
    for (vector<MSDevice_C2C*>::const_iterator device=connected.begin(); device!=connected.end(); ++device) {
        if ((*device)->getClusterId()<0) {
            clusterHeaders.push_back(*device);
            (*device)->buildMyCluster(t, clusterId);
            clusterId++;
        }
    }
    // send information
    std::vector<MSVehicle*>::iterator q;
    for (vector<MSDevice_C2C*>::const_iterator device=clusterHeaders.begin(); device!=clusterHeaders.end(); ++device) {
        (*device)->sendInfos(t);
    }
    // Rerouting?
    for (vector<MSDevice_C2C*>::const_iterator device=connected.begin(); device!=connected.end(); ++device) {
        (*device)->checkReroute(t);
    }
}


MSDevice_C2C::MSDevice_C2C(MSVehicle &holder) throw()
        : MSDevice(holder), akt(0)
{}


MSDevice_C2C::~MSDevice_C2C() throw()
{
    myCells->remove(this);
    delete akt;
    for (ConnectionCont::iterator i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
        delete(*i).second;
    }
    myNeighbors.clear();
    for (ClusterCont::iterator i=clusterCont.begin(); i!=clusterCont.end(); ++i) {
        delete(*i);
    }
    clusterCont.clear();
    for (InfoCont::iterator i=infoCont.begin(); i!=infoCont.end(); ++i) {
        delete(*i).second;
    }
    infoCont.clear();
}


std::string
MSDevice_C2C::buildID()
{
    return string("c2c_") + getHolder().getID();
}

void
MSDevice_C2C::enterLaneAtMove(MSLane* enteredLane, SUMOReal driven, bool inBetweenJump)
{
    delete akt;
    akt = new Information(0, MSNet::getInstance()->getCurrentTimeStep());
}


void
MSDevice_C2C::enterLaneAtEmit(MSLane* enteredLane, const MSVehicle::State &)
{
    delete akt;
    akt = new Information(0, MSNet::getInstance()->getCurrentTimeStep());
}


void
MSDevice_C2C::leaveLaneAtMove(SUMOReal /*driven*/)
{
    // checke whether the vehicle needed longer than expected
    float factor = getHolder().getEdge()->getEffort(&getHolder(), MSNet::getInstance()->getCurrentTimeStep());
    float nt = (float)(MSNet::getInstance()->getCurrentTimeStep() - akt->time);
    if (nt>10&&nt>factor*MSGlobals::gAddInfoFactor) { // !!! explicite
        // if so, check whether an information about the edge was already existing
        std::map<const MSEdge * const, Information *>::iterator i = infoCont.find(getHolder().getEdge());
        if (i==infoCont.end()) {
            // no, add the new information
            Information *info = new Information(*akt);
            info->neededTime = nt;
            infoCont[getHolder().getEdge()] = info;
            i = infoCont.find(getHolder().getEdge());
        } else {
            // yes, update the existing information
            (*i).second->neededTime = nt;
        }
        // save the information
        MSCORN::saveSavedInformationData(MSNet::getInstance()->getCurrentTimeStep(),
                                         getID(),getHolder().getEdge()->getID(),"congestion",(*i).second->time,nt);
        totalNrOfSavedInfos++;
    } else if (infoCont.find(getHolder().getEdge())!=infoCont.end()) {
        // ok, we could pass the edge faster than assumed; remove the information
        infoCont.erase(getHolder().getEdge());
    }
    delete akt;
    akt = 0;
}


void
MSDevice_C2C::onTripEnd()
{
    for (ConnectionCont::iterator i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
        delete(*i).second;
    }
    myNeighbors.clear();
    for (ClusterCont::iterator i=clusterCont.begin(); i!=clusterCont.end(); ++i) {
        delete(*i);
    }
    clusterCont.clear();
    for (InfoCont::iterator i=infoCont.begin(); i!=infoCont.end(); ++i) {
        delete(*i).second;
    }
    infoCont.clear();
}


void
MSDevice_C2C::addNeighbors(vector<MSDevice_C2C*>* devices, SUMOTime time)
{
    if (devices == 0 || &getHolder().getLane()==0) {
        // vehicle is being teleported
        return;
    }

    for (vector<MSDevice_C2C*>::const_iterator other = devices->begin(); other != devices->end(); ++other) {
        // check whether the other vehicle is in range
        if (&(*other)->getHolder().getLane()==0 && isInDistance(&getHolder(), &(*other)->getHolder())) {
            Position2D pos1 = getHolder().getPosition();
            Position2D pos2 = (*other)->getHolder().getPosition();
            if (pos1.x() < pos2.x()) {
                std::map<MSDevice_C2C*, C2CConnection*>::iterator i = myNeighbors.find(*other);
                if (i==myNeighbors.end()) {
                    // the vehicles will establish a new connection
                    myNeighbors[*other] = new C2CConnection(*other, time);
                    (*other)->myNeighbors[this] = new C2CConnection(this, time);
                    // the cara must inform each other if removed from the network
                    (*other)->getHolder().quitRemindedEntered(this);
                    getHolder().quitRemindedEntered(*other);
                } else {
                    // ok, the vehicles already interact
                    //  increment the connection time
                    (*i).second->lastTimeSeen = time;
                    (*other)->myNeighbors[this]->lastTimeSeen = time;
                }
                MSCORN::saveVehicleInRangeData(time, getID(), (*other)->getID(),
                                               pos1.x(),pos1.y(), pos2.x(),pos2.y());
            }
        }
    }
}


void
MSDevice_C2C::cleanUpConnections(SUMOTime time)
{
    std::vector<MSDevice_C2C *> toErase;
    std::map<MSDevice_C2C*, C2CConnection*>::iterator i;
    // recheck connections
    for (i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
        MSDevice_C2C *neigh = (*i).first;
        if ((*i).second->lastTimeSeen != time) {
            // the vehicle is not longer in range
            toErase.push_back(neigh);
            // the other vehicle must no longer inform us about being removed from the network
            neigh->getHolder().quitRemindedLeft(this);
        }
    }

    // go through the list of invalid connections, erase them
    for (vector<MSDevice_C2C*>::iterator j=toErase.begin(); j!=toErase.end(); ++j) {
        i = myNeighbors.find(*j);
        delete(*i).second;
        myNeighbors.erase(i);
    }
}


bool
MSDevice_C2C::isInDistance(MSVehicle* veh1, MSVehicle* veh2)
{
    Position2D pos1 = veh1->getPosition();
    Position2D pos2 = veh2->getPosition();
    if (pos1.x()==-1000||pos2.x()==-1000) {
        return false;
    }
    SUMOReal distance = sqrt((pos1.x()-pos2.x())*(pos1.x()-pos2.x()) + (pos1.y()-pos2.y())*(pos1.y()-pos2.y()));
    return (distance>0)&&(distance<=MSGlobals::gLANRange);
}


void
MSDevice_C2C::updateInfos(SUMOTime time)
{
    // first, count how long the vehicle is waiting at the same position
    if (getHolder().getSpeed()<1.) {
        timeSinceStop++;
    } else {
        timeSinceStop = 0;
    }

    // second, save the information as "Congestion", if the vehicle is at the same
    // position longer as 2 minutes
    if (timeSinceStop > 120) {
        SUMOReal timeByMeanSpeed1 = (SUMOReal) timeSinceStop;
        if (akt!=0&&&getHolder().getLane()!=0) {
            SUMOReal neededTime = (SUMOReal)(MSNet::getInstance()->getCurrentTimeStep() - akt->time);
            timeByMeanSpeed1 = getHolder().getLane().length() / (getHolder().getPositionOnLane()/neededTime);
        }
        SUMOReal estimatedTime = timeByMeanSpeed1;
        map<const MSEdge * const, Information *>::iterator i = infoCont.find(getHolder().getEdge());
        if (i == infoCont.end()) {
            // this is a new information about an edge where speed is too low
            Information *info = new Information(estimatedTime, time);
            infoCont[getHolder().getEdge()] = info;
            MSCORN::saveSavedInformationData(time, getID(), getHolder().getEdge()->getID(), "congestion", info->time, 0);
            totalNrOfSavedInfos++;
        } else {
            // this edge is already known as being too slow
            // - replace prior information by own
            (*i).second->neededTime = estimatedTime;
            (*i).second->time = time;
        }
    }

    // remove information older than wished
    std::vector<const MSEdge * > toErase;
    std::map<const MSEdge * const, Information*>::iterator j = infoCont.begin();
    for (; j!= infoCont.end(); ++j) {
        if ((*j).second->time < time - MSGlobals::gLANRefuseOldInfosOffset) {
            toErase.push_back((*j).first);
        }
    }
    // go through the list of invalid information, erase them
    for (vector<const MSEdge *>::iterator k=toErase.begin(); k!=toErase.end(); ++k) {
        infoCont.erase(infoCont.find(*k));
    }
}

void
MSDevice_C2C::removeOnTripEnd(MSVehicle *veh) throw()
{
    MSDevice_C2C *dev = myVehiclesToDevicesMap[veh];
    assert(myNeighbors.find(dev)!=myNeighbors.end());
    std::map<MSDevice_C2C *, C2CConnection*>::iterator i = myNeighbors.find(dev);
    delete(*i).second;
    myNeighbors.erase(i);
}


bool
MSDevice_C2C::knowsEdgeTest(MSEdge &edge) const
{
    return infoCont.find(&edge)!=infoCont.end();
}


const MSDevice_C2C::ConnectionCont &
MSDevice_C2C::getConnections() const
{
    return myNeighbors;
}


void
MSDevice_C2C::setClusterId(int Id)
{
    clusterId = Id;
}


int
MSDevice_C2C::getClusterId(void) const
{
    return clusterId;
}


int
MSDevice_C2C::buildMyCluster(int t, int clId)
{
    int count = 1;
    // build the cluster
    {
        clusterId = clId;
        std::map<MSDevice_C2C *, C2CConnection*>::iterator i;
        for (i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
            if ((*i).first->getClusterId()<0) {
                count++;
                (*i).second->connectedVeh->setClusterId(clId);
                clusterCont.push_back((*i).second);
                std::map<MSDevice_C2C *, C2CConnection*>::iterator j;
                for (j=(*i).first->myNeighbors.begin(); j!=(*i).second->connectedVeh->myNeighbors.end(); j++) {
                    if ((*i).first->getClusterId()<0) {
                        count++;
                        (*i).first->setClusterId(clId);
                        clusterCont.push_back((*j).second);
                    }
                }
            } else if ((*i).second->connectedVeh->getClusterId()==clusterId) {
                // du bist zwar mein Nachbarn, aber du wrdest von einem anderen Nachbarn von mir schon eingeladen,
                // dann werde ich deine nachbarn einladen.
                std::map<MSDevice_C2C *, C2CConnection*>::iterator j;
                for (j=(*i).first->myNeighbors.begin(); j!=(*i).second->connectedVeh->myNeighbors.end(); j++) {
                    if ((*i).first->getClusterId()<0) {
                        count++;
                        (*i).first->setClusterId(clId);
                        clusterCont.push_back((*j).second);
                    }
                }
            }
        }
    }

    // write output
    {
        ostringstream vehs;
        for (std::vector<C2CConnection*>::const_iterator i=clusterCont.begin(); i!=clusterCont.end(); ++i) {
            if (i!=clusterCont.begin()) {
                vehs << ' ';
            }
            vehs << (*i)->connectedVeh->getID();
        }
        MSCORN::saveClusterInfoData(t, clId, getID(), vehs.str(), count);
    }
    return count;
}


void
MSDevice_C2C::sendInfos(SUMOTime time)
{
    // the number of possible packets
    size_t numberOfSendingPos = (size_t) MSGlobals::gNumberOfSendingPos; // 732
    // the number of information per packet
    size_t infoPerPaket = (size_t) MSGlobals::gInfoPerPaket; // 14
    size_t numberOfInfo = numberOfSendingPos*infoPerPaket; // 10248

    if (infoCont.size()>0 && numberOfSendingPos>0) {
        // send information to direct neighbors
        for (ConnectionCont::const_iterator i=myNeighbors.begin(); i!=myNeighbors.end(); ++i) {
            // compute the number of netto-sendable packets
            // !!! maybe it would be prettier to have computed this by the retriever
            size_t nofP = numOfInfos(this, (*i).first);
            if (nofP>numberOfInfo) {
                nofP = (int) numberOfInfo;
            }
            // send the computed number of information to the neighbor
            (*i).first->transferInformation(getID(), infoCont, nofP, time);
        }
        // reduce the number of packets that still may be sent
        size_t sentBruttoP = MIN2((size_t) ceil((SUMOReal)(infoCont.size())/(SUMOReal) infoPerPaket), (size_t) numberOfSendingPos);
        myNoSent += sentBruttoP;
        numberOfSendingPos = numberOfSendingPos - sentBruttoP;
        numberOfInfo = numberOfInfo - infoCont.size();
    }

    if (numberOfInfo<=0) {
        return;
    }

    // now let all neighbors send
    for (ClusterCont::const_iterator o=clusterCont.begin(); o!=clusterCont.end()&&numberOfInfo>0&&numberOfSendingPos>0; ++o) {
        if ((*o)->connectedVeh->infoCont.size()>0 && numberOfSendingPos>0) {
            for (ConnectionCont::const_iterator j=(*o)->connectedVeh->myNeighbors.begin(); j!=(*o)->connectedVeh->myNeighbors.end(); ++j) {
                // compute the number of netto-sendable packets
                // !!! maybe it would be prettier to have computed this by the retriever
                size_t nofP = numOfInfos((*j).second->connectedVeh,(*o)->connectedVeh);
                if (nofP>numberOfInfo) {
                    nofP = (int) numberOfInfo;
                }
                // send the computed number of information to the neighbor
                (*j).second->connectedVeh->transferInformation((*o)->connectedVeh->getID(),(*o)->connectedVeh->infoCont, nofP, time);
            }
            size_t sentBruttoP = MIN2((size_t) ceil((SUMOReal)((*o)->connectedVeh->infoCont.size())/(SUMOReal) infoPerPaket), (size_t) numberOfSendingPos);
            numberOfSendingPos = numberOfSendingPos - sentBruttoP;
            numberOfInfo = numberOfInfo - (*o)->connectedVeh->infoCont.size();
        }
    }
    clusterCont.clear();
}


size_t
MSDevice_C2C::numOfInfos(MSDevice_C2C *veh1, MSDevice_C2C* veh2)
{
    Position2D pos1 = veh1->getHolder().getPosition();
    Position2D pos2 = veh2->getHolder().getPosition();
    SUMOReal distance = sqrt(pow(pos1.x()-pos2.x(),2) + pow(pos1.y()-pos2.y(),2));
    SUMOReal x = (SUMOReal)(((-2.3*distance + 1650.)*MSGlobals::gNumberOfSendingPos)/1500.);  //approximation function
    return (int)(x*MSGlobals::gInfoPerPaket);
}


void
MSDevice_C2C::transferInformation(const std::string &senderID, const InfoCont &infos,
                                  int NofP, SUMOTime currentTime)
{
    if (NofP>0&&infos.size()>0) {
        myLastInfoTime = currentTime;
    }
    int count = 0;
    std::map<const MSEdge * const, Information *>::const_iterator i;
    // go through the saved information
    for (i=infos.begin(); i!=infos.end() && count<NofP; ++i) {
        std::map<const MSEdge * const, Information *>::iterator j = infoCont.find((*i).first);
        if (j==infoCont.end()) {
            if ((*i).second->neededTime > 0) {
                infoCont[(*i).first] = new Information(*(*i).second);
                ++myNoGot;
            }
        } else if (((*i).second->time > (*j).second->time) && (*i).second->neededTime > 0) {
            // save the information about a previously known edge
            //  (it is newer than the stored)
            delete infoCont[(*i).first];
            infoCont[(*i).first] = new Information(*(*i).second);
            ++myNoGot;
        }
        count++;
        MSCORN::saveTransmittedInformationData(MSNet::getInstance()->getCurrentTimeStep(),senderID,getID(),(*i).first->getID(),(*i).second->time,(*i).second->neededTime);
        // if the edge is on the route, mark that a relevant information has been added
        bool bWillPass = getHolder().willPass((*i).first);
        if (bWillPass) {
            myHaveRouteInfo = true;
            ++myNoGotRelevant;
        }
    }
}


SUMOReal
MSDevice_C2C::getEffort(const MSEdge * const e, SUMOTime /*t*/) const
{
    if (infoCont.find(e)==infoCont.end()) {
        return -1;
    }
    return infoCont.find(e)->second->neededTime;
}

void
MSDevice_C2C::checkReroute(SUMOTime t)
{
    // do not try to reroute when no new information is available
    if (myLastInfoTime!=t) {
        return;
    }
    // do not try to reroute when no information about the own route is available
    if (!myHaveRouteInfo) {
        return;
    }
    // try to reroute
    if (!myHolder.hasStops()) {
        myHaveRouteInfo = false;
        myHolder.reroute(t);
    }
}


size_t
MSDevice_C2C::getNoGot() const
{
    return myNoGot;
}


size_t
MSDevice_C2C::getNoSent() const
{
    return myNoSent;
}


size_t
MSDevice_C2C::getNoGotRelevant() const
{
    return myNoGotRelevant;
}


/****************************************************************************/

