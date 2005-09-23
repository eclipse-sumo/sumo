#ifndef NIElmar2NodesHandler_h
#define NIElmar2NodesHandler_h
/***************************************************************************
                          NIElmar2NodesHandler.h
             A LineHandler-derivate to load nodes form a elmar-nodes-file
                             -------------------
    project              : SUMO
    begin                : Sun, 16 May 2004
    copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.3  2005/09/23 06:02:24  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.2  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2005/07/14 11:05:28  dkrajzew
// elmar unsplitted import added
//
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
#include <utils/importio/LineHandler.h>
#include <utils/common/FileErrorReporter.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NIElmar2NodesHandler
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain node definitions
 * in Cell-format
 */
class NIElmar2NodesHandler : public LineHandler,
                           public FileErrorReporter {
public:
    /// constructor
    NIElmar2NodesHandler(NBNodeCont &nc, const std::string &file,
        SUMOReal xmin, SUMOReal ymin, std::map<std::string, Position2DVector> &geoms);

    /// destructor
    ~NIElmar2NodesHandler();

    /** implementation of the LineHandler-interface called by a LineReader
        interprets the retrieved information and stores it into the global
        NBNodeCont */
    bool report(const std::string &result);

protected:
    /// The number of the line that will be read as next
    size_t myCurrentLine;

    SUMOReal myInitX, myInitY;

    NBNodeCont &myNodeCont;

    std::map<std::string, Position2DVector> &myGeoms;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
