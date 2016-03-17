#ifndef PHEMlightSTART
#define PHEMlightSTART

#include <string>
#include <vector>
#include <stdexcept>
#include "stringbuilder.h"

//C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace PHEMlightdll { class CEPHandler; }
namespace PHEMlightdll { class Helpers; }
namespace PHEMlightdll { class VehicleResult; }
namespace PHEMlightdll { class CEP; }


namespace PHEMlightdll {
    class Start {
    private:
        CEPHandler* DataInput;
    public:
        Helpers* Helper;

        //******************* Parameters of Array or Single calculation *******************
        //********************************* INPUT ******************************************
        //***  DATATYP            |  UNIT   |  VARIBLE              |      Description  ***
        //string                  |  [-]    |   VEH                 | Name of vehicle file (e.g. "PC_D_EU4" path neede if not in "Default Vehicles") or aggregated name (PC, HDV, BUS, TW) by FleetMix calculation
        //List<double> / double   |  [s]    |   Time                | Time signal
        //List<double> / double   |  [m/s]  |   Velocity            | Velocity signal
        //double                  |  [m/s^2]|   acc                 | Acceleration (ONLY NEDDED BY SINGLE CALCULATION)
        //List<double> / double   |  [%]    |   Gradient            | Gradient of the route
        //out List<VehicleResult> |  [-]    |   VehicleResultsOrg   | Returned result list
        //bool                    |  [-]    |   fleetMix = false    | Optional parameter if fleetMix should be calculate
        //string                  |  [-]    |   PHEMDataV = "V4"    | Optional parameter for "Default Vehicles"
        //string                  |  [-]    |   CommentPref = "c"   | Optional parameter for comment prefix

        //********************************* OUPUT: VehicleResultsOrg **********************
        //***  DATATYP            |  UNIT   |  VARIBLE     |      Description  ***
        //double                  |  [s]    |   time       | Time
        //double                  |  [m/s]  |   speed      | Velocity
        //double                  |  [kW]   |   power      | Calculated power at the engine (ICE for conventional and HEV vehicles, electric engine for BEVs) including engine inertia and auxiliaries; not limited for engine fullload and braking limitations
        //double                  |  [kW]   |   P_pos      | Positive engine power limited with engine rated power
        //double                  |  [-]    |   pNormRated | Engine power normalised with rated engine power and limited with the power range (fullload and drag) as specified in the characteristic curve for fuel consumption
        //double                  |  [-]    |   pNormDrive | Engine power normalised with "P_drive" and limited with the power range (fullload and drag) as specified in the characteristic curve for emissions
        //double                  |  [m/s^2]|   acc        | Caclulated/given acceleration
        //double                  |  [g/h]  |   fc         | Calculated fuel consumption (Different unit for BEV vehicles)
        //                        |  [kWh/h]|              | 
        //double                  |  [g/h]  |   cO2        | Calculated CO2 emissions
        //double                  |  [g/h]  |   NOx        | Calculated NOx emissions
        //double                  |  [g/h]  |   HC         | Calculated HC emissions
        //double                  |  [g/h]  |   PM         | Calculated PM emissions
        //double                  |  [g/h]  |   CO         | Calculated CO emissions

        //Calculate data from array
        bool CALC_Array(const std::string& VEH, std::vector<double>& Time, std::vector<double>& Velocity, std::vector<double>& Gradient, std::vector<VehicleResult*>& VehicleResultsOrg, bool fleetMix = false, const std::string& PHEMDataV = "V4", const std::string& CommentPref = "c");

        //Calculate single data
        bool CALC_Single(const std::string& VEH, double Time, double Velocity, double acc, double Gradient, std::vector<VehicleResult*>& VehicleResultsOrg, bool fleetMix = false, const std::string& PHEMDataV = "V4", const std::string& CommentPref = "c");

        bool ExportData(const std::string& path, const std::string& vehicle, std::vector<VehicleResult*>& _VehicleResult);

        bool ExportSumData(const std::string& path, const std::string& vehicle, const std::string& cycle, VehicleResult* _VehicleResult);

        VehicleResult* GenerateSumData(std::vector<VehicleResult*>& _VehicleResult);

    private:
        void InitializeInstanceFields();

public:
        Start() {
            InitializeInstanceFields();
        }
    };

    //Calculation
    class PHEMLight {
    public:
        static VehicleResult* CreateVehicleStateData(Helpers* Helper, CEP* currCep, double time, double inputSpeed, double inputAcc, double Gradient = 0);
    };
}


#endif	//#ifndef PHEMlightSTART
