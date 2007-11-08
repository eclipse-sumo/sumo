/****************************************************************************/
/// @file    AbstractMutex.h
/// @author  Daniel Krajzewicz
/// @date    2005-07-12
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef AbstractMutex_h
#define AbstractMutex_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


class AbstractMutex
{
public:
    AbstractMutex() { }
    virtual ~AbstractMutex() { }
    virtual void lock() = 0;
    virtual void unlock() = 0;

};


#endif

/****************************************************************************/

