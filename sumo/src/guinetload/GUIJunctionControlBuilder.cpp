#include <netload/NLJunctionControlBuilder.h>
#include <utils/geom/Position2DVector.h>
#include "GUIJunctionControlBuilder.h"
#include <guisim/GUINoLogicJunction.h>
#include <guisim/GUIRightOfWayJunction.h>


GUIJunctionControlBuilder::GUIJunctionControlBuilder()
    : NLJunctionControlBuilder()
{
}


GUIJunctionControlBuilder::~GUIJunctionControlBuilder()
{
}


void
GUIJunctionControlBuilder::addJunctionShape(const Position2DVector &shape)
{
    myShape = shape;
}


MSJunction *
GUIJunctionControlBuilder::buildNoLogicJunction()
{
    MSNoLogicJunction::InLaneCont *cont =
        new MSNoLogicJunction::InLaneCont();
    cont->reserve(m_pActiveInLanes.size());
    for(LaneCont::iterator i=m_pActiveInLanes.begin();
            i!=m_pActiveInLanes.end(); i++) {
        cont->push_back(*i);
    }
    return new GUINoLogicJunction(m_CurrentId, m_X, m_Y, cont, myShape);
    myShape.clear();
}


MSJunction *
GUIJunctionControlBuilder::buildLogicJunction()
{
    MSJunctionLogic *jtype = getJunctionLogicSecure();
    MSRightOfWayJunction::InLaneCont cont = getInLaneContSecure();
    // build the junction
    return new GUIRightOfWayJunction(m_CurrentId, m_X, m_Y,
        cont, jtype, myShape);
    myShape.clear();
}


MSJunction *
GUIJunctionControlBuilder::buildTrafficLightJunction()
{
    throw 1;
}


