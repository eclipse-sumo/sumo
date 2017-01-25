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
// Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/StringTokenizer.h>
#include "MSSOTLPolicy5DFamilyStimulus.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSSOTLPolicy5DFamilyStimulus::MSSOTLPolicy5DFamilyStimulus(std::string keyPrefix,
        const std::map<std::string, std::string>& parameters) :
    MSSOTLPolicyDesirability(keyPrefix, parameters) {

    default_values["_STIM_COX"] = "1";
    default_values["_STIM_OFFSET_IN"] = "1";
    default_values["_STIM_OFFSET_OUT"] = "1";
    default_values["_STIM_OFFSET_DISPERSION_IN"] = "1";
    default_values["_STIM_OFFSET_DISPERSION_OUT"] = "1";
    default_values["_STIM_DIVISOR_IN"] = "1";
    default_values["_STIM_DIVISOR_OUT"] = "1";
    default_values["_STIM_DIVISOR_DISPERSION_IN"] = "1";
    default_values["_STIM_DIVISOR_DISPERSION_OUT"] = "1";
    default_values["_STIM_COX_EXP_IN"] = "0";
    default_values["_STIM_COX_EXP_OUT"] = "0";
    default_values["_STIM_COX_EXP_DISPERSION_IN"] = "0";
    default_values["_STIM_COX_EXP_DISPERSION_OUT"] = "0";

    params_names.push_back("_STIM_COX");
    params_names.push_back("_STIM_OFFSET_IN");
    params_names.push_back("_STIM_OFFSET_OUT");
    params_names.push_back("_STIM_OFFSET_DISPERSION_IN");
    params_names.push_back("_STIM_OFFSET_DISPERSION_OUT");
    params_names.push_back("_STIM_DIVISOR_IN");
    params_names.push_back("_STIM_DIVISOR_OUT");
    params_names.push_back("_STIM_DIVISOR_DISPERSION_IN");
    params_names.push_back("_STIM_DIVISOR_DISPERSION_OUT");
    params_names.push_back("_STIM_COX_EXP_IN");
    params_names.push_back("_STIM_COX_EXP_OUT");
    params_names.push_back("_STIM_COX_EXP_DISPERSION_IN");
    params_names.push_back("_STIM_COX_EXP_DISPERSION_OUT");


    int size_family = int(readParameter(keyPrefix + "_SIZE_FAMILY", 1));
    DBG(

        std::ostringstream str;
        str << keyPrefix << "\n" << "size fam" << size_family;
        WRITE_MESSAGE(str.str());
    )

    std::vector< std::map <std::string, std::string > > sliced_maps;

    for (int i = 0; i < size_family; i++) {
        sliced_maps.push_back(std::map<std::string, std::string>());
    }

    //For each param list, slice values
    for (int i = 0; i < (int)params_names.size(); i ++) {
        std::string key = keyPrefix + params_names[i];
        std::string param_list = getParameter(key, default_values[params_names[i]]);
        std::vector<std::string> tokens = StringTokenizer(param_list, ";").getVector();

        for (int token_counter = 0; token_counter < size_family; ++token_counter) {
            if (token_counter >= (int)tokens.size()) {
                std::ostringstream errorMessage;
                errorMessage << "Error in " << key << ": not enough tokens.";
                WRITE_ERROR(errorMessage.str());
                assert(-1);
            }
            DBG(
                std::ostringstream str;
                str << "found token " << tokens[token_counter] << " position " << token_counter;
                WRITE_MESSAGE(str.str());
            )
            sliced_maps[token_counter][key] = tokens[token_counter];
        }
    }

    for (int i = 0; i < size_family; i++) {
        std::map<std::string, std::string>& ref_map = sliced_maps[i];
        family.push_back(new MSSOTLPolicy5DStimulus(keyPrefix, ref_map));
    }

}


SUMOReal MSSOTLPolicy5DFamilyStimulus::computeDesirability(SUMOReal vehInMeasure, SUMOReal vehOutMeasure, SUMOReal vehInDispersionMeasure, SUMOReal vehOutDispersionMeasure) {
    /*DBG(
    		std::ostringstream str;
    		str << "cox=" << getStimCox() << ", cox_exp_in=" << getStimCoxExpIn() << ", cox_exp_out=" << getStimCoxExpOut()
    				<< ", off_in=" << getStimOffsetIn() << ", off_out=" << getStimOffsetOut() << ", div_in=" << getStimDivisorIn() << ", div_out=" << getStimDivisorOut(); WRITE_MESSAGE(str.str());)
     */
    //		it seems to be not enough, a strange segmentation fault appears...
    //	 if((getStimCoxExpIn()!=0.0 && getStimDivisorIn()==0.0)||(getStimCoxExpOut()!=0.0 && getStimDivisorOut()==0.0)){

    SUMOReal best_stimulus = -1;
    for (std::vector<MSSOTLPolicy5DStimulus*>::const_iterator it  = family.begin(); it != family.end(); it++) {
        SUMOReal temp_stimulus = (*it)->computeDesirability(vehInMeasure, vehOutMeasure, vehInDispersionMeasure, vehOutDispersionMeasure);
        DBG(
            std::ostringstream str;
            str << "STIMULUS: " << temp_stimulus;
            WRITE_MESSAGE(str.str());
        )
        if (temp_stimulus > best_stimulus) {
            best_stimulus = temp_stimulus;
        }
    }

    DBG(
        std::ostringstream str;
        str << "BEST STIMULUS: " << best_stimulus;
        WRITE_MESSAGE(str.str());
    )
    return best_stimulus;
}


SUMOReal MSSOTLPolicy5DFamilyStimulus::computeDesirability(SUMOReal vehInMeasure, SUMOReal vehOutMeasure) {

    return computeDesirability(vehInMeasure, vehOutMeasure, 0, 0);
}

std::string MSSOTLPolicy5DFamilyStimulus::getMessage() {
    std::ostringstream ot;
    for (int i = 0; i < (int)family.size(); i++) {
        ot << " gaussian " << i << ":" << family[i]->getMessage();
    }
    return ot.str();
}

/*
std::vector<std::string> inline MSSOTLPolicy5DFamilyStimulus::StringSplit(const std::string &source, const char *delimiter = " ", bool keepEmpty = false)
{
    std::vector<std::string> results;

    int prev = 0;
    std::string::size_type next = 0;

    while ((next = source.find_first_of(delimiter, prev)) != std::string::npos)
    {
        if (keepEmpty || (next - prev != 0))
        {
            results.push_back(source.substr(prev, next - prev));
        }
        prev = next + 1;
    }

    if (prev < source.size())
    {
        results.push_back(source.substr(prev));
    }

    return results;
}
*/
