/**
 * @file   MSDetectorSubSys.cpp
 * @author Daniel Krajzewicz
 * @date   Tue Jul 29 10:43:35 2003
 * @version $Id$
 * @brief  Implementation of class MSDetectorSubSys
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

#include "MSDetectorSubSys.h"
#include "MSDetector2File.h"
#include "MSInductLoop.h"
#include "MSLaneState.h"

void
MSDetectorSubSys::createDictionaries( void )
{
    deleteDictionariesAndContents();
    LaneStateDict::create();
    LoopDict::create();
//     MSDetector2File<MSInductLoop>::create( 900 );
}

void
MSDetectorSubSys::setDictionariesFindMode( void )
{
    // This is better done during the construction process. But until the
    // SingletonDictionaries aren't widely used, we can do this here.
    LaneStateDict::getInstance()->setFindMode();
    LoopDict::getInstance()->setFindMode();
}

/*
template< class Iter > void
MSDetectorSubSys::deleteDictionaryContents( Iter start, Iter end )
{
    while( start != end ) {
        delete *start;
        ++start;
    }
}
*/

void
MSDetectorSubSys::deleteDictionariesAndContents( void )
{
    if(LaneStateDict::created()) {
        LaneStateDict::ValueVector lsVec(
            LaneStateDict::getInstance()->getStdVector() );
        for(LaneStateDict::ValueVector::iterator i1=lsVec.begin();
            i1!=lsVec.end(); i1++) {
            delete(*i1);
        }
        delete LaneStateDict::getInstance();
    }
//    deleteDictionaryContents( lsVec.begin(), lsVec.end() );

//     if(MSDetector2File<MSInductLoop>::created()) {
//         delete MSDetector2File<MSInductLoop>::getInstance();
//     }
    delete MSDetector2File::getInstance();

    if(LoopDict::created()) {
        LoopDict::ValueVector loopVec(
            LoopDict::getInstance()->getStdVector() );
        for(LoopDict::ValueVector::iterator i2=loopVec.begin();
            i2!=loopVec.end(); i2++) {
            delete(*i2);
        }
//    deleteDictionaryContents( loopVec.begin(), loopVec.end() );
        delete LoopDict::getInstance();
    }



}

