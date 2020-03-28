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

#include <foreign/fmi/fmi2Functions.h>

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
   /* For co-simulation, this function call has to perform all actions of a slave which are necessary
   before a simulation run starts */
   if (loggingOn) 
      functions->logger(functions->componentEnvironment, instanceName, fmi2Error, "error", "Not yet implemented");
   
   return NULL;
}

/* Disposes the given instance, unloads the loaded model, and frees all the allocated memory
and other resources that have been allocated by the functions of the FMU interface. */
void 
fmi2FreeInstance(fmi2Component c) {
   if (!c) return;
   
   /* Frees the model instance */
   // ModelInstance *comp = (ModelInstance *)c;
   // c->freeMemory(c->componentEnvironment, 
}
