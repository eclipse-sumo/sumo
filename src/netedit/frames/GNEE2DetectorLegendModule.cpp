/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEE2DetectorLegendModule.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2023
///
// Frame for E2 detector legend
/****************************************************************************/
#include <config.h>

#include <netedit/GNEViewNet.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEE2DetectorLegendModule.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEE2DetectorLegendModule::GNEE2DetectorLegendModule(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Information")) {
    // declare label
    FXLabel* legendLabel = nullptr;
    // edge candidate
    legendLabel = new FXLabel(getCollapsableFrame(), TL(" edge candidate"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.possible));
    legendLabel->setTextColor(MFXUtils::getFXColor(RGBColor::WHITE));
    // last edge selected
    legendLabel = new FXLabel(getCollapsableFrame(), TL(" last edge selected"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.target));
    // edge selected
    legendLabel = new FXLabel(getCollapsableFrame(), TL(" edge selected"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.source));
    // edge disconnected
    legendLabel = new FXLabel(getCollapsableFrame(), TL(" edge disconnected"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.conflict));
}


GNEE2DetectorLegendModule::~GNEE2DetectorLegendModule() {}


void
GNEE2DetectorLegendModule::showE2DetectorLegend() {
    show();
}


void
GNEE2DetectorLegendModule::hideE2DetectorLegend() {
    hide();
}

/****************************************************************************/
