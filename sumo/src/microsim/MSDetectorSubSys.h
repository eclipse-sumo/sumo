#ifndef MSDetectorSubSys_h
#define MSDetectorSubSys_h

/**
 * @file   MSDetectorSubSys.h
 * @author Daniel Krajzewicz
 * @date   Tue Jul 29 10:41:01 2003
 * @version $Id$
 * @brief  Declaration of class MSDetectorSubSys
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
