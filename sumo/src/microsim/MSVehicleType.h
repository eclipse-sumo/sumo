#ifndef MSVehicleType_H
#define MSVehicleType_H
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
// $Log$
// Revision 1.19  2006/11/14 13:02:30  dkrajzew
// warnings removed
//
// Revision 1.18  2006/10/04 13:18:17  dkrajzew
// debugging internal lanes, multiple vehicle emission and net building
//
// Revision 1.17  2006/09/18 10:08:33  dkrajzew
// added vehicle class support to microsim
//
// Revision 1.16  2006/08/02 11:58:23  dkrajzew
// first try to make junctions tls-aware
//
// Revision 1.15  2006/07/06 07:33:22  dkrajzew
// rertrieval-methods have the "get" prependix; EmitControl has no dictionary; MSVehicle is completely scheduled by MSVehicleControl; new lanechanging algorithm
//
// Revision 1.14  2006/05/15 05:54:11  dkrajzew
// debugging saving/loading of states
//
// Revision 1.14  2006/05/08 11:06:59  dkrajzew
// debugging loading/saving of states
//
// Revision 1.13  2006/03/17 09:01:12  dkrajzew
// .icc-files removed
//
// Revision 1.12  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.11  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.10  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2004/11/23 10:20:11  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.8  2004/07/02 09:26:24  dkrajzew
// classes prepared to be derived
//
// Revision 1.7  2003/10/17 06:52:01  dkrajzew
// acceleration is now time-dependent
//
// Revision 1.6  2003/07/30 10:02:38  dkrajzew
// support for the vehicle display removed by now
//
// Revision 1.5  2003/05/27 18:40:49  roessel
// Removed superfluous const.
//
// Revision 1.4  2003/05/20 09:31:47  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output
// debugged; setting and retrieval of some parameter added
//
// Revision 1.3  2003/02/07 10:41:51  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:39:03  dkrajzew
// complete deletion within destructors implemented; clear-operator added for
// container; global file include
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.4  2002/07/31 17:33:01  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.4  2002/07/31 14:41:05  croessel
// New methods return often used precomputed values.
//
// Revision 1.3  2002/05/29 17:06:04  croessel
// Inlined some methods. See the .icc files.
//
// Revision 1.2  2002/04/11 15:25:56  croessel
// Changed SUMOReal to SUMOReal.
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
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
// Changed id type from unsigned int to string.
//  Added string-pointer dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
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

#include <cassert>
#include <map>
#include <string>
#include <utils/common/SUMOTime.h>
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOVehicleClass.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLane;
class BinaryInputDevice;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSVehicleType
 * @brief Storage for parameters of vehicles of the same abstract type
 *
 * The vehicle type stores the parameter of a single vehicle type.
 * It is assumed that within the simulation many vehicles are using the same
 *  vehicle type, quite common is using only one vehicle type for all vehicles.
 * You can think of it like of having a vehicle type for each VW Golf or
 *  Ford Mustang in your simulation while the car instances just refer to it.
 */
class MSVehicleType
{
public:
    /// Constructor.
    MSVehicleType( const std::string &id, SUMOReal length, SUMOReal maxSpeed,
        SUMOReal accel, SUMOReal decel, SUMOReal dawdle,
        SUMOVehicleClass vclass);

    /// Destructor.
    virtual ~MSVehicleType();









    /**  */
    virtual SUMOReal brakeGap( SUMOReal speed ) const {
        return speed * speed * myInverseTwoDecel + speed * myTau;
    }

    virtual SUMOReal approachingBrakeGap( SUMOReal speed ) const {
        return speed * speed * myInverseTwoDecel;
    }

