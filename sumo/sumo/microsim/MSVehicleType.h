/***************************************************************************
                          MSVehicleType.h  -  Base Class for Vehicle
                          parameters. 
                             -------------------
    begin                : Mon, 12 Mar 2001
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

#ifndef MSVehicleType_H
#define MSVehicleType_H

// $Log$
// Revision 1.1  2002/04/08 07:21:24  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:20  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.9  2002/02/13 10:10:20  croessel
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
// Revision 1.6  2001/12/20 14:53:34  croessel
// using namespace std replaced by std::
// Copy-ctor and assignment-operator moved to private.
//
// Revision 1.5  2001/11/15 17:12:14  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.4  2001/11/14 11:45:54  croessel
// Resolved conflicts which appeared during suffix-change and
// CR-line-end commits.
//
// Revision 1.3  2001/09/06 15:37:06  croessel
// Set default values in the constructor.
//
// Revision 1.2  2001/07/16 12:55:47  croessel
// Changed id type from unsigned int to string. Added string-pointer dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//

#include <map>
#include <string>

class MSLane;


/**
 */
class MSVehicleType
{
public:
    
    friend class MSVehicle;

    /// Constructor.
    MSVehicleType( std::string id, float length, float maxSpeed,
                   float accel = 0.8, float decel = 4.5, float dawdle = 0.5 );

    /// Destructor.
    ~MSVehicleType();
     
    /// Get vehicle type's length [m].
    float length() const;

    /// Get vehicle type's maximum speed [m/s].
    float maxSpeed() const;
     
    /// Get the vehicle type's maximum acceleration [m/s^2]
    float accel() const;

    /// Get the vehicle type's maximum deceleration [m/s^2]
    float decel() const;

    /// Get the vehicle type's dawdle-probability. Out of [0,1]
    float dawdle() const;
    
    /// Returns the minimum deceleration-ability of all vehicle-types.
    static float minDecel();
    
    /// Returns the maximum length of all vehicle-types.
    static float maxLength();

    /** Inserts a MSVehicleType into the static dictionary and returns true
        if the key id isn't already in the dictionary. Otherwise returns
        false. */
    static bool dictionary( std::string id, MSVehicleType* edge);
    /** Returns the MSVehicleType associated to the key id if exists,
        otherwise returns 0. */
    static MSVehicleType* dictionary( std::string id);

protected:
     
private:
    /// Unique ID.
    std::string myID;
     
    /// Vehicle type's length [m].
    float myLength;
     
    /// Vehicle type's maximum speed [m/s].
    float myMaxSpeed;

    /// The vehicle type's maximum acceleration [m/s^2]
    float myAccel;
     
    /// The vehicle type's maximum deceleration [m/s^2]
    float myDecel;

    /// The vehicle type's dawdle-parameter. 0 for no dawdling, 1 for max.
    float myDawdle;
    
    /// Minimum deceleration-ability of all vehicle-types.
    static float myMinDecel;
    
    /// Maximum length of all vehicle-types.   
    static float myMaxLength;

    /// Static dictionary to associate string-ids with objects.
    typedef std::map< std::string, MSVehicleType* > DictType;
    static DictType myDict;

    /// Default constructor.
    MSVehicleType();
     
    /// Copy constructor.
    MSVehicleType( const MSVehicleType& );

    /// Assignment operator.
    MSVehicleType& operator=( const MSVehicleType& );

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

//#ifndef DISABLE_INLINE
//#include "MSVehicleType.iC"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
