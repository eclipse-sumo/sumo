/****************************************************************************/
/// @file    MSDevice_C2C.cpp
/// @author  Michael Behrisch, Daniel Krajzewicz
/// @date    Tue, 04 Dec 2007
/// @version $Id$
///
// C2C communication and rerouting device
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <algorithm>
#include "MSDevice_C2C.h"
#include <microsim/MSNet.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSLane.h>
#include <microsim/MSRoute.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/DijkstraRouterTT.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSGlobals.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include <microsim/MSEdgeWeightsStorage.h>

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
std::vector<MSDevice_C2C::Cell*> MSDevice_C2C::myCells;
unsigned int MSDevice_C2C::myLookupXSize, MSDevice_C2C::myLookupYSize;
SUMOReal MSDevice_C2C::myXCellSize, MSDevice_C2C::myYCellSize;
std::vector<MSDevice_C2C*> MSDevice_C2C::myConnected;
std::vector<MSDevice_C2C*> MSDevice_C2C::myClusterHeaders;
std::vector<MSEdge*> MSDevice_C2C::myAllEdges;
std::map<const MSEdge*, std::vector<size_t> > MSDevice_C2C::myEdgeCells;
std::map<const MSEdge*, std::vector<const MSEdge*> > MSDevice_C2C::myEdgeNeighbors;
std::map<const MSEdge*, std::vector<MSDevice_C2C*> > MSDevice_C2C::myEdgeVehicles;
StaticCommand< MSDevice_C2C > *MSDevice_C2C::myC2CComputationCommand;
std::map<const MSVehicle*, MSDevice_C2C*> MSDevice_C2C::myVehiclesToDevicesMap;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods for look-up computation
// ---------------------------------------------------------------------------
void
MSDevice_C2C::buildLookUpInformation() {
    Boundary boundary = GeoConvHelper::getConvBoundary();
    myLookupXSize = (unsigned)((boundary.xmax()-boundary.xmin())/MSGlobals::gLANRange) + 1;
    myLookupYSize = (unsigned)((boundary.ymax()-boundary.ymin())/MSGlobals::gLANRange) + 1;
    // allocate grid
    size_t size = myLookupXSize*myLookupYSize;
    // get the boundary
    if (boundary.getHeight()==0||boundary.getWidth()==0) {
        boundary.add(boundary.xmin()+1, boundary.ymax()+1);
        boundary.add(boundary.xmin()-1, boundary.ymax()-1);
    }
    // compute the cell size
    myXCellSize = MSGlobals::gLANRange;
    myYCellSize = MSGlobals::gLANRange;
    for (size_t i=0; i<size; i++) {
        Cell *cell = new Cell();
        cell->index = i;
        cell->xcellsize = myXCellSize;
        cell->ycellsize = myYCellSize;
        myCells.push_back(cell);
    }
    // divide edges on grid
    size_t index = 0;
    for (vector<MSEdge*>::iterator i=myAllEdges.begin(); i!=myAllEdges.end(); ++i, ++index) {
        computeEdgeCells(*i);
    }
    // set neighborhood
    size_t i = 0;
    for (vector<Cell*>::const_iterator ci=myCells.begin(); ci!=myCells.end(); ++i, ++ci) {
        vector<Cell*> neighbors = getNeighbors(i);
        for (std::vector<Cell*>::iterator j=neighbors.begin(); j!=neighbors.end(); ++j) {
            Cell *neighbor = *j;
            for (std::vector<const MSEdge*>::iterator k=neighbor->ownEdges.begin(); k!=neighbor->ownEdges.end(); ++k) {
                if (find((*ci)->neighborEdges.begin(), (*ci)->neighborEdges.end(), *k)==(*ci)->neighborEdges.end()) {
                    (*ci)->neighborEdges.push_back(*k);
                }
            }
        }
    }
    for (vector<MSEdge*>::iterator i=myAllEdges.begin(); i!=myAllEdges.end(); ++i, ++index) {
        vector<size_t> &edgeCells = myEdgeCells[*i];
        vector<const MSEdge*> edgeNeighbors;
        for (vector<size_t>::iterator j=edgeCells.begin(); j!=edgeCells.end(); ++j) {
            vector<Cell*> neighbors = getNeighbors(*j);
            for (vector<Cell*>::iterator k=neighbors.begin(); k!=neighbors.end(); ++k) {
                const vector<const MSEdge*> &ownEdges = (*k)->ownEdges;
                for (vector<const MSEdge*>::const_iterator l=ownEdges.begin(); l!=ownEdges.end(); ++l) {
                    if (find(edgeNeighbors.begin(), edgeNeighbors.end(), *l)==edgeNeighbors.end()) {
                        edgeNeighbors.push_back(*l);
                    }
                }
            }
        }
        myEdgeNeighbors[*i] = edgeNeighbors;
    }
    if (OutputDevice::createDeviceByOption("c2x.edge-near-info", "edge-neighbors")) {
        writeNearEdges(OutputDevice::getDeviceByOption("c2x.edge-near-info"));
    }
}


