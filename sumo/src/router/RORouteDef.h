#ifndef RORouteDef_h
#define RORouteDef_h

#include <string>
#include <iostream>
#include <utils/common/Named.h>
#include "ReferencedItem.h"

class ROEdge;
class RORouter;
class RORoute;

class RORouteDef : public ReferencedItem,
                   public Named {
public:
    RORouteDef(const std::string &id);
	virtual ~RORouteDef();
    virtual ROEdge *getFrom() const = 0;
    virtual ROEdge *getTo() const = 0;
    bool computeAndSave(RORouter &router, long begin,
        std::ostream &res, std::ostream &altres);
protected:
    virtual RORoute *buildCurrentRoute(RORouter &router, long begin) = 0;
    virtual void addAlternative(RORoute *current, long begin) = 0;
    virtual void xmlOutCurrent(std::ostream &res) const = 0;
    virtual void xmlOutAlternatives(std::ostream &altres) const = 0;
};

#endif
