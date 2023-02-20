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
/// @file    sumo2fmi_bridge.h
/// @author  Robert Hilbrich
/// @author  Matthias Schwamborn
/// @date    Mon, 24 Aug 2020
///
// Declarations for the logic and data strcutures for the SUMO to FMI bridge
/****************************************************************************/

#ifndef SUMO2FMI_BRIDGE_H
#define SUMO2FMI_BRIDGE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <foreign/fmi/fmi2FunctionTypes.h>
#include <foreign/fmi/fmi2TypesPlatform.h>

/* Type definitions for callback functions */
typedef void* (*allocateMemoryType)(size_t nobj, size_t size);
typedef void (*loggerType)(void* componentEnvironment, const char* instanceName, int status, const char* category, const char* message, ...);
typedef void (*freeMemoryType)(void* obj);

/* Several declarations for the model component (housekeeping stuff) */
typedef struct {
    void* componentEnvironment;
    const char* instanceName;
    const char* resourceLocation;

    loggerType logger;
    allocateMemoryType allocateMemory;
    freeMemoryType freeMemory;

    double startTime;
    double stopTime;

    char* libsumoCallOptions;

    /**
     * @brief Parameters stored for the next (libsumo) getter call.
     *        Workaround for FMIv2 not allowing input values for an output
     *        model variable (see modelDescription.xml).
     */
    char* getterParameters;

    /* Buffer for string value array used in fmi2GetString(). */
    fmi2String* bufferArray;
    int bufferArrayLength;

    bool logEvents;
    bool logErrors;
} ModelInstance;

/* Declarations of utility functions */
void        sumo2fmi_logEvent(ModelInstance* comp, const char* message, ...);
void        sumo2fmi_logError(ModelInstance* comp, const char* message, ...);
void        sumo2fmi_logMessage(ModelInstance* comp, int status, const char* category, const char* message, va_list args);

/* Getter/Setter Functions */
fmi2Status  sumo2fmi_getInteger(ModelInstance* comp, const fmi2ValueReference vr, int* value);
fmi2Status  sumo2fmi_getString(ModelInstance* comp, const fmi2ValueReference vr, fmi2String* value);
fmi2Status  sumo2fmi_setString(ModelInstance* comp, fmi2ValueReference vr, fmi2String value);

/* Stepping Functions */
fmi2Status  sumo2fmi_step(ModelInstance* comp, double tNext);

/* Setting the start values for all parameters */
void        sumo2fmi_set_startValues(ModelInstance* comp);

#endif /* SUMO2FMI_BRIDGE_H */
