#include <string>
#include <utils/common/Named.h>
#include <utils/common/SErrorHandler.h>
#include "ROEdge.h"
#include "RORoute.h"
#include "RORouter.h"
#include "ReferencedItem.h"
#include "RORouteDef.h"

using namespace std;


RORouteDef::RORouteDef(const std::string &id)
    : ReferencedItem(), Named(id)
{
}

RORouteDef::~RORouteDef()
{
}


bool 
RORouteDef::computeAndSave(RORouter &router, long begin,
                           std::ostream &res, std::ostream &altres)
{
    RORoute *current = buildCurrentRoute(router, begin);
    if(current->size()<2) {
        SErrorHandler::add(
            string("The route '") + _id 
            + string("' is too short, propably ending at the starting edge."));
        SErrorHandler::add("Skipping...");
        delete current;
        return false;
    }
    addAlternative(current, begin);
    xmlOutCurrent(res);
//    current->xmlOut(res);
    xmlOutAlternatives(altres);
    return true;
}


