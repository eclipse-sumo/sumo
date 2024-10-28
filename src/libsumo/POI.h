/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    POI.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
#include <vector>
#include <libsumo/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
#ifndef LIBTRACI
class NamedRTree;
class PointOfInterest;
class PositionVector;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class POI
 * @brief C++ TraCI client API implementation
 */
namespace LIBSUMO_NAMESPACE {
class POI {
public:
    static std::string getType(const std::string& poiID);
    static libsumo::TraCIPosition getPosition(const std::string& poiID, const bool includeZ = false);
    static libsumo::TraCIColor getColor(const std::string& poiID);
    static double getWidth(const std::string& poiID);
    static double getHeight(const std::string& poiID);
    static double getAngle(const std::string& poiID);
    static std::string getImageFile(const std::string& poiID);

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_SUBSCRIPTION_API

    static void setType(const std::string& poiID, const std::string& poiType);
    static void setColor(const std::string& poiID, const libsumo::TraCIColor& color);
    static void setPosition(const std::string& poiID, double x, double y);
    static void setWidth(const std::string& poiID, double width);
    static void setHeight(const std::string& poiID, double height);
    static void setAngle(const std::string& poiID, double angle);
    static void setImageFile(const std::string& poiID, const std::string& imageFile);
    static bool add(const std::string& poiID, double x, double y, const libsumo::TraCIColor& color,
                    const std::string& poiType = "", int layer = 0, const std::string& imgFile = "",
                    double width = 1, double height = 1, double angle = 0, const std::string& icon = "");
    static bool remove(const std::string& poiID, int layer = 0);
    static void highlight(const std::string& poiID, const libsumo::TraCIColor& col = libsumo::TraCIColor(255, 0, 0, 255),
                          double size = -1, const int alphaMax = -1, const double duration = -1, const int type = 0);


#ifndef LIBTRACI
#ifndef SWIG
    /** @brief Returns a tree filled with PoI instances
     *  @return The rtree of PoIs
     */
    static NamedRTree* getTree();
    static void cleanup();

    /** @brief Saves the shape of the requested object in the given container
    *  @param id The id of the poi to retrieve
    *  @param shape The container to fill
    */
    static void storeShape(const std::string& id, PositionVector& shape);

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData);

private:
    static PointOfInterest* getPoI(const std::string& id);


private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
    static NamedRTree* myTree;
#endif
#endif

    /// @brief invalidated standard constructor
    POI() = delete;
};


}
