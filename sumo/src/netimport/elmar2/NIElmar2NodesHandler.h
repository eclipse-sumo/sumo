/****************************************************************************/
/// @file    NIElmar2NodesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sun, 16 May 2004
/// @version $Id$
///
// A LineHandler-derivate to load nodes form a elmar-nodes-file
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
#ifndef NIElmar2NodesHandler_h
#define NIElmar2NodesHandler_h
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
#include <utils/common/FileErrorReporter.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * NIElmar2NodesHandler
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain node definitions
 * in Cell-format
 */
class NIElmar2NodesHandler : public LineHandler,
            public FileErrorReporter
{
public:
    /// constructor
    NIElmar2NodesHandler(NBNodeCont &nc, const std::string &file,
                         std::map<std::string, Position2DVector> &geoms);

    /// destructor
    ~NIElmar2NodesHandler();

    /** implementation of the LineHandler-interface called by a LineReader
        interprets the retrieved information and stores it into the global
        NBNodeCont */
    bool report(const std::string &result);

protected:
    SUMOReal myInitX, myInitY;

    NBNodeCont &myNodeCont;

    std::map<std::string, Position2DVector> &myGeoms;

};


#endif

/****************************************************************************/