void
MSDevice_C2C::computeEdgeCells(const MSEdge *edge) {
    myEdgeCells[edge] = vector<size_t>();
    const std::vector<MSLane*> &lanes = edge->getLanes();
    for (size_t i=0; i<lanes.size(); i++) {
        MSLane *lane = lanes[i];
        computeLaneCells(lane->getShape(), edge);
    }
}


void
MSDevice_C2C::computeLaneCells(const Position2DVector &lane, const MSEdge *edge) {
    // compute the outer and inner positions of the edge
    //  (meaning the real edge position and the position yielding from
    //  adding the offset of lanes)
    const Position2D &beg = lane.getBegin();
    const Position2D &end = lane.getEnd();
    SUMOReal length = beg.distanceTo(end);
    std::pair<SUMOReal, SUMOReal> offsets(0, 0);
    if (length!=0) {
        offsets = GeomHelper::getNormal90D_CW(beg, end, length, 3.5 / 2.0);
    }
    SUMOReal x11 = beg.x() - offsets.first;
    SUMOReal y11 = beg.y() + offsets.second;
    SUMOReal x12 = end.x() - offsets.first;
    SUMOReal y12 = end.y() + offsets.second;

    SUMOReal x21 = beg.x() + offsets.first;
    SUMOReal y21 = beg.y() - offsets.second;
    SUMOReal x22 = end.x() + offsets.first;
    SUMOReal y22 = end.y() - offsets.second;

    Boundary bb1;
    for (unsigned i=0; i<lane.size(); i++) {
        bb1.add(lane[i]);
    }
    // compute the cells the lane is going through
    for (int y=(int)(bb1.ymin()/myYCellSize); y<(int)((bb1.ymax()/myYCellSize)+1)&&y<(int) myLookupYSize; y++) {
        SUMOReal ypos1 = SUMOReal(y) * myYCellSize;
        for (int x=(int)(bb1.xmin()/myXCellSize); x<(int)((bb1.xmax()/myXCellSize)+1)&&x<(int) myLookupXSize; x++) {
            SUMOReal xpos1 = SUMOReal(x) * myXCellSize;

            size_t offset = myLookupXSize * y + x;
            if (
                GeomHelper::intersects(x11, y11, x12, y12,
                                       xpos1, ypos1, xpos1+myXCellSize, ypos1) ||
                GeomHelper::intersects(x11, y11, x12, y12,
                                       xpos1, ypos1, xpos1, ypos1+myYCellSize) ||
                GeomHelper::intersects(x11, y11, x12, y12,
                                       xpos1, ypos1+myYCellSize, xpos1+myXCellSize,
                                       ypos1+myYCellSize) ||
                GeomHelper::intersects(x11, y11, x12, y12,
                                       xpos1+myXCellSize, ypos1, xpos1+myXCellSize,
                                       ypos1+myYCellSize) ||

                GeomHelper::intersects(x21, y21, x22, y22,
                                       xpos1, ypos1, xpos1+myXCellSize, ypos1) ||
                GeomHelper::intersects(x21, y21, x22, y22,
                                       xpos1, ypos1, xpos1, ypos1+myYCellSize) ||
                GeomHelper::intersects(x21, y21, x22, y22,
                                       xpos1, ypos1+myYCellSize, xpos1+myXCellSize,
                                       ypos1+myYCellSize) ||
                GeomHelper::intersects(x21, y21, x22, y22,
                                       xpos1+myXCellSize, ypos1, xpos1+myXCellSize,
                                       ypos1+myYCellSize) ||

                (x11>=xpos1&&x11<xpos1+myXCellSize&&y11>=ypos1&&y11<ypos1+myYCellSize) ||
                (x12>=xpos1&&x12<xpos1+myXCellSize&&y12>=ypos1&&y12<ypos1+myYCellSize) ||
                (x21>=xpos1&&x21<xpos1+myXCellSize&&y21>=ypos1&&y21<ypos1+myYCellSize) ||
                (x22>=xpos1&&x22<xpos1+myXCellSize&&y22>=ypos1&&y22<ypos1+myYCellSize)
            ) {
                myCells[offset]->ownEdges.push_back(edge);
                myEdgeCells[edge].push_back(offset);
            }
        }
    }
}


