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
NIVissimDistrictConnection::dict_BuildDistricts()
{
    // Build the districts first
    //  pre-assign connections to districts, by the way
    IntVector buildDistricts;
    std::map<int, IntVector> districtsConnections;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimDistrictConnection *c = (*i).second;
        const IntVector &districts = c->myDistricts;
        for(IntVector::const_iterator j=districts.begin(); j!=districts.end(); j++) {
            // build a district only if it was not build before
            if(find(buildDistricts.begin(), buildDistricts.end(), *j)==buildDistricts.end()) {
                NBDistrict *build = new NBDistrict(
                    toString<int>(*j), toString<int>(*j), true, true);
                NBDistrictCont::insert(build);
                buildDistricts.push_back(*j);
            }
            // assign connection to district
            districtsConnections[*j].push_back((*i).first);
        }
    }

    // add the sources and sinks
    //  their normalised propability is computed within NBDistrict
    //   to avoid double code writing and more securty within the converter
    //  go through the district table
    for(std::map<int, IntVector>::iterator k=districtsConnections.begin(); k!=districtsConnections.end(); k++) {
        // get the connections
        const IntVector &connections = (*k).second;
            // retrieve the current district
        NBDistrict *district =
            NBDistrictCont::retrieve(toString<int>((*k).first));

        // compute the middle of the district
        Position2DVector pos;
        for(IntVector::const_iterator j=connections.begin(); j!=connections.end(); j++) {
            NIVissimDistrictConnection *c = dictionary(*j);
            pos.push_back(c->geomPosition());
        }
        Position2D distCenter = pos.center();
        district->setCenter(distCenter.x(), distCenter.y());

        // add them as sources and sinks to the current district
        for(IntVector::const_iterator l=connections.begin(); l!=connections.end(); l++) {
            // get the current connections
            NIVissimDistrictConnection *c = dictionary(*l);
            // get the edge to connect the parking place to
            NBEdge *e = NBEdgeCont::retrieve(toString<int>(c->myEdgeID));
            Position2D edgepos = c->geomPosition();
            NBNode *edgeend = e->tryGetNodeAtPosition(c->myPosition);
            if(edgeend==0) {
                assert(false);
                // Edge splitting omitted on build district connections by now
            }

            // build the source-district-node
            string id = "VissimParkingplaceSource"
                + toString<int>((*k).first) + "-"
                + toString<int>(c->myID);
            Position2D nodePos =
                GeomHelper::interpolate(edgepos, distCenter, 100);
            NBNode *districtSourceNode =
                new NBNode(id, nodePos.x(), -nodePos.y()); // !!!
            NBNodeCont::insert(districtSourceNode); // !!! sollte bei der Allokation geschehen

            // build the destination-district-node
            id = "VissimParkingplaceDestination"
                + toString<int>((*k).first) + "-"
                + toString<int>(c->myID);
            nodePos =
                GeomHelper::interpolate(edgepos, distCenter, 10);
            NBNode *districtDestinationNode =
                new NBNode(id, nodePos.x(), -nodePos.y()); // !!!
            NBNodeCont::insert(districtDestinationNode); // !!! sollte bei der Allokation geschehen

            // build the connection to the source
            id = string("VissimFromParkingplace")
                + toString<int>((*k).first) + "-"
                + toString<int>(c->myID);
            NBEdge *source =
                new NBEdge(id, id, districtSourceNode, edgeend,
                "Connection", 100/3.6, 2, 100, 0,
                NBEdge::EDGEFUNCTION_SOURCE);
            NBEdgeCont::insert(source); // !!! (in den Konstruktor)

            // build the connection to the destination
            id = string("VissimToParkingplace")
                + toString<int>((*k).first) + "-"
                + toString<int>(c->myID);
            NBEdge *destination =
                new NBEdge(id, id, edgeend, districtSourceNode,
                "Connection", 100/3.6, 2, 100, 0,
                NBEdge::EDGEFUNCTION_SINK);
            NBEdgeCont::insert(destination); // !!! (in den Konstruktor)

            // add both the source and the sink to the district
            double percNormed =
                c->myPercentages[(*k).first];;
            district->addSource(source, percNormed);
            district->addSink(destination, percNormed);
        }
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




