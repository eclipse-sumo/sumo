//---------------------------------------------------------------------------//
//                        NIVissimDistrictConnection.cpp -  ccc
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.11  2003/06/05 11:46:56  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <map>
#include <string>
#include <algorithm>
#include <cassert>
#include <utils/common/IntVector.h>
#include <utils/convert/ToString.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Position2DVector.h>
#include "NIVissimAbstractEdge.h"
#include "NIVissimEdge.h"
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBDistrict.h>
#include <netbuild/NBDistrictCont.h>
#include "NIVissimDistrictConnection.h"

using namespace std;

NIVissimDistrictConnection::DictType NIVissimDistrictConnection::myDict;
std::map<int, IntVector> NIVissimDistrictConnection::myDistrictsConnections;

NIVissimDistrictConnection::NIVissimDistrictConnection(int id,
        const std::string &name,
        const IntVector &districts, const DoubleVector &percentages,
        int edgeid, double position, const IntVector &assignedVehicles)
    : myID(id), myName(name), myDistricts(districts),
    myEdgeID(edgeid), myPosition(position),
    myAssignedVehicles(assignedVehicles)
{
    IntVector::iterator i=myDistricts.begin();
    DoubleVector::const_iterator j=percentages.begin();
    while(i!=myDistricts.end()) {
        myPercentages[*i] = *j;
        i++;
        j++;
    }
}


NIVissimDistrictConnection::~NIVissimDistrictConnection()
{
}



bool
NIVissimDistrictConnection::dictionary(int id, const std::string &name,
        const IntVector &districts, const DoubleVector &percentages,
        int edgeid, double position,
        const IntVector &assignedVehicles)
{
    NIVissimDistrictConnection *o =
        new NIVissimDistrictConnection(id, name, districts, percentages,
            edgeid, position, assignedVehicles);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimDistrictConnection::dictionary(int id, NIVissimDistrictConnection *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimDistrictConnection *
NIVissimDistrictConnection::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}

void
NIVissimDistrictConnection::dict_BuildDistrictNodes()
{
    //  pre-assign connections to districts
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimDistrictConnection *c = (*i).second;
        const IntVector &districts = c->myDistricts;
        for(IntVector::const_iterator j=districts.begin(); j!=districts.end(); j++) {
            // assign connection to district
            myDistrictsConnections[*j].push_back((*i).first);
        }
    }

    for(std::map<int, IntVector>::iterator k=myDistrictsConnections.begin(); k!=myDistrictsConnections.end(); k++) {
        // get the connections
        const IntVector &connections = (*k).second;
            // retrieve the current district
        string dsid = toString<int>((*k).first);
        NBDistrict *district = new NBDistrict(dsid, dsid, true, true);
        NBDistrictCont::insert(district);
        // compute the middle of the district
        Position2DVector pos;
        for(IntVector::const_iterator j=connections.begin(); j!=connections.end(); j++) {
            NIVissimDistrictConnection *c = dictionary(*j);
            pos.push_back(c->geomPosition());
        }
        Position2D distCenter = pos.center();
        if(connections.size()==1) { // !!! ok, ok, maybe not the best way just to add an offset
            distCenter.add(10, 10);
        }
        district->setCenter(distCenter.x(), distCenter.y());
        // build the node
        string id = "District" + district->getID();
        NBNode *districtNode =
            new NBNode(id,
                district->getXCoordinate(),
                district->getYCoordinate(),
                district);
        if(!NBNodeCont::insert(districtNode)) {
            cout << "nope, NIVissimDistrictConnection" << endl;
            throw 1;
        }
    }
}

