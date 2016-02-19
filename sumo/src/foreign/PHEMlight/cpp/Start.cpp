#include "Start.h"
#include "CEPHandler.h"
#include "CEP.h"
#include "Helpers.h"


namespace PHEMlightdll {

    bool Start::CALC_Array(const std::string& VEH, std::vector<double>& Time, std::vector<double>& Velocity, std::vector<double>& Gradient, std::vector<VehicleResult*>& VehicleResultsOrg, bool fleetMix = false, const std::string& PHEMDataV = "V4", const std::string& CommentPref = "c") {
        //Declaration
        int i;
        double acc;
        std::vector<VehicleResult*> _VehicleResult;

        //Borrow
        Helper->CommentPrefix = CommentPref;
        Helper->PHEMDataV = PHEMDataV;
        if ((VEH.rfind("\\")) >= 0) {
            _DataPath = VEH.substr(0, VEH.rfind("\\"));
        }
        else {
            _DataPath = Assembly::GetExecutingAssembly()->Location->substr(0, Assembly::GetExecutingAssembly()->Location->rfind("\\")) + std::string("\\Default Vehicles\\") + Helper->PHEMDataV;
        }

        //Read the vehicle and emission data
        if (fleetMix) {
            //Set the vehicle class
            Helper->gClass = std::string("AggClass_") + VEH;

            //Generate the class
            DataInput = new CEPHandler();

            //Read the FleetShares
            if (!DataInput->ReadFleetShares(_DataPath, Helper)) {
                VehicleResultsOrg.clear();
                return false;
            }
            //Read the vehicle and emission data
            if (!DataInput->GetFleetCEP(_DataPath, VEH, Helper, Data)) {
                VehicleResultsOrg.clear();
                return false;
            }
        }
        else {
            //Get vehicle string
            if (!Helper->setclass(VEH)) {
                VehicleResultsOrg.clear();
                return false;
            }

            //Generate the class
            DataInput = new CEPHandler();

            //Read the vehicle and emission data
            if (!DataInput->GetCEP(_DataPath, Helper, Data)) {
                VehicleResultsOrg.clear();
                return false;
            }
        }

        //Calculate emissions per second
        for (i = 1; i <= Time.size() - 1; i++) {
            //Calculate the acceleration
            acc = (Velocity[i] - Velocity[i - 1]) / (Time[i] - Time[i - 1]);

            //Calculate and save the data in the List
            _VehicleResult.push_back(PHEMLight::CreateVehicleStateData(Helper, DataInput->CEPS[Helper->gClass], Time[i - 1], Velocity[i - 1], acc, Gradient[i - 1]));
            if (Helper->ErrMsg != 0) {
                VehicleResultsOrg.clear();
                return false;
            }
        }
        VehicleResultsOrg = _VehicleResult;
        return true;
    }

    bool Start::CALC_Single(const std::string& VEH, double Time, double Velocity, double acc, double Gradient, std::vector<VehicleResult*>& VehicleResultsOrg, bool fleetMix = false, const std::string& PHEMDataV = "V4", const std::string& CommentPref = "c") {
        //Declaration
        std::vector<VehicleResult*> _VehicleResult;
        VehicleResultsOrg = _VehicleResult;

        //Borrow
        Helper->CommentPrefix = CommentPref;
        Helper->PHEMDataV = PHEMDataV;
        if ((VEH.rfind("\\")) >= 0) {
            _DataPath = VEH.substr(0, VEH.rfind("\\"));
        }
        else {
            _DataPath = Assembly::GetExecutingAssembly()->Location->substr(0, Assembly::GetExecutingAssembly()->Location->rfind("\\")) + std::string("\\Default Vehicles\\") + Helper->PHEMDataV;
        }

        //Read the vehicle and emission data
        if (fleetMix) {
            //Set the vehicle class
            Helper->gClass = std::string("AggClass_") + VEH;

            //Generate the class
            DataInput = new CEPHandler();

            //Read the FleetShares
            if (!DataInput->ReadFleetShares(_DataPath, Helper)) {
                VehicleResultsOrg.clear();
                return false;
            }
            //Read the vehicle and emission data
            if (!DataInput->GetFleetCEP(_DataPath, VEH, Helper, Data)) {
                VehicleResultsOrg.clear();
                return false;
            }
        }
        else {
            //Get vehicle string
            if (!Helper->setclass(VEH)) {
                VehicleResultsOrg.clear();
                return false;
            }

            //Generate the class
            DataInput = new CEPHandler();

            //Read the vehicle and emission data
            if (!DataInput->GetCEP(_DataPath, Helper, Data)) {
                VehicleResultsOrg.clear();
                return false;
            }
        }

        //Calculate and save the data in the List
        _VehicleResult.push_back(PHEMLight::CreateVehicleStateData(Helper, DataInput->CEPS[Helper->gClass], Time, Velocity, acc, Gradient));
        VehicleResultsOrg = _VehicleResult;
        return true;
    }

