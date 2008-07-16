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
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
        // c2x-output
        CORN_OUT_CLUSTER_INFO,
        CORN_OUT_SAVED_INFO,
        CORN_OUT_SAVED_INFO_FREQ,
        CORN_OUT_TRANS_INFO,
        CORN_OUT_VEH_IN_RANGE,

        CORN_VEH_DEPART_TIME,
        CORN_VEH_DEPART_DEF,
        CORN_VEH_ARRIVAL_DEF,
        CORN_VEH_DEPART_INFO,
        CORN_VEH_ARRIVAL_INFO,
        CORN_VEH_WAITINGTIME,
        CORN_MEAN_VEH_TRAVELTIME,
        CORN_MEAN_VEH_WAITINGTIME,
        CORN_VEH_SAVEREROUTING,
        CORN_VEH_LASTREROUTEOFFSET,
        CORN_VEH_BLINKER,
        CORN_VEH_NUMBERROUTE,
        CORN_VEH_VAPORIZED,


        CORN_MAX
    };

    enum Pointer {
        CORN_P_VEH_DEPART_INFO,
        CORN_P_VEH_ARRIVAL_INFO,
        CORN_P_VEH_OWNCOL,
        CORN_P_VEH_OLDROUTE,
        CORN_P_VEH_OLD_REPETITION_ROUTE,
        CORN_P_VEH_PASSENGER,
        CORN_P_MAX
    };

    static void init();
    static bool wished(Function f);
    static void setWished(Function f);

public:
    static void saveTOSS2_CalledPositionData(SUMOTime time,
            int callID, const std::string &pos, int quality);


    //car2car
    static void saveClusterInfoData(SUMOTime step, int id,
                                    const std::string &headID,
                                    const std::string &vehs, int quantity);
    static void saveSavedInformationData(SUMOTime step, const std::string &veh,
                                         const std::string &edge, const std::string &type,
                                         SUMOTime time, SUMOReal nt);
    static void saveTransmittedInformationData(SUMOTime step, const std::string &from,
            const std::string &to, const std::string &edge, SUMOTime time, SUMOReal nt);
    static void saveVehicleInRangeData(SUMOTime step, const std::string &veh1,
                                       const std::string &veh2,
                                       SUMOReal x1, SUMOReal y1,
                                       SUMOReal x2 , SUMOReal y2);

private:
    //Car2car
    static SUMOTime myLastStepClusterInfoOutput;
    static SUMOTime myLastStepSavedInfoOutput;
    static SUMOTime myLastStepTransmittedInfoOutput;
    static SUMOTime myLastStepVehicleInRangeOutput;

    static bool myWished[CORN_MAX];
    static bool myFirstCall[CORN_MAX];

private:
    MSCORN();
    ~MSCORN();

};


#endif

/****************************************************************************/

