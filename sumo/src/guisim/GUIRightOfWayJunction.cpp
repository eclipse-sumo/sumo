#include <microsim/MSRightOfWayJunction.h>
#include <bitset>
#include <vector>
#include <string>
#include "GUIJunctionWrapper.h"
#include "GUIRightOfWayJunction.h"


GUIRightOfWayJunction::~GUIRightOfWayJunction()
{
}


GUIRightOfWayJunction::GUIRightOfWayJunction( const std::string &id,
                                             double x, double y,
                                             InLaneCont in,
                                             MSJunctionLogic* logic,
                                             const Position2DVector &shape)
    : MSRightOfWayJunction(id, x, y, in, logic), myShape(shape)
{
}


GUIJunctionWrapper *
GUIRightOfWayJunction::buildJunctionWrapper(GUIGlObjectStorage &idStorage)
{
    return new GUIJunctionWrapper(idStorage, *this, myShape);
}
