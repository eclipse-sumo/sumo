/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_Polygon.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting polygon values via TraCI
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


private:
    /** @brief Returns the named polygon
     * @param[in] id The id of the searched polygon
     * @return The named polygon, or 0 if it is not known
     */
    static SUMOPolygon* getPolygon(const std::string& id);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Polygon(const TraCIServerAPI_Polygon& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Polygon& operator=(const TraCIServerAPI_Polygon& s);


};


#endif

/****************************************************************************/

