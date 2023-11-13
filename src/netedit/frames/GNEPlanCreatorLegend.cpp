/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GNEPlanCreatorLegend.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for path legends
/****************************************************************************/
#include <config.h>

#include <netedit/GNEViewNet.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEPlanCreatorLegend.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEPlanCreatorLegend::GNEPlanCreatorLegend(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Information")) {
    // declare label
    FXLabel* legendLabel = nullptr;
    // edge candidate
    legendLabel = new FXLabel(getCollapsableFrame(), TL(" Valid element"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    legendLabel->setBackColor(MFXUtils::getFXColor(RGBColor::ORANGE));
    // last edge selected
    legendLabel = new FXLabel(getCollapsableFrame(), TL(" From element"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    legendLabel->setBackColor(MFXUtils::getFXColor(RGBColor::GREEN));
    // edge selected
    legendLabel = new FXLabel(getCollapsableFrame(), TL(" To element"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    legendLabel->setBackColor(MFXUtils::getFXColor(RGBColor::MAGENTA));
}


GNEPlanCreatorLegend::~GNEPlanCreatorLegend() {}


void
GNEPlanCreatorLegend::showPlanCreatorLegend() {
    show();
}

void
GNEPlanCreatorLegend::hidePlanCreatorLegend() {
    hide();
}

/****************************************************************************/
