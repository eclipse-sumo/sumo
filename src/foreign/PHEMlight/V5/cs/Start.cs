#define FLEET
using System;
using System.IO;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;

namespace PHEMlightdll
{
    public class Start
    {
        private List<string> _DataPath;
        private CEPHandler DataInput;
        public Helpers Helper = new Helpers();

        //******************* Parameters of Array or Single calculation *******************
        //********************************* INPUT ******************************************
        //***  DATATYP              |  UNIT   |  VARIBLE                  |     Description  ***
        //List<string>              |  [-]    |   DataFiles (VEH, FC, EMI)| Name of file (e.g. "PC_D_EU4" path neede if not in "Default Vehicles") or aggregated name (PC, HDV, BUS, TW) by FleetMix calculation
        //List<double> / double     |  [s]    |   Time                    | Time signal
        //List<double> / double     |  [m/s]  |   Velocity                | Velocity signal
        //double                    |  [m/s^2]|   acc                     | Acceleration (ONLY NEDDED BY SINGLE CALCULATION)
        //List<double> / double     |  [%]    |   Gradient                | Gradient of the route
        //out List<VehicleResult>   |  [-]    |   VehicleResultsOrg       | Returned result list
        //bool                      |  [-]    |   fleetMix = false        | Optional parameter if fleetMix should be calculate
        //string                    |  [-]    |   CommentPref = "c"       | Optional parameter for comment prefix

        //********************************* OUPUT: VehicleResultsOrg **********************
        //***  DATATYP              |  UNIT   |  VARIBLE       |      Description  ***
        //string                    |  [-]    |   vehicle      | Name of the vehicle
        //string                    |  [-]    |   cycle        | Name of the cycle
        //double                    |  [s]    |   time         | Time
        //double                    |  [m/s]  |   speed        | Velocity
        //double                    |  [kW]   |   power        | Calculated power at the engine (ICE for conventional and HEV vehicles, electric engine for BEVs) including engine inertia and auxiliaries; not limited for engine fullload and braking limitations
        //double                    |  [kW]   |   P_pos        | Positive engine power limited with engine rated power
        //double                    |  [-]    |   pNormRated   | Engine power normalised with rated engine power and limited with the power range (fullload and drag) as specified in the characteristic curve for fuel consumption
        //double                    |  [-]    |   pNormDrive   | Engine power normalised with "P_drive" and limited with the power range (fullload and drag) as specified in the characteristic curve for emissions
        //double                    |  [m/s^2]|   acc          | Caclulated/given acceleration
        //Dictionary<string, double>|  [*]    |   Emissiondata | Calculated emissions for all components which are defined in the emission curves. Unit dependent of emission component
        

