#ifndef RORouteAlternativesDef_h
#define RORouteAlternativesDef_h

#include <string>
#include <vector>
#include "ROEdgeVector.h"
#include "RORouteDef.h"

class DijkstraRouter;
class ROEdge;

class RORouteAlternativesDef : public RORouteDef {
private:
    bool _newRoute;
//    int _current;
    int _lastUsed;
    typedef std::vector<RORoute*> AlternativesVector;
    AlternativesVector _alternatives;
    double _gawronBeta;
    double _gawronA;
public:
    RORouteAlternativesDef(const std::string &id, size_t lastUsed,
        double gawronBeta, double gawronA);
    virtual ~RORouteAlternativesDef();
    virtual void addLoadedAlternative(RORoute *alternative);
    ROEdge *getFrom() const;
    ROEdge *getTo() const;
protected:
    RORoute *buildCurrentRoute(RORouter &router, long begin);
    void addAlternative(RORoute *current, long begin);
    void xmlOutCurrent(std::ostream &res) const;
    void xmlOutAlternatives(std::ostream &altres) const;
private:
    int findRoute(RORoute *opt) const;
    double gawronF(double pdr, double pds, double x);
    double gawronG(double a, double x);
};

#endif
