#ifndef NIElmarNodesHandler_h
#define NIElmarNodesHandler_h
/***************************************************************************
                          NIElmarNodesHandler.h
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
// Revision 1.1  2004/07/02 09:34:38  dkrajzew
// elmar and tiger import added
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
 * NIElmarNodesHandler
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain node definitions
 * in Cell-format
 */
class NIElmarNodesHandler : public LineHandler,
                           public FileErrorReporter {
public:
    /// constructor
    NIElmarNodesHandler(const std::string &file,
        double xmin, double ymin);

    /// destructor
    ~NIElmarNodesHandler();

    /** implementation of the LineHandler-interface called by a LineReader
        interprets the retrieved information and stores it into the global
        NBNodeCont */
    bool report(const std::string &result);

protected:
    /// The number of the line that will be read as next
    size_t myCurrentLine;

    double myInitX, myInitY;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