std::vector<MSDevice_C2C::Cell*>
MSDevice_C2C::getNeighbors(size_t i) {
    std::vector<Cell*> ret;
    ret.push_back(myCells[i]);

    size_t x = i % myLookupXSize;
    if (x<myLookupXSize-1) {
        ret.push_back(myCells[i+1]);
    }
    if (x>0) {
        ret.push_back(myCells[i-1]);
    }

    size_t y = i / myLookupXSize;
    if (y<myLookupYSize-1) {
        ret.push_back(myCells[i+myLookupXSize]);
    }
    if (y>0) {
        ret.push_back(myCells[i-myLookupXSize]);
    }

    if ((x>0) && (y>0)) {
        ret.push_back(myCells[i-myLookupXSize-1]);
    }
    if ((x>0) && (y<myLookupYSize-1)) {
        ret.push_back(myCells[i+myLookupXSize-1]);
    }
    if ((x<myLookupXSize-1) && (y<myLookupYSize-1)) {
        ret.push_back(myCells[i+myLookupXSize+1]);
    }
    if ((x<myLookupXSize-1) && (y>0)) {
        ret.push_back(myCells[i-myLookupXSize+1]);
    }
    return ret;
}


void
MSDevice_C2C::writeNearEdges(OutputDevice &od) {
    size_t index = 0;
    for (vector<MSEdge*>::iterator i=myAllEdges.begin(); i!=myAllEdges.end(); ++i, ++index) {
        MSEdge *e = *i;
        const vector<const MSEdge*> &neighbors = myEdgeNeighbors[e];
        od << "   <edge id=\"" << e->getID() << "\" neighborNo=\"" << neighbors.size() << "\">";
        for (std::vector<const MSEdge*>::const_iterator j=neighbors.begin(); j!=neighbors.end(); ++j) {
            if (j!=neighbors.begin()) {
                od << ' ';
            }
            od << (*j)->getID();
        }
        od << "</edge>\n";
    }
}


// ---------------------------------------------------------------------------
// static methods for options i/o and device building
// ---------------------------------------------------------------------------
void
MSDevice_C2C::insertOptions() throw() {
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addOptionSubTopic("C2C");

    oc.doRegister("device.c2x.probability", new Option_Float(0.));//!!! describe
    oc.addDescription("device.c2x.probability", "C2C", "The probability for a vehicle to have c2c");

    oc.doRegister("device.c2x.knownveh", new Option_String());//!!! describe
    oc.addDescription("device.c2x.knownveh", "C2C", "Assign a device to named vehicles");

    oc.doRegister("device.c2x.deterministic", new Option_Bool(false)); //!!! describe
    oc.addDescription("device.c2x.deterministic", "C2C", "The devices are set deterministic using a fraction of 1000");

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
    for (vector<Cell*>::const_iterator i=myCells.begin(); i!=myCells.end(); ++i) {
        delete(*i);
    }
    myC2CComputationCommand = 0;
    myAllEdges.clear();
    myConnected.clear();
    myClusterHeaders.clear();
    myEdgeVehicles.clear();
}


void
MSDevice_C2C::buildVehicleDevices(MSVehicle &v, std::vector<MSDevice*> &into) throw() {
    OptionsCont &oc = OptionsCont::getOptions();
    if (oc.getFloat("device.c2x.probability")==0&&!oc.isSet("device.c2x.knownveh")) {
        // no c2c communication is modelled
        return;
    }
    // c2c communication is enabled
    bool haveByNumber = false;
    if (oc.getBool("device.c2x.deterministic")) {
        haveByNumber = ((myVehicleIndex%1000) < (int)(oc.getFloat("device.c2x.probability")*1000.));
    } else {
        haveByNumber = RandHelper::rand()<=oc.getFloat("device.c2x.probability");
    }
    bool haveByName = oc.isSet("device.c2x.knownveh") && OptionsCont::getOptions().isInStringVector("device.c2x.knownveh", v.getID());
    if (haveByNumber||haveByName) {
        MSDevice_C2C* device = new MSDevice_C2C(v, "c2c_" + v.getID());
        into.push_back(device);
        myVehiclesToDevicesMap[&v] = device;
        if (myCells.size()==0) {
            myAllEdges = MSNet::getInstance()->getEdgeControl().getEdges();
            buildLookUpInformation();
        }
        // make the weights be updated
        if (myC2CComputationCommand==0) {
            myC2CComputationCommand = new StaticCommand< MSDevice_C2C >(&MSDevice_C2C::computeC2CExecute);
            MSNet::getInstance()->getEndOfTimestepEvents().addEvent(
                myC2CComputationCommand, 0, MSEventControl::ADAPT_AFTER_EXECUTION);
        }
    }
    myVehicleIndex++;
}


