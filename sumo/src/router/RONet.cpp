#include <string>
#include <iostream>
#include <fstream>
#include <deque>
#include <queue>
#include "RONodeCont.h"
#include "ROEdgeCont.h"
#include "ROVehTypeCont.h"
#include "ROEdge.h"
#include "RONode.h"
#include "RONet.h"
#include "RORoute.h"
#include "RORouteDef.h"
#include "RORouteDefCont.h"
#include "ROVehicle.h"
#include "ROVehicleType.h"
#include "ROVehicleType_Krauss.h"
#include "RORouter.h"
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SErrorHandler.h>

using namespace std;

RONet::RONet(bool multireferencedRoutes)
    : _vehicleTypes(new ROVehicleType_Krauss()),
    _multireferencedRoutes(multireferencedRoutes)
{
}


RONet::~RONet()
{
}


void
RONet::addEdge(const std::string &name, ROEdge *edge)
{
    if(!_edges.add(name, edge)) {
        SErrorHandler::add(string("The edge '") + name + string("' occures at least twice."));
    }
}


ROEdge *
RONet::getEdge(const std::string &name) const
{
    return _edges.get(name);
}


void
RONet::addNode(const std::string &name, RONode *node)
{
    _nodes.add(name, node);
}


bool
RONet::isKnownVehicleID(const std::string &id) const
{
    VehIDCont::const_iterator i=_vehIDs.find(id);
    if(i==_vehIDs.end())
        return false;
    return true;
}

void
RONet::addVehicleID(const std::string &id)
{
    _vehIDs.insert(id);
}

/*
void
RONet::computeWeights()
{
    _edges.computeWeights();
}
*/

ROVehicleType *
RONet::getVehicleType(const std::string &name) const
{
    return _vehicleTypes.get(name);
}

RORouteDef *
RONet::getRouteDef(const std::string &name) const
{
    return _routes.get(name);
}

/*
std::string
RONet::addRouteDef(ROEdge *from, ROEdge *to) {
    return _routes.add(from, to);
}
*/
void
RONet::addRouteDef(RORouteDef *def) {
    _routes.add(def->getID(), def);
}

ROVehicleType *
RONet::addVehicleType(const std::string &id) {
    // check whether the type was already known
    ROVehicleType *type = _vehicleTypes.get(id);
    if(type!=0) {
        return type;
    }
    return getDefaultVehicleType();
}


ROVehicleType *
RONet::getDefaultVehicleType() const {
    return _vehicleTypes.getDefault();
}

void
RONet::addVehicleType(ROVehicleType *type)
{
    _vehicleTypes.add(type->getID(), type);
}

void
RONet::addVehicle(const std::string &id, ROVehicle *veh) {
    _vehicles.add(id, veh);
}

/*
void
RONet::computeAndSave(OptionsCont &oc) {
    // start the computation and save the build routes
    ofstream res(oc.getString("o").c_str());
    if(!res.good()) {
        throw ProcessError();
    }
    // build the router
    DijkstraRouter router(&_edges);
    // sort the list of route definitions
    BinaryHeap<ROVehicle*, ROHelper::VehicleByDepartureComperator> &sortedVehicles = _vehicles.sort();
    // begin writing
    res << "<routes>" << endl;
    // write all vehicles (and additional structures)
    while(!sortedVehicles.isEmpty()) {
        // get the next vehicle
        ROVehicle *veh = sortedVehicles.findMin();
        sortedVehicles.deleteMin();
        // write the type if it's new
        saveType(res, veh->getType(), veh->getID());
        // write the route if it's new
        saveRoute(router, res, veh->getRoute(), veh->getID());
        // write the vehicle
        res << "   ";
        veh->xmlOut(res);
    }
    // end writing
    res << "</routes>" << endl;
    res.close();
}
*/

void
RONet::saveType(std::ostream &os, ROVehicleType *type,
                const std::string &vehID) {
    if(type==0) {
        type = _vehicleTypes.getDefault();
        // !!! warn??
        cout << "The vehicle '" << vehID
            << "' has no valid type; Using default." << endl;
    // !!! warn??
    }
    os << "   ";
    type->xmlOut(os);
    type->markSaved();
}


bool
RONet::saveRoute(RORouter &router, 
                 std::ostream &res, 
                 std::ostream &altres, 
                 ROVehicle *veh) 
{
    RORouteDef *routeDef = veh->getRoute();
    // check if the route definition is valid
    if(routeDef==0) {
        SErrorHandler::add(string("The vehicle '") + veh->getID() 
            + string("' has no valid route."));
        return false;
    }
    // check whether the route was already saved
    if(routeDef->isSaved()) {
        return true;
    }
    // build and save the route
    return routeDef->computeAndSave(router, veh->getDepartureTime(), 
        res, altres);
}

void
RONet::saveAndRemoveRoutes(std::ofstream &res, std::ofstream &altres)
{
    // build the router
    RORouter router(*this, &_edges);
    // sort the list of route definitions
    priority_queue<ROVehicle*, 
        std::vector<ROVehicle*>,
        ROHelper::VehicleByDepartureComperator> 
        &sortedVehicles = _vehicles.sort();
    // write all vehicles (and additional structures)
    while(!sortedVehicles.empty()) {
        // get the next vehicle
        ROVehicle *veh = sortedVehicles.top();
        sortedVehicles.pop();
        // write the route
        if(saveRoute(router, res, altres, veh)) {
            // write the type if it's new
            if(!veh->getType()->isSaved()) {
                saveType(res, veh->getType(), veh->getID());
                saveType(altres, veh->getType(), veh->getID());
            }
            // write the vehicle
            res << "   ";
            veh->xmlOut(res);
            altres << "   ";
            veh->xmlOut(altres);
            // remove the route if it is not longer used
            removeRouteSecure(veh->getRoute());
        }
    }
    _vehicles.clear();
}


void 
RONet::removeRouteSecure(RORouteDef *route)
{
    // !!! later, a counter should be used to keep computed routes in the memory
    _routes.erase(route->getID());
}


bool
RONet::addRouteSnipplet(const ROEdgeVector &item)
{
    return _snipplets.add(item);
}


const ROEdgeVector &
RONet::getRouteSnipplet(ROEdge *from, ROEdge *to) const
{
    return _snipplets.get(from, to);
}


bool
RONet::knowsRouteSnipplet(ROEdge *from, ROEdge *to) const
{
    return _snipplets.knows(from, to);
}

