/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Polygon.h
/// @author  Gregor L\"ammel
/// @date    15.03.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef SUMO_Polygon_H
#define SUMO_Polygon_H


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <vector>
#include <libsumo/TraCIDefs.h>
#include <libsumo/TraCIConstants.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NamedRTree;
class SUMOPolygon;
namespace libsumo {
class VariableWrapper;
}


// ===========================================================================
// class definitions
// ===========================================================================
namespace libsumo {
class Polygon {
public:
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static std::string getType(const std::string& polygonID);
    static TraCIPositionVector getShape(const std::string& polygonID);
    static TraCIColor getColor(const std::string& polygonID);
    static bool getFilled(const std::string& polygonID);
    static double getLineWidth(const std::string& polygonID);
    static std::string getParameter(const std::string& polygonID, const std::string& key);
    static void setType(const std::string& polygonID, const std::string& setType);
    static void setShape(const std::string& polygonID, const TraCIPositionVector& shape);
    static void setColor(const std::string& polygonID, const TraCIColor& c);
    static void add(const std::string& polygonID, const TraCIPositionVector& shape, const TraCIColor& color, bool fill = false, double lineWidth = 1, const std::string& polygonType = "", int layer = 0);
    static void remove(const std::string& polygonID, int layer = 0);

    static void setFilled(std::string polygonID, bool filled);
    static void setLineWidth(std::string polygonID, double lineWidth);
    static void setParameter(const std::string& polygonID, const std::string& key, const std::string& value);

    LIBSUMO_SUBSCRIPTION_API

    /** @brief Returns a tree filled with polygon instances
     * @return The rtree of polygons
     */
    static NamedRTree* getTree();

    /** @brief Saves the shape of the requested object in the given container
    *  @param id The id of the poi to retrieve
    *  @param shape The container to fill
    */
    static void storeShape(const std::string& id, PositionVector& shape);

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper);

private:
    static SUMOPolygon* getPolygon(const std::string& id);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;

    /// @brief invalidated standard constructor
    Polygon() = delete;
};


}


#endif //SUMO_Polygon_H

/****************************************************************************/
