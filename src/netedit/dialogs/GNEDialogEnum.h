/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEDialogEnum.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Enums for list all dialog types (used for Internal tests)
/****************************************************************************/
#pragma once
#include <config.h>

// ===========================================================================
// enumeration
// ===========================================================================

enum class DialogType {
    ABOUT,
    ATTRIBUTECARRIER,
    BASIC_ERROR,
    BASIC_HELP,
    BASIC_INFORMATION,
    BASIC_WARNING,
    CALIBRATOR,
    COLOR,
    FILE,
    FILEPATH,
    FIX_ADDITIONALELEMENTS,
    FIX_DEMANDELEMENTS,
    FIX_NETWORKELEMENTS,
    GEOMETRYPOINT,
    NETGENERATE,
    OPTIONS_NETEDIT,
    OPTIONS_NETGENERATE,
    OPTIONS_SUMO,
    OVERWRITE,
    PARAMETERS,
    PYTHON,
    QUESTION,
    SAVE,
    REROUTER,
    REROUTERINTERVAL,
    RUN,
    UNDOLIST,
    VCLASS,
    VSS,
    VTYPE,
    DEFAULT
};
