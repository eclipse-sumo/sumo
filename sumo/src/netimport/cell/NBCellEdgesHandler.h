#ifndef NBCellEdgesHandler_h
#define NBCellEdgesHandler_h
/***************************************************************************
                          NBCellEdgesHandler.h
             A LineHandler-derivate to load edges form a cell-edges-file
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
// Revision 1.1  2002/10/16 15:40:03  dkrajzew
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
#include <netbuild/NBCapacity2Lanes.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * NBCellEdgesHandler
 * Being a LineHandler, this class retrieves each line from a LineReader
 * and parses these information assuming they contain edge definitions
 * in Cell-format
 */
class NBCellEdgesHandler : public LineHandler {
private:
    /// the parse file (for error reports)
    std::string _file;
    /// the converter for converting capacities to laneno
    NBCapacity2Lanes _capacity2Lanes;
public:
    /// constructor
    NBCellEdgesHandler(const std::string &file, bool warn, bool verbose,
        NBCapacity2Lanes capacity2Lanes);
    /// destructor
    ~NBCellEdgesHandler();
    /** implementation of the LineHandler-interface called by a LineReader
        interprets the retrieved information and stores it into the global
        NBEdgeCont */
    bool report(const std::string &result);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NBCellEdgesHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


