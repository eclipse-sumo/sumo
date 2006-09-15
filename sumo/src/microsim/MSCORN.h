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
// Revision 1.11  2006/09/15 09:28:47  ericnicolay
// TO SS2 SQL output added
//
// Revision 1.10  2006/03/16 15:19:35  ericnicolay
// add ss2 interface for cells and LAs
//
// Revision 1.9  2006/02/23 11:31:09  dkrajzew
// TO SS2 output added
//
// Revision 1.8  2006/01/09 11:55:34  dkrajzew
// further visualization possibilities added
//
// Revision 1.7  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/07/12 12:22:18  dkrajzew
// code style adapted
//
// Revision 1.4  2004/11/23 10:20:09  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.3  2004/08/02 12:13:33  dkrajzew
// output device handling rechecked; tiny documentation added
//
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

#include <iostream>
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;
class OutputDevice;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSCORN
 * Helper calls to store information about what shall be computed
 *  (CORN == COmpute Or Not)
 */
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
        CORN_OUT_DEVICE_TO_SS2,
		CORN_OUT_CELL_TO_SS2,
		CORN_OUT_LA_TO_SS2,
		CORN_OUT_DEVICE_TO_SS2_SQL,
		CORN_OUT_CELL_TO_SS2_SQL,
		CORN_OUT_LA_TO_SS2_SQL,

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
        CORN_VEH_OWNCOL_RED,
        CORN_VEH_OWNCOL_GREEN,
        CORN_VEH_OWNCOL_BLUE,
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
    static void setVehicleDeviceTOSS2Output(OutputDevice *s);
	static void setCellTOSS2Output(OutputDevice *s);
	static void setLATOSS2Output(OutputDevice *s);
	static void setVehicleDeviceTOSS2SQLOutput(OutputDevice *s);
	static void setCellTOSS2SQLOutput(OutputDevice *s);
	static void setLATOSS2SQLOutput(OutputDevice *s);

    static void compute_TripDurationsOutput(MSVehicle *v);
	static void compute_VehicleRouteOutput(MSVehicle *v);

    static void saveTOSS2_CalledPositionData(SUMOTime time,
        int callID, const std::string &pos, int quality);

	static void saveTOSS2_CellStateData(SUMOTime time, 
		int Cell_Id, int Calls_In, int Calls_Out, int Dyn_Calls_In, 
		int Dyn_Calls_Out, int Sum_Calls, int Intervall);
	
	static void saveTOSS2_LA_ChangesData(SUMOTime time, int position_id, 
        int dir, int sum_changes, int quality_id, int intervall);

	static void saveTOSS2SQL_CalledPositionData(SUMOTime time,
        int callID, const std::string &pos, int quality);

	static void saveTOSS2SQL_CellStateData(SUMOTime time, 
		int Cell_Id, int Calls_In, int Calls_Out, int Dyn_Calls_In, 
		int Dyn_Calls_Out, int Sum_Calls, int Intervall);
	
	static void saveTOSS2SQL_LA_ChangesData(SUMOTime time, int position_id, 
        int dir, int sum_changes, int quality_id, int intervall);

private:
    static OutputDevice *myTripDurationsOutput;
    static OutputDevice *myVehicleRouteOutput;
    static OutputDevice *myVehicleDeviceTOSS2Output;
	static OutputDevice *myCellTOSS2Output;
	static OutputDevice *myLATOSS2Output;
	static OutputDevice *myVehicleDeviceTOSS2SQLOutput;
	static OutputDevice *myCellTOSS2SQLOutput;
	static OutputDevice *myLATOSS2SQLOutput;
   
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

