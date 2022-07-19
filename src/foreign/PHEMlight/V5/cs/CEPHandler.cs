#define FLEET
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Globalization;
using Newtonsoft.Json;

namespace PHEMlightdll
{
    public class CEPHandler
    {
        #region Constructor
        public CEPHandler() => _ceps = new Dictionary<string, CEP>();
        #endregion

        #region CEPS
        private Dictionary<string, CEP> _ceps;
        public Dictionary<string, CEP> CEPS => _ceps;
        #endregion

#if FLEET
        #region FleetShares
        private Dictionary<string, Dictionary<string, double>> _fleetShares;
        public Dictionary<string, Dictionary<string, double>> FleetShares => _fleetShares;
        #endregion
#endif

        #region GetCEP
        public bool GetCEP(List<string> DataPath, Helpers Helper, Correction DataCor)
        {
            if (!CEPS.ContainsKey(Helper.gClass))
            {
                if (!Load(DataPath, Helper, DataCor))
                    return false;
            }
            return true;
        }
        #endregion

        #if FLEET
        #region GetFleetCEP
        public bool GetFleetCEP(List<string> DataPath, string AggClass, Helpers Helper, Correction DataCor)
        {
            if (!CEPS.ContainsKey(Helper.gClass))
            {
                if (Constants.AGGREGATED_VEHICLECLASSES.Contains(AggClass))
                {
                    List<CEP> weightedCEPS = new List<CEP>();

                    if (FleetShares.ContainsKey(AggClass))
                    {
                        foreach (string aggVehClass in FleetShares[AggClass].Keys)
                        {
                            if (!Helper.setclass(aggVehClass))
                            {
                                return false;
                            }
                            if (!CEPS.ContainsKey(aggVehClass) && !Load(DataPath, Helper, DataCor, true))
                            {
                                return false;
                            }
                            weightedCEPS.Add(CEPS[aggVehClass] * FleetShares[AggClass][aggVehClass]);
                        }
                        if (CEP.CheckClass(weightedCEPS.ToArray()))
                        {
                            _ceps.Add(AggClass, CEP.AddRangeCeps(weightedCEPS.ToArray(), Helper));
                        }
                        else
                        {
                            Helper.ErrMsg = "The aggregated vehicle class (" + AggClass + ") includes heavy and non heavy vehicles! This is not allowed. Aggregated vehicle class not added.";
                            return false;
                        }

                        //Set the vehicle class back
                        Helper.gClass = AggClass;
                        Helper.pClass = _ceps[_ceps.Last().Key].FuelType;
                    }
                    else
                    {
                        Helper.ErrMsg = "The aggregated vehicle class (" + AggClass + ") is not available in the FleetShare file!";
                        return false;
                    }
                }
                else
                {
                    Helper.ErrMsg = "The aggregated vehicle class (" + AggClass + ") is a unknown class!";
                    return false;
                }
            }
            return true;
        }
        #endregion
#endif

        #region Correction
        private bool CalcCorrection(Correction DataCor, Helpers Helper, VEHPHEMLightJSON.Vehicle_Data vehicle_Data)
        {
            if (DataCor.UseDet)
            {
                DataCor.VehMileage = -1;
                if (vehicle_Data.Mileage.HasValue) DataCor.VehMileage = vehicle_Data.Mileage.Value;

                if (!DataCor.IniDETfactor(Helper))
                    return false;
            }
            if (DataCor.UseTNOx)
            {
                if (!DataCor.IniTNOxfactor(Helper))
                    return false;
            }

            //Return value
            return true;
        }
        #endregion

        #region Load
        private bool Load(List<string> DataPath, Helpers Helper, Correction DataCor, bool fleetMix = false)
        {
            //Deklaration
            // to hold everything.
            List<List<double>> matrixFCvalues;
            List<List<double>> matrixPollutants;
            List<double> idlingValuesFCvalues;
            List<double> idlingValuesPollutants;
            List<string> headerFCvalues;
            List<string> headerPollutants;
            VEHPHEMLightJSON.VEH Vehicle;

            if (!ReadVehicleFile(DataPath[0],
                                 Helper,
                                 fleetMix,
                                 out Vehicle))
                return false;

            if (DataCor != null)
            {
                if (!CalcCorrection(DataCor, Helper, Vehicle.VehicleData))
                    return false;
            }

            if (!ReadEmissionData(true, DataPath[1], Helper, fleetMix, DataCor, out headerFCvalues, out matrixFCvalues, out idlingValuesFCvalues))
                return false;
            if (!ReadEmissionData(false, DataPath[2], Helper, fleetMix, DataCor, out headerPollutants, out matrixPollutants, out idlingValuesPollutants))
                return false;

            _ceps.Add(Helper.gClass, new CEP(Vehicle,
                                headerFCvalues,
                                matrixFCvalues,
                                headerPollutants,
                                matrixPollutants,
                                idlingValuesFCvalues,
                                idlingValuesPollutants));
            return true;
        }
        #endregion

