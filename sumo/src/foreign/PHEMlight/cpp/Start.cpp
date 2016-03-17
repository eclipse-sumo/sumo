#include <cstdlib>
#include <fstream>
#include "Start.h"
#include "CEPHandler.h"
#include "Helpers.h"
#include "cResult.h"
#include "CEP.h"
#include "Constants.h"


namespace PHEMlightdll {

    bool Start::CALC_Array(const std::string& VEH, std::vector<double>& Time, std::vector<double>& Velocity, std::vector<double>& Gradient, std::vector<VehicleResult*>& VehicleResultsOrg, bool fleetMix, const std::string& PHEMDataV, const std::string& CommentPref) {
        //Declaration
        int i;
        double acc;
        std::vector<VehicleResult*> _VehicleResult;

        //Initialisation
        Helper->setErrMsg("");

        //Borrow
        Helper->setCommentPrefix(CommentPref);
        Helper->setPHEMDataV(PHEMDataV);
        std::vector<std::string> phemPath;
        if (getenv("PHEMLIGHT_PATH") != 0) {
            phemPath.push_back(std::string(getenv("PHEMLIGHT_PATH")) + "/");
        }
        if (getenv("SUMO_HOME") != 0) {
            phemPath.push_back(std::string(getenv("SUMO_HOME")) + "/data/emissions/PHEMlight/");
        }

        {
            //Get vehicle string
            if (!Helper->setclass(VEH)) {
                VehicleResultsOrg.clear();
                return false;
            }

            //Generate the class
            DataInput = new CEPHandler();

            //Read the vehicle and emission data
            if (!DataInput->GetCEP(phemPath, Helper)) {
                VehicleResultsOrg.clear();
                return false;
            }
        }

        //Calculate emissions per second
        for (i = 1; i <= Time.size() - 1; i++) {
            //Calculate the acceleration
            acc = (Velocity[i] - Velocity[i - 1]) / (Time[i] - Time[i - 1]);

            //Calculate and save the data in the List
            _VehicleResult.push_back(PHEMLight::CreateVehicleStateData(Helper, DataInput->getCEPS().find(Helper->getgClass())->second, Time[i - 1], Velocity[i - 1], acc, Gradient[i - 1]));
            if (Helper->getErrMsg() != "") {
                VehicleResultsOrg.clear();
                return false;
            }
        }
        VehicleResultsOrg = _VehicleResult;
        return true;
    }

    bool Start::CALC_Single(const std::string& VEH, double Time, double Velocity, double acc, double Gradient, std::vector<VehicleResult*>& VehicleResultsOrg, bool fleetMix, const std::string& PHEMDataV, const std::string& CommentPref) {
        //Declaration
        std::vector<VehicleResult*> _VehicleResult;
        VehicleResultsOrg = _VehicleResult;

        //Borrow
        Helper->setCommentPrefix(CommentPref);
        Helper->setPHEMDataV(PHEMDataV);
        std::vector<std::string> phemPath;
        if (getenv("PHEMLIGHT_PATH") != 0) {
            phemPath.push_back(std::string(getenv("PHEMLIGHT_PATH")) + "/");
        }
        if (getenv("SUMO_HOME") != 0) {
            phemPath.push_back(std::string(getenv("SUMO_HOME")) + "/data/emissions/PHEMlight/");
        }

        {
        //Read the vehicle and emission data
            //Get vehicle string
            if (!Helper->setclass(VEH)) {
                VehicleResultsOrg.clear();
                return false;
            }

            //Generate the class
            DataInput = new CEPHandler();

            //Read the vehicle and emission data
            if (!DataInput->GetCEP(phemPath, Helper)) {
                VehicleResultsOrg.clear();
                return false;
            }
        }

        //Calculate and save the data in the List
        _VehicleResult.push_back(PHEMLight::CreateVehicleStateData(Helper, DataInput->getCEPS().find(Helper->getgClass())->second, Time, Velocity, acc, Gradient));
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

        allLines->appendLine(std::string("Vehicletype: ,") + vehicle);
        allLines->appendLine("Time, Speed, Gradient, Accelaration, Engine power raw, P_pos, P_norm_rated, P_norm_drive, FC, FC el., CO2, NOx, CO, HC, PM");
        allLines->appendLine("[s], [m/s], [%], [m/s^2], [kW], [kW], [-], [-], [g/h], [kWh/h], [g/h], [g/h], [g/h], [g/h], [g/h]");

        //Write data
        for (std::vector<VehicleResult*>::const_iterator Result = _VehicleResult.begin(); Result != _VehicleResult.end(); ++Result) {
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getTime())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getSpeed())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getGrad())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getAccelaration())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getPower())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getPPos())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getPNormRated())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getPNormDrive())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getEmissionData()->getFC())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getEmissionData()->getFCel())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getEmissionData()->getCO2())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getEmissionData()->getNOx())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getEmissionData()->getCO())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getEmissionData()->getHC())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
            allLines->append((*Result)->getEmissionData()->getPM())->append(lineEnding);
        }

        // Write the string to a file.
        try {
            std::ofstream file(path.c_str());
            file << allLines->toString();
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

        //Write head
        allLines->appendLine("PHEMLight Results");
        allLines->appendLine("");
        allLines->appendLine("Vehicle, Cycle, Time, Speed, Gradient, Accelaration, Engine power raw, P_pos, P_norm_rated, P_norm_drive, FC, FC el., CO2, NOx, CO, HC, PM");
        allLines->appendLine("[-], [-], [s], [km/h], [%], [m/s^2], [kW], [kW], [-], [-], [g/km], [kWh/km], [g/km], [g/km], [g/km], [g/km], [g/km]");

        //Write data
        allLines->append(vehicle + std::string(","));
        allLines->append(cycle + std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getTime())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getSpeed())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getGrad())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getAccelaration())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getPower())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getPPos())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getPNormRated())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getPNormDrive())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getEmissionData()->getFC())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getEmissionData()->getFCel())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getEmissionData()->getCO2())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getEmissionData()->getNOx())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getEmissionData()->getCO())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getEmissionData()->getHC())->append(std::string(","));
