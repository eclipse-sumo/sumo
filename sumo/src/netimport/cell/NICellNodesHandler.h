#ifndef NICellNodesHandler_h
#define NICellNodesHandler_h
/***************************************************************************
                          NICellNodesHandler.h
             A LineHandler-derivate to load nodes form a cell-nodes-file
                             -------------------
    project              : SUMO
    begin                : Fri, 19 Jul 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.1  2003/02/07 11:10:56  dkrajzew
// names changed
//
// Revision 1.1  2002/10/16 15:40:04  dkrajzew
// initial commit for cell importing classes
//
// Revision 1.1  2002/07/25 08:41:45  dkrajzew
// Visum7.5 and Cell import added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <utils/importio/LineHandler.h>
#include <utils/common/FileErrorReporter.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NICellNodesHandler
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain node definitions
 * in Cell-format
 */
class NICellNodesHandler : public LineHandler,
                           public FileErrorReporter {
private:
public:
    /// constructor
    NICellNodesHandler(const std::string &file, bool warn, bool verbose);
    /// destructor
    ~NICellNodesHandler();
    /** implementation of the LineHandler-interface called by a LineReader
        interprets the retrieved information and stores it into the global
        NBNodeCont */
    bool report(const std::string &result);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NICellNodesHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