   /** */
    virtual SUMOReal interactionGap( SUMOReal vF, SUMOReal laneMaxSpeed, SUMOReal vL ) const
    {
        // Resolve the vsafe equation to gap. Assume predecessor has
        // speed != 0 and that vsafe will be the current speed plus acceleration,
        // i.e that with this gap there will be no interaction.
        SUMOReal vNext = MIN2(maxNextSpeed(vF), laneMaxSpeed);
        SUMOReal gap = (vNext - vL) *
            ( ( vF + vL ) * myInverseTwoDecel + myTau ) +
            vL * myTau;

        // Don't allow timeHeadWay < deltaT situations.
        return MAX2(gap, timeHeadWayGap(vNext));
    }

    /**  */
    bool hasSafeGap(  SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal laneMaxSpeed) const
    {
        SUMOReal vSafe = ffeV( speed, gap, predSpeed );
        SUMOReal vNext = MIN3( maxNextSpeed(speed), laneMaxSpeed, vSafe);
        return (vNext>=getSpeedAfterMaxDecel(speed)
            &&
            gap   >= timeHeadWayGap( predSpeed ) );
    }

    /** Returns the minimum gap between this driving vehicle and a
     * possibly emitted vehicle with speed 0. */
    SUMOReal safeEmitGap(SUMOReal speed) const
    {
        SUMOReal vNextMin = getSpeedAfterMaxDecel(speed); // ok, minimum next speed
        SUMOReal safeGap  = vNextMin * ( speed * myInverseTwoDecel + myTau );
        return MAX2(safeGap, timeHeadWayGap( speed ) ) + ACCEL2DIST(getMaxAccel(speed));
    }


    /** Dawdle according the vehicles dawdle parameter. Return value >= 0 */
    SUMOReal dawdle( SUMOReal speed ) const {
        // generate random number out of [0,1]
        SUMOReal random = SUMOReal( rand() ) / SUMOReal( RAND_MAX );
        // Dawdle.
        if(speed<getMaxAccel(0)) {
            // we should not prevent vehicles from driving just due to dawdling
            //  if someone is starting, he should definitely start
            // (but what about slow-to-start?)!!!
            speed -= myDawdle * speed * random;
        } else {
            speed -= myDawdle * getMaxAccel(speed) * random;
        }
        return MAX2( SUMOReal( 0 ), speed );
    }

    SUMOReal getSpeedAfterMaxDecel(SUMOReal v) const {
        return MAX2((SUMOReal) 0, v - ACCEL2SPEED(myDecel));
    }

    SUMOReal maxNextSpeed( SUMOReal v ) const {
        return MIN2(v+ACCEL2SPEED(getMaxAccel(v)), myMaxSpeed);
    }

    SUMOReal ffeV(SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed) const {
        return MIN2(_vsafe(speed, myDecel, gap2pred, predSpeed), maxNextSpeed(speed) );
    }

    SUMOReal ffeS(SUMOReal speed, SUMOReal gap2pred) const {
        return MIN2(_vsafe(speed, myDecel, gap2pred, 0), maxNextSpeed(speed) );
    }

    SUMOReal getSecureGap( SUMOReal speed, SUMOReal predSpeed, SUMOReal predLength ) const {
        SUMOReal safeSpace2 = brakeGap(speed);
        SUMOReal vSafe = ffeV(0, 0, predSpeed);
        SUMOReal safeSpace3 =
            ( (vSafe - predSpeed)
            * (SUMOReal) ((vSafe+predSpeed) / 2.0 / (2.0 * MSVehicleType::getMinVehicleDecel()) + myTau) )
            + predSpeed * myTau;
        SUMOReal safeSpace = safeSpace2 > safeSpace3 ? safeSpace2 : safeSpace3;
        safeSpace = safeSpace > ACCEL2SPEED(myDecel) ? safeSpace : ACCEL2SPEED(myDecel);
        safeSpace += predLength;
        safeSpace += ACCEL2SPEED(getMaxAccel(speed));
        return safeSpace;
    }

    SUMOReal decelAbility() const {
        return ACCEL2SPEED(myDecel); // !!! really the speed?
    }


    /** */
    SUMOReal timeHeadWayGap(SUMOReal speed) const {
        assert( speed >= 0 );
        return SPEED2DIST(speed);
    }












