#ifndef ROOrigDestRouteDef_h
#define ROOrigDestRouteDef_h

#include <string>
#include "RORouteDef.h"

class ROEdge;
class RORoute;
class RORouter;

class ROOrigDestRouteDef : public RORouteDef {
protected:
	ROEdge *_from, *_to;
    RORoute *_current;
    long _startTime;
public:
    ROOrigDestRouteDef(const std::string &id, ROEdge *from, ROEdge *to);
	virtual ~ROOrigDestRouteDef();
    ROEdge *getFrom() const;
    ROEdge *getTo() const;
protected:
    RORoute *buildCurrentRoute(RORouter &router, long begin);
    void addAlternative(RORoute *current, long begin);
    void xmlOutCurrent(std::ostream &res) const;
    void xmlOutAlternatives(std::ostream &altres) const;
};

#endif
