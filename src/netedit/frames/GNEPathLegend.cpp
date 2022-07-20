/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEPathLegend.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for path legends
/****************************************************************************/
#include <config.h>

#include <netedit/GNEViewNet.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEPathLegend.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEM_PathLegend::GNEM_PathLegend(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, "Information") {
    // declare label
    FXLabel* legendLabel = nullptr;
    // edge candidate
    legendLabel = new FXLabel(getCollapsableFrame(), " edge candidate", 0, GUIDesignLabelLeft);
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.possible));
    legendLabel->setTextColor(MFXUtils::getFXColor(RGBColor::WHITE));
    // last edge selected
    legendLabel = new FXLabel(getCollapsableFrame(), " last edge selected", 0, GUIDesignLabelLeft);
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.target));
    // edge selected
    legendLabel = new FXLabel(getCollapsableFrame(), " edge selected", 0, GUIDesignLabelLeft);
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.source));
    // edge conflict (vClass)
    legendLabel = new FXLabel(getCollapsableFrame(), " edge conflict (vClass)", 0, GUIDesignLabelLeft);
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.special));
    // edge disconnected
    legendLabel = new FXLabel(getCollapsableFrame(), " edge disconnected", 0, GUIDesignLabelLeft);
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.conflict));
}


GNEM_PathLegend::~GNEM_PathLegend() {}


void
GNEM_PathLegend::showPathLegendModule() {
    show();
}

void
GNEM_PathLegend::hidePathLegendModule() {
    hide();
}

/****************************************************************************/
