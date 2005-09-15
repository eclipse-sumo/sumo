#ifndef RORDLoader_Artemis_h
#define RORDLoader_Artemis_h
//---------------------------------------------------------------------------//
//                        RORDLoader_Artemis.h -
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
// Revision 1.5  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:50:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
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
// Revision 1.2  2003/03/17 14:26:38  dkrajzew
// debugging
//
// Revision 1.1  2003/03/12 16:39:19  dkrajzew
// artemis route support added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include "ROAbstractRouteDefLoader.h"
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
 * @class RORDLoader_Artemis
 * A loader for ARTEMIS-routes
 */
class RORDLoader_Artemis :
            public LineHandler,
            public ROAbstractRouteDefLoader {
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
    virtual bool init(OptionsCont &_options);

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
    typedef std::pair<std::string, double> DestPercentage;

    /// Definition of a list of destination probabilities
    typedef std::vector<DestPercentage> DestProbVector;

    /// Definition of the container for probabilities for each node
    typedef std::map<std::string, DestProbVector> NodeDestProbs;

    /// Node destination probabilities
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

    /// the current time step
    SUMOTime myCurrentTime;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

