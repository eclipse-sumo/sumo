#ifndef CellDriverInfoParser_h
#define CellDriverInfoParser_h
//---------------------------------------------------------------------------//
//                        CellDriverInfoParser.h -
//  A parser for single driver informations during FastLane-import
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.8  2006/01/26 08:37:23  dkrajzew
// removed warnings 4786
//
// Revision 1.7  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2004/12/16 12:26:52  dkrajzew
// debugging
//
// Revision 1.3  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <fstream>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RORouteDef;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class CellDriverInfoParser
 * A parser for cell driver entries. As cell files may differ in dependence
 * to the origination system (Intel/Sparc), they are read using helper functions
 * and the information whether to switch the byte order (SUMO may currently
 * run on one of both systems, too) must be given.
 * Also, it is supported to choose between using the last route or the best
 * route in the list.
 */
class CellDriverInfoParser {
public:
    /**
     * A single cell driver entry as read from cell files
     */
    struct Driver {
        /// the departure time
        int start;

        /// the number of performed assignments
        int age;

        /// index to the routes
        int route[3];

        /// the propability of the routes
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

    /// Returns the propability of the route specified by her index
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

