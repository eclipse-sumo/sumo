/****************************************************************************/
/// @file    MSCORN.h
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// A storage for optional things to compute
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
#ifndef MSCORN_h
#define MSCORN_h
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

#include <iostream>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSCORN
 * Helper calls to store information about what shall be computed
 *  (CORN == COmpute Or Not)
 */
class MSCORN
{
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
        CORN_OUT_SAVED_INFO,
        CORN_OUT_SAVED_INFO_FREQ,
        CORN_OUT_TRANS_INFO,
        CORN_OUT_VEH_IN_RANGE,

        CORN_VEH_REALDEPART,
        CORN_MEAN_VEH_TRAVELTIME,
        CORN_MEAN_VEH_WAITINGTIME,
        CORN_VEHCONTROL_WANTS_DEPARTURE_INFO,
        CORN_VEHCONTROL_WANTS_EDGECHANGE_INFO,
        CORN_VEH_WASREROUTED,
        CORN_VEH_SAVEREROUTING,
        CORN_VEH_LASTREROUTEOFFSET,
        CORN_VEH_NUMBERROUTE,
        CORN_VEH_DEV_NO_CPHONE,
        CORN_VEH_REROUTE_TIME,

        CORN_BLA = CORN_VEH_REROUTE_TIME + 1000,

        CORN_MAX
    };

    enum Pointer {
        CORN_P_VEH_OWNCOL,
        CORN_P_VEH_OLDROUTE,
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
                                    const std::string &headID, const std::string &vehs, int quantity, int a);
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


#endif

/****************************************************************************/

