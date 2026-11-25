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
/// @file    GNEAngleSnap.h
/// @author  Josh
/// @date    2026-11-26
///
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>
#include <utils/geom/Position.h>


// An opt-in planar constraint for Create Edge and Network Move.
class GNEAngleSnap : public FXVerticalFrame {
    FXDECLARE(GNEAngleSnap)

public:
    explicit GNEAngleSnap(FXComposite* parent);

    bool isEnabled() const;
    Position project(const Position& anchor, const Position& target) const;
    Position shortEdgeOffset(double length) const;

    long onCmdChange(FXObject*, FXSelector, void*);

protected:
    GNEAngleSnap() = default;

private:
    FXComboBox* myMode = nullptr;
    FXTextField* myDegrees = nullptr;
    double myAngle = 45;
};
