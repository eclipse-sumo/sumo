/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNECalibratorRoute.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// Route used by GNECalibrators
/****************************************************************************/
#ifndef GNECalibratorRoute_h
#define GNECalibratorRoute_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/RGBColor.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNECalibrator;
class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECalibratorRoute
 * vehicle route used by GNECalibrators
 */
class GNECalibratorRoute {
public:

    /// @brief default constructor
    GNECalibratorRoute(GNECalibrator* calibratorParent);

    /// @brief parameter constructor 1 (Using edges IDs)
    GNECalibratorRoute(GNECalibrator* calibratorParent, std::string routeID, std::vector<std::string> edges, const RGBColor& color);

    /// @brief parameter constructor 2 (Using GNEEdges)
    GNECalibratorRoute(GNECalibrator* calibratorParent, std::string routeID, std::vector<GNEEdge*> edges, const RGBColor& color);

    /// @brief destructor
    ~GNECalibratorRoute();

    /// @brief get pointer to calibrator parent
    GNECalibrator* getCalibratorParent() const;

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get route ID
    const std::string& getRouteID() const;

    /// @brief get IDs of Edges
    std::vector<std::string> getEdgesIDs() const;

    /// @brief get edges
    const std::vector<GNEEdge*>& getEdges() const;

    /// @brief get color of route
    const RGBColor& getColor() const;

    /**@brief set route ID
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setRouteID(std::string routeID);

    /**@brief set edges of route using IDs
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setEdges(const std::vector<std::string>& edgeIDs);

    /**@brief set edges of route using pointers
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setEdges(const std::vector<GNEEdge*>& edges);

    /**@brief set edges of route using a single string
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setEdges(const std::string& edgeIDs);

    /**@brief set color of route
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setColor(const RGBColor& color);

    /**@brief set color of route (String Version
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setColor(std::string color = "black");

    /**@brief check if a list of edges is valid to set a route
    * @return "" if is correct, a error string in other case
    */
    std::string checkEdgeRoute(const std::vector<std::string>& edge) const;

    /// @brief overload operator ==
    bool operator==(const GNECalibratorRoute& calibratorRoute) const;

private:
    /// @brief pointer to calibrator parent
    GNECalibrator* myCalibratorParent;

    /// @brief route in which this flow is used
    std::string myRouteID;

    /// @brief edges of route
    std::vector<GNEEdge*> myEdges;

    /// @brief color of flow
    RGBColor myColor;
};

#endif
/****************************************************************************/
