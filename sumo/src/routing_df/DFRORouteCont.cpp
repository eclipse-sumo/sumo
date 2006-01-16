/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include "DFRORouteDesc.h"
#include "DFRORouteCont.h"


DFRORouteCont::DFRORouteCont()
{
}


DFRORouteCont::~DFRORouteCont()
{
}


void
DFRORouteCont::addRouteDesc(const DFRORouteDesc &desc)
{
    ROEdge *start = desc.edges2Pass[0];
    if(myRoutes.find(start)==myRoutes.end()) {
        myRoutes[start] = std::vector<DFRORouteDesc>();
    }
    myRoutes[start].push_back(desc);
}


bool
DFRORouteCont::readFrom(const std::string &file)
{
    throw 1;
}


bool
DFRORouteCont::save(const std::string &file)
{
    throw 1;
}


bool
DFRORouteCont::computed() const
{
    return myRoutes.size()!=0;
}


