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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2004/11/25 16:26:47  dkrajzew
// consolidated and debugged some detectors and their usage
//
// Revision 1.2  2004/11/25 11:53:49  dkrajzew
// patched the bug on false intervals stamps if begin!=0
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSDetector2File.h"
#include <microsim/MSEventControl.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <string>
#include <cassert>
#include <utility>
#include <iostream>
#include <helpers/OneArgumentCommand.h>
#include <sstream>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member variables
 * ======================================================================= */
// initialize static member
MSDetector2File*
MSDetector2File::instanceM = 0;


/* =========================================================================
 * method definitions
 * ======================================================================= */
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
    // flush the last values
    Intervals::iterator it;
    for ( it = intervalsM.begin(); it != intervalsM.end(); ++it ) {
        write2file( (*it).first );
    }
    // clear the instance
    instanceM = 0;
    // close files and delete the detectors
    for ( it = intervalsM.begin(); it != intervalsM.end(); ++it ) {
        for( DetectorFileVec::iterator df =
                 it->second.begin(); df != it->second.end(); ++df ) {
            MSDetectorFileOutput *det = df->first;
            if(df->second->needsHeader()) {
                det->writeXMLDetectorInfoEnd(*(df->second));
                df->second->closeInfo();
            }
//!!! deleted in SharedOutputDevices            delete df->second;
        }
    }
    intervalsM.clear();
    // Detector* should be deleted via the SingletonDictionary!!!
}


void
MSDetector2File::addDetectorAndInterval( MSDetectorFileOutput* det,
                                         OutputDevice *device,
                                         MSUnit::Seconds sampleInterval,
                                         MSUnit::Seconds write2fileInterval )
{
    if ( det->getDataCleanUpSteps() < sampleInterval ) {
        sampleInterval = det->getDataCleanUpSteps();
        WRITE_WARNING("MSDetector2File::addDetectorAndInterval: ");
        WRITE_WARNING(" Sample interval greater than detectors clean-up interval.");
        WRITE_WARNING(" Reducing sample interval to clean-up interval.");
    }
    if ( det->getDataCleanUpSteps() < write2fileInterval ) {
        write2fileInterval = det->getDataCleanUpSteps();
        WRITE_WARNING("MSDetector2File::addDetectorAndInterval: ");
        WRITE_WARNING("Write2File interval greater than detectors clean-up interval.");
        WRITE_WARNING("Reducing Write2File interval to clean-up interval.");
    }

    IntervalsKey key = make_pair( sampleInterval, write2fileInterval );
    Intervals::iterator it = intervalsM.find( key );
    if ( it == intervalsM.end() ) {
        DetectorFileVec detAndFileVec;
        detAndFileVec.push_back( make_pair( det, device ) );
        intervalsM.insert( make_pair( key, detAndFileVec ) );
        // Add command for given key only once to MSEventControl
        Command* writeData =
            new OneArgumentCommand< MSDetector2File, IntervalsKey >
            ( this, &MSDetector2File::write2file, key );
        MSEventControl::getEndOfTimestepEvents()->addEvent(
            writeData,
            OptionsSubSys::getOptions().getInt("begin") + write2fileInterval,
            MSEventControl::ADAPT_AFTER_EXECUTION );
        myLastCalls[sampleInterval] =
            OptionsSubSys::getOptions().getInt("begin");
    } else {
        DetectorFileVec& detAndFileVec = it->second;
        if ( find_if( detAndFileVec.begin(), detAndFileVec.end(),
                      bind2nd( detectorEquals(), det ) )
             == detAndFileVec.end() ) {
            detAndFileVec.push_back( make_pair( det, device ) );
        }
        else {
            // detector already in container. Don't add several times
            WRITE_WARNING("MSDetector2File::addDetectorAndInterval: detector already in container. Ignoring.");
            return;
        }
    }
    if(device->needsHeader()) {
        det->writeXMLHeader(*device);
        det->writeXMLDetectorInfoStart(*device);
        device->closeInfo();
    }
}


MSUnit::IntSteps
MSDetector2File::write2file( IntervalsKey key )
{
    Intervals::iterator iIt = intervalsM.find( key );
    assert( iIt != intervalsM.end() );
    DetectorFileVec dfVec = iIt->second;
    MSUnit::IntSteps sampleInterval = key.first;
    MSUnit::IntSteps write2fileInterval = key.second;
    MSUnit::Seconds stopTime = MSNet::getInstance()->simSeconds();
    MSUnit::Seconds startTime = myLastCalls[sampleInterval];
    // check whether at the end the output was already generated
    if(stopTime==startTime) {
        return 0; // a dummy value only; this should only be the case
        // when this container is destroyed
    }
    for ( DetectorFileVec::iterator it = dfVec.begin();
          it != dfVec.end(); ++it ) {
        MSDetectorFileOutput* det = it->first;
        it->first->writeXMLOutput( *(it->second), startTime, stopTime-1 );
        it->second->closeInfo();
    }
    myLastCalls[sampleInterval] = MSNet::getInstance()->simSeconds();
    return write2fileInterval;
}


MSDetector2File::MSDetector2File( void )
{}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
