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
// Revision 1.3  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.2  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:36:47  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.6  2002/07/31 17:34:50  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.6  2002/07/26 10:49:42  dkrajzew
// Detector-output destination may now be specified using relative pathnames
//
// Revision 1.5  2002/06/11 14:39:24  dkrajzew
// windows eol removed
//
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
#include <microsim/MSNet.h>
#include <microsim/MSDetector.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * This class builds the detectors from their descriptions
 */
class NLDetectorBuilder {
public:
    /// builds an induct loop
    static MSDetector *buildInductLoop(const std::string &id,
        const std::string &lane, float pos, int splInterval,
        const std::string &style, std::string filename,
        const std::string &basePath);

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
