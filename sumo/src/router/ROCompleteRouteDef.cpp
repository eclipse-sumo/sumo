#include <string>
#include <deque>
#include "ROEdgeVector.h"
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include "RORouter.h"
#include "ROCompleteRouteDef.h"

using namespace std;

ROCompleteRouteDef::ROCompleteRouteDef(const std::string &id,
                                       const ROEdgeVector &edges)
    : RORouteDef(id), _edges(edges)
{
}


ROCompleteRouteDef::~ROCompleteRouteDef()
{
}


ROEdge *
ROCompleteRouteDef::getFrom() const
{
    return _edges.getFirst();
}


ROEdge *
ROCompleteRouteDef::getTo() const
{
    return _edges.getLast();
}


RORoute *
ROCompleteRouteDef::buildCurrentRoute(RORouter &router, long begin) 
{
    return new RORoute(_id, 0, 1, _edges);
}


void 
ROCompleteRouteDef::addAlternative(RORoute *current, long begin)
{
    delete current;
}


void 
ROCompleteRouteDef::xmlOutCurrent(std::ostream &res) const
{
    res << "   <route id=\"" << _id << "/>" << _edges 
        << "</route>" << endl;
}


void 
ROCompleteRouteDef::xmlOutAlternatives(std::ostream &altres) const
{
    altres << "   <routealt id=\"" << _id
        << "\" last=\"0\">" << endl;
    altres << "      <route cost=\"0\" propability=\"1\">";
    altres << _edges;
    altres << "</route>" << endl;
    altres << "   </routealt>" << endl;
}
