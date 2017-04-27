/****************************************************************************/
/// @file    TraCI_Polygon.h
/// @author  Gregor L\"ammel
/// @date    15.03.2017
/// @version $Id$
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


#ifndef SUMO_TRACI_POLYGON_H
#define SUMO_TRACI_POLYGON_H

#include <string>
#include <vector>
#include <traci-server/TraCIDefs.h>
#include <utils/shapes/Polygon.h>
class TraCI_Polygon {

public:


    static std::vector<std::string> getIDList();
    static std::string getType(const std::string& polygonID);
    static TraCIPositionVector getShape(const std::string& polygonID);
    static TraCIColor getColor(const std::string& polygonID);
    static bool getFilled(const std::string& polygonID);
    static std::string getParameter(const std::string& polygonID, const std::string& paramName);
    static void setType(const std::string& polygonID, const std::string& setType);
    static void setShape(const std::string& polygonID, const TraCIPositionVector& shape);
    static void setColor(const std::string& polygonID, const TraCIColor& c);
    static void add(const std::string& polygonID, const TraCIPositionVector& shape, const TraCIColor& c, bool fill, const std::string& type, int layer);
    static void remove(const std::string& polygonID, int layer = 0);


    static void subscribe(const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars);
    static void subscribeContext(const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<int>& vars);

    static void setFilled(std::string polygonID, bool filled);
    static void setParameter(std::string& name, std::string& value, std::string& string);
private:

    static SUMO::Polygon* getPolygon(const std::string& id);

    /// @brief invalidated standard constructor
    TraCI_Polygon();

    /// @brief invalidated copy constructor
    TraCI_Polygon(const TraCI_Polygon& src);

    /// @brief invalidated assignment operator
    TraCI_Polygon& operator=(const TraCI_Polygon& src);

};

#endif //SUMO_TRACI_POLYGON_H
