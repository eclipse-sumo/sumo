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
// Revision 1.20  2007/01/11 06:33:53  dkrajzew
// speeded up c2c computation
//
// Revision 1.19  2006/12/18 14:43:56  dkrajzew
// debugging c2c
//
// Revision 1.18  2006/12/06 16:56:36  ericnicolay
// added new output for cellphone_dump
//
// Revision 1.17  2006/11/27 14:08:51  dkrajzew
// added Danilot's current changes
//
// Revision 1.16  2006/11/16 07:02:17  dkrajzew
// warnings removed
//
// Revision 1.15  2006/11/03 22:59:26  behrisch
// Syntactic sugar
//
// Revision 1.14  2006/11/02 11:44:50  dkrajzew
// added Danilo Teta-Boyom's changes to car2car-communication
//
// Revision 1.13  2006/10/19 11:03:13  ericnicolay
// change code for the ss2-sql-output
//
// Revision 1.12  2006/10/12 10:14:27  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
//
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
        // standard output
        CORN_OUT_TRIPDURATIONS,
        CORN_OUT_EMISSIONS,
        CORN_OUT_VEHROUTES,
        // TrafficOnline-output
        CORN_OUT_DEVICE_TO_SS2,
        CORN_OUT_CELL_TO_SS2,
        CORN_OUT_LA_TO_SS2,
        CORN_OUT_DEVICE_TO_SS2_SQL,
        CORN_OUT_CELL_TO_SS2_SQL,
        CORN_OUT_LA_TO_SS2_SQL,
		CORN_OUT_CELLPHONE_DUMP_TO,
        // c2x-output
        CORN_OUT_CLUSTER_INFO,
        CORN_OUT_EDGE_NEAR,
        CORN_OUT_SAVED_INFO,
		CORN_OUT_SAVED_INFO_FREQ,
        CORN_OUT_TRANS_INFO,
        CORN_OUT_VEH_IN_RANGE,

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

        CORN_MAX
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
	static void setCELLPHONEDUMPOutput(OutputDevice *s);
    // Car2Car
    static void setClusterInfoOutput(OutputDevice *s);
    static void setEdgeNearInfoOutput(OutputDevice *s);
    static void setSavedInfoOutput(OutputDevice *s);
	static void setSavedInfoOutputFreq(OutputDevice *s);
    static void setTransmittedInfoOutput(OutputDevice *s);
	static void setVehicleInRangeOutput(OutputDevice *s);

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

	static void saveCELLPHONEDUMP(SUMOTime time, int cell_id, int call_id, int event_type);

	//car2car
    static void saveClusterInfoData(SUMOTime step, int id,
		const std::string &vehs, int quantity, int a);
    static void saveEdgeNearInfoData(const std::string id,
		const std::string neighbor, int quantity);
    static void saveSavedInformationData(SUMOTime step, const std::string veh,
		const std::string edge, std::string type, int time, SUMOReal nt, int a);
    static void saveSavedInformationDataFreq(SUMOTime step, const MSVehicle &veh);
    static void saveTransmittedInformationData(SUMOTime step, const std::string from,
		const std::string to, const std::string edge, int time, SUMOReal nt, int a);
    static void saveVehicleInRangeData(SUMOTime step, const std::string veh1,
		const std::string veh2, SUMOReal x1, SUMOReal y1, SUMOReal x2 , SUMOReal y2, int a);

private:
    //
    static OutputDevice *myTripDurationsOutput;
    static OutputDevice *myVehicleRouteOutput;
    static OutputDevice *myVehicleDeviceTOSS2Output;
    ///
	static OutputDevice *myCellTOSS2Output;
	static OutputDevice *myLATOSS2Output;
	static OutputDevice *myVehicleDeviceTOSS2SQLOutput;
	static OutputDevice *myCellTOSS2SQLOutput;
	static OutputDevice *myLATOSS2SQLOutput;
	static OutputDevice *myCELLPHONEDUMPOutput;
	//Car2car
	static OutputDevice *myClusterInfoOutput;
    static OutputDevice *myEdgeNearInfoOutput;
	static OutputDevice *mySavedInfoOutput;
	static OutputDevice *mySavedInfoOutputFreq;
	static OutputDevice *myTransmittedInfoOutput;
	static OutputDevice *myVehicleInRangeOutput;

    static bool myWished[CORN_MAX];
	static bool myFirstCall[CORN_MAX];

private:
    MSCORN();
    ~MSCORN();

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