    bool Start::ExportData(const std::string& path, const std::string& vehicle, std::vector<VehicleResult*>& _VehicleResult) {
        if (path == "" || vehicle == "" || _VehicleResult.empty() || _VehicleResult.empty()) {
            return false;
        }

        //Write head
        StringBuilder* allLines = new StringBuilder();
        std::string lineEnding = "\r\n";

        allLines->AppendLine(std::string("Vehicletype: ,") + vehicle);
        allLines->AppendLine("Time, Speed, Gradient, Accelaration, Engine power raw, P_pos, P_norm_rated, P_norm_drive, FC, FC el., CO2, NOx, CO, HC, PM");
        allLines->AppendLine("[s], [m/s], [%], [m/s^2], [kW], [kW], [-], [-], [g/h], [kWh/h], [g/h], [g/h], [g/h], [g/h], [g/h]");

        //Write data
        for (std::vector<VehicleResult*>::const_iterator Result = _VehicleResult.begin(); Result != _VehicleResult.end(); ++Result) {
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->Time.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->Speed.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->Grad.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->Accelaration.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->Power.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->PPos.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->PNormRated.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->PNormDrive.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->EmissionData.FC.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->EmissionData.FCel.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->EmissionData.CO2.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->EmissionData.NOx.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->EmissionData.CO.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->EmissionData.HC.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->Append((*Result)->EmissionData.PM.ToString("0.0000", CultureInfo::InvariantCulture) + lineEnding);
        }

        // Write the string to a file.
        if (path.find(".", 0) < 0) {
            path = path + std::string(".sta");
        }
        try {
            StreamWriter* file = new StreamWriter(path);
            file->WriteLine(allLines);
            file->Close();
            return true;
        }
        catch (std::exception& ex) {
            return false;
        }
    }

