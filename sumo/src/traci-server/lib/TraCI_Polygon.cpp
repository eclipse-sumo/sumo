/****************************************************************************/
/// @file    TraCIAPI.h
/// @author  Gregor L\"ammel
/// @date    15.03.2017
/// @version $Id: TraCI_Polygon.cpp 23205 2017-03-03 11:10:37Z namdre $
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2017 - 2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/



#include <microsim/MSNet.h>
#include <utils/shapes/Polygon.h>
#include <utils/shapes/ShapeContainer.h>

#include "TraCI_Polygon.h"
#include "TraCI.h"

std::vector<std::string> TraCI_Polygon::getIDList()
{
    std::vector<std::string> ids;
    ShapeContainer& shapeCont = MSNet::getInstance()->getShapeContainer();
    shapeCont.getPolygons().insertIDs(ids);
    return ids;
}
std::string TraCI_Polygon::getType(const std::string& polygonID)
{
    return getPolygon(polygonID)->getType();
}
TraCIPositionVector TraCI_Polygon::getShape(const std::string& polygonID)
{

    SUMO::Polygon* p = getPolygon(polygonID);
    return TraCI::makeTraCIPositionVector(p->getShape());
}
bool TraCI_Polygon::getFilled(const std::string& polygonID)
{
    return getPolygon(polygonID)->getFill();
}
TraCIColor TraCI_Polygon::getColor(const std::string& polygonID)
{
    SUMO::Polygon* p = getPolygon(polygonID);
    TraCIColor tc;
    tc.a = p->getColor().alpha();
    tc.b = p->getColor().blue();
    tc.g = p->getColor().green();
    tc.r = p->getColor().red();
    return tc;
}
std::string TraCI_Polygon::getParameter(const std::string& polygonID, const std::string& paramName)
{
    return getPolygon(polygonID)->getParameter(paramName,"");
}
void TraCI_Polygon::setType(const std::string& polygonID, const std::string& setType)
{

}
void TraCI_Polygon::setShape(const std::string& polygonID, const TraCIPositionVector& shape)
{

}
void TraCI_Polygon::setColor(const std::string& polygonID, const TraCIColor& c)
{

}
void
TraCI_Polygon::add(const std::string& polygonID, const TraCIPositionVector& shape, const TraCIColor& c, bool fill, const std::string& type, int layer)
{

}
void TraCI_Polygon::remove(const std::string& polygonID, int layer)
{

}
SUMO::Polygon* TraCI_Polygon::getPolygon(const std::string& id)
{
    return MSNet::getInstance()->getShapeContainer().getPolygons().get(id);
}



