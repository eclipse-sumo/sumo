/****************************************************************************/
/// @file    TraCIServerAPI_Polygon.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting polygon values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCIServerAPI_Polygon_h
#define TraCIServerAPI_Polygon_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef NO_TRACI

#include "TraCIException.h"
#include "TraCIServer.h"
#include <foreign/tcpip/storage.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIServerAPI_Polygon
 * @brief APIs for getting/setting polygon values via TraCI
 */
class TraCIServerAPI_Polygon {
public:
    /** @brief Processes a get value command (Command 0xa8: Get Polygon Variable)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /** @brief Processes a set value command (Command 0xc8: Change Polygon State)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /** @brief Returns the named polygons's shape
     *
     * @param[in] id The id of the searched polygon
     * @param[out] shape The shape, if the polygon is known
     * @return Whether the polygon is known
     */
    static bool getShape(const std::string& id, PositionVector& shape);


    /** @brief Returns a tree filled with polygon instances
     * @return The rtree of polygons
     */
    static NamedRTree* getTree();


private:
    /** @brief Returns the named polygon
     * @param[in] id The id of the searched polygon
     * @return The named polygon, or 0 if it is not known
     */
    static Polygon* getPolygon(const std::string& id);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Polygon(const TraCIServerAPI_Polygon& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Polygon& operator=(const TraCIServerAPI_Polygon& s);


};


#endif

#endif

/****************************************************************************/

