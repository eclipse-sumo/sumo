#include <string>
#include <iostream>
#include "ROEdge.h"
#include "RORouteDef.h"
#include "RORoute.h"
#include "ROOrigDestRouteDef.h"
#include "RORouter.h"

using namespace std;

ROOrigDestRouteDef::ROOrigDestRouteDef(const std::string &id, 
                                       ROEdge *from, ROEdge *to)
    : RORouteDef(id), _from(from), _to(to), _current(0)
{
}

ROOrigDestRouteDef::~ROOrigDestRouteDef()
{
    delete _current;
}

ROEdge *
ROOrigDestRouteDef::getFrom() const
{
    return _from;
}


ROEdge *
ROOrigDestRouteDef::getTo() const
{
    return _to;
}


RORoute *
ROOrigDestRouteDef::buildCurrentRoute(RORouter &router, long begin)
{
    return new RORoute(_id, 0, 1, router.compute(_from, _to));
}


void 
ROOrigDestRouteDef::addAlternative(RORoute *current, long begin)
{
    _current = current;
}


void 
ROOrigDestRouteDef::xmlOutCurrent(std::ostream &res) const
{
    _current->xmlOut(res);
}


void 
ROOrigDestRouteDef::xmlOutAlternatives(std::ostream &altres) const
{
    altres << "   <routealt id=\"" << _current->getID() 
        << "\" last=\"0\">" << endl;
    altres << "      <route cost=\"0\" propability=\"1\">";
    _current->xmlOutEdges(altres);
    altres << "</route>" << endl;
    altres << "   </routealt>" << endl;
}


