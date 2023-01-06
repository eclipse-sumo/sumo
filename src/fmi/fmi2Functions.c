/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2020-2023 German Aerospace Center (DLR) and others.
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
/// @author  Matthias Schwamborn
/// @date    Tue, 03 Mar 2020
///
// Implementation of the FMI2 interface functions
/****************************************************************************/

#ifdef _MSC_VER
// Avoid warnings in windows build because of strcpy instead of strcpy_s,
// because the latter is not available on all platforms
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4820 4514 5045)
#endif

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <foreign/fmi/fmi2Functions.h>
#include "sumo2fmi_bridge.h"
#include "libsumocpp2c.h"

/* Explicit definition of unused parameters to avoid compiler warnings */
#define UNREFERENCED_PARAMETER(P) (P)

/* **********************************************************************************************
 * * IMPLEMENTATION OF GENERIC FUNCTIONALITY
 * **********************************************************************************************/
const char* fmi2GetTypesPlatform(void) {
    return fmi2TypesPlatform;
}

const char* fmi2GetVersion(void) {
    return fmi2Version;
}

/* ***********************************************************************************************
   * CREATION AND DESTRUCTION OF AN FMU
   ***********************************************************************************************/

/* Define what should be logged - if logging is enabled globally */
fmi2Status
fmi2SetDebugLogging(fmi2Component c, fmi2Boolean loggingOn, size_t nCategories, const fmi2String categories[]) {

    ModelInstance *comp = (ModelInstance *)c;

    if (loggingOn) {
        size_t i;
        for (i = 0; i < nCategories; i++) {
            if (categories[i] == NULL) {
                sumo2fmi_logError(comp, "Log category[%d] must not be NULL", i);
                return fmi2Error;
            } else if (strcmp(categories[i], "logStatusError") == 0) {
                comp->logErrors = true;
            } else if (strcmp(categories[i], "logEvents") == 0) {
                comp->logEvents = true;
            } else {
                sumo2fmi_logError(comp, "Log category[%d] must be one of logEvents or logStatusError but was %s", i, categories[i]);
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

/* The function returns a new instance of an FMU. If a null pointer is returned, then instantiation
   failed.*/
fmi2Component
fmi2Instantiate(fmi2String instanceName, fmi2Type fmuType, fmi2String fmuGUID,
                  fmi2String fmuResourceLocation, const fmi2CallbackFunctions *functions,
                  fmi2Boolean visible, fmi2Boolean loggingOn)
{
    UNREFERENCED_PARAMETER(fmuType);
    UNREFERENCED_PARAMETER(fmuGUID);
    UNREFERENCED_PARAMETER(visible);

    allocateMemoryType funcAllocateMemory = (allocateMemoryType)functions->allocateMemory;
    ModelInstance* comp = (ModelInstance *) funcAllocateMemory(1, sizeof(ModelInstance));

    if (comp) {
        comp->componentEnvironment = functions->componentEnvironment;

        /* Callback functions for specific logging, malloc and free;
           we need callback functions because we cannot know, which functions
           the environment will provide for us */
        comp->logger = (loggerType)functions->logger;
        comp->allocateMemory = (allocateMemoryType)functions->allocateMemory;
        comp->freeMemory = (freeMemoryType)functions->freeMemory;

        comp->instanceName = (char *)comp->allocateMemory(1 + strlen(instanceName), sizeof(char));
        strcpy((char *)comp->instanceName, (char *)instanceName);

        if (fmuResourceLocation) {
            comp->resourceLocation = (char *)comp->allocateMemory(1 + strlen(fmuResourceLocation), sizeof(char));
            strcpy((char *)comp->resourceLocation, (char *)fmuResourceLocation);
        } else {
            comp->resourceLocation = NULL;
        }

        comp->logEvents = loggingOn;
        comp->logErrors = true; // always log errors
    }

    return comp;
}

/* Disposes the given instance, unloads the loaded model, and frees all the allocated memory
and other resources that have been allocated by the functions of the FMU interface. */
void
fmi2FreeInstance(fmi2Component c) {
    ModelInstance *comp = (ModelInstance *)c;

    /* Store the pointer to the freeMemory function, because we
       are going to free comp as well */
    freeMemoryType freeMemoryFunc = comp->freeMemory;

    /* We want to free everything that we allocated in fmi2Instantiate */
    freeMemoryFunc((void *)comp->instanceName);
    freeMemoryFunc((void *)comp->resourceLocation);
    freeMemoryFunc((void *)comp->libsumoCallOptions);
    freeMemoryFunc((void *)comp->getterParameters);
    int i;
    for (i = 0; i < comp->bufferArrayLength; i++) {
        freeMemoryFunc((void *)comp->bufferArray[i]);
    }
    freeMemoryFunc((void *)comp->bufferArray);
    freeMemoryFunc((void *)comp);
}

fmi2Status
fmi2SetupExperiment(fmi2Component c, fmi2Boolean toleranceDefined, fmi2Real tolerance,
                    fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime) {

    UNREFERENCED_PARAMETER(toleranceDefined);
    UNREFERENCED_PARAMETER(tolerance);
    UNREFERENCED_PARAMETER(stopTimeDefined);

    // ignore arguments: toleranceDefined, tolerance
    ModelInstance *comp = (ModelInstance *)c;

    // Store the start and stop times of the experiment
    comp->startTime = startTime;
    comp->stopTime = stopTime;

    sumo2fmi_set_startValues(comp);

    return fmi2OK;
}

// Will be called after instantiation and after initial variables have been set
fmi2Status
fmi2EnterInitializationMode(fmi2Component c) {
    UNREFERENCED_PARAMETER(c);

    return fmi2OK;
}

// Informs the FMU to exit Initialization Mode
fmi2Status
fmi2ExitInitializationMode(fmi2Component c) {
    ModelInstance *comp = (ModelInstance *)c;

    sumo2fmi_logEvent(comp, "Calling libsumo with the following options: \"%s\"", comp->libsumoCallOptions);
    libsumo_load(comp->libsumoCallOptions);

    return fmi2OK;
}

// Informs the FMU that the simulation run is terminated
// --> let libsumo know, that we want to close the simulation
fmi2Status
fmi2Terminate(fmi2Component c) {
    UNREFERENCED_PARAMETER(c);

    libsumo_close();
    return fmi2OK;
}

// Is called by the environment to reset the FMU after a simulation run
fmi2Status
fmi2Reset(fmi2Component c) {
    UNREFERENCED_PARAMETER(c);

    // Should we set some start values?
    return fmi2OK;
}

// Implementation of the getter features
fmi2Status
fmi2GetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(vr);
    UNREFERENCED_PARAMETER(nvr);
    UNREFERENCED_PARAMETER(value);

    return fmi2Error;
}

fmi2Status
fmi2GetInteger(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]) {

    ModelInstance *comp = (ModelInstance *)c;

    // Check for null pointer errors
    if (nvr > 0 && (!vr || !value)) {
        return fmi2Error;
    }

    fmi2Status status = fmi2OK;

    // Go through the list of arrays and save all requested values
    size_t i;
    for (i = 0; i < nvr; i++) {
        fmi2Status s = sumo2fmi_getInteger(comp, vr[i], &(value[i]));
        status = s > status ? s : status;

        if (status > fmi2Warning) {
            return status;
        }
    }

    return status;
}

fmi2Status
fmi2GetBoolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(vr);
    UNREFERENCED_PARAMETER(nvr);
    UNREFERENCED_PARAMETER(value);

    return fmi2Error;
}

fmi2Status
fmi2GetString(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2String value[]) {

    ModelInstance *comp = (ModelInstance *)c;

    // Check for null pointer errors
    if (nvr > 0 && (!vr || !value)) {
        return fmi2Error;
    }

    fmi2Status status = fmi2OK;

    /** Clear value array buffer before reuse */
    int b;
    for (b = 0; b < comp->bufferArrayLength; b++) {
        comp->freeMemory((void *)comp->bufferArray[b]);
    }
    comp->freeMemory((void *)comp->bufferArray);
    comp->bufferArray = (fmi2String *)comp->allocateMemory(nvr, sizeof(fmi2String));
    comp->bufferArrayLength = (int)nvr;

    // Go through the list of arrays and save all requested values
    size_t i;
    for (i = 0; i < nvr; i++) {
        fmi2Status s = sumo2fmi_getString(comp, vr[i], &(comp->bufferArray[i]));
        value[i] = comp->bufferArray[i];
        if (value[i] == NULL) {
            s = fmi2Error;
        }

        status = s > status ? s : status;
        if (status > fmi2Warning) {
            return status;
        }
    }

    return status;
}

// Implementation of the setter features
fmi2Status
fmi2SetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(vr);
    UNREFERENCED_PARAMETER(nvr);
    UNREFERENCED_PARAMETER(value);
    return fmi2Error;
}

