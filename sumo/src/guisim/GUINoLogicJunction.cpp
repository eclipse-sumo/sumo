#include <string>
#include <vector>
#include <bitset>
#include <microsim/MSNoLogicJunction.h>
#include "GUINoLogicJunction.h"



GUINoLogicJunction::~GUINoLogicJunction()
{
}


GUINoLogicJunction::GUINoLogicJunction( const std::string &id,
                                       double x, double y,
                                       InLaneCont* in,
                                       const Position2DVector &shape)
    : MSNoLogicJunction(id, x, y, in), myShape(shape)
{
}

