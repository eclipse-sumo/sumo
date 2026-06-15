/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2026 German Aerospace Center (DLR) and others.
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
/// @file    DialogType.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// Enum of dialog types used by the internal test system to identify and
// drive dialogs from the netedit GUI.  Lives in utils/tests because it
// forms the (test-driver <-> GUI dialog) contract; netedit includes it
// from here so that the test layer does not have to depend on netedit.
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
    DISTRIBUTION_REF,
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
    REROUTER,
    REROUTERINTERVAL,
    RUN,
    SAVE,
    UNDOLIST,
    VCLASS,
    VSS,
    VTYPE,
    DEFAULT
};
