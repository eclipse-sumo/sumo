/**
 * @file   MSDetector2File.cpp
 * @author Christian Roessel <christian.roessel@dlr.de>
 * @date   Started Wed Aug 6 16:16:26 2003
 * @version $Id$
 * @brief  Implementation of class MSDetector2File.
 *
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

// $Id$

#include "MSDetector2File.h"
#include "MSEventControl.h"
#include <string>
#include <cassert>
#include <utility>
#include <iostream>
#include <helpers/OneArgumentCommand.h>

using namespace std;

// initialize static member
MSDetector2File*
MSDetector2File::instanceM = 0;


MSDetector2File*
MSDetector2File::getInstance( void )
{
    if ( instanceM == 0 ) {
        instanceM = new MSDetector2File();
    }
    return instanceM;
}


MSDetector2File::~MSDetector2File( void )
{
    instanceM = 0;
    // close files
    for ( Intervals::iterator it = intervalsM.begin();
          it != intervalsM.end(); ++it ) {
        for( DetectorFileVec::iterator df =
                 it->second.begin(); df != it->second.end(); ++df ) {
            *(df->second) << (df->first)->getXMLDetectorInfoEnd()
                          << endl;
            delete df->second;
        }
    }
    intervalsM.clear();
    // Detector* should be deleted via the SingletonDictionary
}


void
MSDetector2File::addDetectorAndInterval( Detector* det,
                                         const string& filename,
                                         MSNet::Time intervalInSeconds )
{
    MSNet::Time intervalInSteps( MSNet::getSteps( intervalInSeconds ));
    assert( intervalInSteps >= 1 );

    /*            Detector* det = 
                  DetectorDict::getInstance()->getValue( detectorId );*/
    /*            string filename = det->getNamePrefix() + "_" +
                  toString( intervalInSeconds ) + ".xml";*/

    if ( det->getDataCleanUpSteps() < intervalInSteps ) {
        intervalInSteps = det->getDataCleanUpSteps();
        cerr << "MSDetector2File::addDetectorAndInterval: "
            "intervalInSeconds greater than\ndetectors clean-up "
            "interval. Reducing intervalInSeconds to clean-up "
            "interval." << endl;
    }
    ofstream* ofs = 0;
    Intervals::iterator it =
        intervalsM.find( intervalInSteps );
    if ( it == intervalsM.end() ) {
        DetectorFileVec detAndFileVec;
        ofs = new ofstream( filename.c_str() );
        assert( ofs != 0 );
        detAndFileVec.push_back( make_pair( det, ofs ) );
        intervalsM.insert(
            make_pair( intervalInSteps, detAndFileVec ) );

        // Add command for given interval only once to MSEventControl
        Command* writeData =
            new OneArgumentCommand< MSDetector2File, MSNet::Time >
            ( this, &MSDetector2File::write2file, intervalInSteps );
        MSEventControl::getEndOfTimestepEvents()->addEvent(
            writeData,
            intervalInSteps - 1,
            MSEventControl::ADAPT_AFTER_EXECUTION );
    }
    else {
        DetectorFileVec& detAndFileVec = it->second;
        if ( find_if( detAndFileVec.begin(),
                      detAndFileVec.end(),
                      bind2nd( detectorEquals(), det ) )
             == detAndFileVec.end() ) {
            ofs = new ofstream( filename.c_str() );
            assert( ofs != 0 );
            detAndFileVec.push_back( make_pair( det, ofs ) );    
        }
        else {
            // detector already in container. Don't add several times
            cerr << "MSDetector2File::addDetectorAndInterval: "
                "detector already in container. Ignoring."<< endl;
            return;
        }
    }
            
    // write xml-intro
    *ofs << det->getXMLHeader()
         << det->getXMLDetectorInfoStart() << endl;
}


MSNet::Time
MSDetector2File::write2file( MSNet::Time intervalInSteps )
{
    Intervals::iterator intervalIt =
        intervalsM.find( intervalInSteps);
    assert( intervalIt != intervalsM.end() );
    DetectorFileVec dfVec = intervalIt->second;
    for ( DetectorFileVec::iterator it = dfVec.begin();
          it != dfVec.end(); ++it ) {
        double time = MSNet::getInstance()->simSeconds();
        *(it->second) << "<interval start=\""
                      << time - MSNet::getSeconds( intervalInSteps) + 1
                      << "\" stop=\"" << time << "\" "
                      << it->first->getXMLOutput( intervalInSteps )
                      << " />" << endl;
    }
    return intervalInSteps;
}


MSDetector2File::MSDetector2File( void )
{}   


// Local Variables:
// mode:C++
// End:
