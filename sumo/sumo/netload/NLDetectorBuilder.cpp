/***************************************************************************
                          NLDetectorBuilder.cpp
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
namespace
{
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.5  2002/06/07 14:39:58  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
//
// Revision 1.4  2002/04/17 11:17:01  dkrajzew
// windows-newlines removed
//
// Revision 1.3  2002/04/16 06:50:20  dkrajzew
// documentation added; coding standard attachements added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include "../microsim/MSNet.h"
#include "../microsim/MSDetector.h"
#include "../microsim/MSInductLoop.h"
#include "../utils/UtilExceptions.h"
#include "NLDetectorBuilder.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
MSDetector *NLDetectorBuilder::buildInductLoop(const std::string &id, 
        const std::string &lane, float pos, long splInterval,
        const std::string &style, const std::string &filename) {
    // get the output style
    MSDetector::OutputStyle cstyle = convertStyle(id, style);
    // build and check the file
    std::ofstream *file = new std::ofstream(filename.c_str());
    if(!file->good())
        throw InvalidArgument("Could not open output for detector '" + id + "' for writing (file:" + filename + ").");
    // get and check the lane
    MSLane *clane = MSLane::dictionary(lane);
    if(clane==0)
        throw InvalidArgument("The lane with the id '" + lane + "' is not known.");
    return new MSInductLoop(id, clane, pos, splInterval, cstyle, file);
}

MSDetector::OutputStyle NLDetectorBuilder::convertStyle(const std::string &id, const std::string &style) {
    if(style=="GNUPLOT" || style=="GPLOT")
        return MSDetector::GNUPLOT;
    if(style=="CSV")
        return MSDetector::CSV;
    throw InvalidArgument("Unknown output style '" + style + "' while parsing the detector '" + id + "' occured.");
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLDetectorBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:

