/***************************************************************************
                          MSEmitControl.h  -  Controls emission of
                          vehicles into the net. 
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

#ifndef MSEmitControl_H
#define MSEmitControl_H

// $Log$
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:14  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.9  2002/02/05 13:51:51  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.8  2001/12/19 17:01:22  croessel
// using namespace std replaced by std::
// Copy-ctor and assignment-operator moved to private.
//
// Revision 1.7  2001/12/06 13:11:22  traffic
// minor change
//
// Revision 1.6  2001/11/15 17:12:14  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.5  2001/11/14 11:45:54  croessel
// Resolved conflicts which appeared during suffix-change and
// CR-line-end commits.
//
// Revision 1.4  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.3  2001/10/22 12:42:57  traffic
// single person simulation added
//
// Revision 1.2  2001/07/16 12:55:46  croessel
// Changed id type from unsigned int to string. Added string-pointer 
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//

//#include <list>
#include <vector>
#include <map>
#include <string>
#include "MSNet.h"

class MSVehicle;

/**
 */
class MSEmitControl
{
public:
    /** Container for Vehicles. Vehicles contain the trip-data. */
    typedef std::vector< MSVehicle* > VehCont; // Don't change container type
    // without checking MSEmitControl::emitVehicles. Iterators may
    // become invalid.
     
    /** Use this constructor only. It will sort the vehicles by their 
        departure time. */
    MSEmitControl( std::string id, VehCont* allVeh );
     
    /// Destructor.
    ~MSEmitControl();

    /** adds new vehicles to the list */
    void add( MSEmitControl* cont );
     
    /** Emits vehicles at time, if there are vehicles that want to
        depart at time. If emission is not possible, the vehicles
        remain in the list. */ 
    void emitVehicles( MSNet::Time time );

    void addStarting( MSVehicle* veh );

    /** Inserts emitcontrol into the static dictionary and returns true
        if the key id isn't already in the dictionary. Otherwise returns
        false. */
    static bool dictionary( std::string id, MSEmitControl* emitControl );
    /** Returns the MSEdgeControl associated to the key id if exists,
        otherwise returns 0. */
    static MSEmitControl* dictionary( std::string id );

protected:
     
private:
    /// Unique ID.
    std::string myID;
     
    /** The entirety of vehicles that will drive through the net. The 
        vehicles know their departure-time and route. The container
        is sorted by the vehicles departure time. */
    VehCont* myAllVeh;

//      /** The car-following model. Controls emission permission. */ 
//      MSModel* myModel;

    /// Static dictionary to associate string-ids with objects.
    typedef std::map< std::string, MSEmitControl* > DictType;
    static DictType myDict;

    /// Default constructor.
    MSEmitControl();
     
    /// Copy constructor.
    MSEmitControl(const MSEmitControl&);
     
    /// Assignment operator.
    MSEmitControl& operator=(const MSEmitControl&);     
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

//#ifndef DISABLE_INLINE
//#include "MSEmitControl.iC"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
