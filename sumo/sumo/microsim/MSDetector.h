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
// Revision 1.4  2002/04/10 15:51:53  croessel
// Initial commit. Base class for all specific detectors.
//


#include <string>

/**
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
    MSDetector( std::string id,
                OutputStyle style ) : myID( id ),
                                      myStyle( style ) {}
    
    /// Destructor.
    virtual ~MSDetector();

    /// Call sample every timestep to update the detector.
    void virtual sample( double currSimSeconds ) = 0;

protected:

    /// Object's Id.
    std::string myID;

    /// Ouput-style.
    OutputStyle myStyle;    

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










