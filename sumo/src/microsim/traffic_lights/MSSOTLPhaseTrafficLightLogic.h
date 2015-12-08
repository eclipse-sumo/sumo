/****************************************************************************/
/// @file    MSSOTLPhaseTrafficLightLogic.h
/// @author  Gianfilippo Slager
/// @date    Feb 2010
/// @version $Id: MSSOTLPhaseTrafficLightLogic.h 0 2010-02-18 12:40:00Z gslager $
///
// The class for SOTL Phase logics
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSSOTLPhaseTrafficLightLogic_h
#define MSSOTLPhaseTrafficLightLogic_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSSOTLTrafficLightLogic.h"
class MSSOTLPhaseTrafficLightLogic :
	public MSSOTLTrafficLightLogic
{
public:
		/** 
	 * @brief Constructor without sensors passed
     * @param[in] tlcontrol The tls control responsible for this tls
     * @param[in] id This tls' id
     * @param[in] subid This tls' sub-id (program id)
     * @param[in] phases Definitions of the phases
     * @param[in] step The initial phase index
     * @param[in] delay The time to wait before the first switch
     */
	MSSOTLPhaseTrafficLightLogic(MSTLLogicControl &tlcontrol,
                              const std::string &id, const std::string &subid,
                              const Phases &phases, unsigned int step, SUMOTime delay,const std::map<std::string, std::string>& parameters) throw();

	/** 
	 * @brief Constructor with sensors passed
     * @param[in] tlcontrol The tls control responsible for this tls
     * @param[in] id This tls' id
     * @param[in] subid This tls' sub-id (program id)
     * @param[in] phases Definitions of the phases
     * @param[in] step The initial phase index
     * @param[in] delay The time to wait before the first switch
     */
	MSSOTLPhaseTrafficLightLogic(MSTLLogicControl &tlcontrol,
                              const std::string &id, const std::string &subid,
                              const Phases &phases, unsigned int step, SUMOTime delay,const std::map<std::string, std::string>& parameters, MSSOTLSensors *sensors) throw();

protected:

	/*
	 * @brief Contains the logic to decide the phase change
	 */
	bool canRelease() throw();
};

#endif
/****************************************************************************/
