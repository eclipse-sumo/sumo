#include <microsim/MSEdge.h>
#include "GUILane.h"
#include "GUILaneChanger.h"



GUILaneChanger::GUILaneChanger( MSEdge::LaneCont* lanes )
    : MSLaneChanger(lanes)
{
}


GUILaneChanger::~GUILaneChanger()
{
}

void
GUILaneChanger::updateLanes()
{
    // Update the lane's vehicle-container.
    // First: it is bad style to change other classes members, but for
    // this release, other attempts were too time-consuming. In a next
    // release we will change from this lane-centered design to a vehicle-
    // centered. This will solve many problems.
    // Second: this swap would be faster if vehicle-containers would have
    // been pointers, but then I had to change too much of the MSLane code.
    for ( ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce ) {
        ce->lane->swapAfterLaneChange();
    }
}


