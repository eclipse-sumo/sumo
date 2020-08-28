/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2020-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    fmi2Functions.c
/// @author  Robert Hilbrich
/// @date    Tue, 03 Mar 2020
///
// Implementation of the FMI2 interface functions
/****************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <foreign/fmi/fmi2Functions.h>
#include "fmi2main.h"

/* **********************************************************************************************
 * * IMPLEMENTATION OF GENERIC FUNCTIONALITY
 * **********************************************************************************************/

const char* fmi2GetVersion() {
    return fmi2Version;
}

const char* fmi2GetTypesPlatform() {
    return fmi2TypesPlatform;
}

/* ***********************************************************************************************
   * CREATION AND DESTRUCTION OF AN FMU
   ***********************************************************************************************/

/* The function returns a new instance of an FMU. If a null pointer is returned, then instantiation
   failed.*/
fmi2Component 
fmi2Instantiate(fmi2String instanceName, fmi2Type fmuType, fmi2String fmuGUID,
                  fmi2String fmuResourceLocation, const fmi2CallbackFunctions *functions,
                  fmi2Boolean visible, fmi2Boolean loggingOn)
{
  
   allocateMemoryType cbAllocateMemory = (allocateMemoryType)functions->allocateMemory;

   ModelInstance* comp = (ModelInstance *) cbAllocateMemory(1, sizeof(ModelInstance));

   if (comp) {
      comp->componentEnvironment = functions->componentEnvironment;
		
		/* Callback functions for specific logging, malloc and free;
		   we need callback functions because we cannot know, which functions
		   the environment will provide for us */
		comp->logger = (loggerType)functions->logger;
		comp->allocateMemory = (allocateMemoryType)functions->allocateMemory;
		comp->freeMemory = (freeMemoryType)functions->freeMemory;

		comp->instanceName = (char *)comp->allocateMemory(1 + strlen(instanceName), sizeof(char));

		if (fmuResourceLocation) {
		 	comp->resourceLocation = (char *)comp->allocateMemory(1 + strlen(fmuResourceLocation), sizeof(char));
		 	strcpy((char *)comp->resourceLocation, (char *)fmuResourceLocation);
		} else {
		 	comp->resourceLocation = NULL;
		}

		comp->modelData = (ModelData *)comp->allocateMemory(1, sizeof(ModelData));
        
      comp->logEvents = loggingOn;
      comp->logErrors = true; // always log errors
	}
	strcpy((char *)comp->instanceName, (char *)instanceName);

	return comp;
}

/* Disposes the given instance, unloads the loaded model, and frees all the allocated memory
and other resources that have been allocated by the functions of the FMU interface. */
void 
fmi2FreeInstance(fmi2Component c) {
	ModelInstance *comp = (ModelInstance *)c;

    if (!comp) return;

	/* We want to free everything that we allocated in fmi2Instantiate */
	comp->freeMemory((void *)comp->instanceName);
	comp->freeMemory((void *)comp->resourceLocation); 
	comp->freeMemory((void *)comp->modelData);
}

/* Define what should be logged - if logging is enabled globally */ 
fmi2Status 
fmi2SetDebugLogging(fmi2Component c, fmi2Boolean loggingOn, size_t nCategories, const fmi2String categories[]) {
    
    ModelInstance *comp = (ModelInstance *)c;

	if (loggingOn) {
        for (size_t i = 0; i < nCategories; i++) {
            if (categories[i] == NULL) {
                logError(comp, "Log category[%d] must not be NULL", i);
                return fmi2Error;
            } else if (strcmp(categories[i], "logStatusError") == 0) {
                comp->logErrors = true;
            } else if (strcmp(categories[i], "logEvents") == 0) {
                comp->logEvents = true;
            } else {
                logError(comp, "Log category[%d] must be one of logEvents or logStatusError but was %s", i, categories[i]);
                return fmi2Error;
            }
        }
    } else {
		// Logging is disabled globally, no need for a more fine grained logging
        comp->logEvents = false;
        comp->logErrors = false;
    }

    return fmi2OK;
}

fmi2Status 
fmi2SetupExperiment(fmi2Component c, fmi2Boolean toleranceDefined, fmi2Real tolerance,
					fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime) {

    // ignore arguments: toleranceDefined, tolerance
    ModelInstance *comp = (ModelInstance *)c;

	// Store the start and stop times of the experiment
    comp->startTime = startTime;
	comp->stopTime = stopTime;

    return fmi2OK;
}