fmi2Status
fmi2SetInteger(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(vr);
    UNREFERENCED_PARAMETER(nvr);
    UNREFERENCED_PARAMETER(value);

    return fmi2Error;
}

fmi2Status
fmi2SetBoolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(vr);
    UNREFERENCED_PARAMETER(nvr);
    UNREFERENCED_PARAMETER(value);

    return fmi2Error;
}

fmi2Status
fmi2SetString(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String value[]) {

    ModelInstance *comp = (ModelInstance *)c;
    fmi2Status status = fmi2OK;

    size_t i;
    for (i = 0; i < nvr; i++) {
        fmi2Status s = sumo2fmi_setString(comp, vr[i], value[i]);
        status = s > status ? s : status;
        if (status > fmi2Warning) return status;
    }

    return status;
}

fmi2Status
fmi2GetFMUstate(fmi2Component c, fmi2FMUstate* FMUstate) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(FMUstate);
    return fmi2Error; /* Dummy implementation */
}

fmi2Status
fmi2SetFMUstate(fmi2Component c, fmi2FMUstate FMUstate) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(FMUstate);
    return fmi2Error; /* Dummy implementation */
}

fmi2Status
fmi2FreeFMUstate(fmi2Component c, fmi2FMUstate* FMUstate) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(FMUstate);
    return fmi2Error; /* Dummy implementation */
}

