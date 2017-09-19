/****************************************************************************/
/// @file    Constants.h
/// @author  Martin Dippold
/// @author  Michael Behrisch
/// @date    July 2016
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2016-2017 DLR (http://www.dlr.de/) and contributors
// PHEMlight module
// Copyright (C) 2016-2017 Technische Universitaet Graz, https://www.tugraz.at/
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


#ifndef PHEMlightCONSTANTS
#define PHEMlightCONSTANTS

#include <string>

#define FLEET

namespace PHEMlightdll {
    class Constants {
        //Calculation constant
    public:
        static const double GRAVITY_CONST;
        static const double AIR_DENSITY_CONST;
        static const double NORMALIZING_SPEED;
        static const double NORMALIZING_ACCELARATION;
        static const double SPEED_DCEL_MIN;
        static const double ZERO_SPEED_ACCURACY;
        static const double DRIVE_TRAIN_EFFICIENCY_All;
        static const double DRIVE_TRAIN_EFFICIENCY_CB;


        //Strings
        static const std::string HeavyVehicle;

        //Vehiclestrings
        static const std::string strPKW;
        static const std::string strLNF;
        static const std::string strLKW;
        static const std::string strLSZ;
        static const std::string strRB;
        static const std::string strLB;
        static const std::string strMR2;
        static const std::string strMR4;
        static const std::string strKKR;

        //Fuelstrings
        static const std::string strGasoline;
        static const std::string strDiesel;
        static const std::string strCNG;
        static const std::string strLPG;
        static const std::string strHybrid;
        static const std::string strBEV;

        //Euroclasses
        static const std::string strEU;

        //Sizeclasse
        static const std::string strSI;
        static const std::string strSII;
        static const std::string strSIII;


        //Drive train efficiency
        static double _DRIVE_TRAIN_EFFICIENCY;
        const static double&  getDRIVE_TRAIN_EFFICIENCY();
        static void setDRIVE_TRAIN_EFFICIENCY(const double&  value);

    };
}


#endif	//#ifndef PHEMlightCONSTANTS
