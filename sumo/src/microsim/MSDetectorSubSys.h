#ifndef MSDetectorSubSys_h
#define MSDetectorSubSys_h

#include <string>
#include <helpers/SingletonDictionary.h>

class MSLaneState;
class MSInductLoop;

class MSDetectorSubSys {
public:
    typedef SingletonDictionary< std::string, MSLaneState* > LaneStateDict;
    typedef SingletonDictionary< std::string, MSInductLoop* > LoopDict;


    static void createDictionaries( void );

    static void setDictionariesFindMode( void );
/*
    static template< class Iter > void deleteDictionaryContents( 
        Iter start, Iter end );
*/
    static void deleteDictionariesAndContents( void );

};

#endif