fmi2Status
fmi2SerializedFMUstateSize(fmi2Component c, fmi2FMUstate FMUstate, size_t* size) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(FMUstate);
    UNREFERENCED_PARAMETER(size);
    return fmi2Error; /* Dummy implementation */
}

fmi2Status
fmi2SerializeFMUstate(fmi2Component c, fmi2FMUstate FMUstate, fmi2Byte state[], size_t size) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(FMUstate);
    UNREFERENCED_PARAMETER(state);
    UNREFERENCED_PARAMETER(size);
    return fmi2Error; /* Dummy implementation */
}

fmi2Status
fmi2DeSerializeFMUstate(fmi2Component c, const fmi2Byte serializedState[], size_t size, fmi2FMUstate* FMUstate) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(serializedState);
    UNREFERENCED_PARAMETER(size);
    UNREFERENCED_PARAMETER(FMUstate);
    return fmi2Error; /* Dummy implementation */
}

fmi2Status
fmi2GetDirectionalDerivative(fmi2Component c, const fmi2ValueReference vUnknown_ref[], size_t nUnknown,
                             const fmi2ValueReference vKnown_ref[], size_t nKnown, const fmi2Real dvKnown[], fmi2Real dvUnknown[]) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(vUnknown_ref);
    UNREFERENCED_PARAMETER(nUnknown);
    UNREFERENCED_PARAMETER(vKnown_ref);
    UNREFERENCED_PARAMETER(nKnown);
    UNREFERENCED_PARAMETER(dvKnown);
    UNREFERENCED_PARAMETER(dvUnknown);
    return fmi2Error; /* Dummy implementation */
}

/* Further functions for interpolation */
fmi2Status
fmi2SetRealInputDerivatives(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer order[], const fmi2Real value[]) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(vr);
    UNREFERENCED_PARAMETER(nvr);
    UNREFERENCED_PARAMETER(order);
    UNREFERENCED_PARAMETER(value);

    return fmi2Error; /* Ignoring - SUMO cannot interpolate inputs */
}

fmi2Status
fmi2GetRealOutputDerivatives(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer order[], fmi2Real value[]) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(vr);
    UNREFERENCED_PARAMETER(order);

    size_t i;
    for (i = 0; i < nvr; i++) {
        value[i] = 0;    /* We cannot compute derivatives of outputs */
    }
    return fmi2Error;
}

/* Stepping */
fmi2Status
fmi2DoStep(fmi2Component c, fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize, fmi2Boolean noSetFMUStatePriorToCurrentPoint) {
    UNREFERENCED_PARAMETER(noSetFMUStatePriorToCurrentPoint);

    ModelInstance *comp = (ModelInstance *)c;

    if (communicationStepSize <= 0) {
        return fmi2Error;
    }

    return sumo2fmi_step(comp, currentCommunicationPoint + communicationStepSize);
}

fmi2Status
fmi2CancelStep(fmi2Component c) {
    UNREFERENCED_PARAMETER(c);

    return fmi2Error; /* We will never have a modelStepInProgress state */
}

/* Status functions */
fmi2Status
fmi2GetStatus(fmi2Component c, const fmi2StatusKind s, fmi2Status *value) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(s);
    UNREFERENCED_PARAMETER(value);

    return fmi2Discard;
}

fmi2Status
fmi2GetRealStatus(fmi2Component c, const fmi2StatusKind s, fmi2Real *value) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(s);
    UNREFERENCED_PARAMETER(value);

    return fmi2Discard;
}

fmi2Status
fmi2GetIntegerStatus(fmi2Component c, const fmi2StatusKind s, fmi2Integer *value) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(s);
    UNREFERENCED_PARAMETER(value);

    return fmi2Discard;
}

fmi2Status
fmi2GetBooleanStatus(fmi2Component c, const fmi2StatusKind s, fmi2Boolean *value) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(s);
    UNREFERENCED_PARAMETER(value);

    return fmi2Discard;
}

fmi2Status
fmi2GetStringStatus(fmi2Component c, const fmi2StatusKind s, fmi2String *value) {
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(s);
    UNREFERENCED_PARAMETER(value);

    return fmi2Discard;
}
