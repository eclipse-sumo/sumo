/****************************************************************************/
/// @file    DFDetFlowLoader.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id: $
///
// A loader for detector flows
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
#ifndef DFDetFlowLoader_h
#define DFDetFlowLoader_h
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
#include <utils/importio/NamedColumnsParser.h>
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/router/IDSupplier.h>
#include <routing_df/DFDetector.h>
#include <routing_df/DFDetectorFlow.h>


// ===========================================================================
// class declarations
// ===========================================================================
class Options;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class DFDetFlowLoader
 * A loader for Detector Flows
 */
class DFDetFlowLoader :
            public LineHandler
{
public:
    /// Constructor
    ///!!!DFDetFlowLoader();
    DFDetFlowLoader(DFDetectorCon &dets, DFDetectorFlows &into,
                    SUMOTime startTime, SUMOTime endTime,
                    int timeOffset);

    /// Destructor
    ~DFDetFlowLoader();

    void read(const std::string &file, bool fast);

    /* ----- from the LineHandler - "interface" ----- */
    /** @brief Receives input from a line reader (watch full description!)
        Here, either input from the route file or from the route index file
        (when existing) is received. In the first case, the list of route
        indices is build - and read in the second case */
    bool report(const std::string &result);

protected:
    bool parseFast(const std::string &file);

private:
    DFDetectorFlows &myStorage;

    int myTimeOffset;

    SUMOTime myStartTime, myEndTime;

    /// The used reader
    LineReader myReader;

    /// Information whether the next line is the first one
    bool myFirstLine;

/// The value extractor
    NamedColumnsParser myLineHandler;

    /// Supplier for route ids
    IDSupplier myRouteIDSupplier;

    /// The path information is found under
    //std::string fname;
//	DFDetectorFlows * mydetFlows;
    DFDetectorCon &myDetectorContainer;


};


#endif

/****************************************************************************/

