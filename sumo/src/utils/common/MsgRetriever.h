/****************************************************************************/
/// @file    MsgRetriever.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 17 Jun 2003
/// @version $Id$
///
// Retrieves messages about the process
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
#ifndef MsgRetriever_h
#define MsgRetriever_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * MsgRetriever
 */
class MsgRetriever
{
public:
    /// adds a new error to the list
    virtual void inform(const std::string &error) = 0;

    /// destructor
    virtual ~MsgRetriever()
    { }

protected:
    /// standard constructor
    MsgRetriever()
    { }

};


#endif

/****************************************************************************/

