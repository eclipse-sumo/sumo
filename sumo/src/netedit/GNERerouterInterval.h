/****************************************************************************/
/// @file    GNERerouterInterval.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id: GNERerouter.h 22608 2017-01-17 06:28:54Z behrisch $
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

// ===========================================================================
// class declarations
// ===========================================================================

class GNERerouter;
class GNEClosingReroute;
class GNEDestProbReroute;
class GNERouteProbReroute;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNERerouterInterval
 * ------------
 */

class GNERerouterInterval : public std::pair<SUMOReal, SUMOReal> {
public:
    /// @brief constructor
    GNERerouterInterval(SUMOReal begin, SUMOReal end);

    /// @brief destructor
    ~GNERerouterInterval();

    /// @brief insert a new closing reroute
    /// @throw ProcessError if closing reroute was already inserted
    void insertClosingReroutes(GNEClosingReroute* cr);

    /// @brief remove a previously inserted closing reroute
    /// @throw ProcessError if closing reroute cannot be found in the container
    void removeClosingReroutes(GNEClosingReroute* cr);

    /// @brief insert destiny probability reroute
    /// @throw ProcessError if destiny probability reroute was already inserted
    void insertDestProbReroutes(GNEDestProbReroute* dpr);

    /// @brief remove a previously inserted destiny probability reroute
    /// @throw ProcessError if destiny probability reroute cannot be found in the container
    void removeDestProbReroutes(GNEDestProbReroute* dpr);

    /// @brief insert route probability reroute
    /// @throw ProcessError if route probability reroute was already inserted
    void insertRouteProbReroute(GNERouteProbReroute* rpr);

    /// @brief remove a previously inserted route probability reroute
    /// @throw ProcessError if route probability reroute cannot be found in the container
    void removeRouteProbReroute(GNERouteProbReroute* rpr);

    /// @brief get time begin
    SUMOReal getBegin() const;

    /// @brief get time end
    SUMOReal getEnd() const;

    /// @brief get closing reroutes
    std::vector<GNEClosingReroute*> getClosingReroutes() const;

    /// @brief get destiny probability reroutes
    std::vector<GNEDestProbReroute*> getDestProbReroutes() const;

    /// @brief get reoute probability reroutes
    std::vector<GNERouteProbReroute*> getRouteProbReroutes() const;

private:
    /// @brief vector with the closingReroutes
    std::vector<GNEClosingReroute*> myClosingReroutes;

    /// @brief vector with the destProbReroutes
    std::vector<GNEDestProbReroute*> myDestProbReroutes;

    /// @brief vector with the routeProbReroutes
    std::vector<GNERouteProbReroute*> myRouteProbReroutes;
};

#endif

/****************************************************************************/
