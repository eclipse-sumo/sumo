#ifndef ROCellRouteDefHandler_h
#define ROCellRouteDefHandler_h
//---------------------------------------------------------------------------//
//                        ROCellRouteDefHandler.h -
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
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include "ROTypedRoutesLoader.h"
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
 * @class ROCellRouteDefHandler
 * A ROTypedRoutesLoader for loading cell driver files. As route information
 * within cell are splitted into two files, both must be processed. Further,
 * the route file is a list of string lists where evry string is an edge id.
 * This attempt and the unordered structure within the route file - the routes
 * are unsorted and are accessed via their index - disallows fast processing
 * when every route would have to be searched at first - even when simple line-
 * counting is performed. Due to this, a further file is build before the
 * parsing - a route index file which allows to move faster to the place the
 * route is located at.
 */
class ROCellRouteDefHandler :
            public LineHandler,
            public ROTypedRoutesLoader {
public:
    /// Constructor
    ROCellRouteDefHandler(RONet &net, double gawronBeta, double gawronA,
        std::string file="");

    /// Destructor
    ~ROCellRouteDefHandler();

    /* ----- from the LineHandler - "interface" ----- */
    /** @brief Receives input from a line reader (watch full description!)
        Here, either input from the route file or from the route index file
        (when existing) is received. In the first case, the list of route
        indices is build - and read in the second case */
    bool report(const std::string &result);

    /* ----- from the "RORoutesHandler - "interface" ----- */
    /// Retrieves a duplicate parsing the given file
    ROTypedRoutesLoader *getAssignedDuplicate(const std::string &file) const;

    /// Ends the reading process
    void closeReading();

    /// Returns the data name, here "cell routes"
    std::string getDataName() const;

    /// Checks whether both the driver and the route files exists
    bool checkFile(const std::string &file) const;

protected:
    /// Begins the reading
    bool startReadingSteps();

    /** @brief reads the next route
        The next entry of the driver file is read and the index of the route
        to use is extracted. Afterwards, the route itself is read from the
        route file */
    bool readNextRoute(long start);

    /// Initialises the handler for reading
    bool myInit(OptionsCont &_options);

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
    double _gawronBeta, _gawronA;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROCellRouteDefHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

