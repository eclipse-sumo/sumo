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
                                         MSUnit::Seconds sampleInterval,
                                         MSUnit::Seconds write2fileInterval )
{
//     MSUnit::IntSteps intervalInSteps(
//         MSUnit::getInstance()->getIntegerSteps( intervalInSeconds ) );
//     assert( intervalInSteps >= 1 );

    MSUnit::IntSteps sampleSteps(
        MSUnit::getInstance()->getIntegerSteps( sampleInterval ) );
    assert( sampleSteps >= 1 );
    MSUnit::IntSteps write2fileSteps(
        MSUnit::getInstance()->getIntegerSteps( write2fileInterval ) );
    assert( write2fileSteps >= 1 );
    
    /*            Detector* det = 
                  DetectorDict::getInstance()->getValue( detectorId );*/
    /*            string filename = det->getNamePrefix() + "_" +
                  toString( intervalInSeconds ) + ".xml";*/

//     if ( det->getDataCleanUpSteps() < intervalInSteps ) {
//         intervalInSteps = det->getDataCleanUpSteps();
//         cerr << "MSDetector2File::addDetectorAndInterval: "
//             "intervalInSeconds greater than\ndetectors clean-up "
//             "interval. Reducing intervalInSeconds to clean-up "
//             "interval." << endl;
//     }
    if ( det->getDataCleanUpSteps() < sampleSteps ) {
        sampleSteps = det->getDataCleanUpSteps();
        cerr << "MSDetector2File::addDetectorAndInterval: "
            "Sample interval greater than\ndetectors clean-up "
            "interval. Reducing sample interval to clean-up "
            "interval." << endl;
    }
    if ( det->getDataCleanUpSteps() < write2fileSteps ) {
        write2fileSteps = det->getDataCleanUpSteps();
        cerr << "MSDetector2File::addDetectorAndInterval: "
            "Write2File interval greater than\ndetectors clean-up "
            "interval. Reducing Write2File interval to clean-up "
            "interval." << endl;
    }    

    IntervalsKey key = make_pair( sampleSteps, write2fileSteps );
    ofstream* ofs = 0;
    Intervals::iterator it = intervalsM.find( key );
    if ( it == intervalsM.end() ) {
        DetectorFileVec detAndFileVec;
        ofs = new ofstream( filename.c_str() );
        assert( ofs != 0 );
        detAndFileVec.push_back( make_pair( det, ofs ) );
        intervalsM.insert( make_pair( key, detAndFileVec ) );

        // Add command for given key only once to MSEventControl
        Command* writeData =
            new OneArgumentCommand< MSDetector2File, IntervalsKey >
            ( this, &MSDetector2File::write2file, key );
        MSEventControl::getEndOfTimestepEvents()->addEvent(
            writeData,
            write2fileSteps - 1,
            MSEventControl::ADAPT_AFTER_EXECUTION );
    }
    else {
        DetectorFileVec& detAndFileVec = it->second;
        if ( find_if( detAndFileVec.begin(), detAndFileVec.end(),
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


MSUnit::IntSteps
MSDetector2File::write2file( IntervalsKey key )
{
    Intervals::iterator iIt = intervalsM.find( key );
    assert( iIt != intervalsM.end() );
    DetectorFileVec dfVec = iIt->second;
    MSUnit::IntSteps sampleInterval = key.first;
    MSUnit::IntSteps write2fileInterval = key.second;
    for ( DetectorFileVec::iterator it = dfVec.begin();
          it != dfVec.end(); ++it ) {
        Detector* det = it->first;
        ofstream* ofs = it->second;
        MSUnit::Seconds stopTime = MSNet::getInstance()->simSeconds();
        MSUnit::Seconds startTime =
            stopTime - MSUnit::getInstance()->getSeconds( sampleInterval ) + 1;
        *ofs << "<interval start=\"" << startTime
             << "\" stop=\"" << stopTime << "\" "
             << det->getXMLOutput( sampleInterval )
             << " />" << endl;
    }
    return write2fileInterval;
}


MSDetector2File::MSDetector2File( void )
{}   


// Local Variables:
// mode:C++
// End:
