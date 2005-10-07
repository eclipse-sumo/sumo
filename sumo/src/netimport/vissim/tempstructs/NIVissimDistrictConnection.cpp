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
// Revision 1.25  2005/10/07 11:40:10  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.24  2005/09/23 06:02:57  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.23  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.22  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.21  2004/08/02 12:44:13  dkrajzew
// using Position2D instead of two SUMOReals
//
// Revision 1.20  2004/07/02 09:36:03  dkrajzew
// setting of a default speed for unset districts added
//
// Revision 1.19  2004/01/28 12:39:23  dkrajzew
// work on reading and setting speeds in vissim-networks
//
// Revision 1.18  2004/01/12 15:32:57  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.17  2003/11/11 08:24:52  dkrajzew
// debug values removed
//
// Revision 1.16  2003/10/30 09:12:59  dkrajzew
// further work on vissim-import
//
// Revision 1.15  2003/10/27 10:51:55  dkrajzew
// edges speed setting implemented (only on an edges begin)
//
// Revision 1.14  2003/10/17 06:49:05  dkrajzew
// set the number of lanes of sources and sinks to three
//
// Revision 1.13  2003/09/23 14:16:37  dkrajzew
// further work on vissim-import
//
// Revision 1.12  2003/07/07 08:28:48  dkrajzew
// adapted the importer to the new node type description; some further work
//
// Revision 1.11  2003/06/05 11:46:56  dkrajzew
// class templates applied; documentation added
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

#include <map>
#include <string>
#include <algorithm>
#include <cassert>
#include <utils/common/IntVector.h>
#include <utils/common/ToString.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Position2DVector.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include "NIVissimAbstractEdge.h"
#include "NIVissimEdge.h"
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBDistrict.h>
#include <netbuild/NBDistrictCont.h>
#include "NIVissimDistrictConnection.h"
#include <utils/distribution/Distribution.h>
#include <netbuild/NBDistribution.h>
#include <utils/common/MsgHandler.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
NIVissimDistrictConnection::DictType NIVissimDistrictConnection::myDict;
std::map<int, IntVector> NIVissimDistrictConnection::myDistrictsConnections;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVissimDistrictConnection::NIVissimDistrictConnection(int id,
        const std::string &name,
        const IntVector &districts, const DoubleVector &percentages,
        int edgeid, SUMOReal position,
        const std::vector<std::pair<int, int> > &assignedVehicles)
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
        int edgeid, SUMOReal position,
        const std::vector<std::pair<int, int> > &assignedVehicles)
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
NIVissimDistrictConnection::dict_BuildDistrictConnections()
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
}


void
NIVissimDistrictConnection::dict_CheckEdgeEnds()
{
    for(std::map<int, IntVector>::iterator k=myDistrictsConnections.begin(); k!=myDistrictsConnections.end(); k++) {
        const IntVector &connections = (*k).second;
        for(IntVector::const_iterator j=connections.begin(); j!=connections.end(); j++) {
            NIVissimDistrictConnection *c = dictionary(*j);
            c->checkEdgeEnd();
        }
    }
}


void
NIVissimDistrictConnection::checkEdgeEnd()
{
    NIVissimEdge *edge = NIVissimEdge::dictionary(myEdgeID);
    assert(edge!=0);
    edge->checkDistrictConnectionExistanceAt(myPosition);
}


void
NIVissimDistrictConnection::dict_BuildDistrictNodes(NBDistrictCont &dc,
                                                    NBNodeCont &nc)
{
    for(std::map<int, IntVector>::iterator k=myDistrictsConnections.begin(); k!=myDistrictsConnections.end(); k++) {
        // get the connections
        const IntVector &connections = (*k).second;
            // retrieve the current district
        string dsid = toString<int>((*k).first);
        NBDistrict *district = new NBDistrict(dsid, dsid, true, true);
        dc.insert(district);
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
            new NBNode(id, district->getPosition(), district);
        if(!nc.insert(districtNode)) {
            throw 1;
        }
    }
}