void
NIVissimDistrictConnection::dict_BuildDistricts()
{

    // add the sources and sinks
    //  their normalised propability is computed within NBDistrict
    //   to avoid double code writing and more securty within the converter
    //  go through the district table
    for(std::map<int, IntVector>::iterator k=myDistrictsConnections.begin(); k!=myDistrictsConnections.end(); k++) {
        // get the connections
        const IntVector &connections = (*k).second;
            // retrieve the current district
        NBDistrict *district =
            NBDistrictCont::retrieve(toString<int>((*k).first));
        NBNode *districtNode = NBNodeCont::retrieve(
            string("District") + district->getID());
        assert(district!=0&&districtNode!=0);

        for(IntVector::const_iterator l=connections.begin(); l!=connections.end(); l++) {
            NIVissimDistrictConnection *c = dictionary(*l);
            // get the edge to connect the parking place to
            NBEdge *e = NBEdgeCont::retrieve(toString<int>(c->myEdgeID));
            if(e==0) {
                e = NBEdgeCont::retrievePossiblySplitted(
                    toString<int>(c->myEdgeID),
                    c->myPosition);
            }
            assert(e!=0);
            string id = string("ParkingPlace") + toString<int>(*l);
            NBNode *parkingPlace = NBNodeCont::retrieve(id);
            if(parkingPlace==0) {
                double pos = c->getPosition();
                if(pos<e->getLength()-pos) {
                    parkingPlace = e->getFromNode();
                    parkingPlace->invalidateIncomingConnections();
                } else {
                    parkingPlace = e->getToNode();
                    parkingPlace->invalidateOutgoingConnections();
                }
            }
            assert(
                e->getToNode()==parkingPlace
                ||
                e->getFromNode()==parkingPlace);

            // build the connection to the source
            if(e->getFromNode()==parkingPlace) {
                id = string("VissimFromParkingplace")
                    + toString<int>((*k).first) + "-"
                    + toString<int>(c->myID);
                NBEdge *source =
                    new NBEdge(id, id, districtNode, parkingPlace,
                    "Connection", 100/3.6, 2, 100, 0,
                    NBEdge::EDGEFUNCTION_SOURCE);
                if(!NBEdgeCont::insert(source)) { // !!! in den Konstruktor
                    throw 1; // !!!
                }
                double percNormed =
                    c->myPercentages[(*k).first];
                if(!district->addSource(source, percNormed)) {
                    throw 1;
                }
            }

            // build the connection to the destination
            if(e->getToNode()==parkingPlace) {
                id = string("VissimToParkingplace")
                    + toString<int>((*k).first) + "-"
                    + toString<int>(c->myID);
                NBEdge *destination =
                    new NBEdge(id, id, parkingPlace, districtNode,
                    "Connection", 100/3.6, 2, 100, 0,
                    NBEdge::EDGEFUNCTION_SINK);
                if(!NBEdgeCont::insert(destination)) { // !!! (in den Konstruktor)
                    throw 1; // !!!
                }
                double percNormed2 =
                    c->myPercentages[(*k).first];
                if(!district->addSink(destination, percNormed2)) {
                    throw 1; // !!!
                }
            }

                /*
            if(e->getToNode()==districtNode) {
                double percNormed =
                    c->myPercentages[(*k).first];
                district->addSink(e, percNormed);
            }
            if(e->getFromNode()==districtNode) {
                double percNormed =
                    c->myPercentages[(*k).first];
                district->addSource(e, percNormed);
            }
*/
        }

        /*
        // add them as sources and sinks to the current district
        for(IntVector::const_iterator l=connections.begin(); l!=connections.end(); l++) {
            // get the current connections
            NIVissimDistrictConnection *c = dictionary(*l);
            // get the edge to connect the parking place to
            NBEdge *e = NBEdgeCont::retrieve(toString<int>(c->myEdgeID));
            Position2D edgepos = c->geomPosition();
            NBNode *edgeend = e->tryGetNodeAtPosition(c->myPosition,
                e->getLength()/4.0);
            if(edgeend==0) {
                // Edge splitting omitted on build district connections by now
                assert(false);
            }

            // build the district-node if not yet existing
            string id = "VissimParkingplace" + district->getID();
            NBNode *districtNode = NBNodeCont::retrieve(id);
            assert(districtNode!=0);

            if(e->getToNode()==edgeend) {
                // build the connection to the source
                id = string("VissimFromParkingplace")
                    + toString<int>((*k).first) + "-"
                    + toString<int>(c->myID);
                NBEdge *source =
                    new NBEdge(id, id, districtNode, edgeend,
                    "Connection", 100/3.6, 2, 100, 0,
                    NBEdge::EDGEFUNCTION_SOURCE);
                NBEdgeCont::insert(source); // !!! (in den Konstruktor)
                double percNormed =
                    c->myPercentages[(*k).first];
                district->addSource(source, percNormed);
            } else {
                // build the connection to the destination
                id = string("VissimToParkingplace")
                    + toString<int>((*k).first) + "-"
                    + toString<int>(c->myID);
                NBEdge *destination =
                    new NBEdge(id, id, edgeend, districtNode,
                    "Connection", 100/3.6, 2, 100, 0,
                    NBEdge::EDGEFUNCTION_SINK);
                NBEdgeCont::insert(destination); // !!! (in den Konstruktor)

                // add both the source and the sink to the district
                double percNormed =
                    c->myPercentages[(*k).first];
                district->addSink(destination, percNormed);
            }
        }
        */
    }
}



Position2D
NIVissimDistrictConnection::geomPosition() const
{
    NIVissimAbstractEdge *e = NIVissimEdge::dictionary(myEdgeID);
    return e->getGeomPosition(myPosition);
}


NIVissimDistrictConnection *
NIVissimDistrictConnection::dict_findForEdge(int edgeid)
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        if((*i).second->myEdgeID==edgeid) {
            return (*i).second;
        }
    }
    return 0;
}


void
NIVissimDistrictConnection::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}






/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimDistrictConnection.icc"
//#endif

// Local Variables:
// mode:C++
// End:


