#ifndef MSCORN_H
#define MSCORN_H
//---------------------------------------------------------------------------//
//                        MSCORN.h -
//  A storage for optional things to compute
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// $Log$
// Revision 1.3  2004/08/02 12:13:33  dkrajzew
// output device handling rechecked; tiny documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;
class OutputDevice;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class MSCORN {
public:
    /**
     * @enum Function
     * A list of functions
     */
    enum Function {
        CORN_OUT_TRIPOUTPUT = 0,
        CORN_OUT_EMISSIONS = 1,
        CORN_VEH_REALDEPART = 2,
        CORN_MEAN_VEH_TRAVELTIME = 3,
        CORN_MEAN_VEH_WAITINGTIME = 4,
        CORN_VEHCONTROL_WANTS_DEPARTURE_INFO = 6,
        CORN_VEHCONTROL_WANTS_EDGECHANGE_INFO = 7,
        CORN_VEH_WASREROUTET = 8,
        CORN_VEH_OLDROUTE1 = 9,
        CORN_VEH_OLDROUTE2 = 10,
        CORN_VEH_OLDROUTE3 = 11,
        CORN_VEH_OLDROUTE4 = 12,
        CORN_VEH_OLDROUTE5 = 13,
        CORN_VEH_OLDROUTE6 = 14,
        CORN_VEH_SAVEREROUTING = 15,
        CORN_VEH_LASTREROUTEOFFSET = 16,
        CORN_VEH_NUMBREROUTE = 17,
        CORN_MAX = 18,
    };

    static void init();
    static void clear();
    static bool wished(Function f);
    static void setWished(Function f);

public:
    static void setTripInfoOutput(OutputDevice *s);
    static void compute_TripInfoOutput(MSVehicle *v);


private:
    static OutputDevice *myTripInfoOut;
    static bool myWished[CORN_MAX];

private:
    MSCORN();
    ~MSCORN();

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End: