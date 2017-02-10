/****************************************************************************/
/// @file    MSCFModel_Rail.h
/// @author  Gregor L\"ammel
/// @date    Tue, 08 Feb 2017
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMO_MSCFMODEL_RAIL_H
#define SUMO_MSCFMODEL_RAIL_H


#include "MSCFModel_Krauss.h"

class MSCFModel_Rail  : public MSCFModel {

public:
    virtual SUMOReal followSpeed(const MSVehicle *const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed,
                                 SUMOReal predMaxDecel) const;

    virtual SUMOReal stopSpeed(const MSVehicle *const veh, const SUMOReal speed, SUMOReal gap) const;

    virtual int getModelID() const;

    virtual MSCFModel *duplicate(const MSVehicleType *vtype) const;

    /** @brief Constructor
     * @param[in] trainType The train type
     */
    MSCFModel_Rail(const MSVehicleType *vtype, std::string trainType);


    virtual ~MSCFModel_Rail();

    virtual SUMOReal maxNextSpeed(SUMOReal speed, const MSVehicle *const veh) const override;

    virtual SUMOReal minNextSpeed(SUMOReal speed, const MSVehicle *const veh) const override;

private:
    typedef struct TrainParams {
        SUMOReal weight;
        SUMOReal mf;
        SUMOReal length;
        SUMOReal decl;
        SUMOReal vmax;
        SUMOReal recovery;
        SUMOReal rotWeight;
    };

    TrainParams trainParams;


    typedef std::map<SUMOReal,SUMOReal> LookUpMap;
    LookUpMap traction;
    LookUpMap resistance;

    LookUpMap initNGT400Traction() { // early version of NGT 400
        LookUpMap map;
        map[0] = 716.0;
        map[10] = 700.0;
        map[20] = 684.0;
        map[30] = 669.0;
        map[40] = 653.0;
        map[50] = 637.0;
        map[60] = 622.0;
        map[70] = 606.0;
        map[80] = 590.0;
        map[90] = 574.0;
        map[100] = 559.0;
        map[110] = 543.0;
        map[120] = 527.0;
        map[130] = 507.0;
        map[140] = 471.0;
        map[150] = 439.0;
        map[160] = 412.0;
        map[170] = 388.0;
        map[180] = 366.0;
        map[190] = 347.0;
        map[200] = 329.0;
        map[210] = 314.0;
        map[220] = 299.0;
        map[230] = 286.0;
        map[240] = 275.0;
        map[250] = 264.0;
        map[260] = 253.0;
        map[270] = 244.0;
        map[280] = 235.0;
        map[290] = 227.0;
        map[300] = 220.0;
        map[310] = 213.0;
        map[320] = 206.0;
        map[330] = 200.0;
        map[340] = 194.0;
        map[350] = 188.0;
        map[360] = 183.0;
        map[370] = 178.0;
        map[380] = 173.0;
        map[390] = 169.0;
        map[400] = 165.0;
        map[410] = 160.6;
        map[420] = 156.8;
        map[430] = 153.2;
        map[440] = 149.7;
        map[450] = 146.4;
        map[460] = 143.2;
        map[470] = 140.1;
        map[480] = 137.2;
        return  map;
    }

    LookUpMap initNGT400Resistance() { // early version of NGT 400
        LookUpMap map;
        map[0] = 1.9;
        map[10] = 2.1;
        map[20] = 2.4;
        map[30] = 2.9;
        map[40] = 3.5;
        map[50] = 4.2;
        map[60] = 5.0;
        map[70] = 6.0;
        map[80] = 7.0;
        map[90] = 8.2;
        map[100] = 9.6;
        map[110] = 11.0;
        map[120] = 12.6;
        map[130] = 14.3;
        map[140] = 16.1;
        map[150] = 18.0;
        map[160] = 20.1;
        map[170] = 22.3;
        map[180] = 24.6;
        map[190] = 27.0;
        map[200] = 29.5;
        map[210] = 32.2;
        map[220] = 35.0;
        map[230] = 37.9;
        map[240] = 40.9;
        map[250] = 44.1;
        map[260] = 47.4;
        map[270] = 50.8;
        map[280] = 54.3;
        map[290] = 58.0;
        map[300] = 61.7;
        map[310] = 65.6;
        map[320] = 69.7;
        map[330] = 73.8;
        map[340] = 78.1;
        map[350] = 82.4;
        map[360] = 86.9;
        map[370] = 91.6;
        map[380] = 96.3;
        map[390] = 101.2;
        map[400] = 106.2;
        map[410] = 111.3;
        map[420] = 116.6;
        map[430] = 121.9;
        map[440] = 127.4;
        map[450] = 133.0;
        map[460] = 138.7;
        map[470] = 144.6;
        map[480] = 150.6;
        return map;
    }

    TrainParams initNGT400Params() {
        TrainParams params;
        params.weight = 384;
        params.mf = 1.04;
        params.length = 200;
        params.decl = -0.9;
        params.vmax = 500/3.6;
        params.recovery = 0.6;
        params.rotWeight = params.weight * params.mf;
        return params;
    }



    LookUpMap initRB425Traction(){
        LookUpMap map;
        map[0] = 150;
        map[10] = 150;
        map[20] = 150;
        map[30] = 150;
        map[40] = 150;
        map[50] = 150;
        map[60] = 140;
        map[70] = 120;
        map[80] = 105;
        map[90] = 93;
        map[100] = 84;
        map[110] = 75;
        map[120] = 70;
        map[130] = 56;
        map[140] = 52;
        map[150] = 46;
        map[160] = 40;
        return map;
    }

    LookUpMap initRB425Resistance() {
        LookUpMap map;
        map[0] = 2.6;
        map[10] = 2.9;
        map[20] = 3.3;
        map[30] = 3.7;
        map[40] = 4.3;
        map[50] = 4.9;
        map[60] = 5.7;
        map[70] = 6.6;
        map[80] = 7.5;
        map[90] = 8.6;
        map[100] = 9.7;
        map[110] = 11.0;
        map[120] = 12.3;
        map[130] = 13.8;
        map[140] = 15.3;
        map[150] = 16.9;
        map[160] = 18.7;
        return map;
    }

    TrainParams initRB425Params() {
        TrainParams params;
        params.weight = 138;
        params.mf = 1.04;
        params.length = 67.5;
        params.decl = -1.0;
        params.vmax = 160/3.6;
        params.recovery = 0.6;
        params.rotWeight = params.weight * params.mf;
        return params;
    }
//    static std::map<SUMOReal,SUMOReal>

    double getResitance(SUMOReal speed) const;

    double getTraction(double speed) const;
};


#endif //SUMO_MSCFMODEL_RAIL_H
