/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERerouterInterval.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id$
///
//
/****************************************************************************/
#ifndef GNERerouterInterval_h
#define GNERerouterInterval_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNERerouter;
class GNERerouterDialog;
class GNEClosingLaneReroute;
class GNEClosingReroute;
class GNEDestProbReroute;
class GNERouteProbReroute;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNERerouterInterval
 * class used to represent a interval used in rerouters
 */
class GNERerouterInterval : public GNEAttributeCarrier {
    /// @brief declare friend class
    friend class GNEChange_RerouterItem;
    friend class GNEAdditionalHandler;

public:
    /// @brief constructor (Used in GNERerouterDialog)
    GNERerouterInterval(GNERerouterDialog* rerouterDialog);

    /// @brief constructor
    GNERerouterInterval(GNERerouter* rerouterParent, double begin, double end);

    /// @brief destructor
    ~GNERerouterInterval();

    /// @brief write Interval and all of their values
    void writeRerouterInterval(OutputDevice& device) const;

    /// @brief get rerouter parent
    GNERerouter* getRerouterParent() const;

    /// @brief get begin
    double getBegin() const;

    /// @brief get end
    double getEnd() const;

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

    /// @brief get closing reroutes
    const std::vector<GNEClosingLaneReroute*>& getClosingLaneReroutes() const;

    /// @brief get closing reroutes
    const std::vector<GNEClosingReroute*>& getClosingReroutes() const;

    /// @brief get destiny probability reroutes
    const std::vector<GNEDestProbReroute*>& getDestProbReroutes() const;

    /// @brief get reoute probability reroutes
    const std::vector<GNERouteProbReroute*>& getRouteProbReroutes() const;

protected:
    /// @brief pointer to rerouter parent
    GNERerouter* myRerouterParent;

    /// @brief begin timeStep
    double myBegin;

    /// @brief end timeStep
    double myEnd;

    /// @brief vector with the closingLaneReroutes
    std::vector<GNEClosingLaneReroute*> myClosingLaneReroutes;

    /// @brief vector with the closingReroutes
    std::vector<GNEClosingReroute*> myClosingReroutes;

    /// @brief vector with the destProbReroutes
    std::vector<GNEDestProbReroute*> myDestProbReroutes;

    /// @brief vector with the routeProbReroutes
    std::vector<GNERouteProbReroute*> myRouteProbReroutes;

    /// @brief add closing reroute
    void addClosingLaneReroute(GNEClosingLaneReroute* closingLaneReroute);

    /// @brief add closing reroute
    void removeClosingLaneReroute(GNEClosingLaneReroute* closingLaneReroute);

    /// @brief add closing reroute
    void addClosingReroute(GNEClosingReroute* closingReroute);

    /// @brief add closing reroute
    void removeClosingReroute(GNEClosingReroute* closingReroute);

    /// @brief add destiny probability reroute
    void addDestProbReroute(GNEDestProbReroute* destProbReroute);

    /// @brief add destiny probability reroute
    void removeDestProbReroute(GNEDestProbReroute* destProbReroute);

    /// @brief add reoute probability reroute
    void addRouteProbReroute(GNERouteProbReroute* routeProbabilityReroute);

    /// @brief add reoute probability reroute
    void removeRouteProbReroute(GNERouteProbReroute* routeProbabilityReroute);

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNERerouterInterval(GNERerouterInterval*) = delete;

    /// @brief Invalidated assignment operator.
    GNERerouterInterval& operator=(GNERerouterInterval*) = delete;
};

#endif

/****************************************************************************/