void
NIVissimDistrictConnection::dict_BuildDistricts(NBDistrictCont &dc,
                                                NBEdgeCont &ec,
                                                NBNodeCont &nc/*,
                                                NBDistribution &distc*/)
{
    // add the sources and sinks
    //  their normalised probability is computed within NBDistrict
    //   to avoid SUMOReal code writing and more securty within the converter
    //  go through the district table
    for(std::map<int, IntVector>::iterator k=myDistrictsConnections.begin(); k!=myDistrictsConnections.end(); k++) {
        // get the connections
        const IntVector &connections = (*k).second;
            // retrieve the current district
        NBDistrict *district =
            dc.retrieve(toString<int>((*k).first));
        NBNode *districtNode = nc.retrieve(
            string("District") + district->getID());
        assert(district!=0&&districtNode!=0);

        for(IntVector::const_iterator l=connections.begin(); l!=connections.end(); l++) {
            NIVissimDistrictConnection *c = dictionary(*l);
            // get the edge to connect the parking place to
            NBEdge *e = ec.retrieve(toString<int>(c->myEdgeID));
            if(e==0) {
                e = ec.retrievePossiblySplitted(
                    toString<int>(c->myEdgeID),
                    c->myPosition);
            }
            assert(e!=0);
            string id = string("ParkingPlace") + toString<int>(*l);
            NBNode *parkingPlace = nc.retrieve(id);
            if(parkingPlace==0) {
                SUMOReal pos = c->getPosition();
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
                    "Connection", c->getMeanSpeed(/*distc*/)/(SUMOReal) 3.6, 3, 100, 0,
                    NBEdge::LANESPREAD_RIGHT,
                    NBEdge::EDGEFUNCTION_SOURCE);
                if(!ec.insert(source)) { // !!! in den Konstruktor
                    throw 1; // !!!
                }
                SUMOReal percNormed =
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
                    "Connection", (SUMOReal) 100/(SUMOReal) 3.6, 2, 100, 0,
                    NBEdge::LANESPREAD_RIGHT,
                    NBEdge::EDGEFUNCTION_SINK);
                if(!ec.insert(destination)) { // !!! (in den Konstruktor)
                    throw 1; // !!!
                }
                SUMOReal percNormed2 =
                    c->myPercentages[(*k).first];
                if(!district->addSink(destination, percNormed2)) {
                    throw 1; // !!!
                }
            }

                /*
            if(e->getToNode()==districtNode) {
                SUMOReal percNormed =
                    c->myPercentages[(*k).first];
                district->addSink(e, percNormed);
            }
            if(e->getFromNode()==districtNode) {
                SUMOReal percNormed =
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
            NBNode *districtNode = nc.retrieve(id);
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
                SUMOReal percNormed =
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
                SUMOReal percNormed =
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


SUMOReal
NIVissimDistrictConnection::getMeanSpeed(/*NBDistribution &dc*/) const
{
    //assert(myAssignedVehicles.size()!=0);
    if(myAssignedVehicles.size()==0) {
        cout << "No streams assigned at district'" << myID << "'." << endl;
        cout << " Using default speed 200km/h" << endl;
        return (SUMOReal) 200/(SUMOReal) 3.6;
    }
    SUMOReal speed = 0;
    std::vector<std::pair<int, int> >::const_iterator i;
    for(i=myAssignedVehicles.begin(); i!=myAssignedVehicles.end(); i++) {
        speed += getRealSpeed(/*dc, */(*i).second);
    }
    return speed / (SUMOReal) myAssignedVehicles.size();
}


SUMOReal
NIVissimDistrictConnection::getRealSpeed(/*NBDistribution &dc, */int distNo) const
{
    string id = toString<int>(distNo);
    Distribution *dist = NBDistribution::dictionary("speed", id);
    if(dist==0) {
        WRITE_WARNING("The referenced speed distribution '" + id + "' is not known.");
        WRITE_WARNING(". Using default.");
        return OptionsSubSys::getOptions().getFloat("vissim-default-speed");;
    }
    assert(dist!=0);
    SUMOReal speed = dist->getMax();
    if(speed<0||speed>1000) {
        WRITE_WARNING(" False speed at district '" + id);
        WRITE_WARNING(". Using default.");
        speed = OptionsSubSys::getOptions().getFloat("vissim-default-speed");
    }
    return speed;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


