#include "MSLink.h"

/////////////////////////////////////////////////////////////////////////////

MSLink::MSLink(MSLane* succLane, bool yield) :
    myLane(succLane), myPrio(!yield), myDriveRequest(false)
{
}

/////////////////////////////////////////////////////////////////////////////

void
MSLink::setPriority( bool prio )
{
    myPrio = prio;
}

/////////////////////////////////////////////////////////////////////////////

MSLink::LinkRequest::result_type
MSLink::LinkRequest::operator() (first_argument_type MSLink) const
{
    return MSLink->myDriveRequest;
}

