#ifndef NLDetectorBuilder_h
#define NLDetectorBuilder_h
/***************************************************************************
                          NLDetectorBuilder.h
                          A building helper for the detectors
                             -------------------
    begin                : Mon, 15 Apr 2002
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
// Revision 1.4  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/04/17 11:17:01  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/16 06:50:20  dkrajzew
// documentation added; coding standard attachements added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include "../microsim/MSNet.h"
#include "../microsim/MSDetector.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NLDetectorBuilder
 * This class builds the detectors from their descriptions
 */
class NLDetectorBuilder {
public:
    /// builds an induct loop
    static MSDetector *buildInductLoop(const std::string &id,
        const std::string &lane, float pos, long splInterval,
        const std::string &style, const std::string &filename);
private:
    /// converts the name of an output style into it's enumeration value
    static MSDetector::OutputStyle convertStyle(const std::string &id,
        const std::string &style);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLDetectorBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
//
