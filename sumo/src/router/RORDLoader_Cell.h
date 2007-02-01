/****************************************************************************/
/// @file    RORDLoader_Cell.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// A handler for reading FastLane-files
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
#ifndef RORDLoader_Cell_h
#define RORDLoader_Cell_h
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

#include <string>
#include <vector>
#include "ROAbstractRouteDefLoader.h"
#include "CellDriverInfoParser.h"
#include "ROEdgeVector.h"
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/router/IDSupplier.h>


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class Options;
class RORoute;


// ===========================================================================
// class definitions
// ===========================================================================
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
            public ROAbstractRouteDefLoader
{
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


#endif

/****************************************************************************/

