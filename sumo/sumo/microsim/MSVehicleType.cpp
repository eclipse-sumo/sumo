/***************************************************************************
                          MSVehicleType.C  -  Base Class for Vehicle
                          parameters. 
                             -------------------
    begin                : Tue, 06 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

namespace 
{
    const char rcsid[] = 
    "$Id$";
} 

// $Log$
// Revision 1.2  2002/04/11 15:25:56  croessel
// Changed float to double.
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:20  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.9  2002/02/13 10:10:21  croessel
// Added two static methods: minDecel() and maxLength(). They are needed
// to calculate safe gaps in the case no predecessor resp. successor is
// known.
//
// Revision 1.8  2002/02/05 13:51:53  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.7  2002/02/01 13:57:07  croessel
// Changed methods and members bmax/dmax/sigma to more meaningful names
// accel/decel/dawdle.
//
// Revision 1.6  2001/12/20 14:53:07  croessel
// Default ctor, copy-ctor and assignment-operator removed.
// using namespace std added.
//
// Revision 1.5  2001/12/13 12:04:59  croessel
// Default arguments in constructor removed.
//
// Revision 1.4  2001/11/15 17:12:13  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.3  2001/11/14 15:47:34  croessel
// Merged the diffs between the .C and .cpp versions. Numerous changes
// in MSLane, MSVehicle and MSJunction.
//
// Revision 1.2  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.1  2001/10/24 07:18:12  traffic
// new extension
//
// Revision 1.5  2001/10/22 12:44:30  traffic
// single person simulation added
//
// Revision 1.4  2001/09/06 15:37:06  croessel
// Set default values in the constructor.
//
// Revision 1.3  2001/07/25 12:18:06  traffic
// CC problems with make_pair repaired
//
// Revision 1.2  2001/07/16 12:55:47  croessel
// Changed id type from unsigned int to string. Added string-pointer
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSVehicleType.h"
#include <cassert>

using namespace std;

// Init static member.
MSVehicleType::DictType MSVehicleType::myDict;
double MSVehicleType::myMinDecel  = 0;
double MSVehicleType::myMaxLength = 0;


MSVehicleType::~MSVehicleType()
{
}


MSVehicleType::MSVehicleType(string id, double length, double maxSpeed,
                             double accel, double decel, double dawdle ) :
    myID(id),
    myLength(length), myMaxSpeed(maxSpeed), myAccel(accel),
    myDecel(decel), myDawdle(dawdle)
{
    assert( myLength > 0 );
    assert( myMaxSpeed > 0 );
    assert( myAccel > 0 );
    assert( myDecel > 0 );
    assert( myDawdle >= 0 && myDawdle <= 1 );
    
    if ( myMinDecel == 0 || myDecel < myMinDecel ) {
    
        myMinDecel = myDecel;
    }
    if ( myLength > myMaxLength ) {
    
        myMaxLength = myLength;
    }
}


double
MSVehicleType::length() const
{
    return myLength;
}


double
MSVehicleType::maxSpeed() const
{
    return myMaxSpeed;
}


double
MSVehicleType::accel() const
{
    return myAccel;
}


double
MSVehicleType::decel() const
{
    return myDecel;
}


double
MSVehicleType::dawdle() const
{
    return myDawdle;
}

double
MSVehicleType::minDecel()
{
    return myMinDecel;
}

double
MSVehicleType::maxLength() 
{
    return myMaxLength;
}

bool
MSVehicleType::dictionary(string id, MSVehicleType* vehType)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, vehType));
        return true;
    }
    return false;
}


MSVehicleType*
MSVehicleType::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "MSVehicleType.iC"
//#endif

// Local Variables:
// mode:C++
// End:
