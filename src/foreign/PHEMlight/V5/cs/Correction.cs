using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace PHEMlightdll
{
    public class Correction
    {
        public Correction(string PathCor)
        {
            UseDet = false;
            UseTNOx = false;
            AmbTemp = 20;
            Year = DateTime.Now.Year;
            VehMileage = -1;
            DETFilePath = Path.Combine(PathCor, "Deterioration.det");
            VMAFilePath = Path.Combine(PathCor, "Mileage.vma");
            TNOxFilePath = Path.Combine(PathCor, "NOxCor.tno");
        }

        #region ReadDetoriationFiles
        private DET DETdata;
        private VMA VMAdata;
        public Dictionary<string, double> DETFactors;

        #region Properties
        public bool UseDet { get; set; }

        public string DETFilePath { get; set; }

        public string VMAFilePath { get; set; }

        public int Year { get; set; }

        public double VehMileage { get; set; }
        #endregion

        //Read the file
        public bool ReadDet(ref string ErrMSG)
        {
            //Read Detoriation file
            if (!ReadDETFile(ref ErrMSG))
                return false;

            if (!ReadVMAFile(ref ErrMSG))
                return false;

            // Return value
            return true;
        }
        private bool ReadDETFile(ref string ErrMSG)
        {
            //Check the file if exist
            if ((string.IsNullOrEmpty(DETFilePath)) || (!File.Exists(DETFilePath)))
            {
                ErrMSG = "File not found (" + DETFilePath + ") !";
                return false;
            }

            //**** Datei einlesen ****
            DETdata = new DET();
            using (StreamReader r = new StreamReader(DETFilePath))
            {
                try
                {
                    string json = r.ReadToEnd();
                    DETdata = JsonConvert.DeserializeObject<DET>(json);
                }
                catch
                {
                    ErrMSG = "Error during file read! " + "(" + DETFilePath + ")";
                    return false;
                }
            }

            // Return value
            return true;
        }
        private bool ReadVMAFile(ref string ErrMSG)
        {
            //Check the file if exist
            if ((string.IsNullOrEmpty(VMAFilePath)) || (!File.Exists(VMAFilePath)))
            {
                ErrMSG = "File not found (" + VMAFilePath + ") !";
                return false;
            }

            //**** Datei einlesen ****
            VMAdata = new VMA();
            using (StreamReader r = new StreamReader(VMAFilePath))
            {
                try
                {
                    string json = r.ReadToEnd();
                    VMAdata = JsonConvert.DeserializeObject<VMA>(json);
                }
                catch
                {
                    ErrMSG = "Error during file read! " + "(" + VMAFilePath + ")";
                    return false;
                }
            }

            // Return value
            return true;
        }
        #endregion

        #region CalcDetoriation
        //Initialise the Detoriation Factor for the vehicle
        public bool IniDETfactor(Helpers Helper)
        {
            //Initialise
            DETFactors = new Dictionary<string, double>();

            if (DETdata.Vehicle.ContainsKey(Helper.vClass))
            {
                if (DETdata.Vehicle[Helper.vClass].PropulsionClass.ContainsKey(Helper.pClass))
                {
                    foreach (string Key in DETdata.Vehicle[Helper.vClass].PropulsionClass[Helper.pClass].Emission.Keys)
                    {
                        string EUclass = Helper.eClass.Replace("EU", "EURO ");

                        //PC special classes check (ab, c, d, d-Temp). If available use otherwise use "EURO 6" if available
                        if ((Helper.vClass == Constants.strPKW | Helper.vClass == Constants.strLNF) & EUclass.Length > 6)
                        {
                            string EUclassShort = EUclass.Substring(0, 6);

                            if (!DETdata.Vehicle[Helper.vClass].PropulsionClass[Helper.pClass].Emission[Key].EUClass.ContainsKey(EUclass) & 
                                DETdata.Vehicle[Helper.vClass].PropulsionClass[Helper.pClass].Emission[Key].EUClass.ContainsKey(EUclassShort))
                                EUclass = EUclassShort;
                        }

                        //Get the factor
                        if (DETdata.Vehicle[Helper.vClass].PropulsionClass[Helper.pClass].Emission[Key].EUClass.ContainsKey(EUclass))
                        {
                            List<double> Mileage = DETdata.Vehicle[Helper.vClass].PropulsionClass[Helper.pClass].Emission[Key].Mileage;
                            List<double> Factor = DETdata.Vehicle[Helper.vClass].PropulsionClass[Helper.pClass].Emission[Key].EUClass[EUclass];
                            if (VehMileage < 0) VehMileage = GetMileage(Helper);

                            for (int i = 1; i < Mileage.Count; i++)
                            {
                                if (i == 1 & Mileage[i] > VehMileage)
                                {
                                    DETFactors.Add(Key, Factor[0]);
                                    break;
                                }
                                else if (i == Mileage.Count - 1 && VehMileage > Mileage[i])
                                {
                                    DETFactors.Add(Key, Factor[i]);
                                    break;
                                }
                                else if (VehMileage < Mileage[i])
                                {
                                    DETFactors.Add(Key, Interpolate(VehMileage, Mileage[i - 1], Mileage[i], Factor[i - 1], Factor[i]));
                                    break;
                                }
                            }
                        }
                        else
                        {
                            DETFactors.Add(Key, 1);
                        }
                    }
                }
            }

            //Return value
            return true;
        }

        //Get the milage of the vehicle
        private double GetMileage(Helpers Helper)
        {
            // Initialise
            double Mileage = 0;

            if (VMAdata.Vehicle.ContainsKey(Helper.vClass))
            {
                if (VMAdata.Vehicle[Helper.vClass].PropulsionClass.ContainsKey(Helper.pClass))
                {
                    string Sclass = "0";
                    switch (Helper.sClass)
                    {
                        case "":
                            Sclass = "0";
                            break;
                        case "I":
                            Sclass = "1";
                            break;
                        case "II":
                            Sclass = "2";
                            break;
                        case "III":
                            Sclass = "3";
                            break;
                    }

                    if (VMAdata.Vehicle[Helper.vClass].PropulsionClass[Helper.pClass].SizeClass.ContainsKey(Sclass))
                    {
                        string EUclass = Helper.eClass.Replace("EU", "EURO ");

                        //PC special classes check (ab, c, d, d-Temp). If available use otherwise use "EURO 6" if available
                        if ((Helper.vClass == Constants.strPKW | Helper.vClass == Constants.strLNF) & EUclass.Length > 6)
                        {
                            string EUclassShort = EUclass.Substring(0, 6);

                            if (!VMAdata.Vehicle[Helper.vClass].PropulsionClass[Helper.pClass].SizeClass[Sclass].EUClass.ContainsKey(EUclass) &
                                VMAdata.Vehicle[Helper.vClass].PropulsionClass[Helper.pClass].SizeClass[Sclass].EUClass.ContainsKey(EUclassShort))
                                EUclass = EUclassShort;
                        }

                        if (VMAdata.Vehicle[Helper.vClass].PropulsionClass[Helper.pClass].SizeClass[Sclass].EUClass.ContainsKey(EUclass))
                        {
                            //Calculate Mileage
                            List<double> Factor = VMAdata.Vehicle[Helper.vClass].PropulsionClass[Helper.pClass].SizeClass[Sclass].EUClass[EUclass];
                            int AnzYear = Year - 2020 + 1;
                            Mileage = Factor[0] * Math.Pow(AnzYear, 3) + Factor[1] * Math.Pow(AnzYear, 2) + Factor[2] * AnzYear + Factor[3];

                            //Check calculated mileage
                            if (Mileage < 0) Mileage = 0;
                        }
                    }
                }
            }


            //Return value
            return Mileage;
        }
        #endregion

        #region ReadTNOx
        private TNOx TNOxdata;

        #region Properties
        public bool UseTNOx { get; set; }

        public string TNOxFilePath { get; set; }

        public double AmbTemp { get; set; }

        public double TNOxFactor { get; set; }
        #endregion

        //Read the file
        public bool ReadTNOx(ref string ErrMSG)
        {
            //Check the file if exist
            if ((string.IsNullOrEmpty(TNOxFilePath)) || (!File.Exists(TNOxFilePath)))
            {
                ErrMSG = "File not found (" + TNOxFilePath + ") !";
                return false;
            }

            //**** Datei einlesen ****
            TNOxdata = new TNOx();
            using (StreamReader r = new StreamReader(TNOxFilePath))
            {
                try
                {
                    string json = r.ReadToEnd();
                    TNOxdata = JsonConvert.DeserializeObject<TNOx>(json);
                }
                catch
                {
                    ErrMSG = "Error during file read! " + " (" + TNOxFilePath + ")";
                    return false;
                }
            }

            // Return value
            return true;
        }
        #endregion

        #region Calculate TNOx
        public bool IniTNOxfactor(Helpers Helper)
        {
            //Initialise
            TNOxFactor = 1;

            //Calculation only for diesel vehicles
            if (Helper.pClass != Constants.strDiesel) return true;

            if (TNOxdata.Vehicle.ContainsKey(Helper.vClass))
            {
                string EUclass = Helper.eClass.Replace("EU", "EURO ");

                //PC special classes check (ab, c, d, d-Temp). If available use otherwise use "EURO 6" if available
                if ((Helper.vClass == Constants.strPKW | Helper.vClass == Constants.strLNF) & EUclass.Length > 6)
                {
                    string EUclassShort = EUclass.Substring(0, 6);

                    if (!TNOxdata.Vehicle[Helper.vClass].EUClass.ContainsKey(EUclass) &
                        TNOxdata.Vehicle[Helper.vClass].EUClass.ContainsKey(EUclassShort))
                        EUclass = EUclassShort;
                }


                if (TNOxdata.Vehicle[Helper.vClass].EUClass.ContainsKey(EUclass))
                {
                    //Check/set temperature borders, because calculation is a straight function
                    if (AmbTemp < TNOxdata.Vehicle[Helper.vClass].EUClass[EUclass].TB[0])
                        TNOxFactor = TNOxdata.Vehicle[Helper.vClass].EUClass[EUclass].m + TNOxdata.Vehicle[Helper.vClass].EUClass[EUclass].c * TNOxdata.Vehicle[Helper.vClass].EUClass[EUclass].TB[0];
                    else if (AmbTemp > TNOxdata.Vehicle[Helper.vClass].EUClass[EUclass].TB[1])
                        TNOxFactor = 1;
                    else
                        TNOxFactor = TNOxdata.Vehicle[Helper.vClass].EUClass[EUclass].m + TNOxdata.Vehicle[Helper.vClass].EUClass[EUclass].c * AmbTemp;
                }
            }

            //Return value
            return true;
        }
        #endregion

        #region Interpolate
        private double Interpolate(double px, double p1, double p2, double e1, double e2)
        {
            if (p2 == p1)
                return e1;

            return e1 + (px - p1) / (p2 - p1) * (e2 - e1);
        }
        #endregion

        #region Classes
        #region Detoriation
        //Detoriation correction
        private class DET
        {
            public string Type { get; set; }
            public string Model { get; set; }
            public Dictionary<string, VehicledataDET> Vehicle { get; set; }

            public DET()
            {
                Vehicle = new Dictionary<string, VehicledataDET>();
            }
        }

        public class VehicledataDET
        {
            public Dictionary<string, PropClassDET> PropulsionClass { get; set; }

            public VehicledataDET()
            {
                PropulsionClass = new Dictionary<string, PropClassDET>();
            }
        }

        public class PropClassDET
        {
            public Dictionary<string, EmissionClassDET> Emission { get; set; }

            public PropClassDET()
            {
                Emission = new Dictionary<string, EmissionClassDET>();
            }
        }

        public class EmissionClassDET
        {
            public List<double> Mileage { get; set; }
            public Dictionary<string, List<double>> EUClass { get; set; }

            public EmissionClassDET()
            {
                Mileage = new List<double>();
                EUClass = new Dictionary<string, List<double>>();
            }
        }
        #endregion

        #region Vehicle Mileage
        //Mileage correction for Detoriation
        private class VMA
        {
            public string Type { get; set; }
            public string Model { get; set; }
            public Dictionary<string, VehicledataVMA> Vehicle { get; set; }

            public VMA()
            {
                Vehicle = new Dictionary<string, VehicledataVMA>();
            }
        }

        public class VehicledataVMA
        {
            public Dictionary<string, PropClassVMA> PropulsionClass { get; set; }

            public VehicledataVMA()
            {
                PropulsionClass = new Dictionary<string, PropClassVMA>();
            }
        }

        public class PropClassVMA
        {
            public Dictionary<string, EmissionClassVMA> SizeClass { get; set; }

            public PropClassVMA()
            {
                SizeClass = new Dictionary<string, EmissionClassVMA>();
            }
        }

        public class EmissionClassVMA
        {
            public Dictionary<string, List<double>> EUClass { get; set; }

            public EmissionClassVMA()
            {
                EUClass = new Dictionary<string, List<double>>();
            }
        }
        #endregion

        #region TNOx
        //TNOx correction
        private class TNOx
        {
            public string Type { get; set; }
            public string Model { get; set; }
            public Dictionary<string, Vehicledata> Vehicle { get; set; }

            public TNOx()
            {
                Vehicle = new Dictionary<string, Vehicledata>();
            }
        }

        public class Vehicledata
        {
            public Dictionary<string, EmissionClass> EUClass { get; set; }

            public Vehicledata()
            {
                EUClass = new Dictionary<string, EmissionClass>();
            }
        }

        public class EmissionClass
        {
            public double m { get; set; }
            public double c { get; set; }
            public List<double> TB { get; set; }

            public EmissionClass()
            {
                TB = new List<double>();
            }
        }
        #endregion
        #endregion
    }
}