    /// Get vehicle's length [m].
    SUMOReal getLength() const {
        return myLength;
    }

    /// Get vehicle's maximum speed [m/s].
    SUMOReal getMaxSpeed() const {
        return myMaxSpeed;
    }

    /// Get the vehicle's maximum acceleration [m/s^2]
    inline SUMOReal getMaxAccel(SUMOReal v) const {
        return (SUMOReal) (myAccel * (1.0 - (v/myMaxSpeed)));
    }

    /// Get the vehicle's maximum deceleration [m/s^2]
    SUMOReal getMaxDecel() const {
        return myDecel;
    }

    SUMOReal getTau() const {
        return myTau;
    }


    /// returns the name of the vehicle type
    const std::string &getID() const;


    //@{ static functions covering all loaded vehicle types
    /// Returns the minimum deceleration-ability of all vehicle-types.
    static SUMOReal getMinVehicleDecel() {
        return myMinDecel;
    }
    //@}


    //@{ access functions to the vehicle type dictionary
    /** Inserts a MSVehicleType into the static dictionary and returns true
        if the key id isn't already in the dictionary. Otherwise returns
        false. */
    static bool dictionary( std::string id, MSVehicleType* edge);

    /** Returns the MSVehicleType associated to the key id if exists,
        otherwise returns 0. */
    static MSVehicleType* dictionary( std::string id);

    static MSVehicleType* dict_Random();

    /** Clears the dictionary */
    static void clear();

    /// Saves the states of all vehicles
    static void dict_saveState(std::ostream &os, long what);

    /// Saves the states of a vehicle
    void saveState(std::ostream &os, long what);

    /// Loads vehicle state
    static void dict_loadState(BinaryInputDevice &bis, long what);
    //@}

    SUMOVehicleClass getVehicleClass() const {
        return myVehicleClass;
    }


protected:
    /** Returns the SK-vsafe. */
    SUMOReal _vsafe( SUMOReal currentSpeed, SUMOReal /*decelAbility*/,
                  SUMOReal gap2pred, SUMOReal predSpeed ) const {
        if(predSpeed==0&&gap2pred<0.01) {
            return 0;
        }
        assert( currentSpeed     >= SUMOReal(0) );
        assert( gap2pred  >= SUMOReal(0) );
        assert( predSpeed >= SUMOReal(0) );
        SUMOReal vsafe = predSpeed +
            ( ( gap2pred - predSpeed * myTau ) /
            ( ( ( predSpeed + currentSpeed ) * myInverseTwoDecel ) + myTau ) );
        assert( vsafe >= 0 );
        return vsafe;
    }


private:
    /// Unique ID.
    std::string myID;

    /// Vehicle's length [m].
    SUMOReal myLength;

    /// Vehicle's maximum speed [m/s].
    SUMOReal myMaxSpeed;

    /// The vehicle's maximum acceleration [m/s^2]
    SUMOReal myAccel;

    /// The vehicle's maximum deceleration [m/s^2]
    SUMOReal myDecel;

    /// The vehicle's dawdle-parameter. 0 for no dawdling, 1 for max.
    SUMOReal myDawdle;

    /// The precomputed value for 1/(2*d)
    SUMOReal myInverseTwoDecel;



    SUMOReal myTau;
    SUMOVehicleClass myVehicleClass;

    //@{ static members covering all loaded vehicle types
    /// Minimum deceleration-ability of all vehicle-types.
    static SUMOReal myMinDecel;

    /// Maximum length of all vehicle-types.
    static SUMOReal myMaxLength;
    //@}


    //@{ the static dictionary
    /// Definition of the type of the static dictionary to associate string-ids with objects.
    typedef std::map< std::string, MSVehicleType* > DictType;
    /// Static dictionary to associate string-ids with objects.
    static DictType myDict;
    //@}


    /// Invalidated copy constructor
    MSVehicleType( const MSVehicleType& );

    /// Invalidated assignment operator
    MSVehicleType& operator=( const MSVehicleType& );

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
