/****************************************************************************/
/// @file    NICellNodesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// A LineHandler-derivate to load nodes form a cell-nodes-file
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
#ifndef NICellNodesHandler_h
#define NICellNodesHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
 * NICellNodesHandler
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain node definitions
 * in Cell-format
 */
class NICellNodesHandler : public LineHandler,
            public FileErrorReporter
{
private:
public:
    /// constructor
    NICellNodesHandler(NBNodeCont &nc, const std::string &file);

    /// destructor
    ~NICellNodesHandler();

    /** implementation of the LineHandler-interface called by a LineReader
        interprets the retrieved information and stores it into the global
        NBNodeCont */
    bool report(const std::string &result);

protected:
    NBNodeCont &myNodeCont;

};


#endif

/****************************************************************************/