        #region ReadVehicleFile
        private bool ReadVehicleFile(string DataPath,
                                     Helpers Helper,
                                     bool fleetMix,
                                     out VEHPHEMLightJSON.VEH Vehicle)
        {
            string path = "";
            Vehicle = new VEHPHEMLightJSON.VEH();

            //Open file
            if (fleetMix)
                path = DataPath + @"\" + Helper.gClass + ".PHEMLight.veh";
            else
                path = DataPath;

            if (!File.Exists(@path))
            {
                Helper.ErrMsg = "File do not exist! (" + path + ")";
                return false;
            }

            //**** VEH Datei einlesen ****
            using (StreamReader r = new StreamReader(path))
            {
                try
                {
                    string json = r.ReadToEnd();
                    Vehicle = JsonConvert.DeserializeObject<VEHPHEMLightJSON.VEH>(json);
                }
                catch
                {
                    Helper.ErrMsg = "Error during file read! (" + path + ")";
                    return false;
                }
            }

            //**** Vehicle Datei übertragen ****
            //*** Get the vehicle data
            if (Vehicle.VehicleData.MassType == null) Vehicle.VehicleData.MassType = "LV";
            if (Vehicle.VehicleData.FuelType == null) Vehicle.VehicleData.FuelType = "D";
            if (Vehicle.VehicleData.CalcType == null) Vehicle.VehicleData.CalcType = "Conv";
            if (Vehicle.VehicleData.Mass == null) Vehicle.VehicleData.Mass = 0;
            if (Vehicle.VehicleData.Loading == null) Vehicle.VehicleData.Loading = 0;
            if (Vehicle.VehicleData.RedMassWheel == null) Vehicle.VehicleData.RedMassWheel = 0;
            if (Vehicle.VehicleData.WheelDiameter == null) Vehicle.VehicleData.WheelDiameter = 0;
            if (Vehicle.VehicleData.Cw == null) Vehicle.VehicleData.Cw = 0;
            if (Vehicle.VehicleData.A == null) Vehicle.VehicleData.A = 0;

            // Auxiliaries
            if (Vehicle.AuxiliariesData.Pauxnorm == null) Vehicle.AuxiliariesData.Pauxnorm = 0;

            // Engine Data
            if (Vehicle.EngineData.ICEData.Prated == null) Vehicle.EngineData.ICEData.Prated = 0;
            if (Vehicle.EngineData.ICEData.nrated == null) Vehicle.EngineData.ICEData.nrated = 0;
            if (Vehicle.EngineData.ICEData.Idling == null) Vehicle.EngineData.ICEData.Idling = 0;
            if (Vehicle.EngineData.EMData.Prated == null) Vehicle.EngineData.EMData.Prated = 0;
            if (Vehicle.EngineData.EMData.nrated == null) Vehicle.EngineData.EMData.nrated = 0;

            // Rolling resistance
            if (Vehicle.RollingResData.Fr0 == null) Vehicle.RollingResData.Fr0 = 0;
            if (Vehicle.RollingResData.Fr1 == null) Vehicle.RollingResData.Fr1 = 0;
            if (Vehicle.RollingResData.Fr2 == null) Vehicle.RollingResData.Fr2 = 0;
            if (Vehicle.RollingResData.Fr3 == null) Vehicle.RollingResData.Fr3 = 0;
            if (Vehicle.RollingResData.Fr4 == null) Vehicle.RollingResData.Fr4 = 0;

            // Transmission
            if (Vehicle.TransmissionData.AxelRatio == null) Vehicle.TransmissionData.AxelRatio = 0;
            if (Vehicle.TransmissionData.Transm == null)
            {
                Helper.ErrMsg = "Transmission ratios missing in vehicle file! Calculation stopped! (" + path + ")";
                return false;
            }
            else
            {
                if (!Vehicle.TransmissionData.Transm.ContainsKey("Speed"))
                {
                    Helper.ErrMsg = "No Speed signal in transmission data given! Calculation stopped! (" + path + ")";
                    return false;
                }
                if (!Vehicle.TransmissionData.Transm.ContainsKey("GearRatio"))
                {
                    Helper.ErrMsg = "No GearRatio signal in transmission data given! Calculation stopped! (" + path + ")";
                    return false;
                }
                if (!Vehicle.TransmissionData.Transm.ContainsKey("RotMassF"))
                {
                    Helper.ErrMsg = "No RotMassF signal in transmission data given! Calculation stopped! (" + path + ")";
                    return false;
                }
            }

            // Full load and drag
            if (Vehicle.FLDData.P_n_max_v0 == null) Vehicle.FLDData.P_n_max_v0 = 0;
            if (Vehicle.FLDData.P_n_max_p0 == null) Vehicle.FLDData.P_n_max_p0 = 0;
            if (Vehicle.FLDData.P_n_max_v1 == null) Vehicle.FLDData.P_n_max_v1 = 0;
            if (Vehicle.FLDData.P_n_max_p1 == null) Vehicle.FLDData.P_n_max_p1 = 0;
            if (Vehicle.FLDData.DragCurve == null)
            {
                Helper.ErrMsg = "Drag curve missing in vehicle file! Calculation stopped! (" + path + ")";
                return false;
            }
            else
            {
                if (!Vehicle.FLDData.DragCurve.ContainsKey("n_norm"))
                {
                    Helper.ErrMsg = "No n_norm signal in drag curve data given! Calculation stopped! (" + path + ")";
                    return false;
                }
                if (!Vehicle.FLDData.DragCurve.ContainsKey("pe_drag_norm"))
                {
                    Helper.ErrMsg = "No pe_drag_norm signal in drag curve data given! Calculation stopped! (" + path + ")";
                    return false;
                }
            }

            return true;
        }
        #endregion