    bool Start::ExportSumData(const std::string& path, const std::string& vehicle, const std::string& cycle, VehicleResult* _VehicleResult) {
        if (path == "" || vehicle == "" || cycle == "" || _VehicleResult == 0) {
            return false;
        }
        StringBuilder* allLines = new StringBuilder();

        if (path.find(".", 0) < 0) {
            path = path + std::string(".erg");
        }

        if (!File::Exists(path)) {
            //Write head
            allLines->AppendLine("PHEMLight Results");
            allLines->AppendLine("");
            allLines->AppendLine("Vehicle, Cycle, Time, Speed, Gradient, Accelaration, Engine power raw, P_pos, P_norm_rated, P_norm_drive, FC, FC el., CO2, NOx, CO, HC, PM");
            allLines->AppendLine("[-], [-], [s], [km/h], [%], [m/s^2], [kW], [kW], [-], [-], [g/km], [kWh/km], [g/km], [g/km], [g/km], [g/km], [g/km]");
        }

        //Write data
        allLines->Append(vehicle + std::string(","));
        allLines->Append(cycle + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->Time.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->Speed.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->Grad.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->Accelaration.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->Power.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->PPos.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->PNormRated.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->PNormDrive.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->EmissionData.FC.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->EmissionData.FCel.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->EmissionData.CO2.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->EmissionData.NOx.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->EmissionData.CO.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->EmissionData.HC.ToString("0.0000", CultureInfo::InvariantCulture) + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->Append(_VehicleResult->EmissionData.PM.ToString("0.0000", CultureInfo::InvariantCulture));

        // Write the string to a file.
        try {
            StreamWriter* file = new StreamWriter(path, true);
            file->WriteLine(allLines);
            file->Close();
            return true;
        }
        catch (std::exception& ex) {
            return false;
        }
    }

    VehicleResult* Start::GenerateSumData(std::vector<VehicleResult*>& _VehicleResult) {
        //Declaration
        double sum_time = 0;
        double sum_speed = 0;
        double sum_grad = 0;
        double sum_power = 0;
        double sum_pPos = 0;
        double sum_pNormRated = 0;
        double sum_pNormDrive = 0;
        double sum_acc = 0;
        double sum_fc = 0;
        double sum_fcel = 0;
        double sum_cO2 = 0;
        double sum_nOx = 0;
        double sum_hC = 0;
        double sum_pM = 0;
        double sum_cO = 0;

        if (_VehicleResult.empty() || _VehicleResult.empty()) {
            return new VehicleResult(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        }

        //Write data
        for (std::vector<VehicleResult*>::const_iterator Result = _VehicleResult.begin(); Result != _VehicleResult.end(); ++Result) {
            sum_speed += (*Result)->Speed * 3.6;
            sum_power += (*Result)->Power;
            if ((*Result)->PPos > 0) {
                sum_pPos += (*Result)->PPos;
            }
            sum_grad += (*Result)->Grad;
            sum_pNormRated += (*Result)->PNormRated;
            sum_pNormDrive += (*Result)->PNormDrive;
            sum_acc += (*Result)->Accelaration;
            sum_fcel += (*Result)->EmissionData.FCel;
            sum_fc += (*Result)->EmissionData.FC;
            sum_cO2 += (*Result)->EmissionData.CO2;
            sum_nOx += (*Result)->EmissionData.NOx;
            sum_hC += (*Result)->EmissionData.HC;
            sum_pM += (*Result)->EmissionData.PM;
            sum_cO += (*Result)->EmissionData.CO;
        }

        //Build average
        sum_time = _VehicleResult[_VehicleResult.size() - 1]->Time - _VehicleResult[0]->Time;
        sum_power /= _VehicleResult.size();
        sum_pPos /= _VehicleResult.size();
        sum_grad /= _VehicleResult.size();
        sum_pNormRated /= _VehicleResult.size();
        sum_pNormDrive /= _VehicleResult.size();
        sum_acc /= _VehicleResult.size();
        if (sum_speed > 0) {
            sum_fc /= sum_speed;
            sum_fcel /= sum_speed;
            sum_cO2 /= sum_speed;
            sum_nOx /= sum_speed;
            sum_hC /= sum_speed;
            sum_pM /= sum_speed;
            sum_cO /= sum_speed;
        }
        else {
            sum_fc = 0;
            sum_fcel = 0;
            sum_cO2 = 0;
            sum_nOx = 0;
            sum_hC = 0;
            sum_pM = 0;
            sum_cO = 0;
        }
        sum_speed /= _VehicleResult.size();

        return new VehicleResult(sum_time, sum_speed, sum_grad, sum_power, sum_pPos, sum_pNormRated, sum_pNormDrive, sum_acc, sum_fc, sum_fcel, sum_cO2, sum_nOx, sum_hC, sum_pM, sum_cO);
    }

    void Start::InitializeInstanceFields() {
        Helper = new Helpers();
    }

    VehicleResult* PHEMLight::CreateVehicleStateData(Helpers* Helper, CEP* currCep, double time, double inputSpeed, double inputAcc, double Gradient = 0) {
        //Declaration
        double speed = std::max(inputSpeed, 0);
        double acc;
        double P_pos;

        //Speed/Acceleration limitation
        if (speed == 0) {
            acc = 0;
        }
        else {
            acc = inputAcc;
        }

        //Calculate the power
        double power = currCep->CalcPower(speed, acc, Gradient);
        double P_eng = currCep->CalcEngPower(power);

        //Power limitation
        if (P_eng >= 0) {
            P_pos = power;
        }
        else {
            P_pos = 0;
        }

        //Calculate the result values (BEV)
        if (Helper->pClass == Constants->strBEV) {
            return new VehicleResult(time, speed, Gradient, power, P_pos, P_eng / currCep->RatedPower, P_eng / currCep->DrivingPower, acc, 0, currCep->GetEmission("FC", power, speed, Helper), 0, 0, 0, 0, 0);
        }

        //Calculate the decel costing
        double decelCoast = currCep->GetDecelCoast(speed, acc, Gradient);

        //Calculate the result values (Zero emissions by costing)
        if (acc >= decelCoast) {
            return new VehicleResult(time, speed, Gradient, power, P_pos, P_eng / currCep->RatedPower, P_eng / currCep->DrivingPower, acc, currCep->GetEmission("FC", power, speed, Helper), 0, currCep->GetCO2Emission(currCep->GetEmission("FC", power, speed, Helper), currCep->GetEmission("CO", power, speed, Helper), currCep->GetEmission("HC", power, speed, Helper), Helper), currCep->GetEmission("NOx", power, speed, Helper), currCep->GetEmission("HC", power, speed, Helper), currCep->GetEmission("PM", power, speed, Helper), currCep->GetEmission("CO", power, speed, Helper));
        }
        else {
            return new VehicleResult(time, speed, Gradient, power, P_pos, power / currCep->RatedPower, power / currCep->DrivingPower, acc, 0, 0, 0, 0, 0, 0, 0);
        }
    }
}
