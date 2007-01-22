/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <map>
#include <vector>
#include "MSEdgeContinuations.h"

using namespace std;

MSEdgeContinuations::MSEdgeContinuations()
{
}


MSEdgeContinuations::~MSEdgeContinuations()
{
}


void
MSEdgeContinuations::add(MSEdge *to, MSEdge *from)
{
    if(myContinuations.find(to)==myContinuations.end()) {
        myContinuations[to] = vector<MSEdge*>();
    }
    myContinuations[to].push_back(from);
}


const std::vector<MSEdge*> &
MSEdgeContinuations::getInFrontOfEdge(const MSEdge &toEdge) const
{
    return myContinuations.find(static_cast<MSEdge*>(& ((MSEdge&) toEdge)))->second;
}


bool
MSEdgeContinuations::hasFurther(const MSEdge &toEdge) const
{
    return myContinuations.find(static_cast<MSEdge*>(& ((MSEdge&) toEdge)))!=myContinuations.end();
}

