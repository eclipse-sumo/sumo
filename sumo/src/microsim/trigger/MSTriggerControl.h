#ifndef MSTriggerControl_h
#define MSTriggerControl_h
//---------------------------------------------------------------------------//
//                        MSTriggerControl.h -
//  A storage for triggers
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log: MSTrigger.h,v $
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>

class MSTrigger;




class MSTriggerControl {
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