// ---------------------------------------------------------------------------
// static methods for look-up computation
// ---------------------------------------------------------------------------
SUMOTime
MSDevice_C2C::computeC2CExecute(SUMOTime t) {
    // clean up prior information
    myConnected.clear();
    myClusterHeaders.clear();
    //
    for (map<const MSEdge*, vector<MSDevice_C2C*> >::iterator i=myEdgeVehicles.begin(); i!=myEdgeVehicles.end(); ++i) {
        vector<MSDevice_C2C*> &devices = (*i).second;
        for (vector<MSDevice_C2C*>::iterator j=devices.begin(); j!=devices.end(); ++j) {
            // update own information
            // a) insert the current edge if the vehicle is standing for a long period
            // b) remove information older then a specified amount of time (MSGlobals::gLANRefuseOldInfosOffset)
            (*j)->updateInfos(t);

            // go through the neighbors of this vehicle's edge
            const vector<const MSEdge*> &neighborEdges = myEdgeNeighbors[(*i).first];
            for (vector<const MSEdge*>::const_iterator l=neighborEdges.begin(); l!=neighborEdges.end(); ++l) {
                map<const MSEdge*, vector<MSDevice_C2C*> >::iterator ndi = myEdgeVehicles.find(*l);
                if (ndi==myEdgeVehicles.end()) {
                    // no c2c vehicle on this edge...
                    continue;
                }
                vector<MSDevice_C2C*> &neighborDevices = ndi->second;
                for (vector<MSDevice_C2C*>::const_iterator m = neighborDevices.begin(); m!=neighborDevices.end(); ++m) {
                    if (*j!=*m) {
                        // update connection state
                        (*j)->addVehNeighbors(*m, t);
                    }
                }
            }

            // remove connections to vehicles which are no longer in range
            (*j)->cleanUpConnections(t);

            // ...reset the cluster id
            (*j)->setClusterId(-1);
            // for each vehicle with communication partners...
            if ((*j)->getConnections().size()!=0) {
                // ...add the vehicle to list of connected vehicles
                myConnected.push_back(*j);
            }
        }
    }

    // build the clusters
    int clusterId = 1;
    for (vector<MSDevice_C2C*>::iterator q1=myConnected.begin(); q1!=myConnected.end(); q1++) {
        if ((*q1)->getClusterId()<0) {
            /*
            q = q1;
            for(q2=myConnected.begin(); q2!=connected.end(); q2++) {
                int size1 = (*q1)->getConnections().size();
                int size2 = (*q2)->getConnections().size();
            if((*q2)->getClusterId() < 0 && size1 < size2){
            q = q2;
            }
            }
            */
            myClusterHeaders.push_back(*q1);
            (*q1)->buildMyCluster(t, clusterId);
            clusterId++;
        }
    }

    // send information
    for (vector<MSDevice_C2C*>::iterator q = myClusterHeaders.begin(); q!=myClusterHeaders.end(); q++) {
        (*q)->sendInfos(t);
    }

    // Rerouting?
    for (vector<MSDevice_C2C*>::iterator q1 = myConnected.begin(); q1!=myConnected.end(); q1++) {
        (*q1)->checkReroute(t);
    }

    /*
    std::vector<MSDevice_C2C*> connected;
    std::vector<MSDevice_C2C*> clusterHeaders;
    myCells->update();

    for (Cells::CellsIterator cell = myCells->begin(); cell!=myCells->end(); ++cell) {
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
    */
    return 1;
}


MSDevice_C2C::MSDevice_C2C(MSVehicle &holder, const std::string &id) throw()
        : MSDevice(holder, id), akt(0) {}


