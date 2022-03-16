using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PHEMlightdll
{
        public class VehicleResult
    {
        #region Construtor
        public VehicleResult(string vehicle,
                             string cycle,
                             double time,
                             double speed,
                             double grad,
                             double power,
                             double pPos,
                             double pNormRated,
                             double pNormDrive,
                             double acc,
                             Dictionary<string, double> Emi)
        {
            _vehicle = vehicle;
            Cycle = cycle;
            _time = time;
            _speed = speed;
            _grad = grad;
            _power = power;
            _pPos = pPos;
            PNormRated = pNormRated;
            PNormDrive = pNormDrive;
            _accelaration = acc;
            _emissionData = new EmissionData(Emi);
        }
        #endregion
        
        #region Vehicle
        private string _vehicle;
        public string Vehicle => _vehicle;
        #endregion

        #region Cycle
        public string Cycle { get; set; }
        #endregion

        #region Time
        private double _time;
        public double Time => _time;
        #endregion

        #region Speed
        private double _speed;
        public double Speed => _speed;
        #endregion

        #region Grad
        private double _grad;
        public double Grad => _grad;
        #endregion

        #region Power
        private double _power;
        public double Power => _power;
        #endregion

        #region PPos
        private double _pPos;
        public double PPos => _pPos;
        #endregion

        #region PNormRated
        public double PNormRated { get; set; }
        #endregion

        #region PNormDrive
        public double PNormDrive { get; set; }
        #endregion

        #region Accelaration
        private double _accelaration;
        public double Accelaration => _accelaration;
        #endregion

        #region EmissionData
        private EmissionData _emissionData;
        public EmissionData EmissionData => _emissionData;
        #endregion
    }

    public class EmissionData
    {
        #region Constructor
        public EmissionData(Dictionary<string, double> Emi)
        {
            _Emi = Emi;
        }
        #endregion

        #region Emi
        private Dictionary<string, double> _Emi = new Dictionary<string, double>();
        public Dictionary<string, double> Emi => _Emi;
        #endregion
    }

    public class cErgEntry
    {
        //Declaration
        public string Head;
        public string Unit;

        //Create a new cErgEntry class
        public cErgEntry(string HeadStr, string UnitStr)
        {
            Head = HeadStr;
            Unit = UnitStr;
        }
    }
}
