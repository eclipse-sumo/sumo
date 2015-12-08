/****************************************************************************/
/// @file    MSSOTLPolicyStimulus.cpp
/// @author  Riccardo Belletti
/// @author  Simone Bacchilega
/// @date    Aug 2014
/// @version $Id: MSSOTLPolicyDesirability.cpp 0  $
///
// The class for Swarm-based low-level policy
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include "MSSOTLPolicy5DFamilyStimulus.h"

MSSOTLPolicy5DFamilyStimulus::MSSOTLPolicy5DFamilyStimulus(string keyPrefix,
		const std::map<std::string, std::string>& parameters) :
		MSSOTLPolicyDesirability(keyPrefix,parameters) {

	default_values["_STIM_COX"]="1";
	default_values["_STIM_OFFSET_IN"]="1";
	default_values["_STIM_OFFSET_OUT"]="1";
	default_values["_STIM_OFFSET_DISPERSION_IN"]="1";
	default_values["_STIM_OFFSET_DISPERSION_OUT"]="1";
	default_values["_STIM_DIVISOR_IN"]="1";
	default_values["_STIM_DIVISOR_OUT"]="1";
	default_values["_STIM_DIVISOR_DISPERSION_IN"]="1";
	default_values["_STIM_DIVISOR_DISPERSION_OUT"]="1";
	default_values["_STIM_COX_EXP_IN"]="0";
	default_values["_STIM_COX_EXP_OUT"]="0";
	default_values["_STIM_COX_EXP_DISPERSION_IN"]="0";
	default_values["_STIM_COX_EXP_DISPERSION_OUT"]="0";

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
							str<< keyPrefix << "\n"<< "size fam" << size_family;
							WRITE_MESSAGE(str.str());
					)

	vector< map <string ,string > > sliced_maps;

	for(int i = 0; i<size_family;i++)
		sliced_maps.push_back(map<string,string>());

	//For each param list, slice values
	for(int i = 0; i< params_names.size(); i ++){
		string key = keyPrefix + params_names[i];
		string param_list = getParameter(key, default_values[params_names[i]]);

		char *dup = strdup(param_list.c_str());
		int token_counter;
		char * pch;

		for(token_counter = 0, pch = strtok(dup, ";"); token_counter<size_family; ++token_counter, pch = strtok (NULL, ";"))
		{
			if(pch == NULL)
			{
				std::ostringstream errorMessage;
				errorMessage << "Error in " << key <<": not enough tokens.";
				WRITE_ERROR(errorMessage.str());
				assert(-1);
			}
			string token_found(pch);
			DBG(
						std::ostringstream str;
						str << "found token " << token_found << " position " << token_counter;
						WRITE_MESSAGE(str.str());
				)
			(sliced_maps[token_counter])[key] = token_found;
		}
		free(dup);
	}

	for(int i = 0; i<size_family;i++)
	{
		map<string,string> & ref_map = sliced_maps[i];
		family.push_back(new MSSOTLPolicy5DStimulus(keyPrefix,ref_map));
	}

}


double MSSOTLPolicy5DFamilyStimulus::computeDesirability (double vehInMeasure, double vehOutMeasure, double vehInDispersionMeasure,double vehOutDispersionMeasure) {
	/*DBG(
			std::ostringstream str;
			str << "cox=" << getStimCox() << ", cox_exp_in=" << getStimCoxExpIn() << ", cox_exp_out=" << getStimCoxExpOut()
					<< ", off_in=" << getStimOffsetIn() << ", off_out=" << getStimOffsetOut() << ", div_in=" << getStimDivisorIn() << ", div_out=" << getStimDivisorOut(); WRITE_MESSAGE(str.str());)
	 */
	//		it seems to be not enough, a strange segmentation fault appears...
	//	 if((getStimCoxExpIn()!=0.0 && getStimDivisorIn()==0.0)||(getStimCoxExpOut()!=0.0 && getStimDivisorOut()==0.0)){

	double best_stimulus = -1;
	for(std::vector<MSSOTLPolicy5DStimulus*>::const_iterator it  = family.begin(); it!= family.end(); it++)
	{
		double temp_stimulus = (*it)->computeDesirability(vehInMeasure,vehOutMeasure,vehInDispersionMeasure,vehOutDispersionMeasure);
		DBG(
								std::ostringstream str;
								str << "STIMULUS: " << temp_stimulus;
								WRITE_MESSAGE(str.str());
						)
		if (temp_stimulus > best_stimulus)
			best_stimulus = temp_stimulus;
	}

	DBG(
									std::ostringstream str;
									str << "BEST STIMULUS: " << best_stimulus;
									WRITE_MESSAGE(str.str());
							)
	return best_stimulus;
}


double MSSOTLPolicy5DFamilyStimulus::computeDesirability (double vehInMeasure, double vehOutMeasure) {

	return computeDesirability(vehInMeasure,vehOutMeasure,0,0);
}

string MSSOTLPolicy5DFamilyStimulus::getMessage()
{
	std::ostringstream ot;
	string result = "";
	for(int i = 0; i<family.size(); i++)
		ot << " gaussian " << i << ":" << family[i]->getMessage();
		result = result + ot.str();
	return result;
}

/*
std::vector<std::string> inline MSSOTLPolicy5DFamilyStimulus::StringSplit(const std::string &source, const char *delimiter = " ", bool keepEmpty = false)
{
    std::vector<std::string> results;

    size_t prev = 0;
    size_t next = 0;

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
