#ifndef NIElmarEdgesHandler_h
#define NIElmarEdgesHandler_h
/***************************************************************************
                          NIElmarEdgesHandler.h
             A LineHandler-derivate to load edges form a elmar-edges-file
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
#include <netbuild/NBCapacity2Lanes.h>
#include <utils/common/FileErrorReporter.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * NIElmarEdgesHandler
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain edge definitions
 * in Cell-format
 */
/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIElmarEdgesHandler : public LineHandler,
                           public FileErrorReporter {
public:
    /// constructor
    NIElmarEdgesHandler(const std::string &file);

    /// destructor
    ~NIElmarEdgesHandler();

    /** implementation of the LineHandler-interface called by a LineReader
        interprets the retrieved information and stores it into the global
        NBEdgeCont */
    bool report(const std::string &result);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
