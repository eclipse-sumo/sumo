#include <microsim/MSRightOfWayJunction.h>
#include <bitset>
#include <vector>
#include <string>
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
