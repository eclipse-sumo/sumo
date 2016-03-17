#ifndef PHEMlightCRESULT
#define PHEMlightCRESULT

//C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace PHEMlightdll { class EmissionData; }


namespace PHEMlightdll {
    class VehicleResult {
    public:
        VehicleResult(double time, double speed, double grad, double power, double pPos, double pNormRated, double pNormDrive, double acc, double fc, double fcel, double cO2, double nOx, double hC, double pM, double cO);

        //--------------------------------------------------------------------------------------------------
        // GeneralState 
        //--------------------------------------------------------------------------------------------------

    private:
        double _time;
    public:
        const double&  getTime() const;

    private:
        double _speed;
    public:
        const double&  getSpeed() const;

    private:
        double _grad;
    public:
        const double&  getGrad() const;

    private:
        double _power;
    public:
        const double&  getPower() const;

    private:
        double _pPos;
    public:
        const double&  getPPos() const;

    private:
        double _pNormRated;
    public:
        const double&  getPNormRated() const;
        void setPNormRated(const double&  value);

    private:
        double _pNormDrive;
    public:
        const double&  getPNormDrive() const;
        void setPNormDrive(const double&  value);

    private:
        double _accelaration;
    public:
        const double&  getAccelaration() const;

        //--------------------------------------------------------------------------------------------------
        // EmissionData 
        //--------------------------------------------------------------------------------------------------

    private:
        PHEMlightdll::EmissionData* _emissionData;
    public:
        PHEMlightdll::EmissionData* getEmissionData() const;

    private:
        void InitializeInstanceFields();
    };

    class EmissionData {

    public:
        EmissionData(double fc, double fcel, double cO2, double nOx, double hC, double pM, double cO);

        //--------------------------------------------------------------------------------------------------
        // GeneralState 
        //--------------------------------------------------------------------------------------------------

    private:
        double _fc;
    public:
        const double&  getFC() const;

    private:
        double _fcel;
    public:
        const double&  getFCel() const;

    private:
        double _cO2;
    public:
        const double&  getCO2() const;

    private:
        double _nOx;
    public:
        const double&  getNOx() const;

    private:
        double _hC;
    public:
        const double&  getHC() const;

    private:
        double _pM;
    public:
        const double&  getPM() const;

    private:
        double _cO;
    public:
        const double&  getCO() const;

    private:
        void InitializeInstanceFields();
    };
}


#endif	//#ifndef PHEMlightCRESULT
