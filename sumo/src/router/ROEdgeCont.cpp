#include <map>
#include <string>
#include "ROEdge.h"
#include "ROEdgeCont.h"

ROEdgeCont::ROEdgeCont()
{
}


ROEdgeCont::~ROEdgeCont()
{
}


/*
void
ROEdgeCont::computeWeights()
{
    for(myCont::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        (*i).second->computeWeight();
    }
}
*/

void
ROEdgeCont::init()
{
    for(myCont::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        (*i).second->init();
    }
}


