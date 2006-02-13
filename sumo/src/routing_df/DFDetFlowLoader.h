#ifndef DFDetFlowLoader_h
#define DFDetFlowLoader_h
//---------------------------------------------------------------------------//
//                        DFDetFlowLoader.h -
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
// Revision 1.2  2006/02/13 07:27:06  dkrajzew
// current work on the DFROUTER added (unfinished)
//
// Revision 1.1  2006/01/19 17:42:59  ericnicolay
// base classes for the reading of the detectorflows
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
#include <utils/importio/NamedColumnsParser.h>
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/router/IDSupplier.h>
#include <routing_df/DFDetector.h>
#include <routing_df/DFDetectorFlow.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class Options;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class DFDetFlowLoader
 * A loader for Detector Flows
 */
class DFDetFlowLoader :
            public LineHandler{
public:
    /// Constructor
	///!!!DFDetFlowLoader();
	DFDetFlowLoader(DFDetectorCon *detcon);

    /// Destructor
    ~DFDetFlowLoader();

	DFDetectorFlows *read(const std::string &file);

    /* ----- from the LineHandler - "interface" ----- */
    /** @brief Receives input from a line reader (watch full description!)
        Here, either input from the route file or from the route index file
        (when existing) is received. In the first case, the list of route
        indices is build - and read in the second case */
    bool report(const std::string &result);

    /// Initialises the handler for reading
   // virtual bool init(OptionsCont &_options);
private:
    /// Information whether the next line is the first one
    bool myFirstLine;

    /// The used reader
    LineReader myReader;

    /// The value extractor
    NamedColumnsParser myLineHandler;

    /// Supplier for route ids
    IDSupplier myRouteIDSupplier;

    /// The path information is found under
    //std::string fname;
	DFDetectorFlows * mydetFlows;
	DFDetectorCon *detcon;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

