#include <cassert>
#include <utils/geom/Boundery.h>
#include "NIVissimBoundedClusterObject.h"

NIVissimBoundedClusterObject::ContType NIVissimBoundedClusterObject::myDict;

NIVissimBoundedClusterObject::NIVissimBoundedClusterObject()
    : myClusterID(-1),
    myBoundery(0)
{
    myDict.insert(this);
}


NIVissimBoundedClusterObject::~NIVissimBoundedClusterObject()
{
    delete myBoundery;
}


bool
NIVissimBoundedClusterObject::crosses(const AbstractPoly &poly,
                                      double offset) const
{
    assert(myBoundery!=0&&myBoundery->xmax()>=myBoundery->xmin());
    return myBoundery->overlapsWith(poly, offset);
}


void
NIVissimBoundedClusterObject::inCluster(int id)
{
    myClusterID = id;
}


bool
NIVissimBoundedClusterObject::clustered() const
{
    return myClusterID>0;
}


void
NIVissimBoundedClusterObject::closeLoading()
{
    for(ContType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i)->computeBounding();
    }
}



