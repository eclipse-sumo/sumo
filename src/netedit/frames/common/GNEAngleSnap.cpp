/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEAngleSnap.cpp
/// @author  Josh
/// @date    2026-11-26
///
/****************************************************************************/
#include <config.h>

#include <cmath>

#include <utils/common/StringUtils.h>
#include <utils/common/Translation.h>
#include <utils/common/UtilExceptions.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEAngleSnap.h"


FXDEFMAP(GNEAngleSnap) GNEAngleSnapMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_ATTRIBUTE, GNEAngleSnap::onCmdChange),
    FXMAPFUNC(SEL_CHANGED, MID_GNE_SET_ATTRIBUTE, GNEAngleSnap::onCmdChange)
};
FXIMPLEMENT(GNEAngleSnap, FXVerticalFrame, GNEAngleSnapMap, ARRAYNUMBER(GNEAngleSnapMap))


GNEAngleSnap::GNEAngleSnap(FXComposite* parent) :
    FXVerticalFrame(parent, GUIDesignAuxiliarVerticalFrame) {
    GUIDesigns::buildFXLabel(this, TL("Angle snap"), "", "", nullptr, GUIDesignLabel(JUSTIFY_LEFT));
    myMode = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxStaticExtended);
    myMode->appendItem(TL("Off"));
    myMode->appendItem(TL("Horizontal or vertical"));
    myMode->appendItem(TL("Custom angle"));
    myMode->setNumVisible(3);
    myMode->setCurrentItem(0);
    myDegrees = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myDegrees->setText("45");
    myDegrees->setTipText(TL("Degrees counterclockwise from east"));
    myDegrees->disable();
}


bool
GNEAngleSnap::isEnabled() const {
    return myMode->getCurrentItem() != 0;
}


Position
GNEAngleSnap::project(const Position& anchor, const Position& target) const {
    if (!isEnabled()) {
        return target;
    }
    const Position delta = target - anchor;
    if (myMode->getCurrentItem() == 1) {
        return std::abs(delta.x()) >= std::abs(delta.y()) ?
               Position(target.x(), anchor.y(), target.z()) :
               Position(anchor.x(), target.y(), target.z());
    }
    const auto direction = shortEdgeOffset(1);
    const double distance = delta.x() * direction.x() + delta.y() * direction.y();
    return Position(anchor.x() + direction.x() * distance, anchor.y() + direction.y() * distance, target.z());
}


Position
GNEAngleSnap::shortEdgeOffset(double length) const {
    const double radians = myMode->getCurrentItem() == 2 ? DEG2RAD(myAngle) : 0;
    return Position(length * std::cos(radians), length * std::sin(radians));
}


long
GNEAngleSnap::onCmdChange(FXObject*, FXSelector, void*) {
    if (myMode->getCurrentItem() == 2) {
        myDegrees->enable();
        try {
            const double angle = StringUtils::toDouble(myDegrees->getText().text());
            if (std::isfinite(angle)) {
                myAngle = std::fmod(angle, 360);
                myDegrees->setTextColor(GUIDesignTextColorBlack);
                return 1;
            }
        } catch (const NumberFormatException&) {
        } catch (const EmptyData&) {
        }
        myDegrees->setTextColor(GUIDesignTextColorRed);
    } else {
        myDegrees->disable();
    }
    return 1;
}
