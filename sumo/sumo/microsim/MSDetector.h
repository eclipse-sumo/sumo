#ifndef MSDetector_H
#define MSDetector_H

//---------------------------------------------------------------------------//
//                        MSDetector.h  -  Simple detector that emulates 
//                        induction loops.
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
// Revision 1.1  2002/04/08 07:21:23  traffic
// Initial revision
//
// Revision 1.2  2002/03/27 17:56:05  croessel
// Updated version.
//
// Revision 1.1  2002/03/14 18:48:54  croessel
// Initial commit.
//


class MSLane;
#include "MSNet.h"
#include <string>
#include <fstream>

class MSVehicle;

/**
 */
class MSDetector
{
public:

    friend class GnuPlotOut;

    /** Class to generate detector-output in gnuplot-style. 
        Usage, e.g.: cout << GnuPlotOut( detector, ...) << endl; */
    class GnuPlotOut
    {
    public:
        GnuPlotOut( unsigned n,
                    MSNet::Time endOfInterv,
                    double avgDensity,
                    double avgFlow,
                    double avgSpeed,
//                    double avgOccup,
                    double avgLength );
        friend std::ostream& operator<<( std::ostream& os, 
                                         const GnuPlotOut& obj ); 
    private:
        std::ostringstream out;

    };    
    
    friend std::ostream& operator<<( std::ostream& os, 
                                     const GnuPlotOut& obj );




    /** We support two output-styles, gnuplot and "Comma Separated Variable" 
        (CSV). */
    enum OutputStyle { GNUPLOT, CSV };

    /** Constructor: Detector detects on lane at position pos. He collects
        during samplIntervall seconds data and writes them in style to file.
     */ 
    MSDetector( std::string    id,
                MSLane*        lane,
                float          position,
                MSNet::Time    sampleIntervall,
                OutputStyle    style,
                std::ofstream& file );
    
    /// Destructor.
    ~MSDetector();

    ///
    void sample();

protected:
    double localDensity( const MSVehicle& veh );
//      double occupancy( const MSVehicle& veh );
    void   writeData();

private:
    MSVehicle* myPassedVeh;
    
    double myPassingSpeed;
    
    double myPassingTime;

    unsigned myNSamples;

    unsigned myNPassedVeh;

    unsigned myNDensityVeh;

    double myLocalDensitySum;

    double mySpeedSum;

//      double myOccupSum;

    double myVehLengthSum;

    // Number of already processed sampleIntervalls
    unsigned myN;

    std::string myID;

    MSLane* myLane;

    float myPos;

    /// Intervall in seconds.
    MSNet::Time mySampleIntervall;

    OutputStyle myStyle;

    std::ofstream& myFile;

    /// Default constructor.
    MSDetector();
    
    /// Copy constructor.
    MSDetector( const MSDetector& );
    
    /// Assignment operator.
    MSDetector& operator=( const MSDetector& );     
};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

//#ifndef DISABLE_INLINE
//#include "MSDetector.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:










