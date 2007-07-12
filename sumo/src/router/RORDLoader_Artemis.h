/****************************************************************************/
/// @file    RORDLoader_Artemis.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 12 Mar 2003
/// @version $Id$
///
// A handler for Artemis-files
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
#ifndef RORDLoader_Artemis_h
#define RORDLoader_Artemis_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include "ROAbstractRouteDefLoader.h"
#include "CellDriverInfoParser.h"
#include "ROEdgeVector.h"
#include <utils/importio/NamedColumnsParser.h>
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
 * @class RORDLoader_Artemis
 * A loader for ARTEMIS-routes
 */
class RORDLoader_Artemis :
            public LineHandler,
            public ROAbstractRouteDefLoader
{
public:
    /// Constructor
    RORDLoader_Artemis(ROVehicleBuilder &vb, RONet &net,
                       SUMOTime begin, SUMOTime end, std::string file="");

    /// Destructor
    ~RORDLoader_Artemis();

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
    virtual bool init(OptionsCont &myOptions);

protected:
    /** @brief reads the next route
        The next entry of the driver file is read and the index of the route
        to use is extracted. Afterwards, the route itself is read from the
        route file */
    bool myReadRoutesAtLeastUntil(SUMOTime time);

private:
    /// Information whether the O/D-Matrix (true) or the flows (false) are being read
    bool myReadingHVDests;

    /// Information whether the next line is the first one
    bool myFirstLine;

    /// The used reader
    LineReader myReader;

    /// The value extractor
    NamedColumnsParser myLineHandler;

    /// Definition of a container for probability of a destination
    typedef std::pair<std::string, SUMOReal> DestPercentage;

    /// Definition of a list of destination probabilities
    typedef std::vector<DestPercentage> DestProbVector;

    /// Definition of the container for probabilities for each node
    typedef std::map<std::string, DestProbVector> NodeDestProbs;

    /// Node destination probabilities
    NodeDestProbs myNodeConnections;

    /// Definition of a container for flowas (given a node)
    typedef std::map<std::string, SUMOReal> NodeFlows;

    /// Flows out of nodes
    NodeFlows myNodeFlows;

    /// Supplier for route ids
    IDSupplier myRouteIDSupplier;

    /// Supplier for vehicle ids
    IDSupplier myVehIDSupplier;

    /// The path information is found under
    std::string myPath;

    /// the current time step
    SUMOTime myCurrentTime;

};


#endif

/****************************************************************************/

