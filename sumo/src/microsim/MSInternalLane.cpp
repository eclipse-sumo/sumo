namespace
{
    const char rcsid[] =
    "$Id$";
}
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <helpers/PreStartInitialised.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSEdge.h"
#include "MSJunction.h"
#include "MSLogicJunction.h"
#include "MSInductLoop.h"
#include "MSLink.h"
#include "MSInternalLane.h"
#include <cmath>
#include <bitset>
#include <iostream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <iterator>
#include <exception>
#include <climits>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * member method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * methods of MSInternalLane
 * ----------------------------------------------------------------------- */

MSInternalLane::MSInternalLane( MSNet &net, string id, double maxSpeed,
                               double length, MSEdge *e)
    :
    MSLane(net, id, maxSpeed, length, e)
{
}


MSInternalLane::~MSInternalLane()
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "MSInternalLane.icc"
//#endif

// Local Variables:
// mode:C++
// End:
