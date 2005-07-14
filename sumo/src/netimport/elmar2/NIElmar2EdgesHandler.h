#ifndef NIElmar2EdgesHandler_h
#define NIElmar2EdgesHandler_h
/***************************************************************************
                          NIElmar2EdgesHandler.h
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
#include <string>
#include <utils/importio/LineHandler.h>
#include <netbuild/NBCapacity2Lanes.h>
#include <utils/common/FileErrorReporter.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * NIElmar2EdgesHandler
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
class NIElmar2EdgesHandler : public LineHandler,
                           public FileErrorReporter {
public:
    /// constructor
    NIElmar2EdgesHandler(NBNodeCont &nc, NBEdgeCont &ec,
        const std::string &file, std::map<std::string, Position2DVector> &geoms);

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

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
