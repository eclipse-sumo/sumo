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
// Revision 1.5  2005/02/01 10:10:43  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.4  2004/12/16 12:14:59  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
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
#include <microsim/MSNet.h>
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
        }
    }
    intervalsM.clear();
}


void
MSDetector2File::addDetectorAndInterval( MSDetectorFileOutput* det,
                                         OutputDevice *device,
                                         MSUnit::Seconds interval,
                                         bool reinsert)
{
    if ( det->getDataCleanUpSteps() < interval ) {
        interval = det->getDataCleanUpSteps();
        WRITE_WARNING("MSDetector2File::addDetectorAndInterval: ");
        WRITE_WARNING("Write2File interval greater than detectors clean-up interval.");
        WRITE_WARNING("Reducing Write2File interval to clean-up interval.");
    }

    IntervalsKey key = interval;
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
            OptionsSubSys::getOptions().getInt("begin") + interval,
            MSEventControl::ADAPT_AFTER_EXECUTION );
        myLastCalls[interval] =
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
    if(!reinsert&&device->needsHeader()) {
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
    MSUnit::IntSteps interval = key;
    MSUnit::Seconds stopTime = MSNet::getInstance()->simSeconds();
    MSUnit::Seconds startTime = myLastCalls[interval];
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
    myLastCalls[interval] = MSNet::getInstance()->simSeconds();
    return interval;
}


MSDetector2File::MSDetector2File( void )
{}


void
MSDetector2File::resetInterval(MSDetectorFileOutput* det,
                               MSUnit::Seconds newinterval)
{
    for(Intervals::iterator i=intervalsM.begin(); i!=intervalsM.end(); ++i) {
        DetectorFileVec &dets = (*i).second;
        for(DetectorFileVec::iterator j=dets.begin(); j!=dets.end(); ++j) {
            DetectorFilePair &dvp = *j;
            if(dvp.first==det) {
                DetectorFilePair dvpu = *j;
                dets.erase(j);
                addDetectorAndInterval(dvpu.first, dvpu.second,
                    newinterval, true);
                return;
            }
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
