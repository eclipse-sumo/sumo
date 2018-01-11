/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDestProbReroute.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id$
///
//
/****************************************************************************/
#ifndef GNEDestProbReroute_h
#define GNEDestProbReroute_h


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

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNEEdge;
class GNERerouterInterval;
class GNERerouterIntervalDialog;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDestProbReroute
 * forces the rerouter to assign a new route
 */
class GNEDestProbReroute : public GNEAttributeCarrier {

public:
    /// @brief constructor (Used in GNERerouterIntervalDialog)
    GNEDestProbReroute(GNERerouterIntervalDialog* rerouterIntervalDialog);

    /// @brief constructor
    GNEDestProbReroute(GNERerouterInterval* rerouterIntervalParent, GNEEdge* newEdgeDestination, double probability);

    /// @brief destructor
    ~GNEDestProbReroute();

    // @brief write destiny probability reroute
    void writeDestProbReroute(OutputDevice& device) const;

    /// @brief get rerouter interval parent
    GNERerouterInterval* getRerouterIntervalParent() const;

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
    * @param[in] key The attribute key
    * @return string with the value associated to key
    */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
    * @param[in] key The attribute key
    * @param[in] value The new value
    * @param[in] undoList The undoList on which to register changes
    */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their correspond attribute are valids
    * @param[in] key The attribute key
    * @param[in] value The value asociated to key key
    * @return true if the value is valid, false in other case
    */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

protected:
    /// @brief reference to rerouter interval parent
    GNERerouterInterval* myRerouterIntervalParent;

    /// @brief id of new edge destination
    GNEEdge* myNewEdgeDestination;

    /// @brief probability with which a vehicle will use the given edge as destination
    double myProbability;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEDestProbReroute(const GNEDestProbReroute&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDestProbReroute& operator=(const GNEDestProbReroute&) = delete;
};

#endif

/****************************************************************************/