        #region calculate
        //Calculate data from array
        public bool CALC_Array(List<string> DataFiles, 
                               List<double> Time, 
                               List<double> Velocity,
                               List<double> Gradient,
                               out List<VehicleResult> VehicleResultsOrg,
                               bool fleetMix = false,
                               Correction DataCor = null,
                               string CommentPref = "c")
        {
            //Declaration
            int i;
            double acc;
            List<VehicleResult>  _VehicleResult = new List<VehicleResult>();

            //Initialisation
            Helper.ErrMsg = null;

            //Borrow
            Helper.CommentPrefix = CommentPref;
            _DataPath = new List<string>();
            //Set path by normal calculation (on given) and set path by fleetmix (on Default Vehicles) calculation
            for (i = 0; i < DataFiles.Count; i++)
            {
                if ((DataFiles[i].LastIndexOf(@"\")) >= 0)
                {
                    _DataPath.Add(DataFiles[i]);
                }
                else
                {
                    //_DataPath.Add(Assembly.GetExecutingAssembly().Location.Substring(0, Assembly.GetExecutingAssembly().Location.LastIndexOf(@"\")) + @"\Default Vehicles\" + Helper.PHEMDataV);
                    _DataPath.Add(DataFiles[i + 1].Substring(0, DataFiles[i + 1].LastIndexOf(@"\")));
                    _DataPath.Add(DataFiles[i + 1].Substring(0, DataFiles[i + 1].LastIndexOf(@"\")));
                    _DataPath.Add(DataFiles[i + 1].Substring(0, DataFiles[i + 1].LastIndexOf(@"\")));
                    i += 1;
                }
            }

            //Read the vehicle and emission data
            #if FLEET
            if (fleetMix)
            {
                //Set the vehicle class
                Helper.gClass = _DataPath[0];

                //Generate the class
                DataInput = new CEPHandler();

                //Read the FleetShares
                if (!DataInput.ReadFleetShares(DataFiles[1], Helper))
                {
                    VehicleResultsOrg = null;
                    return false;
                }
                //Read the vehicle and emission data
                if (!DataInput.GetFleetCEP(_DataPath, DataFiles[0], Helper, DataCor))
                {
                    VehicleResultsOrg = null;
                    return false;
                }
            }
            else
            #endif
            {
                //Get vehicle string
                if (!Helper.setclass(DataFiles[0]))
                {
                    VehicleResultsOrg = null;
                    return false;
                }

                //Generate the class
                DataInput = new CEPHandler();

                //Read the vehicle and emission data
                if (!DataInput.GetCEP(_DataPath, Helper, DataCor))
                {
                    VehicleResultsOrg = null;
                    return false;
                }
            }

            //Calculate emissions per second
            for (i = 1; i <= Time.Count - 1; i++)
            {
                //Calculate the acceleration
                acc = (Velocity[i] - Velocity[i - 1]) / (Time[i] - Time[i - 1]);
                
                //Calculate and save the data in the List
                _VehicleResult.Add(PHEMLight.CreateVehicleStateData(Helper,
                                                                    DataInput.CEPS[Helper.gClass],
                                                                    Time[i - 1],
                                                                    Velocity[i - 1],
                                                                    acc,
                                                                    Gradient[i - 1]));
                if (Helper.ErrMsg != null)
                {
                    VehicleResultsOrg = null;
                    return false;
                }   
            }
            VehicleResultsOrg = _VehicleResult;
            return true;
        }

        //Calculate single data
        public bool CALC_Single(List<string> DataFiles,
                                double Time,
                                double Velocity,
                                double acc,
                                double Gradient,
                                out List<VehicleResult> VehicleResultsOrg,
                                bool fleetMix = false,
                                Correction DataCor = null,
                                string CommentPref = "c")
        {
            //Declaration
            List<VehicleResult> _VehicleResult = new List<VehicleResult>();
            VehicleResultsOrg = _VehicleResult;

            //Borrow
            Helper.CommentPrefix = CommentPref;
            _DataPath = new List<string>();
            //Set path by normal calculation (on given) and set path by fleetmix (on Fleetshare file) calculation
            for (int i = 0; i < DataFiles.Count; i++)
            {
                if ((DataFiles[i].LastIndexOf(@"\")) >= 0)
                {
                    _DataPath.Add(DataFiles[i]);
                }
                else
                {
                    //_DataPath.Add(Assembly.GetExecutingAssembly().Location.Substring(0, Assembly.GetExecutingAssembly().Location.LastIndexOf(@"\")) + @"\Default Vehicles\" + Helper.PHEMDataV);
                    _DataPath.Add(DataFiles[i + 1].Substring(0, DataFiles[i + 1].LastIndexOf(@"\")));
                    _DataPath.Add(DataFiles[i + 1].Substring(0, DataFiles[i + 1].LastIndexOf(@"\")));
                    _DataPath.Add(DataFiles[i + 1].Substring(0, DataFiles[i + 1].LastIndexOf(@"\")));
                    i += 1;
                }
            }

            //Read the vehicle and emission data
            #if FLEET
            if (fleetMix)
            {
                //Set the vehicle class
                Helper.gClass = "AggClass_" + DataFiles[0];

                //Generate the class
                DataInput = new CEPHandler();

                //Read the FleetShares
                if (!DataInput.ReadFleetShares(DataFiles[1], Helper))
                {
                    VehicleResultsOrg = null;
                    return false;
                }
                //Read the vehicle and emission data
                if (!DataInput.GetFleetCEP(_DataPath, DataFiles[0], Helper, DataCor))
                {
                    VehicleResultsOrg = null;
                    return false;
                }
            }
            else
            #endif
            {
                //Get vehicle string
                if (!Helper.setclass(DataFiles[0]))
                {
                    VehicleResultsOrg = null;
                    return false;
                }

                //Generate the class
                DataInput = new CEPHandler();

                //Read the vehicle and emission data
                if (!DataInput.GetCEP(_DataPath, Helper, DataCor))
                {
                    VehicleResultsOrg = null;
                    return false;
                }
            }

            //Calculate and save the data in the List
            _VehicleResult.Add(PHEMLight.CreateVehicleStateData(Helper,
                                                                DataInput.CEPS[Helper.gClass],
                                                                Time,
                                                                Velocity,
                                                                acc,
                                                                Gradient));
            VehicleResultsOrg = _VehicleResult;
            return true;
        }
        #endregion

        #region ExportData
        private Dictionary<string, cErgEntry> ErgEntries = new Dictionary<string, cErgEntry>();
        private List<string> ErgEntryList = new List<string>();                                   //Needed because Dictionary is not sorted

        //Add to ERG file
        private void AddToErg(string IDstring, string Head, string Unit)
        {
            if (!ErgEntries.ContainsKey(IDstring))
            {
                ErgEntries.Add(IDstring, new cErgEntry(Head, Unit));
                ErgEntryList.Add(IDstring);
            }
        }

        //Create result head
        private string ErgHead()
        {
            //Declaration
            StringBuilder s = new StringBuilder();
            string key = null;
            bool First = true;

            foreach (string key_loopVariable in ErgEntryList)
            {
                key = key_loopVariable;
                if (!First)
                    s.Append(",");
                s.Append(ErgEntries[key].Head);
                First = false;
            }
            //Return value
            return s.ToString();
        }

        //Create result units
        private string ErgUnits()
        {
            //Declaration
            StringBuilder s = new StringBuilder();
            bool First = true;
            string key = null;

            foreach (string key_loopVariable in ErgEntryList)
            {
                key = key_loopVariable;
                if (!First)
                    s.Append(",");
                s.Append(ErgEntries[key].Unit);
                First = false;
            }

            //Return value
            return s.ToString();
        }

        //Output sequence for the emissions
        private void OutSeq(VehicleResult _VehicleResult, bool STA = false, bool add = false)
        {
            string Unit = "/km";
            List<string> OutSeqStr = new List<string> { "FC", "FC_EL", "CO2", "NOX", "CO", "HC", "PM", "PN" };

            if (STA) Unit = "/h";

            if (!add)
            {
                //Clear the result arrays
                ErgEntries.Clear();
                ErgEntryList.Clear();

                AddToErg("FC", "FC", "[g" + Unit + "]");
                AddToErg("FC_EL", "Engine Power", "[kWh" + Unit + "]");
                AddToErg("CO2", "CO2", "[g" + Unit + "]");
                AddToErg("NOX", "NOx", "[g" + Unit + "]");
                AddToErg("CO", "CO", "[g" + Unit + "]");
                AddToErg("HC", "HC", "[g" + Unit + "]");
                AddToErg("PM", "PM", "[g" + Unit + "]");
                AddToErg("PN", "PN", "[#" + Unit + "]");

                foreach (string id in _VehicleResult.EmissionData.Emi.Keys)
                {
                    if (!OutSeqStr.Contains(id)) AddToErg(id.ToUpper(), id, "[g" + Unit + "]");
                }
            }
            else
            {
                foreach (string id in _VehicleResult.EmissionData.Emi.Keys)
                {
                    if (!ErgEntries.ContainsKey(id.ToUpper())) AddToErg(id.ToUpper(), id, "[g" + Unit + "]");
                }
            }
        }

        //Export the data
        public bool ExportData(string path, List<VehicleResult> _VehicleResult)
        {
            if (path == null || _VehicleResult == null || _VehicleResult.Count == 0) return false;

            //Write head
            StringBuilder allLines = new StringBuilder();
            string lineEnding = "\r\n";

            //Produce emission output sequence. Only first one needed because should be always the same for a single vehicle
            OutSeq(_VehicleResult[0], true);

            //Vehicle type
            allLines.AppendLine("Vehicletype: ," + _VehicleResult[0].Vehicle);

            //Header and unit
            allLines.AppendLine("Time, Speed, Gradient, Accelaration, Engine power raw, P_pos, P_norm_rated, P_norm_drive," + ErgHead());
            allLines.AppendLine("[s], [m/s], [%], [m/s^2], [kW], [kW], [-], [-]," + ErgUnits());

            //Write data
            foreach (VehicleResult Result in _VehicleResult)
            {
                allLines.Append(Result.Time.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.Speed.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.Grad.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.Accelaration.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.Power.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.PPos.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.PNormRated.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.PNormDrive.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                foreach (string id in ErgEntryList)
                {
                    if (Result.EmissionData.Emi.ContainsKey(id))
                        allLines.Append(Result.EmissionData.Emi[id].ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                    else
                        allLines.Append("-,");
                }
                allLines.Append(lineEnding);
            }

            // Write the string to a file.
            if (path.IndexOf(".", 0) < 0)
            {
                path = path + ".sta";
            }
            try
            {
                StreamWriter file = new StreamWriter(path);
                file.WriteLine(allLines);
                file.Close();
                return true;
            }
            catch (Exception ex)
            {
                return false;
            }
        }

        //Export summerized data
        public bool ExportSumData(string path, List<VehicleResult> _VehicleResult)
        {
            if (path == null || _VehicleResult == null) return false;
            StringBuilder allLines = new StringBuilder();

            if (path.IndexOf(".", 0) < 0)
            {
                path = path + ".erg";
            }

            //Produce emission output sequence for all calculated vehicles
            OutSeq(_VehicleResult[0], false);
            foreach (VehicleResult Result in _VehicleResult)
            {
                OutSeq(Result, false, true);
            }

            if (!File.Exists(path))
            {
                //Write head
                allLines.AppendLine("PHEMLight Results");
                allLines.AppendLine("");
                allLines.AppendLine("Vehicle, Cycle, Time, Speed, Gradient, Accelaration, Engine power raw, P_pos, P_norm_rated, P_norm_drive," + ErgHead());
                allLines.AppendLine("[-], [-], [s], [km/h], [%], [m/s^2], [kW], [kW], [-], [-]," + ErgUnits());
            }

            //Write data
            foreach (VehicleResult Result in _VehicleResult)
            {
                allLines.Append(Result.Vehicle + ",");
                allLines.Append(Result.Cycle + ",");
                allLines.Append(Result.Time.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.Speed.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.Grad.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.Accelaration.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.Power.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.PPos.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.PNormRated.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                allLines.Append(Result.PNormDrive.ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                foreach (string id in ErgEntryList)
                {
                    if (Result.EmissionData.Emi.ContainsKey(id))
                        allLines.Append(Result.EmissionData.Emi[id].ToString("0.0000", CultureInfo.InvariantCulture) + ",");
                    else
                        allLines.Append("-,");
                }
                allLines.Append("\r\n");
            }
            // Write the string to a file.
            try
            {
                StreamWriter file = new StreamWriter(path, true);
                file.WriteLine(allLines);
                file.Close();
                return true;
            }
            catch (Exception ex)
            {
                return false;
            }
        }

        public VehicleResult GenerateSumData(List<VehicleResult> _VehicleResult)
        {
            //Declaration
            string vehicle = "";
            string cycle = "";
            double sum_time = 0;
            double sum_speed = 0;
            double sum_grad = 0;
            double sum_power = 0;
            double sum_pPos = 0;
            double sum_pNormRated = 0;
            double sum_pNormDrive = 0;
            double sum_acc = 0;
            Dictionary<string, double> sum_Emi = new Dictionary<string, double>();

            if (_VehicleResult == null || _VehicleResult.Count == 0) return new VehicleResult("", "", 0, 0, 0, 0, 0, 0, 0, 0, sum_Emi);

            //Vehicle and cycle are always the same here
            vehicle = _VehicleResult[0].Vehicle;
            cycle = _VehicleResult[0].Cycle;

            //Write data
            foreach (VehicleResult Result in _VehicleResult)
            {
                sum_speed += Result.Speed * 3.6;
                sum_power += Result.Power;
                if (Result.PPos > 0) { sum_pPos += Result.PPos; }
                sum_grad += Result.Grad;
                sum_pNormRated += Result.PNormRated;
                sum_pNormDrive += Result.PNormDrive;
                sum_acc += Result.Accelaration;
                foreach(string id in Result.EmissionData.Emi.Keys)
                {
                    if (sum_Emi.ContainsKey(id))
                        sum_Emi[id] += Result.EmissionData.Emi[id];
                    else
                        sum_Emi.Add(id, Result.EmissionData.Emi[id]);
                }
            }

            //Build average
            sum_time = _VehicleResult[_VehicleResult.Count - 1].Time - _VehicleResult[0].Time;
            sum_power /= _VehicleResult.Count;
            sum_pPos /= _VehicleResult.Count;
            sum_grad /= _VehicleResult.Count;
            sum_pNormRated /= _VehicleResult.Count;
            sum_pNormDrive /= _VehicleResult.Count;
            sum_acc /= _VehicleResult.Count;
            if (sum_speed > 0)
            {
                foreach (string id in sum_Emi.Keys.ToList())
                {
                    sum_Emi[id] /= sum_speed;
                }
            }
            else
            {
                foreach (string id in sum_Emi.Keys)
                {
                    sum_Emi[id] = 0;
                }
            }
            sum_speed /= _VehicleResult.Count;

            return new VehicleResult(vehicle,
                                     cycle,
                                     sum_time,
                                     sum_speed,
                                     sum_grad,
                                     sum_power,
                                     sum_pPos,
                                     sum_pNormRated,
                                     sum_pNormDrive,
                                     sum_acc,
                                     sum_Emi);
        }
        #endregion
    }

    //Calculation
    class PHEMLight
    {
        #region CreateVehicleStateData
        static public VehicleResult CreateVehicleStateData(Helpers Helper,
                                                           CEP currCep,
                                                           double time,
                                                           double inputSpeed,
                                                           double inputAcc,
                                                           double Gradient = 0,
                                                           Correction DataCor = null)
        {
            //Declaration
            double speed = Math.Max(inputSpeed, 0);
            double acc;
            double P_pos;

            //Speed/Acceleration limitation
            if (speed == 0)
                acc = 0;
            else
                acc = Math.Min(inputAcc, currCep.GetMaxAccel(speed, Gradient, (Helper.pClass == Constants.strBEV | Helper.uClass == Constants.strHybrid)));

            //Calculate the power
            double power = currCep.CalcPower(speed, acc, Gradient, (Helper.pClass == Constants.strBEV | Helper.uClass == Constants.strHybrid));
            double P_eng = currCep.CalcEngPower(power);
            double Pwheel = 0;
            if (Helper.uClass == Constants.strHybrid) Pwheel = currCep.CalcWheelPower(speed, acc, Gradient);
            //Power limitation
            if (P_eng >= 0) 
                P_pos = power;
            else 
                P_pos = 0;

            //Calculate the result values (BEV)
            if (Helper.pClass == Constants.strBEV)
            {
                return new VehicleResult(Helper.gClass,
                                         "",
                                         time,
                                         speed,
                                         Gradient,
                                         P_eng,
                                         P_pos,
                                         P_eng / currCep.RatedPower,
                                         P_eng / currCep.DrivingPower,
                                         acc,
                                         currCep.GetAllEmission(P_eng, speed, Helper));
            }

            //Calculate the decel costing
            double decelCoast = currCep.GetDecelCoast(speed, acc, Gradient);

            //Calculate the result values (Zero emissions by costing, Idling emissions by v <= 0.5m / s²)
            if (acc >= decelCoast || speed <= Constants.ZERO_SPEED_ACCURACY)
            {
                if (Helper.uClass == Constants.strHybrid)
                    return new VehicleResult(Helper.gClass,
                         "",
                         time,
                         speed,
                         Gradient,
                         P_eng,
                         P_pos,
                         P_eng / currCep.RatedPower,
                         P_eng / currCep.DrivingPower,
                         acc,
                         currCep.GetAllEmission(Pwheel, speed, Helper));
                else
                    return new VehicleResult(Helper.gClass,
                                             "",
                                             time,
                                             speed,
                                             Gradient,
                                             P_eng,
                                             P_pos,
                                             P_eng / currCep.RatedPower,
                                             P_eng / currCep.DrivingPower,
                                             acc,
                                             currCep.GetAllEmission(P_eng, speed, Helper));
            }
            else
            {
                if (Helper.uClass == Constants.strHybrid)
                    return new VehicleResult(Helper.gClass,
                     "",
                     time,
                     speed,
                     Gradient,
                     P_eng,
                     P_pos,
                     P_eng / currCep.RatedPower,
                     P_eng / currCep.DrivingPower,
                     acc,
                     currCep.GetAllEmission(Pwheel, speed, Helper, true));
                else
                    return new VehicleResult(Helper.gClass,
                                         "",
                                         time,
                                         speed,
                                         Gradient,
                                         P_eng,
                                         P_pos,
                                         P_eng / currCep.RatedPower,
                                         P_eng / currCep.DrivingPower,
                                         acc,
                                         currCep.GetAllEmission(P_eng, speed, Helper, true));
            }
        }
        #endregion
    }
}
