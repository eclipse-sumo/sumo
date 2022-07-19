#define FLEET
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PHEMlightdll
{
    public class CEP
    {
        #region Constructor
        public CEP(VEHPHEMLightJSON.VEH Vehicle,
                   List<string> headerLineFCvalues,
                   List<List<double>> matrixFCvalues,
                   List<string> headerLinePollutants,
                   List<List<double>> matrixPollutants,
                   List<double> idlingFCvalues,
                   List<double> idlingPollutants)
        {
            _resistanceF0 = Vehicle.RollingResData.Fr0.Value;
            _resistanceF1 = Vehicle.RollingResData.Fr1.Value;
            _resistanceF2 = Vehicle.RollingResData.Fr2.Value;
            _resistanceF3 = Vehicle.RollingResData.Fr3.Value;
            _resistanceF4 = Vehicle.RollingResData.Fr4.Value;
            _cWValue = Vehicle.VehicleData.Cw.Value;
            _crossSectionalArea = Vehicle.VehicleData.A.Value;
            _massVehicle = Vehicle.VehicleData.Mass.Value;
            _vehicleLoading = Vehicle.VehicleData.Loading.Value;
            _vehicleMassRot = Vehicle.VehicleData.RedMassWheel.Value;
            CalcType = Vehicle.VehicleData.CalcType;
            switch (CalcType)
            {
                case "Conv":
                    RatedPower = Vehicle.EngineData.ICEData.Prated.Value;
                    _engineRatedSpeed = Vehicle.EngineData.ICEData.nrated.Value;
                    _engineIdlingSpeed = Vehicle.EngineData.ICEData.Idling.Value;
                    break;
                case "HEV":
                    // Power von beiden zusammen Rest bezogen auf ICE
                    RatedPower = Vehicle.EngineData.ICEData.Prated.Value + Vehicle.EngineData.EMData.Prated.Value;
                    _engineRatedSpeed = Vehicle.EngineData.ICEData.nrated.Value;
                    _engineIdlingSpeed = Vehicle.EngineData.ICEData.Idling.Value;
                    break;
                case "BEV":
                    RatedPower = Vehicle.EngineData.EMData.Prated.Value;
                    _engineRatedSpeed = Vehicle.EngineData.EMData.nrated.Value;
                    _engineIdlingSpeed = 0;
                    break;
            }
            
            _effectiveWheelDiameter = Vehicle.VehicleData.WheelDiameter.Value;
            HeavyVehicle = Vehicle.VehicleData.MassType == Constants.HeavyVehicle;
            FuelType = Vehicle.VehicleData.FuelType;
            _axleRatio = Vehicle.TransmissionData.AxelRatio.Value;
            _auxPower = Vehicle.AuxiliariesData.Pauxnorm.Value;

            _pNormV0 = Vehicle.FLDData.P_n_max_v0.Value / 3.6;
            _pNormP0 = Vehicle.FLDData.P_n_max_p0.Value;
            _pNormV1 = Vehicle.FLDData.P_n_max_v1.Value / 3.6;
            _pNormP1 = Vehicle.FLDData.P_n_max_p1.Value;

            // Init pollutant identifiers, unit and measures
            List<string> FCvaluesIdentifier = new List<string>();
            List<List<double>> FCvaluesMeasures = new List<List<double>>();
            List<List<double>> normalizedFCvaluesMeasures = new List<List<double>>();
            for (int i = 0; i < headerLineFCvalues.Count; i++)
            {
                FCvaluesIdentifier.Add(headerLineFCvalues[i]);
                FCvaluesMeasures.Add(new List<double>());
                normalizedFCvaluesMeasures.Add(new List<double>());
            }

            // Init pollutant identifiers, unit and measures
            List<string> pollutantIdentifier = new List<string>();
            List<List<double>> pollutantMeasures = new List<List<double>>();
            List<List<double>> normalizedPollutantMeasures = new List<List<double>>();
            for (int i = 0; i < headerLinePollutants.Count; i++)
            {
                pollutantIdentifier.Add(headerLinePollutants[i]);
                pollutantMeasures.Add(new List<double>());
                normalizedPollutantMeasures.Add(new List<double>());
            }

            // Assigning values for speed rotational table
            _speedPatternRotational = new List<double>();
            for (int i = 0; i < Vehicle.TransmissionData.Transm["Speed"].Count; i++)
                _speedPatternRotational.Add(Vehicle.TransmissionData.Transm["Speed"][i] / 3.6);

            _gearTransmissionCurve = Vehicle.TransmissionData.Transm["GearRatio"];
            _speedCurveRotational = Vehicle.TransmissionData.Transm["RotMassF"];

            // Assigning values for drag table
            _nNormTable = Vehicle.FLDData.DragCurve["n_norm"];
            _dragNormTable = Vehicle.FLDData.DragCurve["pe_drag_norm"];

            // Looping through matrix and assigning values for FC values
            _powerPatternFCvalues = new List<double>();
            _normalizedPowerPatternFCvalues = new List<double>();

            int headerFCCount = headerLineFCvalues.Count;
            for (int i = 0; i < matrixFCvalues.Count; i++)
            {
                for (int j = 0; j < matrixFCvalues[i].Count; j++)
                {
                    if (matrixFCvalues[i].Count != headerFCCount + 1)
                        return;

                    if (j == 0)
                    {
                        _normalizedPowerPatternFCvalues.Add(matrixFCvalues[i][j]);
                        _powerPatternFCvalues.Add(matrixFCvalues[i][j] * RatedPower);
                    }
                    else
                    {
                        FCvaluesMeasures[j - 1].Add(matrixFCvalues[i][j] * RatedPower);
                        normalizedFCvaluesMeasures[j - 1].Add(matrixFCvalues[i][j]);
                    }
                }
            }

            _cepCurveFCvalues = new Dictionary<string, List<double>>();
            _idlingValueFCvalues = new Dictionary<string, double>();
            _normedCepCurveFCvalues = new Dictionary<string, List<double>>();

            for (int i = 0; i < headerLineFCvalues.Count; i++)
            {
                _cepCurveFCvalues.Add(FCvaluesIdentifier[i], FCvaluesMeasures[i]);
                _normedCepCurveFCvalues.Add(FCvaluesIdentifier[i], normalizedFCvaluesMeasures[i]);
                _idlingValueFCvalues.Add(FCvaluesIdentifier[i], idlingFCvalues[i] * RatedPower);
            }

            // looping through matrix and assigning values for pollutants
            double pollutantMultiplyer = 1;
            DrivingPower = NormalizingPower = CalcPower(Constants.NORMALIZING_SPEED, Constants.NORMALIZING_ACCELARATION, 0, (CalcType == "HEV" | CalcType == "BEV"));
            if (HeavyVehicle)
            {
                NormalizingPower = RatedPower;
                NormalizingType = eNormalizingType.RatedPower;
                pollutantMultiplyer = RatedPower;
            }
            else
            {
                NormalizingPower = DrivingPower;
                NormalizingType = eNormalizingType.DrivingPower;
            }

            _powerPatternPollutants = new List<double>();
            _normailzedPowerPatternPollutants = new List<double>();
            _cepNormalizedCurvePollutants = new Dictionary<string, List<double>>();

            int headerCount = headerLinePollutants.Count;
            for (int i = 0; i < matrixPollutants.Count; i++)
            {
                for (int j = 0; j < matrixPollutants[i].Count; j++)
                {
                    if (matrixPollutants[i].Count != headerCount + 1)
                        return;

                    if (j == 0)
                    {
                        _normailzedPowerPatternPollutants.Add(matrixPollutants[i][j]);
                        _powerPatternPollutants.Add(matrixPollutants[i][j] * NormalizingPower);
                    }
                    else
                    {
                        pollutantMeasures[j - 1].Add(matrixPollutants[i][j] * pollutantMultiplyer);
                        normalizedPollutantMeasures[j - 1].Add(matrixPollutants[i][j]);
                    }
                }
            }

            _cepCurvePollutants = new Dictionary<string, List<double>>();
            _idlingValuesPollutants = new Dictionary<string, double>();

            for (int i = 0; i < headerLinePollutants.Count; i++)
            {
                _cepCurvePollutants.Add(pollutantIdentifier[i], pollutantMeasures[i]);
                _cepNormalizedCurvePollutants.Add(pollutantIdentifier[i], normalizedPollutantMeasures[i]);
                _idlingValuesPollutants.Add(pollutantIdentifier[i], idlingPollutants[i] * pollutantMultiplyer);
            }

            _FleetMix = new Dictionary<string, double>();
            _FleetMix.Add(Constants.strGasoline, 0);
            _FleetMix.Add(Constants.strDiesel, 0);
            _FleetMix.Add(Constants.strCNG, 0);
            _FleetMix.Add(Constants.strLPG, 0);
        }
        #endregion

        #if FLEET
        #region ConstrutorForFleetmix
        private CEP(bool heavyVehicle,
                    double vehicleMass,
                    double vehicleLoading,
                    double vehicleMassRot,
                    double crossArea,
                    double cWValue,
                    double f0,
                    double f1,
                    double f2,
                    double f3,
                    double f4,
                    double axleRatio,
                    double auxPower,
                    double ratedPower,
                    double engineIdlingSpeed,
                    double engineRatedSpeed,
                    double effictiveWheelDiameter,
                    double pNormV0,
                    double pNormP0,
                    double pNormV1,
                    double pNormP1)
        {
            _resistanceF0 = f0;
            _resistanceF1 = f1;
            _resistanceF2 = f2;
            _resistanceF3 = f3;
            _resistanceF4 = f4;
            _cWValue = cWValue;
            _crossSectionalArea = crossArea;
            _massVehicle = vehicleMass;
            _vehicleLoading = vehicleLoading;
            _vehicleMassRot = vehicleMassRot;
            RatedPower = ratedPower;
            _engineIdlingSpeed = engineIdlingSpeed;
            _engineRatedSpeed = engineRatedSpeed;
            _effectiveWheelDiameter = effictiveWheelDiameter;

            _axleRatio = axleRatio;
            _auxPower = auxPower;

            _pNormV0 = pNormV0;
            _pNormP0 = pNormP0;
            _pNormV1 = pNormV1;
            _pNormP1 = pNormP1;

            HeavyVehicle = heavyVehicle;
        }
        #endregion
        #endif

        public bool HeavyVehicle { get; }
        public string FuelType { get; private set; }
        public string CalcType { get; private set; }

        #region NormalizingType
        public enum eNormalizingType
        {
            RatedPower,
            DrivingPower
        }

        public eNormalizingType NormalizingType { get; private set; }
        #endregion

        public double RatedPower { get; set; }
        public double NormalizingPower { get; private set; }
        public double DrivingPower { get; set; }

        #region Private Members
        protected double _massVehicle;
        protected double _vehicleLoading;
        protected double _vehicleMassRot;
        protected double _crossSectionalArea;
        protected double _cWValue;
        protected double _resistanceF0;
        protected double _resistanceF1;
        protected double _resistanceF2;
        protected double _resistanceF3;
        protected double _resistanceF4;
        protected double _axleRatio;
        protected double _auxPower;
        protected double _pNormV0;
        protected double _pNormP0;
        protected double _pNormV1;
        protected double _pNormP1;

        protected double _engineRatedSpeed;
        protected double _engineIdlingSpeed;
        protected double _effectiveWheelDiameter;

        protected List<double> _speedPatternRotational;
        protected List<double> _powerPatternFCvalues;
        protected List<double> _normalizedPowerPatternFCvalues;
        protected List<double> _normailzedPowerPatternPollutants;
        protected List<double> _powerPatternPollutants;

        protected Dictionary<string, List<double>> _cepCurveFCvalues;
        protected Dictionary<string, List<double>> _normedCepCurveFCvalues;
        protected List<double> _gearTransmissionCurve;
        protected List<double> _speedCurveRotational;
        protected Dictionary<string, List<double>> _cepCurvePollutants;
        protected Dictionary<string, List<double>> _cepNormalizedCurvePollutants;
        protected Dictionary<string, double> _FleetMix;
        protected Dictionary<string, double> _idlingValueFCvalues;
        protected Dictionary<string, double> _idlingValuesPollutants;

        protected List<double> _nNormTable;
        protected List<double> _dragNormTable;
        #endregion

        #region CalcPower
        public double CalcPower(double speed, double acc, double gradient, bool HBEV)
        {
            //Declaration
            double power = 0;
            double rotFactor = GetRotationalCoeffecient(speed);
            double powerAux = (_auxPower * RatedPower);

            //Calculate the power
            power += (_massVehicle + _vehicleLoading) * Constants.GRAVITY_CONST * (_resistanceF0 + _resistanceF1 * speed + _resistanceF4 * Math.Pow(speed, 4)) * speed;
            power += (_crossSectionalArea * _cWValue * Constants.AIR_DENSITY_CONST / 2) * Math.Pow(speed, 3);
            power += (_massVehicle * rotFactor + _vehicleMassRot + _vehicleLoading) * acc * speed;
            power += (_massVehicle + _vehicleLoading) * Constants.GRAVITY_CONST * gradient * 0.01 * speed;
            power /= 1000;
            power /= Constants._DRIVE_TRAIN_EFFICIENCY;
            if (!HBEV)
                power += powerAux;

            //Return result
            return power;
        }

        public double CalcWheelPower(double speed, double acc, double gradient)
        {
            //Declaration
            double power = 0;
            double rotFactor = GetRotationalCoeffecient(speed);

            //Calculate the power
            power += (_massVehicle + _vehicleLoading) * Constants.GRAVITY_CONST * (_resistanceF0 + _resistanceF1 * speed + _resistanceF4 * Math.Pow(speed, 4)) * speed;
            power += (_crossSectionalArea * _cWValue * Constants.AIR_DENSITY_CONST / 2) * Math.Pow(speed, 3);
            power += (_massVehicle * rotFactor + _vehicleMassRot + _vehicleLoading) * acc * speed;
            power += (_massVehicle + _vehicleLoading) * Constants.GRAVITY_CONST * gradient * 0.01 * speed;
            power /= 1000;

            //Return result
            return power;
        }
        #endregion

        #region CalcEngPower
        public double CalcEngPower(double power)
        {
            if (power < _powerPatternFCvalues.First()) return _powerPatternFCvalues.First();
            if (power > _powerPatternFCvalues.Last()) return _powerPatternFCvalues.Last();

            return power;
        }
        #endregion

        #region GetEmission
        public Dictionary<string, double> GetAllEmission(double power, double speed, Helpers VehicleClass, bool SetZero = false)
        {
            //Declaration
            Dictionary<string, double> Emi = new Dictionary<string, double>();

            if (!SetZero)
            {
                //FC values
                foreach (string id in _cepCurveFCvalues.Keys)
                {
                    if (id.ToUpper() == "FC_EL" & VehicleClass.pClass == Constants.strBEV)
                        Emi.Add(id.ToUpper(), GetEmission(id, power, speed, VehicleClass) + _auxPower * RatedPower);
                    else
                        Emi.Add(id.ToUpper(), GetEmission(id, power, speed, VehicleClass));
                }

                //Emission
                foreach (string id in _cepCurvePollutants.Keys)
                {
                    if (VehicleClass.pClass != Constants.strBEV)
                    {
                        Emi.Add(id.ToUpper(), GetEmission(id, power, speed, VehicleClass));
                    }
                    else
                    {
                        //If BEV set all emissions to 0
                        Emi.Add(id.ToUpper(), 0);
                    }
                }

                if (!Emi.ContainsKey("CO2"))
                {
                    if (Emi.ContainsKey("FC") & Emi.ContainsKey("CO") & Emi.ContainsKey("HC") & VehicleClass.pClass != Constants.strBEV)
                        Emi.Add("CO2", GetCO2Emission(Emi["FC"], Emi["CO"], Emi["HC"], VehicleClass));
                    else
                        Emi.Add("CO2", 0);
                }
            }
            else
            {
                foreach (string id in _cepCurveFCvalues.Keys)
                {
                    if (VehicleClass.pClass == "Mix" & id.ToUpper() == "FC_EL")
                        Emi.Add(id, GetEmission(id, power, speed, VehicleClass));
                    else
                        Emi.Add(id.ToUpper(), 0);
                }

                foreach (string id in _cepCurvePollutants.Keys)
                {
                    Emi.Add(id.ToUpper(), 0);
                }
                if (!Emi.ContainsKey("CO2")) Emi.Add("CO2", 0);
            }

            //Return value
            return Emi;
        }

        private double GetEmission(string pollutant, double power, double speed, Helpers VehicleClass)
        {
            //Declaration
            List<double> emissionCurve = new List<double>();
            List<double> powerPattern = new List<double>();

            // bisection search to find correct position in power pattern	
            int upperIndex;
            int lowerIndex;

            if (Math.Abs(speed) <= Constants.ZERO_SPEED_ACCURACY)
            {
                if (!_cepCurvePollutants.ContainsKey(pollutant) & !_cepCurveFCvalues.ContainsKey(pollutant))
                {
                    VehicleClass.ErrMsg = "Emission pollutant or fuel value " + pollutant + " not found!";
                    return 0;
                }

                if (_cepCurveFCvalues.ContainsKey(pollutant))
                    return _idlingValueFCvalues[pollutant];
                else if (_cepCurvePollutants.ContainsKey(pollutant))
                    return _idlingValuesPollutants[pollutant];
            }

            if (!_cepCurvePollutants.ContainsKey(pollutant) & !_cepCurveFCvalues.ContainsKey(pollutant))
            {
                VehicleClass.ErrMsg = "Emission pollutant or fuel value " + pollutant + " not found!";
                return 0;
            }

            if (_cepCurveFCvalues.ContainsKey(pollutant))
            {
                emissionCurve = _cepCurveFCvalues[pollutant];
                powerPattern = _powerPatternFCvalues;
            }
            else if (_cepCurvePollutants.ContainsKey(pollutant))
            {
                emissionCurve = _cepCurvePollutants[pollutant];
                powerPattern = _powerPatternPollutants;
            }

            if (emissionCurve.Count == 0)
            {
                VehicleClass.ErrMsg = "Empty emission curve for " + pollutant + " found!";
                return 0;
            }
            if (emissionCurve.Count == 1)
            {
                return emissionCurve[0];
            }

            // in case that the demanded power is smaller than the first entry (smallest) in the power pattern the first is returned (should never happen)
            if (power <= powerPattern.First())
            {
                return emissionCurve[0];
            }

            // if power bigger than all entries in power pattern return the last (should never happen)
            if (power >= powerPattern.Last())
            {
                return emissionCurve.Last();
            }

            FindLowerUpperInPattern(out lowerIndex, out upperIndex, powerPattern, power);
            return Interpolate(power, powerPattern[lowerIndex], powerPattern[upperIndex], emissionCurve[lowerIndex], emissionCurve[upperIndex]);
        }
        #endregion

#if FLEET
        #region GetNormedEmission
        private double GetNormedEmission(string pollutant, double power, double speed, Helpers VehicleClass)
        {
            //Declaration
            List<double> emissionCurve = new List<double>();
            List<double> powerPattern = new List<double>();

            // bisection search to find correct position in power pattern	
            int upperIndex;
            int lowerIndex;

            if (!_cepCurvePollutants.ContainsKey(pollutant) & !_cepCurveFCvalues.ContainsKey(pollutant))
            {
                VehicleClass.ErrMsg = "Emission pollutant or fuel value " + pollutant + " not found!";
                return 0;
            }

            if (_cepCurveFCvalues.ContainsKey(pollutant))
            {
                emissionCurve = _normedCepCurveFCvalues[pollutant];
                powerPattern = _normalizedPowerPatternFCvalues;
            }
            else if (_cepCurvePollutants.ContainsKey(pollutant))
            {
                emissionCurve = _cepNormalizedCurvePollutants[pollutant];
                powerPattern = _normailzedPowerPatternPollutants;
            }

            if (emissionCurve.Count == 0)
            {
                VehicleClass.ErrMsg = "Empty emission curve for " + pollutant + " found!";
                return 0;
            }
            if (emissionCurve.Count == 1)
            {
                return emissionCurve[0];
            }
            // in case that the demanded power is smaller than the first entry (smallest) in the power pattern the first is returned (should never happen)
            if (power <= powerPattern.First())
            {
                return emissionCurve[0];
            }

            // if power bigger than all entries in power pattern the last is returned (should never happen)
            if (power >= powerPattern.Last())
            {
                return emissionCurve.Last();
            }

            FindLowerUpperInPattern(out lowerIndex, out upperIndex, powerPattern, power);
            return Interpolate(power, powerPattern[lowerIndex], powerPattern[upperIndex], emissionCurve[lowerIndex], emissionCurve[upperIndex]);
        }
        #endregion
        #endif

        #region GetCO2Emission
        public double GetCO2Emission(double _FC, double _CO, double _HC, Helpers VehicleClass)
        {
            //Declaration
            double fCBr, fCHC, fCCO, fCCO2;
            
            fCBr = 0;
            fCHC = 0;
            fCCO = 0;
            fCCO2 = 0;

            if (FuelType != "Mix")
            {
                if (!GetfcVals(FuelType, ref fCBr, ref fCHC, ref fCCO, ref fCCO2, VehicleClass))
                    return 0;
            }
            else
            {
                if (!CalcfCValMix(ref fCBr, ref fCHC, ref fCCO, ref fCCO2, VehicleClass))
                    return 0;
            }

            return (_FC * fCBr - _CO * fCCO - _HC * fCHC) / fCCO2;
        }

        //Calculate the weighted fuel factor values for Fleetmix
        private bool CalcfCValMix(ref double _fCBr, ref double _fCHC, ref double _fCCO, ref double _fCCO2, Helpers VehicleClass)
        {
            //Declaration
            double Sum = 0;
            double sumfCBr, sumfCHC, sumfCCO, sumfCCO2;

            //Initialise
            sumfCBr = 0;
            sumfCHC = 0;
            sumfCCO = 0;
            sumfCCO2 = 0;

            //calculate the sum
            foreach (string id in _FleetMix.Keys)
            {
                Sum += _FleetMix[id];
            }

            //Calculate the weighted fuel factors
            if (Sum <= 0)
            {
                VehicleClass.ErrMsg = "All propolsion types in the fleetshares file are not known!";
                return false;
            }
            else
            {
                foreach (string id in _FleetMix.Keys)
                {
                    if (!GetfcVals(id, ref _fCBr, ref _fCHC, ref _fCCO, ref _fCCO2, VehicleClass))
                    {
                        return false;
                    }
                    else
                    {
                        sumfCBr += _fCBr * _FleetMix[id] / Sum;
                        sumfCHC += _fCHC * _FleetMix[id] / Sum;
                        sumfCCO += _fCCO * _FleetMix[id] / Sum;
                        sumfCCO2 += _fCCO2 * _FleetMix[id] / Sum;
                    }
                }
            }
            //Result values
            _fCBr = sumfCBr;
            _fCHC = sumfCHC;
            _fCCO = sumfCCO;
            _fCCO2 = sumfCCO2;
            return true;
        }

        // Get the fuel factor values
        private bool GetfcVals(string _fuelTypex, ref double _fCBr, ref double _fCHC, ref double _fCCO, ref double _fCCO2, Helpers VehicleClass)
        {
            _fCHC = 0.866;
            _fCCO = 0.429;
            _fCCO2 = 0.273;

            switch (_fuelTypex)
            {
                case Constants.strGasoline:
                    _fCBr = 0.865;
                    break;
                case Constants.strDiesel:
                    _fCBr = 0.863;
                    break;
                case Constants.strCNG:
                    _fCBr = 0.693;
                    _fCHC = 0.803;
                    break;
                case Constants.strLPG:
                    _fCBr = 0.825;
                    _fCHC = 0.825;
                    break;
                default:
                    VehicleClass.ErrMsg = "The propolsion type is not known! (" + FuelType + ")";
                    return false;
            }
            return true;
        }
        #endregion

        #region GetDecelCoast
        public double GetDecelCoast(double speed, double acc, double gradient)
        {
            //Declaration
            int upperIndex;
            int lowerIndex;

            if (speed < Constants.SPEED_DCEL_MIN)
            {
                return speed / Constants.SPEED_DCEL_MIN * GetDecelCoast(Constants.SPEED_DCEL_MIN, acc, gradient);
            }

            double rotCoeff = GetRotationalCoeffecient(speed);
            FindLowerUpperInPattern(out lowerIndex, out upperIndex, _speedPatternRotational, speed);
            double iGear = Interpolate(speed,
                                        _speedPatternRotational[lowerIndex],
                                        _speedPatternRotational[upperIndex],
                                        _gearTransmissionCurve[lowerIndex],
                                        _gearTransmissionCurve[upperIndex]);

            double iTot = iGear * _axleRatio;

            double n = (30 * speed * iTot) / ((_effectiveWheelDiameter / 2) * Math.PI);
            double nNorm = (n - _engineIdlingSpeed) / (_engineRatedSpeed - _engineIdlingSpeed);

            FindLowerUpperInPattern(out lowerIndex, out upperIndex, _nNormTable, nNorm);

            double fMot = 0;

            if (speed >= 10e-2)
            {
                fMot = (-Interpolate(nNorm,
                                          _nNormTable[lowerIndex],
                                          _nNormTable[upperIndex],
                                          _dragNormTable[lowerIndex],
                                          _dragNormTable[upperIndex]) * RatedPower * 1000 / speed) / Constants.DRIVE_TRAIN_EFFICIENCY;
            }

            double fRoll = (_resistanceF0
                + _resistanceF1 * speed
                + Math.Pow(_resistanceF2 * speed, 2)
                + Math.Pow(_resistanceF3 * speed, 3)
                + Math.Pow(_resistanceF4 * speed, 4)) * (_massVehicle + _vehicleLoading) * Constants.GRAVITY_CONST;

            double fAir = _cWValue * _crossSectionalArea * Constants.AIR_DENSITY_CONST * 0.5 * Math.Pow(speed, 2);

            double fGrad = (_massVehicle + _vehicleLoading) * Constants.GRAVITY_CONST * gradient / 100;

            return -(fMot + fRoll + fAir + fGrad) / ((_massVehicle + _vehicleLoading) * rotCoeff);
        }
        #endregion

        #region GetRotationalCoeffecient
        public double GetRotationalCoeffecient(double speed)
        {
            //Declaration
            int upperIndex;
            int lowerIndex;

            FindLowerUpperInPattern(out lowerIndex, out upperIndex, _speedPatternRotational, speed);
            return Interpolate(speed,
                                        _speedPatternRotational[lowerIndex],
                                        _speedPatternRotational[upperIndex],
                                        _speedCurveRotational[lowerIndex],
                                        _speedCurveRotational[upperIndex]);
        }
        #endregion

        #if FLEET
        #region GetGearCoeffecient
        public double GetGearCoeffecient(double speed)
        {
            //Declaration
            int upperIndex;
            int lowerIndex;

            FindLowerUpperInPattern(out lowerIndex, out upperIndex, _speedPatternRotational, speed);
            return Interpolate(speed,
                                        _speedPatternRotational[lowerIndex],
                                        _speedPatternRotational[upperIndex],
                                        _gearTransmissionCurve[lowerIndex],
                                        _gearTransmissionCurve[upperIndex]);
        }
        #endregion

        #region GetDragCoeffecient
        public double GetDragCoeffecient(double nNorm)
        {
            //Declaration
            int upperIndex;
            int lowerIndex;

            FindLowerUpperInPattern(out lowerIndex, out upperIndex, _nNormTable, nNorm);
            return Interpolate(nNorm,
                                        _nNormTable[lowerIndex],
                                        _nNormTable[upperIndex],
                                        _dragNormTable[lowerIndex],
                                        _dragNormTable[upperIndex]);
        }
        #endregion
        #endif

        #region FindLowerUpperInPattern
        private void FindLowerUpperInPattern(out int lowerIndex, out int upperIndex, List<double> pattern, double value)
        {
            lowerIndex = 0;
            upperIndex = 0;

            if (value <= pattern.First())
            {
                lowerIndex = 0;
                upperIndex = 0;
                return;
            }

            if (value >= pattern.Last())
            {
                lowerIndex = pattern.Count - 1;
                upperIndex = pattern.Count - 1;
                return;
            }

            // bisection search to find correct position in power pattern	
            int middleIndex = (pattern.Count - 1) / 2;
            upperIndex = pattern.Count - 1;
            lowerIndex = 0;

            while (upperIndex - lowerIndex > 1)
            {
                if (pattern[middleIndex] == value)
                {
                    lowerIndex = middleIndex;
                    upperIndex = middleIndex;
                    return;
                }
                else if (pattern[middleIndex] < value)
                {
                    lowerIndex = middleIndex;
                    middleIndex = (upperIndex - lowerIndex) / 2 + lowerIndex;
                }
                else
                {
                    upperIndex = middleIndex;
                    middleIndex = (upperIndex - lowerIndex) / 2 + lowerIndex;
                }
            }

            if (pattern[lowerIndex] <= value && value < pattern[upperIndex])
            {
                return;
            }
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

        #region GetMaxAccel
        public double GetMaxAccel(double speed, double gradient, bool HBEV)
        {
            double rotFactor = GetRotationalCoeffecient(speed);
            double pMaxForAcc = GetPMaxNorm(speed) * RatedPower - CalcPower(speed, 0, gradient, HBEV);

            return (pMaxForAcc * 1000) / ((_massVehicle * rotFactor + _vehicleMassRot + _vehicleLoading) * speed);
        }
        #endregion

        #region GetPMaxNorm
        private double GetPMaxNorm(double speed)
        {
            // Linear function between v0 and v1, constant elsewhere
            if (speed <= _pNormV0)
                return _pNormP0;
            else if (speed >= _pNormV1)
                return _pNormP1;
            else
            {
                return Interpolate(speed, _pNormV0, _pNormV1, _pNormP0, _pNormP1);
            }
        }
        #endregion

        //--------------------------------------------------------------------------------------------------
        // Operators for fleetmix
        //--------------------------------------------------------------------------------------------------

        #if FLEET
        #region AddRangeCeps
        public static CEP AddRangeCeps(CEP[] cps, Helpers Helper)
        {
            #region SingleValues
            CEP newCEP = new CEP(cps.Select(p => p.HeavyVehicle ? 1 : 0).Sum() > 0,
                cps.Select(p => p._massVehicle).Sum(),
                cps.Select(p => p._vehicleLoading).Sum(),
                cps.Select(p => p._vehicleMassRot).Sum(),
                cps.Select(p => p._crossSectionalArea).Sum(),
                cps.Select(p => p._cWValue).Sum(),
                cps.Select(p => p._resistanceF0).Sum(),
                cps.Select(p => p._resistanceF1).Sum(),
                cps.Select(p => p._resistanceF2).Sum(),
                cps.Select(p => p._resistanceF3).Sum(),
                cps.Select(p => p._resistanceF4).Sum(),
                cps.Select(p => p._axleRatio).Sum(),
                cps.Select(p => p._auxPower).Sum(),
                cps.Select(p => p.RatedPower).Sum(),
                cps.Select(p => p._engineIdlingSpeed).Sum(),
                cps.Select(p => p._engineRatedSpeed).Sum(),
                cps.Select(p => p._effectiveWheelDiameter).Sum(),
                cps.Select(p => p._pNormV0).Sum(),
                cps.Select(p => p._pNormP0).Sum(),
                cps.Select(p => p._pNormV1).Sum(),
                cps.Select(p => p._pNormP1).Sum());

            #region Fleetmix and Fueltype
            if (cps.Select(p => p.FuelType).Min() == cps.Select(p => p.FuelType).Max())
                newCEP.FuelType = cps.First().FuelType;
            else
                newCEP.FuelType = "Mix";

            newCEP._FleetMix = new Dictionary<string, double>();
            foreach (string id in cps.First()._FleetMix.Keys)
            {
                newCEP._FleetMix.Add(id, cps.Select(p => p._FleetMix[id]).Sum());
            }
            #endregion
            #endregion

            #region SpeedRotationalTable
            double minSpeedRotational = cps.Select(p => p._speedPatternRotational.First()).Min();
            double maxSpeedRotational = cps.Select(p => p._speedPatternRotational.Last()).Max();

            newCEP._speedPatternRotational
                = CreatePattern(minSpeedRotational,
                maxSpeedRotational,
                Constants.SPEED_ROTATIONAL_INCREMENT);

            newCEP._speedCurveRotational = new List<double>();
            newCEP._gearTransmissionCurve = new List<double>();

            for (int i = 0; i < newCEP._speedPatternRotational.Count; i++)
            {
                newCEP._speedCurveRotational.Add(cps.Select(p => p.GetRotationalCoeffecient(newCEP._speedPatternRotational[i])).Sum());

                newCEP._gearTransmissionCurve.Add(cps.Select(p => p.GetGearCoeffecient(newCEP._speedPatternRotational[i])).Sum());
            }
            #endregion

            #region NormalizingPower
            newCEP.DrivingPower = newCEP.CalcPower(Constants.NORMALIZING_SPEED, Constants.NORMALIZING_ACCELARATION, 0, (Helper.pClass == Constants.strBEV | Helper.uClass == Constants.strHybrid));

            if (newCEP.HeavyVehicle)
            {
                newCEP.NormalizingPower = newCEP.RatedPower;
                newCEP.NormalizingType = eNormalizingType.RatedPower;
            }
            else
            {
                newCEP.NormalizingPower = newCEP.DrivingPower;
                newCEP.NormalizingType = eNormalizingType.DrivingPower;
            }
            #endregion

            #region FC
            double minNormPowerPatternFC = cps.Select(p => p._normalizedPowerPatternFCvalues.First()).Min();
            double maxNormPowerPatternFC = cps.Select(p => p._normalizedPowerPatternFCvalues.Last()).Max();

            newCEP._normalizedPowerPatternFCvalues
                = CreatePattern(minNormPowerPatternFC,
                maxNormPowerPatternFC,
                Constants.POWER_FC_INCREMENT);

            newCEP._cepCurveFCvalues = new Dictionary<string, List<double>>();
            newCEP._normedCepCurveFCvalues = new Dictionary<string, List<double>>();
            newCEP._powerPatternFCvalues = new List<double>();

            foreach (string id in cps.First()._cepCurveFCvalues.Keys)
            {
                newCEP._cepCurveFCvalues.Add(id, new List<double>());
                newCEP._normedCepCurveFCvalues.Add(id, new List<double>());
            }

            for (int i = 0; i < newCEP._normalizedPowerPatternFCvalues.Count; i++)
            {
                foreach (string id in newCEP._cepCurveFCvalues.Keys)
                {
                    double newCepVal = cps.Select(p => p.GetNormedEmission(id, newCEP._normalizedPowerPatternFCvalues[i], double.MaxValue, Helper)).Sum();
                    newCEP._cepCurveFCvalues[id].Add(newCepVal * newCEP.RatedPower);
                    newCEP._normedCepCurveFCvalues[id].Add(newCepVal);
                }
                newCEP._powerPatternFCvalues.Add(newCEP._normalizedPowerPatternFCvalues[i] * newCEP.RatedPower);
            }
            #endregion

            #region Pollutants
            double minNormPowerPattern = cps.Select(p => p._normailzedPowerPatternPollutants.First()).Min();
            double maxNormPowerPattern = cps.Select(p => p._normailzedPowerPatternPollutants.Last()).Max();

            newCEP._normailzedPowerPatternPollutants
                 = CreatePattern(minNormPowerPattern,
                 maxNormPowerPattern,
                 Constants.POWER_POLLUTANT_INCREMENT);

            newCEP._cepCurvePollutants = new Dictionary<string, List<double>>();
            newCEP._cepNormalizedCurvePollutants = new Dictionary<string, List<double>>();
            newCEP._powerPatternPollutants = new List<double>();

            foreach (string id in cps.First()._cepCurvePollutants.Keys)
            {
                newCEP._cepCurvePollutants.Add(id, new List<double>());
                newCEP._cepNormalizedCurvePollutants.Add(id, new List<double>());
            }

            for (int i = 0; i < newCEP._normailzedPowerPatternPollutants.Count; i++)
            {
                foreach (string id in newCEP._cepCurvePollutants.Keys)
                {
                    if (newCEP.NormalizingType == eNormalizingType.RatedPower)
                    {
                        double newCepVal = cps.Select(p => p.GetNormedEmission(id, newCEP._normailzedPowerPatternPollutants[i], double.MaxValue, Helper)).Sum();
                        newCEP._cepCurvePollutants[id].Add(newCepVal * newCEP.RatedPower);
                        newCEP._cepNormalizedCurvePollutants[id].Add(newCepVal);
                    }
                    else
                    {
                        newCEP._cepCurvePollutants[id].Add(cps.Select(p => p.GetEmission(id, newCEP._normailzedPowerPatternPollutants[i] * p.NormalizingPower, double.MaxValue, Helper)).Sum());
                        newCEP._cepNormalizedCurvePollutants[id].Add(cps.Select(p => p.GetNormedEmission(id, newCEP._normailzedPowerPatternPollutants[i], double.MaxValue, Helper)).Sum());
                    }
                }
                newCEP._powerPatternPollutants.Add(newCEP._normailzedPowerPatternPollutants[i] * newCEP.NormalizingPower);
            }
            #endregion

            #region IdlingValues
            newCEP._idlingValueFCvalues = new Dictionary<string, double>();
            newCEP._idlingValuesPollutants = new Dictionary<string, double>();

            foreach (string id in cps.First()._idlingValueFCvalues.Keys)
            {
                newCEP._idlingValueFCvalues.Add(id, cps.Select(p => p._idlingValueFCvalues[id]).Sum());
            }

            foreach (string id in cps.First()._idlingValuesPollutants.Keys)
            {
                newCEP._idlingValuesPollutants.Add(id, cps.Select(p => p._idlingValuesPollutants[id]).Sum());
            }
            #endregion

            #region TragTable
            double minTragTable = cps.Select(p => p._nNormTable.First()).Min();
            double maxTragTable = cps.Select(p => p._nNormTable.Last()).Max();

            newCEP._nNormTable
                = CreatePattern(minTragTable,
                maxTragTable,
                Constants.NNORM_INCREMENT);

            newCEP._dragNormTable = new List<double>();

            for (int i = 0; i < newCEP._nNormTable.Count; i++)
            {
                newCEP._dragNormTable.Add(cps.Select(p => p.GetDragCoeffecient(newCEP._nNormTable[i])).Sum());
            }
            #endregion
            return newCEP;
        }
        #endregion

        #region Operator *
        public static CEP operator *(CEP cp1, double d)
        {
            #region SingleValues
            CEP newCEP = new CEP(cp1.HeavyVehicle,
                d * cp1._massVehicle,
                d * cp1._vehicleLoading,
                d * cp1._vehicleMassRot,
                d * cp1._crossSectionalArea,
                d * cp1._cWValue,
                d * cp1._resistanceF0,
                d * cp1._resistanceF1,
                d * cp1._resistanceF2,
                d * cp1._resistanceF3,
                d * cp1._resistanceF4,
                d * cp1._axleRatio,
                d * cp1._auxPower,
                d * cp1.RatedPower,
                d * cp1._engineIdlingSpeed,
                d * cp1._engineRatedSpeed,
                d * cp1._effectiveWheelDiameter,
                d * cp1._pNormV0,
                d * cp1._pNormP0,
                d * cp1._pNormV1,
                d * cp1._pNormP1);

            #region Fleetmix and Fueltype
            newCEP.FuelType = cp1.FuelType;
            newCEP._FleetMix = new Dictionary<string, double>();
            foreach (string id in cp1._FleetMix.Keys)
            {
                if (newCEP.FuelType == id)
                    newCEP._FleetMix.Add(newCEP.FuelType, d);
                else
                    newCEP._FleetMix.Add(id, 0);
            }
            #endregion
            #endregion

            #region SpeedRotationalTable
            newCEP._speedPatternRotational = new List<double>(cp1._speedPatternRotational);
            newCEP._speedCurveRotational = new List<double>(cp1._speedCurveRotational.Select(p => p * d));
            newCEP._gearTransmissionCurve = new List<double>(cp1._gearTransmissionCurve.Select(p => p * d));
            #endregion

            #region NormalizingPower
            newCEP.DrivingPower = newCEP.CalcPower(Constants.NORMALIZING_SPEED, Constants.NORMALIZING_ACCELARATION, 0, (cp1.CalcType == "HEV" | cp1.CalcType == "BEV"));

            if (newCEP.HeavyVehicle)
            {
                newCEP.NormalizingPower = newCEP.RatedPower;
                newCEP.NormalizingType = eNormalizingType.RatedPower;
            }
            else
            {
                newCEP.NormalizingPower = newCEP.DrivingPower;
                newCEP.NormalizingType = eNormalizingType.DrivingPower;
            }
            #endregion

            #region FC
            newCEP._powerPatternFCvalues = new List<double>(cp1._powerPatternFCvalues.Select(p => p * d));
            newCEP._normalizedPowerPatternFCvalues = new List<double>(cp1._normalizedPowerPatternFCvalues);
            newCEP._cepCurveFCvalues = new Dictionary<string, List<double>>();
            newCEP._normedCepCurveFCvalues = new Dictionary<string, List<double>>();

            foreach (string id in cp1._cepCurveFCvalues.Keys)
            {
                newCEP._cepCurveFCvalues.Add(id, new List<double>(cp1._cepCurveFCvalues[id].Select(p => p * d)));
                newCEP._normedCepCurveFCvalues.Add(id, new List<double>(cp1._normedCepCurveFCvalues[id].Select(p => p * d)));
            }
            #endregion

            #region Pollutants
            newCEP._powerPatternPollutants = new List<double>(cp1._normailzedPowerPatternPollutants.Select(p => p * newCEP.NormalizingPower));
            newCEP._normailzedPowerPatternPollutants = new List<double>(cp1._normailzedPowerPatternPollutants);
            newCEP._cepCurvePollutants = new Dictionary<string, List<double>>();
            newCEP._cepNormalizedCurvePollutants = new Dictionary<string, List<double>>();

            foreach (string id in cp1._cepCurvePollutants.Keys)
            {
                newCEP._cepCurvePollutants.Add(id, new List<double>(cp1._cepCurvePollutants[id].Select(p => p * d)));
                newCEP._cepNormalizedCurvePollutants.Add(id, new List<double>(cp1._cepNormalizedCurvePollutants[id].Select(p => p * d)));
            }
            #endregion

            #region IdlingValues
            newCEP._idlingValueFCvalues = new Dictionary<string, double>();
            newCEP._idlingValuesPollutants = new Dictionary<string, double>();

            foreach (string id in cp1._idlingValueFCvalues.Keys)
            {
                newCEP._idlingValueFCvalues.Add(id,
                    cp1._idlingValueFCvalues[id] * d);
            }

            foreach (string id in cp1._idlingValuesPollutants.Keys)
            {
                newCEP._idlingValuesPollutants.Add(id,
                    cp1._idlingValuesPollutants[id] * d);
            }
            #endregion

            #region DragTable
            newCEP._nNormTable = new List<double>(cp1._nNormTable);
            newCEP._dragNormTable = new List<double>(cp1._dragNormTable.Select(p => p * d));
            #endregion
            return newCEP;
        }
        #endregion

        #region CreatePattern
        static public List<double> CreatePattern(double min, double max, double increment)
        {
            //Declaration
            List<double> pattern = new List<double>();
            double actualMin = min;
            double actualMax = max;

            if (min < 0)
                actualMin = Math.Ceiling(min / increment) * increment;
            else
                actualMin = Math.Floor(min / increment) * increment;

            if (max < 0)
                actualMax = Math.Floor(max / increment) * increment;
            else
                actualMax = Math.Ceiling(max / increment) * increment;

            double curVal = actualMin;

            while (curVal <= actualMax)
            {
                pattern.Add(curVal);
                curVal += increment;
            }
            return pattern;
        }
        #endregion

        #region CheckClass
        public static bool CheckClass(CEP[] cps)
        {
            return cps.Select(p => p.HeavyVehicle ? 1 : 0).Min() == cps.Select(p => p.HeavyVehicle ? 1 : 0).Max();
        }
        #endregion
        #endif
    }
}
