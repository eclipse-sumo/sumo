#ifndef ROArtemisRouteDefHandler_h
#define ROArtemisRouteDefHandler_h
//---------------------------------------------------------------------------//
//                        ROArtemisRouteDefHandler.h -
//  A handler for Artemis-files
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 12 Mar 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.2  2003/03/17 14:26:38  dkrajzew
// debugging
//
// Revision 1.1  2003/03/12 16:39:19  dkrajzew
// artemis route support added
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
#include <utils/importio/NamedColumnsParser.h>
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
 * @class ROArtemisRouteDefHandler
 */
class ROArtemisRouteDefHandler :
            public LineHandler,
            public ROTypedRoutesLoader {
public:
    /// Constructor
    ROArtemisRouteDefHandler(RONet &net, std::string file="");

    /// Destructor
    ~ROArtemisRouteDefHandler();

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

    /// Checks whether the needed files are acceassable
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
    /// Information whether the O/D-Matrix (true) or the flows (false) are being read
    bool myReadingHVDests;

    /// Information whether the next line is the first one
    bool myFirstLine;

    /// The used reader
    LineReader myReader;

    /// The value extractor
    NamedColumnsParser myLineHandler;

    /// Definition of a container for propability of a destination
    typedef std::pair<std::string, double> DestPercentage;

    /// Definition of a list of destination propabilities
    typedef std::vector<DestPercentage> DestProbVector;

    /// Definition of the container for propabilities for each node
    typedef std::map<std::string, DestProbVector> NodeDestProbs;

    /// Node destination propabilities
    NodeDestProbs myNodeConnections;

    /// Definition of a container for flowas (given a node)
    typedef std::map<std::string, double> NodeFlows;

    /// Flows out of nodes
    NodeFlows myNodeFlows;

    /// Supplier for route ids
    IDSupplier myRouteIDSupplier;

    /// Supplier for vehicle ids
    IDSupplier myVehIDSupplier;

    /// The path information is found under
    std::string myPath;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROArtemisRouteDefHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

