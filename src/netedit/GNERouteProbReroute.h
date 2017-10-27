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
/// @file    GNERouteProbReroute.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id$
///
//
/****************************************************************************/
#ifndef GNERouteProbReroute_h
#define GNERouteProbReroute_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEEdge;
class GNERerouterInterval;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNERerouter
 * GNERouteProbReroute
 */
class GNERouteProbReroute {
public:
    /// @brief constructor
    GNERouteProbReroute(GNERerouterInterval& rerouterIntervalParent, std::string newRouteId, double probability);

    /// @brief destructor
    ~GNERouteProbReroute();

    /// @brief get new route id
    std::string getNewRouteId() const;

    /// @brief set new route id
    void setNewRouteId(std::string newRouteId);

    /// @brief get probability
    double getProbability() const;

    /// @brief set probability, if the new probability is valid
    bool setProbability(double probability);

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get rerouter interval parent
    const GNERerouterInterval& getRerouterIntervalParent() const;

    /// @brief overload == operator
    bool operator==(const GNERouteProbReroute& routeProbReroute) const;

protected:
    /// @brief reference to rerouter interval parent
    GNERerouterInterval* myRerouterIntervalParent;

    /// @brief id of new route
    /// @todo change string to GNERoute
    std::string myNewRouteId;

    /// @brief probability with which a vehicle will use the given edge as destination
    double myProbability;

    /// @brief XML Tag of route probability reroute
    SumoXMLTag myTag;
};

#endif

/****************************************************************************/
