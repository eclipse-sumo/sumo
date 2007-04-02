/****************************************************************************/
/// @file    CellDriverInfoParser.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A parser for single driver informations during FastLane-import
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef CellDriverInfoParser_h
#define CellDriverInfoParser_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fstream>


// ===========================================================================
// class declarations
// ===========================================================================
class RORouteDef;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CellDriverInfoParser
 * @brief A parser for cell driver entries. 
 *
 * As cell files may differ in dependence to the origination system (Intel/Sparc), 
 *  they are read using helper functions and the information whether to switch 
 *  the byte order (SUMO may currently run on one of both systems, too) must be given.
 * Also, it is supported to choose between using the last route or the best
 *  route in the list.
 */
class CellDriverInfoParser
{
public:
    /**
     * A single cell driver entry as read from cell files
     */
    struct Driver
    {
        /// the departure time
        int start;
        /// the number of performed assignments
        int age;
        /// index to the routes
        int route[3];
        /// the probability of the routes
        SUMOReal p[3];
        /// the costs of the routes
        SUMOReal cost[3];
        /// cost of the last route
        int lastcost;
        /// index of the last route
        int lastroute;
    };

public:
    /// Constructor
    CellDriverInfoParser(bool useLast, bool intel);

    /// Destructor
    ~CellDriverInfoParser();

    /// Parses drivers from the given file
    void parseFrom(std::ifstream &strm);

    /// Returns the number of the current route
    int getRouteNo() const;

    /// Returns the begin of the curent route
    int getRouteStart() const;

    /// Returns the (previously computed) index of the last route
    int getLast() const;

    /// Returns the cost of the route specified by her index
    SUMOReal getAlternativeCost(size_t pos) const;

    /// Returns the probability of the route specified by her index
    SUMOReal getAlternativeProbability(size_t pos) const;

    /// Returns the route index within the routes file of the route specified by her index
    int getRouteNo(size_t pos) const;

    /// Sets whether the byte order shall be swapped
    void isIntel(bool value);

    /// Sets the information whether to use the last (or the best) route
    void useLast(bool value);

private:
    /// computes the route index to use
    int computeRouteNo();

private:
    /// Information whether the last route shall be used
    bool _useLast;

    /// Information whether the byte order shall be swapped
    bool _intel;

    /// The current route index
    int _routeNo;

    /// The current driver information
    Driver _driver;

};


#endif

/****************************************************************************/

