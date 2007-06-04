/****************************************************************************/
/// @file    MSVehicleType.cpp
/// @author  Christian Roessel
/// @date    Tue, 06 Mar 2001
/// @version $Id$
///
// parameters.
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

#include "MSVehicleType.h"
#include "MSNet.h"
#include <cassert>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/RandHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
SUMOReal MSVehicleType::myMinDecel  = 0;
SUMOReal MSVehicleType::myMaxLength = 0;


// ===========================================================================
// method definitions
// ===========================================================================
MSVehicleType::~MSVehicleType()
{}


MSVehicleType::MSVehicleType(const string &id, SUMOReal length,
                             SUMOReal maxSpeed, SUMOReal accel,
                             SUMOReal decel, SUMOReal dawdle,
                             SUMOReal tau,
                             SUMOVehicleClass vclass)
        : myID(id), myLength(length), myMaxSpeed(maxSpeed), myAccel(accel),
        myDecel(decel), myDawdle(dawdle), myTau(tau), myVehicleClass(vclass)
{
    assert(myLength > 0);
    assert(myMaxSpeed > 0);
    assert(myAccel > 0);
    assert(myDecel > 0);
    assert(myDawdle >= 0 && myDawdle <= 1);
    if (myMinDecel==0 || myDecel<myMinDecel) {
        myMinDecel = myDecel;
    }
    if (myLength>myMaxLength) {
        myMaxLength = myLength;
    }
    myInverseTwoDecel = SUMOReal(1) / (SUMOReal(2) * myDecel);
}


const std::string &
MSVehicleType::getID() const
{
    return myID;
}


void
MSVehicleType::saveState(std::ostream &os, long /*what*/)
{
    FileHelpers::writeString(os, myID);
    FileHelpers::writeFloat(os, myLength);
    FileHelpers::writeFloat(os, myMaxSpeed);
    FileHelpers::writeFloat(os, myAccel);
    FileHelpers::writeFloat(os, myDecel);
    FileHelpers::writeFloat(os, myDawdle);
    FileHelpers::writeFloat(os, myTau);
    FileHelpers::writeInt(os, (int) myVehicleClass);
}


/****************************************************************************/

