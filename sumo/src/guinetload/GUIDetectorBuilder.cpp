/***************************************************************************
                          GUIDetectorBuilder.cpp
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
namespace
{
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.3  2003/08/14 13:45:46  dkrajzew
// new detector usage applied
//
// Revision 1.2  2003/08/04 11:35:51  dkrajzew
// only GUIVehicles need a color definition; process of building cars changed
//
// Revision 1.1  2003/07/22 14:58:33  dkrajzew
// changes due to new detector handling
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include <microsim/MSNet.h>
#include <microsim/MSInductLoop.h>
#include <guisim/GUIInductLoop.h>
#include <microsim/MSDetector2File.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/logging/LoggedValue_Single.h>
#include <utils/logging/LoggedValue_TimeFixed.h>
#include "GUIDetectorBuilder.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
GUIDetectorBuilder::buildInductLoop(const std::string &id,
        const std::string &lane, float pos, int splInterval,
        const std::string &/*style*/, std::string filename,
        const std::string &basePath)
{
     // get the output style
//   MSDetector::OutputStyle cstyle = convertStyle(id, style);
     // check whether the file must be converted into a relative path
     if(!FileHelpers::isAbsolute(filename)) {
         filename = FileHelpers::getConfigurationRelative(basePath, filename);
     }
     // build and check the file
/*     std::ofstream *file = new std::ofstream(filename.c_str());
     if(!file->good()) {
         throw InvalidArgument(
             string("Could not open output for induct loop '") + id
             + string("' for writing (file:") + filename
             + string(")."));
     }*/
    // get and check the lane
    MSLane *clane = MSLane::dictionary(lane);
    if(clane==0) {
        throw InvalidArgument(
            string("On detector building:\n")
            + string("The lane with the id '") + lane
            + string("' is not known."));
    }
//     // build in dependence to the sample interval
//     if(splInterval==1) {
//         return
//             new MSInductLoop<LoggedValue_Single<double> >
//                 (id, clane, pos, splInterval, cstyle, file, false);
//     } else {
//         return
//             new MSInductLoop<LoggedValue_TimeFixed<double> >
//                 (id, clane, pos, splInterval, cstyle, file, false);
//     }
    if(pos<0) {
        pos = clane->length() + pos;
    }
    MSInductLoop *loop = new GUIInductLoop(id, clane, pos);
    // add the file output
    MSDetector2File* det2file =
        MSDetector2File::getInstance();
    det2file->addDetectorAndInterval(loop, filename, splInterval);
}
/*
MSDetector::OutputStyle GUIDetectorBuilder::convertStyle(const std::string &id,
        const std::string &style)
{
     if(style=="GNUPLOT" || style=="GPLOT")
         return MSDetector::GNUPLOT;
     if(style=="CSV")
         return MSDetector::CSV;
     throw InvalidArgument("Unknown output style '" + style + "' while parsing the detector '" + id + "' occured.");
}
*/

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIDetectorBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:

