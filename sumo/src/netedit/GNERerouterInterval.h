/****************************************************************************/
/// @file    GNERerouterInterval.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id$
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
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

#include <vector>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "GNERerouter.h"

#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEDestProbReroute.h"
#include "GNERouteProbReroute.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNERerouterInterval
 * class used to represent a interval used in rerouters
 */
class GNERerouterInterval {
public:
    /// @brief constructor
    GNERerouterInterval(GNERerouter* rerouterParent, SUMOReal begin, SUMOReal end);

    /// @brief destructor
    ~GNERerouterInterval();

    /**@brief insert a new closing lane reroute
     * @return true if insertion was sucesfully, false if is duplicated
     */
    bool insertClosinLanegReroutes(const GNEClosingLaneReroute& clr);

    /**@brief remove a previously inserted closing reroute
     * @return true if deletetion was sucesfully, false if wasn't found
     */
    bool removeClosingLaneReroutes(const GNEClosingLaneReroute& clr);

    /**@brief insert a new closing lane reroute
     * @return true if insertion was sucesfully, false if is duplicated
     */
    bool insertClosingReroutes(const GNEClosingReroute& cr);

    /**@brief remove a previously inserted closing reroute
     * @return true if deletetion was sucesfully, false if wasn't found
     */
    bool removeClosingReroutes(const GNEClosingReroute& cr);

    /**@brief insert destiny probability reroute
     * @return true if deletetion was sucesfully, false if wasn't found
     */
    bool insertDestProbReroutes(const GNEDestProbReroute& dpr);

    /**@brief remove a previously inserted destiny probability reroute
     * @return true if deletetion was sucesfully, false if wasn't found
     */
    bool removeDestProbReroutes(const GNEDestProbReroute& dpr);

    /**@brief insert route probability reroute
     * @return true if insertion was sucesfully, false if is duplicated
     */
    bool insertRouteProbReroute(const GNERouteProbReroute& rpr);

    /**@brief remove a previously inserted route probability reroute
     * @return true if deletetion was sucesfully, false if wasn't found
     */
    bool removeRouteProbReroute(const GNERouteProbReroute& rpr);

    /// @brief get rerouter parent
    GNERerouter* getRerouterParent() const;

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get begin time
    SUMOReal getBegin() const;

    /// @brief get end time
    SUMOReal getEnd() const;

    /// @brief set begin time
    void setBegin(SUMOReal begin);

    /// @brief set end time
    void setEnd(SUMOReal end);

    /// @brief get closing reroutes
    const std::vector<GNEClosingLaneReroute>& getClosingLaneReroutes() const;

    /// @brief get closing reroutes
    const std::vector<GNEClosingReroute>& getClosingReroutes() const;

    /// @brief get destiny probability reroutes
    const std::vector<GNEDestProbReroute>& getDestProbReroutes() const;

    /// @brief get reoute probability reroutes
    const std::vector<GNERouteProbReroute>& getRouteProbReroutes() const;

    /// @brief set closing reroutes
    void setClosingLaneReroutes(const std::vector<GNEClosingLaneReroute>& closingLaneReroutes);

    /// @brief set closing reroutes
    void setClosingReroutes(const std::vector<GNEClosingReroute>& closingReroutes);

    /// @brief set destiny probability reroutes
    void setDestProbReroutes(const std::vector<GNEDestProbReroute>& destProbReroutes);

    /// @brief set reoute probability reroutes
    void setRouteProbReroutes(const std::vector<GNERouteProbReroute>& rerouteProbabilityReroutes);

    /// @brief overload operator =
    GNERerouterInterval& operator=(const GNERerouterInterval& rerouterInterval);

protected:
    /// @brief pointer to rerouter parent
    GNERerouter* myRerouterParent;

    /// @brief begin timeStep
    SUMOReal myBegin;

    /// @brief end timeStep
    SUMOReal myEnd;

    /// @brief XML Tag of a rerouter interval
    SumoXMLTag myTag;

    /// @brief vector with the closingLaneReroutes
    std::vector<GNEClosingLaneReroute> myClosingLaneReroutes;

    /// @brief vector with the closingReroutes
    std::vector<GNEClosingReroute> myClosingReroutes;

    /// @brief vector with the destProbReroutes
    std::vector<GNEDestProbReroute> myDestProbReroutes;

    /// @brief vector with the routeProbReroutes
    std::vector<GNERouteProbReroute> myRouteProbReroutes;
};

#endif

/****************************************************************************/
