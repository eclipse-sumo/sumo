#ifndef NIVissimNodeDef_Poly_h
#define NIVissimNodeDef_Poly_h

#include <string>
#include <map>
#include <utils/geom/Position2DVector.h>
#include "NIVissimNodeDef_Edges.h"

class NIVissimNodeDef_Poly :
        public NIVissimNodeDef_Edges {
public:
    NIVissimNodeDef_Poly(int id, const std::string &name,
        const Position2DVector &poly);
    virtual ~NIVissimNodeDef_Poly();
    static bool dictionary(int id, const std::string &name,
        const Position2DVector &poly);
    virtual void computeBounding();
//    virtual void searchAndSetConnections();
//    bool matches(const IntVector &edges);
private:
    Position2DVector myPoly;

};

#endif
