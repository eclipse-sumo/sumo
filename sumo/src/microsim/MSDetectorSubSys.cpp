#include "MSDetectorSubSys.h"
#include "MSDetector2File.h"
#include "MSInductLoop.h"
#include "MSLaneState.h"

void
MSDetectorSubSys::createDictionaries( void )
{
    LaneStateDict::create();
    LoopDict::create();
    MSDetector2File<MSInductLoop>::create( 900 );
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
        for(LaneStateDict::ValueVector::iterator i1=lsVec.begin(); i1!=lsVec.end(); i1++) {
            delete(*i1);
        }
        delete LaneStateDict::getInstance();
    }
//    deleteDictionaryContents( lsVec.begin(), lsVec.end() );
    
    if(LoopDict::created()) {
        LoopDict::ValueVector loopVec(
            LoopDict::getInstance()->getStdVector() );
        for(LoopDict::ValueVector::iterator i2=loopVec.begin(); i2!=loopVec.end(); i2++) {
            delete(*i2);
        }
//    deleteDictionaryContents( loopVec.begin(), loopVec.end() );
        delete LoopDict::getInstance();
    }

    if(MSDetector2File<MSInductLoop>::created()) {
        delete MSDetector2File<MSInductLoop>::getInstance();
    }

} 

