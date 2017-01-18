/****************************************************************************/
/// @file    MSSOTLPolicyStimulus.cpp
/// @author  Riccardo Belletti
/// @author  Simone Bacchilega
/// @date    2014-09-30
/// @version $Id$
///
// The class for Swarm-based low-level policy
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include "MSSOTLPolicy5DStimulus.h"

MSSOTLPolicy5DStimulus::MSSOTLPolicy5DStimulus(std::string keyPrefix,
        const std::map<std::string, std::string>& parameters) :
    MSSOTLPolicyDesirability(keyPrefix, parameters) {

    stimCoxDVal = 1;
    stimOffsetInDVal = 1;
    stimOffsetOutDVal = 1;
    stimOffsetDispersionInDVal = 1;
    stimOffsetDispersionOutDVal = 1;
    stimDivInDVal = 1;
    stimDivOutDVal = 1;
    stimDivDispersionInDVal = 1;
    stimDivDispersionOutDVal = 1;
    stimCoxExpInDVal = 0;
    stimCoxExpOutDVal = 0;
    stimCoxExpDispersionInDVal = 0;
    stimCoxExpDispersionOutDVal = 0;
}

SUMOReal MSSOTLPolicy5DStimulus::computeDesirability(SUMOReal vehInMeasure, SUMOReal vehOutMeasure, SUMOReal vehInDispersionMeasure, SUMOReal vehOutDispersionMeasure) {
    DBG(
        std::ostringstream str;
        str << "cox=" << getStimCox() << ", cox_exp_in=" << getStimCoxExpIn() << ", cox_exp_out=" << getStimCoxExpOut()
        << ", off_in=" << getStimOffsetIn() << ", off_out=" << getStimOffsetOut() << ", div_in=" << getStimDivisorIn() << ", div_out=" << getStimDivisorOut(); WRITE_MESSAGE(str.str());)

    //		it seems to be not enough, a strange segmentation fault appears...
    //	 if((getStimCoxExpIn()!=0.0 && getStimDivisorIn()==0.0)||(getStimCoxExpOut()!=0.0 && getStimDivisorOut()==0.0)){
    if (getStimDivisorIn() == 0 || getStimDivisorOut() == 0) {
        std::ostringstream errorMessage;
        errorMessage << "INCORRECT VALUES" << "\nStimCoxExpIn="
                     << getStimCoxExpIn() << ", StimDivisorIn=" << getStimDivisorIn()
                     << ", StimCoxExpOut=" << getStimCoxExpOut()
                     << ", StimDivisorOut=" << getStimDivisorOut();
        WRITE_ERROR(errorMessage.str());
        assert(-1);
        return -1;
    } else {
        SUMOReal stimulus = getStimCox()
                            * exp(
                                -getStimCoxExpIn()
                                * pow(vehInMeasure - getStimOffsetIn(), 2)
                                / getStimDivisorIn()
                                - getStimCoxExpOut()
                                * pow(vehOutMeasure - getStimOffsetOut(), 2)
                                / getStimDivisorOut()
                                - getStimCoxExpDispersionIn()
                                * pow(vehInDispersionMeasure - getStimOffsetDispersionIn(), 2)
                                / getStimDivisorDispersionIn()
                                - getStimCoxExpDispersionOut()
                                * pow(vehOutDispersionMeasure - getStimOffsetDispersionOut(), 2)
                                / getStimDivisorDispersionOut()

                            );
        return stimulus;
    }
}

SUMOReal MSSOTLPolicy5DStimulus::computeDesirability(SUMOReal vehInMeasure, SUMOReal vehOutMeasure) {

    return computeDesirability(vehInMeasure, vehOutMeasure, 0, 0);
}
std::string  MSSOTLPolicy5DStimulus::getMessage() {
    std::ostringstream _str;
    _str << " stimCox " << getStimCox()
         << " StimOffsetIn " << getStimOffsetIn()
         << " StimOffsetOut " << getStimOffsetOut()
         << " StimDivisorIn " << getStimDivisorIn()
         << " StimDivisorOut " << getStimDivisorOut()
         << " StimCoxExpIn " << getStimCoxExpIn()
         << " StimCoxExpOut " << getStimCoxExpOut()
         << " .";
    return _str.str();
}