        #region ReadEmissionData
        private bool ReadEmissionData(bool readFC,
                                      string DataPath,
                                      Helpers Helper,
                                      bool fleetMix,
                                      Correction DataCor,
                                      out List<string> header,
                                      out List<List<double>> matrix,
                                      out List<double> idlingValues)
        {
            // declare file stream
            string line;
            string path = "";
            header = new List<string>();
            matrix = new List<List<double>>();
            idlingValues = new List<double>();

            if (fleetMix)
            {
                if (readFC)
                    path = DataPath + @"\" + Helper.gClass + "_FC.csv";
                else
                    path = DataPath + @"\" + Helper.gClass + ".csv";
            }
            else
            {
                path = DataPath;
            }

            if (!File.Exists(path))
            {
                Helper.ErrMsg = "File do not exist! (" + path + ")";
                return false;
            }
            StreamReader fileReader = File.OpenText(@path);

            // read header line for pollutant identifiers
            if ((line = ReadLine(fileReader)) != null)
            {
                List<string> entries = split(line, ',');
                // skip first entry "Pe"
                for (int i = 1; i < entries.Count; i++)
                {
                    header.Add(entries[i]);
                }
            }

            // skip units
            ReadLine(fileReader);

            // skip comment
            ReadLine(fileReader);

            //readIdlingValues
            line = ReadLine(fileReader);

            List<string> stringIdlings = split(line, ',').ToList();
            stringIdlings.RemoveAt(0);

            idlingValues = todoubleList(stringIdlings);

            while ((line = ReadLine(fileReader)) != null)
            {
                matrix.Add(todoubleList(split(line, ',')));
            }
            fileReader.Close();

            //Data correction (Det & TNOx)
            if (!CorrectEmissionData(DataCor, ref header, ref matrix, ref idlingValues))
            {
                Helper.ErrMsg = "Error in correction calculation";
                return false;
            }
            
            //Return value
            return true;
        }

        private bool CorrectEmissionData(Correction DataCor,
                                         ref List<string> header,
                                         ref List<List<double>> matrix,
                                         ref List<double> idlingValues)
        {
            for (int i = 0; i < header.Count; i++)
            {
                double CorF = GetDetTempCor(DataCor, header[i]);
                if (CorF != 1)
                {
                    for (int j = 0; j < matrix.Count; j++)
                    {
                        matrix[j][i + 1] *= CorF;
                    }
                    idlingValues[i] *= CorF;
                }
            }

            //Return value
            return true;
        }

