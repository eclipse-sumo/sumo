/****************************************************************************/
/// @file    MSTriggerControl.h
/// @author  Daniel Krajzewicz
/// @date    2006
/// @version $Id$
///
// A storage for triggers
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
#ifndef MSTriggerControl_h
#define MSTriggerControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MSTrigger;



// ===========================================================================
// class definitions
// ===========================================================================
class MSTriggerControl
{
public:
    MSTriggerControl(/*const std::vector<MSTrigger*> &trigger*/);
    ~MSTriggerControl();
    /// adds a trigger
    void addTrigger(MSTrigger *t);
    void addTrigger(const std::vector<MSTrigger*> &all);

    MSTrigger *getTrigger(const std::string &id);

    /// Definition of the container for items to initialise before starting
    typedef std::vector<MSTrigger*> TriggerVector;

protected:
    TriggerVector myTrigger;
};


#endif

/****************************************************************************/

