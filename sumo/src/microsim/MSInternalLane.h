#ifndef MSInternalLane_H
#define MSInternalLane_H
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <helpers/PreStartInitialised.h>
#include "MSLogicJunction.h"
#include "MSEdge.h"
#include "MSLane.h"
#include "MSVehicle.h"
#include "MSEdgeControl.h"
#include <bitset>
#include <deque>
#include <vector>
#include <utility>
#include <map>
#include <string>
#include <iostream>
#include "MSNet.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSInternalLaneChanger;
class MSEmitter;
class MSLink;
class MSMoveReminder;
class GUILaneWrapper;
class GUIGlObjectStorage;



/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSInternalLane
 * Class which represents a single lane. Somekind of the main class of the
 * simulation. Allows moving vehicles.
 */
class MSInternalLane : public MSLane
{
public:
    MSInternalLane( MSNet &net, std::string id, double maxSpeed,
        double length, MSEdge *e);
    ~MSInternalLane();
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#ifndef DISABLE_INLINE
//#include "MSInternalLane.icc"
#endif

#endif

// Local Variables:
// mode:C++
// End:
