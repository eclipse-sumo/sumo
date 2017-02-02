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

// ===========================================================================
// class declarations
// ===========================================================================

class GNERerouter;
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
class GNERerouterInterval : public std::pair<SUMOReal, SUMOReal> {
public:
    /// @brief constructor
    GNERerouterInterval(GNERerouter *rerouterParent, SUMOReal begin, SUMOReal end);

    /// @brief copy constructor using a pointer (used in Rerouter dialog)
    GNERerouterInterval(GNERerouterInterval* rerouterInterval);

    /// @brief destructor
    ~GNERerouterInterval();

    /**@brief insert a new closing lane reroute
     * @throw ProcessError if closing lane reroute was already inserted
     */
    void insertClosinLanegReroutes(GNEClosingLaneReroute* clr);

    /**@brief remove a previously inserted closing reroute
     * @throw ProcessError if closing lane reroute cannot be found
     */
    void removeClosingLaneReroutes(GNEClosingLaneReroute* clr);

    /**@brief insert a new closing lane reroute
     * @throw ProcessError if closing reroute was already inserted
     */
    void insertClosingReroutes(GNEClosingReroute* cr);

    /**@brief remove a previously inserted closing reroute
     * @throw ProcessError if closing reroute cannot be found
     */
    void removeClosingReroutes(GNEClosingReroute* cr);

    /**@brief insert destiny probability reroute
     * @throw ProcessError if destiny probability reroute was already inserted
     */
    void insertDestProbReroutes(GNEDestProbReroute* dpr);

    /**@brief remove a previously inserted destiny probability reroute
     * @throw ProcessError if destiny probability reroute cannot be found
     */
    void removeDestProbReroutes(GNEDestProbReroute* dpr);

    /**@brief insert route probability reroute
     * @throw ProcessError if route probability reroute was already inserted
     */
    void insertRouteProbReroute(GNERouteProbReroute* rpr);

    /**@brief remove a previously inserted route probability reroute
     * @throw ProcessError if route probability reroute cannot be found
     */
    void removeRouteProbReroute(GNERouteProbReroute* rpr);

    /// @brief get rerouter parent
    GNERerouter *getRerouterParent();

    /// @brief get begin time
    SUMOReal getBegin() const;

    /// @brief get end time
    SUMOReal getEnd() const;

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get closing reroutes
    const std::vector<GNEClosingLaneReroute*> &getClosingLaneReroutes() const;

    /// @brief get closing reroutes
    const std::vector<GNEClosingReroute*> &getClosingReroutes() const;

    /// @brief get destiny probability reroutes
    const std::vector<GNEDestProbReroute*> &getDestProbReroutes() const;

    /// @brief get reoute probability reroutes
    const std::vector<GNERouteProbReroute*> &getRouteProbReroutes() const;

protected:
    /// @brief poniter to rerouter parent
    GNERerouter *myRerouterParent;

    /// @brief XML Tag of a rerouter interval
    SumoXMLTag myTag;

    /// @brief vector with the closingLaneReroutes
    std::vector<GNEClosingLaneReroute*> myClosingLaneReroutes;

    /// @brief vector with the closingReroutes
    std::vector<GNEClosingReroute*> myClosingReroutes;

    /// @brief vector with the destProbReroutes
    std::vector<GNEDestProbReroute*> myDestProbReroutes;

    /// @brief vector with the routeProbReroutes
    std::vector<GNERouteProbReroute*> myRouteProbReroutes;

private:
    /// @brief Invalidated copy constructor.
    GNERerouterInterval(const GNERerouterInterval&);

    /// @brief Invalidated assignment operator.
    GNERerouterInterval& operator=(const GNERerouterInterval&);
};

#endif

/****************************************************************************/
