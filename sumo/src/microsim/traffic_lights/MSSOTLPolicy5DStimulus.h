/****************************************************************************/
/// @file    MSSOTLPolicyStimulus.h
/// @author  Riccardo Belletti
/// @author  Simone Bacchilega
/// @date    2014-09-30
/// @version $Id$
///
// The class the low-level policy stimulus
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

#ifndef MSSOTLPOLICY5DSTIMULUS_H_
#define MSSOTLPOLICY5DSTIMULUS_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

//#define SWARM_DEBUG
#include <utils/common/SwarmDebug.h>
#include <utils/common/MsgHandler.h>
#include <sstream>
#include <math.h>
#include "MSSOTLPolicyDesirability.h"

/**
 * \class MSSOTLPolicyStimulus
 * \brief This class determines the stimulus of a MSSOTLPolicy when
 * used in combination with a high level policy.\n
 * The stimulus function is calculated as follows:\n
 * stimulus = cox * exp(-pow(pheroIn - offsetIn, 2)/divisor -pow(pheroOut - offsetOut, 2)/divisor)
 */
class MSSOTLPolicy5DStimulus: public MSSOTLPolicyDesirability {

private:

    SUMOReal stimCoxDVal,
             stimOffsetInDVal, stimOffsetOutDVal, stimOffsetDispersionInDVal, stimOffsetDispersionOutDVal,
             stimDivInDVal, stimDivOutDVal, stimDivDispersionInDVal, stimDivDispersionOutDVal,
             stimCoxExpInDVal, stimCoxExpOutDVal, stimCoxExpDispersionInDVal, stimCoxExpDispersionOutDVal;

public:

    MSSOTLPolicy5DStimulus(std::string keyPrefix, const std::map<std::string, std::string>& parameters);

    SUMOReal getStimCox() {
        std::string key = getKeyPrefix() + "_STIM_COX";
        return readParameter(key, stimCoxDVal);
    }
    void setStimCoxDefVal(SUMOReal defVal) {
        stimCoxDVal = defVal;
    }
    SUMOReal getStimOffsetIn() {
        std::string key = getKeyPrefix() + "_STIM_OFFSET_IN";
        return readParameter(key, stimOffsetInDVal);
    }
    void setStimOffsetInDefVal(SUMOReal defVal) {
        stimOffsetInDVal = defVal;
    }
    SUMOReal getStimOffsetOut() {
        std::string key = getKeyPrefix() + "_STIM_OFFSET_OUT";
        return readParameter(key, stimOffsetOutDVal);
    }

    void setStimOffsetOutDefVal(SUMOReal defVal) {
        stimOffsetOutDVal = defVal;
    }

    SUMOReal getStimOffsetDispersionIn() {
        std::string key = getKeyPrefix() + "_STIM_OFFSET_DISPERSION_IN";
        return readParameter(key, stimOffsetDispersionInDVal);
    }
    void setStimOffsetDispersionInDefVal(SUMOReal defVal) {
        stimOffsetDispersionInDVal = defVal;
    }
    SUMOReal getStimOffsetDispersionOut() {
        std::string key = getKeyPrefix() + "_STIM_OFFSET_DISPERSION_OUT";
        return readParameter(key, stimOffsetDispersionOutDVal);
    }
    void setStimOffsetDispersionOutDefVal(SUMOReal defVal) {
        stimOffsetDispersionOutDVal = defVal;
    }
    SUMOReal getStimDivisorIn() {
        std::string key = getKeyPrefix() + "_STIM_DIVISOR_IN";
        return readParameter(key, stimDivInDVal);
    }

    void setStimDivisorInDefVal(SUMOReal defVal) {
        stimDivInDVal = defVal;
    }

    SUMOReal getStimDivisorOut() {
        std::string key = getKeyPrefix() + "_STIM_DIVISOR_OUT";
        return readParameter(key, stimDivOutDVal);
    }

    void setStimDivisorOutDefVal(SUMOReal defVal) {
        stimDivOutDVal = defVal;
    }

    SUMOReal getStimDivisorDispersionIn() {
        std::string key = getKeyPrefix() + "_STIM_DIVISOR_DISPERSION_IN";
        return readParameter(key, stimDivDispersionInDVal);
    }

    void setStimDivisorDispersionInDefVal(SUMOReal defVal) {
        stimDivDispersionInDVal = defVal;
    }
    SUMOReal getStimDivisorDispersionOut() {
        std::string key = getKeyPrefix() + "_STIM_DIVISOR_DISPERSION_OUT";
        return readParameter(key, stimDivDispersionOutDVal);
    }

    void setStimDivisorDispersionOutDefVal(SUMOReal defVal) {
        stimDivDispersionOutDVal = defVal;
    }
    SUMOReal getStimCoxExpIn() {
        std::string key = getKeyPrefix() + "_STIM_COX_EXP_IN";
        return readParameter(key, stimCoxExpInDVal);
    }
    void setStimCoxExpInDefVal(SUMOReal defVal) {
        stimCoxExpInDVal = defVal;
    }
    SUMOReal getStimCoxExpOut() {
        std::string key = getKeyPrefix() + "_STIM_COX_EXP_OUT";
        return readParameter(key, stimCoxExpOutDVal);
    }
    void setStimCoxExpOutDefVal(SUMOReal defVal) {
        stimCoxExpOutDVal = defVal;
    }

    SUMOReal getStimCoxExpDispersionIn() {
        std::string key = getKeyPrefix() + "_STIM_COX_EXP_DISPERSION_IN";
        return readParameter(key, stimCoxExpDispersionInDVal);
    }
    void setStimCoxExpDispersionInDefVal(SUMOReal defVal) {
        stimCoxExpDispersionInDVal = defVal;
    }
    SUMOReal getStimCoxExpDispersionOut() {
        std::string key = getKeyPrefix() + "_STIM_COX_EXP_DISPERSION_OUT";
        return readParameter(key, stimCoxExpDispersionOutDVal);
    }
    void setStimCoxExpDispersionOutDefVal(SUMOReal defVal) {
        stimCoxExpDispersionOutDVal = defVal;
    }
    std::string getMessage();
    /**
     *	@brief Computes stimulus function
     *  stimulus = cox * exp(-pow(pheroIn - offsetIn, 2)/divisor -pow(pheroOut - offsetOut, 2)/divisor);
     */
    virtual SUMOReal computeDesirability(SUMOReal vehInMeasure,
                                         SUMOReal vehOutMeasure);

    virtual SUMOReal computeDesirability(SUMOReal vehInMeasure, SUMOReal vehOutMeasure, SUMOReal vehInDispersionMeasure, SUMOReal vehOutDispersionMeasure);
};

#endif /* MSSOTLPOLICYSTIMULUS_H_ */
