#ifndef RONet_h
#define RONet_h

#include <string>
#include <set>
#include <fstream>
#include <deque>
#include "RONodeCont.h"
#include "ROEdgeCont.h"
#include "ROVehTypeCont.h"
#include "RORouteDefCont.h"
#include "ROVehicleCont.h"
#include "ROVehicle.h"
#include "RORouteSnippletCont.h"
#include "ROEdgeVector.h"

class ROEdge;
class RONode;
class RORouteDef;
class RORouter;
class ROVehicle;
class OptionsCont;

class RONet {
private:
    typedef std::set<std::string> VehIDCont;
    VehIDCont _vehIDs;
    RONodeCont _nodes;
    ROEdgeCont _edges;
    ROVehTypeCont _vehicleTypes;
    RORouteDefCont _routes;
    ROVehicleCont _vehicles;
    RORouteSnippletCont _snipplets;
    bool _multireferencedRoutes;
public:
    RONet(bool multireferencedRoutes);
    ~RONet();
    void addEdge(const std::string &name, ROEdge *edge);
    ROEdge *getEdge(const std::string &name) const;
    void addNode(const std::string &name, RONode *node);
    bool isKnownVehicleID(const std::string &id) const;
    RORouteDef *getRouteDef(const std::string &name) const;
//    std::string addRouteDef(ROEdge *from, ROEdge *to);
    void addRouteDef(RORouteDef *def);

    ROVehicleType *addVehicleType(const std::string &id);
    void addVehicleType(ROVehicleType *type);
    ROVehicleType *getDefaultVehicleType() const;
    ROVehicleType *getVehicleType(const std::string &name) const;

    void addVehicleID(const std::string &id);
    void addVehicle(const std::string &id, ROVehicle *veh);

    bool addRouteSnipplet(const ROEdgeVector &item);
    const ROEdgeVector &getRouteSnipplet(ROEdge *from, ROEdge *to) const;
    bool knowsRouteSnipplet(ROEdge *from, ROEdge *to) const;

    void saveAndRemoveRoutes(std::ofstream &res, std::ofstream &altres);
    //void computeAndSave(OptionsCont &oc);
private:
    void saveType(std::ostream &os, ROVehicleType *type,
        const std::string &vehID);
    bool saveRoute(RORouter &router, 
        std::ostream &res, std::ostream &altres, ROVehicle *veh);
    void removeRouteSecure(RORouteDef *route);
private:
    /// we made the copy constructor invalid
    RONet(const RONet &src);
    /// we made the assignment operator invalid
    RONet &operator=(const RONet &src);
};

#endif
