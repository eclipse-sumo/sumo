#ifndef RORDLoader_Cell_h
#define RORDLoader_Cell_h
//---------------------------------------------------------------------------//
//                        RORDLoader_Cell.h -
//  A handler for reading FastLane-files
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
// Revision 1.8  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:50:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.3  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.2  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ------------------------------------------------
// Revision 1.5  2003/04/09 15:39:10  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.4  2003/03/20 16:39:16  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/03/03 15:08:20  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
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

#include <string>
#include <vector>
#include "ROAbstractRouteDefLoader.h"
#include "CellDriverInfoParser.h"
#include "ROEdgeVector.h"
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/router/IDSupplier.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class Options;
class RORoute;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class RORDLoader_Cell
 * A ROAbstractRouteDefLoader for loading cell driver files. As route information
 * within cell are splitted into two files, both must be processed. Further,
 * the route file is a list of string lists where evry string is an edge id.
 * This attempt and the unordered structure within the route file - the routes
 * are unsorted and are accessed via their index - disallows fast processing
 * when every route would have to be searched at first - even when simple line-
 * counting is performed. Due to this, a further file is build before the
 * parsing - a route index file which allows to move faster to the place the
 * route is located at.
 */
class RORDLoader_Cell :
            public LineHandler,
            public ROAbstractRouteDefLoader {
public:
    /// Constructor
    RORDLoader_Cell(ROVehicleBuilder &vb, RONet &net,
        SUMOTime begin, SUMOTime end,
        SUMOReal gawronBeta, SUMOReal gawronA, int maxRoutes,
        std::string file="");

    /// Destructor
    ~RORDLoader_Cell();

    /* ----- from the LineHandler - "interface" ----- */
    /** @brief Receives input from a line reader (watch full description!)
        Here, either input from the route file or from the route index file
        (when existing) is received. In the first case, the list of route
        indices is build - and read in the second case */
    bool report(const std::string &result);

    /* ----- from the "RORoutesHandler - "interface" ----- */
    /// Ends the reading process
    void closeReading();

    /// Returns the data name, here "cell routes"
    std::string getDataName() const;

    /// Returns the information whether no routes are available from this loader anymore
    bool ended() const;

    /// Returns the time the current (last read) route starts at
    SUMOTime getCurrentTimeStep() const;

    /// Initialises the handler for reading
    virtual bool init(OptionsCont &_options);

protected:
    /** @brief reads the next route
        The next entry of the driver file is read and the index of the route
        to use is extracted. Afterwards, the route itself is read from the
        route file */
    bool myReadRoutesAtLeastUntil(SUMOTime time);

private:
    /// Initialises the driver file for reading
    bool initDriverFile();

    /// Returns the alternative specified by its position from the current driver entry
    RORoute *getAlternative(size_t pos);

    /// Returns the route from with the given index from the route file
    ROEdgeVector *getRouteInfoFrom(unsigned long routeNo);

private:
    /// Gives route ids
    IDSupplier _routeIdSupplier;

    /// Gives vehicle ids
    IDSupplier _vehicleIdSupplier;

    /// Class to read either from the route or from the route index file
    LineReader _lineReader;

    /// The name of the driver file
    std::string     _driverFile;

    /// The stream reding the driver file
    std::ifstream    _driverStrm;

    /// The name of the file containing the routes
    std::string     _routeDefFile;

    /// The name of the file containing indices of the routes
    std::string     _routeIdxFile;

    /// Byte position of routes within the route file
    std::vector<unsigned long> _routes;

    /// A parser for single driver structures
    CellDriverInfoParser _driverParser;

    /// Information whether an index file exists
    bool _hasIndexFile;

    /// Information whether the byte shall be swapped
    bool _isIntel;

    /// Values for Christian Gawron's DUA
    SUMOReal _gawronBeta, _gawronA;

    /// The time step read as the last one
    SUMOTime myCurrentTime;

    /// The information whether no further routes exist
    bool myHaveEnded;

    /// The number of maximum alternatives
    int myMaxRoutes;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