MSDevice_C2C::~MSDevice_C2C() throw() {
    //!!!myCells->remove(this);
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


void
MSDevice_C2C::enterLaneAtMove(MSLane *lane, SUMOReal) {
    delete akt;
    akt = new Information(0, MSNet::getInstance()->getCurrentTimeStep());
    MSEdge &edge = lane->getEdge();
    if (myEdgeVehicles.find(&edge)==myEdgeVehicles.end()) {
        myEdgeVehicles[&edge] = vector<MSDevice_C2C*>();
    }
    myEdgeVehicles[&edge].push_back(this);
}


void
MSDevice_C2C::enterLaneAtEmit(MSLane *lane, const MSVehicle::State &) {
    delete akt;
    akt = new Information(0, MSNet::getInstance()->getCurrentTimeStep());
    MSEdge &edge = lane->getEdge();
    if (myEdgeVehicles.find(&edge)==myEdgeVehicles.end()) {
        myEdgeVehicles[&edge] = vector<MSDevice_C2C*>();
    }
    myEdgeVehicles[&edge].push_back(this);
}


void
MSDevice_C2C::leaveLaneAtMove(SUMOReal) {
    // checke whether the vehicle needed longer than expected
    SUMOReal factor = getHolder().getEdge()->getVehicleEffort(&getHolder(), MSNet::getInstance()->getCurrentTimeStep());
    SUMOReal nt = (float)(MSNet::getInstance()->getCurrentTimeStep() - akt->time);
    const MSEdge * const passedEdge = getHolder().getEdge();
    if (nt>10&&nt>factor*MSGlobals::gAddInfoFactor) { // !!! explicite
        // if so, check whether an information about the edge was already existing
        std::map<const MSEdge * const, Information *>::iterator i = infoCont.find(passedEdge);
        if (i==infoCont.end()) {
            // no, add the new information
            Information *info = new Information(*akt);
            info->neededTime = nt;
            infoCont[passedEdge] = info;
            i = infoCont.find(passedEdge);
        } else {
            // yes, update the existing information
            (*i).second->neededTime = nt;
        }
        // save the information
        MSCORN::saveSavedInformationData(MSNet::getInstance()->getCurrentTimeStep(),
                                         getID(),passedEdge->getID(),"congestion",(*i).second->time,nt);
        totalNrOfSavedInfos++;
    } else if (infoCont.find(passedEdge)!=infoCont.end()) {
        // ok, we could pass the edge faster than assumed; remove the information
        Information *info = infoCont[passedEdge];
        infoCont.erase(passedEdge);
        delete info;
    }
    delete akt;
    akt = 0;
    vector<MSDevice_C2C*> &devices = myEdgeVehicles.find(passedEdge)->second;
    devices.erase(find(devices.begin(), devices.end(), this));
}


void
MSDevice_C2C::onTripEnd() {
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
MSDevice_C2C::addNeighbors(vector<MSDevice_C2C*>* devices, SUMOTime time) {
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
MSDevice_C2C::addVehNeighbors(MSDevice_C2C *device, SUMOTime time) {
    if (!device->getHolder().isOnRoad()) {
        // obviously, one of the vehicles is being teleported
        return;
    }

    Position2D pos1 = getHolder().getPosition();
    Position2D pos2 = device->getHolder().getPosition();
    if (pos1.x()==-1000||pos2.x()==-1000) {
        return;
    }
    if (fabs(pos1.x()-pos2.x())<MSGlobals::gLANRange && fabs(pos1.y()-pos2.y())<MSGlobals::gLANRange) {
        SUMOReal distance = sqrt(pow(pos1.x()-pos2.x(),2) + pow(pos1.y()-pos2.y(),2));
        if ((distance>0)&&(distance<=MSGlobals::gLANRange)) {
            std::map<MSDevice_C2C*, C2CConnection*>::iterator i = myNeighbors.find(device);
            if (i==myNeighbors.end()) {
                // the vehicles will establish a new connection
                C2CConnection *con = new C2CConnection(device, time);
                //con->state = disconnected;
                //			con->timeSinceSeen = 1;
                //			con->timeSinceConnect = 0;
                myNeighbors[device] = con;
                // the other car must inform THIS vehicle if it's removed from the network
                device->getHolder().quitRemindedEntered(this);
            } else {
                // ok, the vehicles already interact
                //  increment the connection time
                (*i).second->lastTimeSeen = time;
            }
            MSCORN::saveVehicleInRangeData(time, getID(), device->getID(),
                                           pos1.x(),pos1.y(), pos2.x(),pos2.y());
        }
    }
}





void
MSDevice_C2C::cleanUpConnections(SUMOTime time) {
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
MSDevice_C2C::isInDistance(MSVehicle* veh1, MSVehicle* veh2) {
    Position2D pos1 = veh1->getPosition();
    Position2D pos2 = veh2->getPosition();
    if (pos1.x()==-1000||pos2.x()==-1000) {
        return false;
    }
    SUMOReal distance = sqrt((pos1.x()-pos2.x())*(pos1.x()-pos2.x()) + (pos1.y()-pos2.y())*(pos1.y()-pos2.y()));
    return (distance>0)&&(distance<=MSGlobals::gLANRange);
}


void
MSDevice_C2C::updateInfos(SUMOTime time) {
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
            timeByMeanSpeed1 = getHolder().getLane().getLength() / (getHolder().getPositionOnLane()/neededTime);
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
MSDevice_C2C::removeOnTripEnd(MSVehicle *veh) throw() {
    MSDevice_C2C *dev = myVehiclesToDevicesMap[veh];
    assert(myNeighbors.find(dev)!=myNeighbors.end());
    std::map<MSDevice_C2C *, C2CConnection*>::iterator i = myNeighbors.find(dev);
    delete(*i).second;
    myNeighbors.erase(i);
}


bool
MSDevice_C2C::knowsEdgeTest(MSEdge &edge) const {
    return infoCont.find(&edge)!=infoCont.end();
}


const MSDevice_C2C::ConnectionCont &
MSDevice_C2C::getConnections() const {
    return myNeighbors;
}


void
MSDevice_C2C::setClusterId(int Id) {
    clusterId = Id;
}


int
MSDevice_C2C::getClusterId(void) const {
    return clusterId;
}


int
MSDevice_C2C::buildMyCluster(SUMOTime t, int clId) {
    int count = 1;
    // build the cluster
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
    // write output
    ostringstream vehs;
    for (std::vector<C2CConnection*>::const_iterator i=clusterCont.begin(); i!=clusterCont.end(); ++i) {
        if (i!=clusterCont.begin()) {
            vehs << ' ';
        }
        vehs << (*i)->connectedVeh->getID();
    }
    MSCORN::saveClusterInfoData(t, clId, getID(), vehs.str(), count);
    return count;
}


void
MSDevice_C2C::sendInfos(SUMOTime time) {
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
                nofP = numberOfInfo;
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
MSDevice_C2C::numOfInfos(MSDevice_C2C *veh1, MSDevice_C2C* veh2) {
    Position2D pos1 = veh1->getHolder().getPosition();
    Position2D pos2 = veh2->getHolder().getPosition();
    SUMOReal distance = sqrt(pow(pos1.x()-pos2.x(),2) + pow(pos1.y()-pos2.y(),2));
    SUMOReal x = (SUMOReal)(((-2.3*distance + 1650.)*MSGlobals::gNumberOfSendingPos)/1500.);  //approximation function
    return (int)(x*MSGlobals::gInfoPerPaket);
}


void
MSDevice_C2C::transferInformation(const std::string &senderID, const InfoCont &infos,
                                  size_t NofP, SUMOTime currentTime) {
    if (NofP>0&&infos.size()>0) {
        myLastInfoTime = currentTime;
    }
    size_t count = 0;
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
MSDevice_C2C::getEffort(const MSEdge * const e, const MSVehicle * const v, SUMOTime t) const {
    MSDevice_C2C * device = myVehiclesToDevicesMap.find(v)->second;
    InfoCont::iterator i = device->infoCont.find(e);
    if (i==device->infoCont.end()) {
        SUMOReal value;
        if(MSNet::getInstance()->getWeightsStorage().retrieveExistingEffort(e, 0, t, value)) {
            return value;
        }
        const MSLane * const l = e->getLanes()[0];
        return l->getLength() / l->getMaxSpeed();
    }
    return i->second->neededTime;
}

void
MSDevice_C2C::checkReroute(SUMOTime t) {
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
        DijkstraRouterTT_Direct<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle> >
        router(MSEdge::dictSize(), true, &MSEdge::getVehicleEffort);
        myHolder.reroute(t, router);
    }
}


size_t
MSDevice_C2C::getNoGot() const {
    return myNoGot;
}


size_t
MSDevice_C2C::getNoSent() const {
    return myNoSent;
}


size_t
MSDevice_C2C::getNoGotRelevant() const {
    return myNoGotRelevant;
}


/****************************************************************************/

