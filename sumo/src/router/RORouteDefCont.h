#ifndef RORouteDefCont_h
#define RORouteDefCont_h

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <utils/router/IDSupplier.h>
#include "RORouteDef.h"
#include <utils/common/NamedObjectCont.h>

class ROEdge;

class RORouteDefCont : public NamedObjectCont<RORouteDef*> {
private:
    typedef std::map<std::pair<ROEdge*, ROEdge*>, RORouteDef*> idMap;
    idMap _known;
    IDSupplier _idSupplier;
public:
    RORouteDefCont();
    ~RORouteDefCont();
    bool add(std::string id, RORouteDef *def);
//    std::string add(ROEdge *from, ROEdge *to);
    static void writeXML(std::ostream &os);
    bool known(const std::string &name) const;
    bool known(ROEdge *from, ROEdge *to) const;
    std::string getID(ROEdge *from, ROEdge *to) const;
    void clear();
private:
    /// we made the copy constructor invalid
    RORouteDefCont(const RORouteDefCont &src);
    /// we made the assignment operator invalid
    RORouteDefCont &operator=(const RORouteDefCont &src);
};

#endif
