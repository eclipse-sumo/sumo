#ifndef MSDetector_H
#define MSDetector_H

//---------------------------------------------------------------------------//
//                        MSDetector.h  -  Base Class for all detector types.
//                           -------------------
//  begin                : Thu, 14 Mar 2002
//  copyright            : (C) 2002 by Christian Roessel
//  organisation         : ZAIK http://www.zaik.uni-koeln.de/AFS
//  email                : roessel@zpr.uni-koeln.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

// $Log$
// Revision 1.4  2003/03/19 08:02:01  dkrajzew
// debugging due to Linux-build errors
//
// Revision 1.3  2003/03/03 14:56:19  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.5  2002/04/11 16:14:42  croessel
// Moved ofstream myFile from MSInductLoop to MSDetector. Removed double
// declaration of OutputStyle.
//
// Revision 1.4  2002/04/10 15:51:53  croessel
// Initial commit. Base class for all specific detectors.
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <fstream>
#include "MSNet.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSDetector
 * Virtual class for detectors.
 */
class MSDetector
{
public:
    /** We support two output-styles, gnuplot and "Comma Separated Variable"
        (CSV). */
    enum OutputStyle { GNUPLOT, CSV };

    /** Constructor: Detector detects on lane at position pos. He collects
        during samplIntervall seconds data and writes them in style to file.
     */
    MSDetector( std::string id, OutputStyle style, std::ofstream* file = 0,
                MSNet::Time sampleInterval=1, bool floating=false);

    /// Destructor.
    virtual ~MSDetector();

    /// Call sample every timestep to update the detector.
    virtual void sample( double currSimSeconds ) = 0;

protected:
    /// Object's Id.
    std::string myID;

    /// Ouput-style.
    OutputStyle myStyle;

    /// File where output goes to.
    std::ofstream* myFile;

    /// The sample interval
    MSNet::Time mySampleInterval;

    /// The information whether the value floats
    bool myAmTimeFloating;

private:

};

//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

//#ifndef DISABLE_INLINE
//#include "MSDetector.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
