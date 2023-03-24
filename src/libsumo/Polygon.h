/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2023 German Aerospace Center (DLR) and others.
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
/// @file    Polygon.h
/// @author  Gregor L\"ammel
/// @date    15.03.2017
///
// C++ TraCI client API implementation
/****************************************************************************/
#pragma once
#include <string>
#include <vector>
#include <libsumo/TraCIDefs.h>
#include <libsumo/TraCIConstants.h>


// ===========================================================================
// class declarations
// ===========================================================================
#ifndef LIBTRACI
class NamedRTree;
class SUMOPolygon;
class SUMOTrafficObject;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
namespace LIBSUMO_NAMESPACE {
class Polygon {
public:
    static std::string getType(const std::string& polygonID);
    static libsumo::TraCIPositionVector getShape(const std::string& polygonID);
    static libsumo::TraCIColor getColor(const std::string& polygonID);
    static bool getFilled(const std::string& polygonID);
    static double getLineWidth(const std::string& polygonID);

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_SUBSCRIPTION_API

    static void setType(const std::string& polygonID, const std::string& setType);
    static void setShape(const std::string& polygonID, const libsumo::TraCIPositionVector& shape);
    static void setColor(const std::string& polygonID, const libsumo::TraCIColor& color);
    static void add(const std::string& polygonID, const libsumo::TraCIPositionVector& shape, const libsumo::TraCIColor& color, bool fill = false, const std::string& polygonType = "", int layer = 0, double lineWidth = 1);

    static void addDynamics(const std::string& polygonID, const std::string& trackedObjectID = "", const std::vector<double>& timeSpan = std::vector<double>(), const std::vector<double>& alphaSpan = std::vector<double>(), bool looped = false, bool rotate = true);
    static void remove(const std::string& polygonID, int layer = 0);

    static void setFilled(std::string polygonID, bool filled);
    static void setLineWidth(std::string polygonID, double lineWidth);

#ifndef LIBTRACI
#ifndef SWIG
    // currently only used as a Helper function by POI and Vehicle, not part of the public API (and the clients)
    static void addHighlightPolygon(const std::string& objectID, const int type, const std::string& polygonID, const libsumo::TraCIPositionVector& shape, const libsumo::TraCIColor& color, bool fill, const std::string& polygonType, int layer, double lineWidth);

    /** @brief Returns a tree filled with polygon instances
     * @return The rtree of polygons
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

    /// Checks if a polygon of the given name exists already in the simulation
    static bool exists(std::string polyID);

private:
    static SUMOPolygon* getPolygon(const std::string& id);
    /// @brief Obtain a traffic object with the given id if one exists
    /// @return Searches the domains Vehicle and Person for the given id (priorizes vehicles)
    static SUMOTrafficObject* getTrafficObject(const std::string& id);


private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
    static NamedRTree* myTree;
#endif
#endif

    /// @brief invalidated standard constructor
    Polygon() = delete;
};


}
