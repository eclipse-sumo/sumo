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
// Revision 1.7  2003/07/21 18:07:44  roessel
// Adaptions due to new MSInductLoop.
//
// Revision 1.6  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.5  2003/03/18 15:00:32  roessel
// Changed Loggedvalue to LoggedValue in #includes
//
// Revision 1.4  2003/03/17 14:24:30  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/03 15:06:33  dkrajzew
// new import format applied; new detectors applied
//
// Revision 1.2  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.8  2002/07/31 17:34:50  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.9  2002/07/26 10:49:41  dkrajzew
// Detector-output destination may now be specified using relative pathnames
//
// Revision 1.8  2002/07/22 12:44:32  dkrajzew
// Source loading structures added
//
// Revision 1.7  2002/06/11 14:39:26  dkrajzew
// windows eol removed
//
// Revision 1.6  2002/06/11 13:44:32  dkrajzew
// Windows eol removed
//
// Revision 1.5  2002/06/07 14:39:58  dkrajzew
// errors occured while building larger nets and adaption of new netconverting
// methods debugged
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
#include <microsim/MSNet.h>
#include <microsim/MSInductLoop.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/logging/LoggedValue_Single.h>
#include <utils/logging/LoggedValue_TimeFixed.h>
#include "NLDetectorBuilder.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSInductLoop* NLDetectorBuilder::buildInductLoop(const std::string &id,
        const std::string &lane, float pos, int splInterval,
        const std::string &style, std::string filename,
        const std::string &basePath)
{
//     // get the output style
//     MSDetector::OutputStyle cstyle = convertStyle(id, style);
//     // check whether the file must be converted into a relative path
//     if(!FileHelpers::isAbsolute(filename)) {
//         filename = FileHelpers::getConfigurationRelative(basePath, filename);
//     }
//     // build and check the file
//     std::ofstream *file = new std::ofstream(filename.c_str());
//     if(!file->good()) {
//         throw InvalidArgument(
//             string("Could not open output for detector '") + id
//             + string("' for writing (file:") + filename
//             + string(")."));
//     }
    // get and check the lane
    MSLane *clane = MSLane::dictionary(lane);
    if(clane==0) {
        throw InvalidArgument(
            string("The lane with the id '") + lane
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
    return new MSInductLoop(id, clane, pos);
}

// MSDetector::OutputStyle NLDetectorBuilder::convertStyle(const std::string &id,
//         const std::string &style)
// {
//     if(style=="GNUPLOT" || style=="GPLOT")
//         return MSDetector::GNUPLOT;
//     if(style=="CSV")
//         return MSDetector::CSV;
//     throw InvalidArgument("Unknown output style '" + style + "' while parsing the detector '" + id + "' occured.");
// }


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLDetectorBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:

