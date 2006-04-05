#ifndef DFDetFlowLoader_h
#define DFDetFlowLoader_h
/***************************************************************************
                          DFDetFlowLoader.h
    A loader for detector flows
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.5  2006/04/05 05:35:27  dkrajzew
// further work on the dfrouter
//
// Revision 1.4  2006/03/17 09:04:26  dkrajzew
// class-documentation added/patched
//
// Revision 1.3  2006/03/08 12:51:29  dkrajzew
// further work on the dfrouter
//
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
	DFDetFlowLoader(DFDetectorCon *detcon,
        SUMOTime startTime, SUMOTime endTime,
        SUMOTime stepOffset);

    /// Destructor
    ~DFDetFlowLoader();

	DFDetectorFlows *read(const std::string &file, bool fast);

    /* ----- from the LineHandler - "interface" ----- */
    /** @brief Receives input from a line reader (watch full description!)
        Here, either input from the route file or from the route index file
        (when existing) is received. In the first case, the list of route
        indices is build - and read in the second case */
    bool report(const std::string &result);

protected:
    bool parseFast(const std::string &file);

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

