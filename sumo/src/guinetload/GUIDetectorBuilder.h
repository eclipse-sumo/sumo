#ifndef GUIDetectorBuilder_h
#define GUIDetectorBuilder_h
/***************************************************************************
                          GUIDetectorBuilder.h
                          A building helper for the detectors
                             -------------------
    begin                : Tue, 22 Jul 2003
    copyright            : (C) 2003 by DLR http://ivf.dlr.de/
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
// Revision 1.1  2003/07/22 14:58:33  dkrajzew
// changes due to new detector handling
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <microsim/MSNet.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * This class builds the detectors from their descriptions
 */
class GUIDetectorBuilder {
public:
    /// builds an induct loop
    static void buildInductLoop(const std::string &id,
        const std::string &lane, float pos, int splInterval,
        const std::string &style, std::string filename,
        const std::string &basePath);

private:
     /// converts the name of an output style into it's enumeration value
/*     static MSDetector::OutputStyle convertStyle(const std::string &id,
         const std::string &style);*/

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "GUIDetectorBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
//