//C# TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'ToString':
        allLines->append(_VehicleResult->getEmissionData()->getPM());

        // Write the string to a file.
        try {
            std::ofstream file(path.c_str());
            file << allLines->toString();
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
            sum_speed += (*Result)->getSpeed() * 3.6;
            sum_power += (*Result)->getPower();
            if ((*Result)->getPPos() > 0) {
                sum_pPos += (*Result)->getPPos();
            }
            sum_grad += (*Result)->getGrad();
            sum_pNormRated += (*Result)->getPNormRated();
            sum_pNormDrive += (*Result)->getPNormDrive();
            sum_acc += (*Result)->getAccelaration();
            sum_fcel += (*Result)->getEmissionData()->getFCel();
            sum_fc += (*Result)->getEmissionData()->getFC();
            sum_cO2 += (*Result)->getEmissionData()->getCO2();
            sum_nOx += (*Result)->getEmissionData()->getNOx();
            sum_hC += (*Result)->getEmissionData()->getHC();
            sum_pM += (*Result)->getEmissionData()->getPM();
            sum_cO += (*Result)->getEmissionData()->getCO();
        }

        //Build average
        sum_time = _VehicleResult[_VehicleResult.size() - 1]->getTime() - _VehicleResult[0]->getTime();
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

    VehicleResult* PHEMLight::CreateVehicleStateData(Helpers* Helper, CEP* currCep, double time, double inputSpeed, double inputAcc, double Gradient) {
        //Declaration
        double speed = inputSpeed > 0 ? inputSpeed : 0;
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
        if (Helper->gettClass() == Constants::strBEV) {
            return new VehicleResult(time, speed, Gradient, power, P_pos, P_eng / currCep->getRatedPower(), P_eng / currCep->getDrivingPower(), acc, 0, currCep->GetEmission("FC", power, speed, Helper), 0, 0, 0, 0, 0);
        }

        //Calculate the decel costing
        double decelCoast = currCep->GetDecelCoast(speed, acc, Gradient);

        //Calculate the result values (Zero emissions by costing)
        if (acc >= decelCoast) {
            return new VehicleResult(time, speed, Gradient, power, P_pos, P_eng / currCep->getRatedPower(), P_eng / currCep->getDrivingPower(), acc, currCep->GetEmission("FC", power, speed, Helper), 0, currCep->GetCO2Emission(currCep->GetEmission("FC", power, speed, Helper), currCep->GetEmission("CO", power, speed, Helper), currCep->GetEmission("HC", power, speed, Helper), Helper), currCep->GetEmission("NOx", power, speed, Helper), currCep->GetEmission("HC", power, speed, Helper), currCep->GetEmission("PM", power, speed, Helper), currCep->GetEmission("CO", power, speed, Helper));
        }
        else {
            return new VehicleResult(time, speed, Gradient, power, P_pos, power / currCep->getRatedPower(), power / currCep->getDrivingPower(), acc, 0, 0, 0, 0, 0, 0, 0);
        }
    }
}