        //Calculate correction factor for detoriation and temperature correction
        private double GetDetTempCor(Correction DataCor, string Emi)
        {
            //Initialisation
            double CorF = 1;

            if (DataCor != null)
            {
                if (DataCor.UseDet)
                {
                    foreach (string Key in DataCor.DETFactors.Keys)
                    {
                        if (Emi.ToUpper() == Key.ToUpper())
                        {
                            CorF += (DataCor.DETFactors[Key] - 1);
                            break;
                        }
                    }
                }
                if (DataCor.UseTNOx)
                {
                    if (Emi.ToUpper().Contains("NOX")) CorF += (DataCor.TNOxFactor - 1);
                }
            }

            //Return value
            return CorF;
        }
        #endregion

#if FLEET
        #region ReadFleetShares
        public bool ReadFleetShares(string DataPath, Helpers Helper)
        {
            //Declaration
            string line;
            string path = DataPath;
            if (!File.Exists(@path))
            {
                Helper.ErrMsg = "FleetShares file does not exist! (" + path + ")";
                return false;
            }
            StreamReader shareReader = File.OpenText(@path);

            _fleetShares = new Dictionary<string, Dictionary<string, double>>();

            while ((line = ReadLine(shareReader)) != null)
            {
                if (line.Substring(0, 1) == Helper.CommentPrefix)
                    continue;

                List<string> splitLine = split(line, ',');
                string aggregateClass = splitLine[0];

                if (!FleetShares.ContainsKey(aggregateClass))
                    FleetShares.Add(aggregateClass, new Dictionary<string, double>());

                string subClass = splitLine[1];

                if (!FleetShares[aggregateClass].ContainsKey(subClass))
                    FleetShares[aggregateClass].Add(subClass, todouble(splitLine[2]));
            }
            return true;
        }
        #endregion
        #endif

        #region Functions
        //Split the string
        private List<string> split(string s, char delim)
        {
            return s.Split(delim).ToList();
        }

        //Convert string to double
        private double todouble(string s)
        {
            return double.Parse(s, CultureInfo.InvariantCulture);
        }

        //Convert string to double list
        private List<double> todoubleList(List<string> s)
        {
            return s.Select(p => todouble(p)).Cast<double>().ToList();
        }

        //Read a line from file
        private string ReadLine(StreamReader s)
        {
            return s.ReadLine();
        }
        #endregion
    }

    //PHEMLight vehicle
    public class VEHPHEMLightJSON
    {
        //Root object
        public class VEH
        {
            public string Type { get; set; }
            public string Version { get; set; }
            public Vehicle_Data VehicleData { get; set; }
            public Aux_Data AuxiliariesData { get; set; }
            public Engine_Data EngineData { get; set; }
            public Rollres_Data RollingResData { get; set; }
            public FullLoadDrag_Data FLDData { get; set; }
            public Transmission_Data TransmissionData { get; set; }

            public VEH()
            {
                VehicleData = new Vehicle_Data();
                RollingResData = new Rollres_Data();
                EngineData = new Engine_Data();
                AuxiliariesData = new Aux_Data();
                FLDData = new FullLoadDrag_Data();
                TransmissionData = new Transmission_Data();
            }
        }

        #region Vehicle Data
        // Vehicle data
        public class Vehicle_Data
        {
            public string MassType { get; set; }
            public string FuelType { get; set; }
            public string CalcType { get; set; }
            public double? Mass { get; set; }
            public double? Loading { get; set; }
            public double? RedMassWheel { get; set; }
            public double? WheelDiameter { get; set; }
            public double? Cw { get; set; }
            public double? A { get; set; }
            public double? Mileage { get; set; }
        }
        #endregion

        #region Rolling Resistance
        // Rolling resistance data
        public class Rollres_Data
        {
            public double? Fr0 { get; set; }
            public double? Fr1 { get; set; }
            public double? Fr2 { get; set; }
            public double? Fr3 { get; set; }
            public double? Fr4 { get; set; }
        }
        #endregion

        #region Engine
        // Engine data
        public class Engine_Data
        {
            public ICE_Data ICEData { get; set; }
            public EM_Data EMData { get; set; }

            public Engine_Data()
            {
                ICEData = new ICE_Data();
                EMData = new EM_Data();
            }
        }

        // ICE engine
        public class ICE_Data
        {
            public double? Prated { get; set; }
            public double? nrated { get; set; }
            public double? Idling { get; set; }
        }

        // EM engine
        public class EM_Data
        {
            public double? Prated { get; set; }
            public double? nrated { get; set; }
        }
        #endregion

        #region Auxiliaries
        // Auxiliaries data
        public class Aux_Data
        {
            public double? Pauxnorm { get; set; }
        }
        #endregion

        #region Full load and Drag data
        // Full load and Drag data
        public class FullLoadDrag_Data
        {
            public double? P_n_max_v0 { get; set; }
            public double? P_n_max_p0 { get; set; }
            public double? P_n_max_v1 { get; set; }
            public double? P_n_max_p1 { get; set; }
            public Dictionary<string, List<double>> DragCurve { get; set; }

            public FullLoadDrag_Data()
            {
                DragCurve = new Dictionary<string, List<double>>();
            }
        }
        #endregion

        #region Transmission
        // Transmission data
        public class Transmission_Data
        {
            public double? AxelRatio { get; set; }
            public Dictionary<string, List<double>> Transm { get; set; }

            public Transmission_Data()
            {
                Transm = new Dictionary<string, List<double>>();
            }
        }
        #endregion
    }
}
