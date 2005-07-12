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
// Revision 1.5  2005/07/12 12:22:18  dkrajzew
// code style adapted
//
// Revision 1.4  2004/11/23 10:20:09  dkrajzew
// new detectors and tls usage applied; debugging
//
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
        CORN_OUT_TRIPDURATIONS,
        CORN_OUT_EMISSIONS,
        CORN_OUT_VEHROUTES,

        CORN_VEH_REALDEPART,
        CORN_MEAN_VEH_TRAVELTIME,
        CORN_MEAN_VEH_WAITINGTIME,
        CORN_VEHCONTROL_WANTS_DEPARTURE_INFO,
        CORN_VEHCONTROL_WANTS_EDGECHANGE_INFO,
        CORN_VEH_WASREROUTET,
        CORN_VEH_SAVEREROUTING,
        CORN_VEH_LASTREROUTEOFFSET,
        CORN_VEH_NUMBERROUTE,
        CORN_VEH_DEV_NO_CPHONE,
        CORN_VEH_REROUTE_TIME,

        CORN_BLA = CORN_VEH_REROUTE_TIME + 1000,

        CORN_MAX,
    };

    enum Pointer {
		CORN_P_VEH_OLDROUTE = 0,
        CORN_P_VEH_ROUTE_BEGIN_EDGE = CORN_P_VEH_OLDROUTE + 1000,
        CORN_P_VEH_DEV_CPHONE = CORN_P_VEH_ROUTE_BEGIN_EDGE + 1000,
		CORN_P_MAX = CORN_P_VEH_DEV_CPHONE + 40
	};

    static void init();
    static void clear();
    static bool wished(Function f);
    static void setWished(Function f);

public:
    static void setTripDurationsOutput(OutputDevice *s);
    static void setVehicleRouteOutput(OutputDevice *s);
    static void compute_TripDurationsOutput(MSVehicle *v);
    static void compute_VehicleRouteOutput(MSVehicle *v);


private:
    static OutputDevice *myTripDurationsOutput;
    static OutputDevice *myVehicleRouteOutput;
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
