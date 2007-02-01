/****************************************************************************/
/// @file    NIElmar2EdgesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sun, 16 May 2004
/// @version $Id: $
///
// A LineHandler-derivate to load edges form a elmar-edges-file
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
#ifndef NIElmar2EdgesHandler_h
#define NIElmar2EdgesHandler_h
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
#include <utils/importio/LineHandler.h>
#include <netbuild/NBCapacity2Lanes.h>
#include <utils/common/FileErrorReporter.h>


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * NIElmar2EdgesHandler
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain edge definitions
 * in Cell-format
 */
// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIElmar2EdgesHandler : public LineHandler,
            public FileErrorReporter
{
public:
    /// constructor
    NIElmar2EdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
                         const std::string &file, std::map<std::string, Position2DVector> &geoms,
                         bool useNewLaneNumberInfoPlain);

    /// destructor
    ~NIElmar2EdgesHandler();

    /** implementation of the LineHandler-interface called by a LineReader
        interprets the retrieved information and stores it into the global
        NBEdgeCont */
    bool report(const std::string &result);

protected:
    NBNodeCont &myNodeCont;
    NBEdgeCont &myEdgeCont;
    std::map<std::string, Position2DVector> &myGeoms;
    bool myUseNewLaneNumberInfoPlain;

};


#endif

/****************************************************************************/

