/****************************************************************************/
/// @file    MSInterface_NetRun.h
/// @author  Daniel Krajzewicz
/// @date    2004-07-02
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
#ifndef MSInterface_NetRun_h
#define MSInterface_NetRun_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>


// ===========================================================================
// class definitions
// ===========================================================================
class MSInterface_NetRun
{
public:
    MSInterface_NetRun()
    { }
    virtual ~MSInterface_NetRun()
    {}
    virtual bool simulate(SUMOTime start, SUMOTime stop) = 0;
    virtual void initialiseSimulation() = 0;
    virtual void closeSimulation(SUMOTime start, SUMOTime stop) = 0;
    virtual void simulationStep(SUMOTime start, SUMOTime step) = 0;
    virtual bool haveAllVehiclesQuit() = 0;
    virtual SUMOTime getCurrentTimeStep() const = 0;
    virtual void preSimStepOutput() const = 0;
    virtual void postSimStepOutput() const = 0;
    virtual long getSimStepDurationInMillis() const = 0;

};


#endif

/****************************************************************************/

