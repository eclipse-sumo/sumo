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
/// @file    GNESelectorParent.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Frame for select parents
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNESelectorParent : public MFXGroupBoxModule {

public:
    /// @brief constructor
    GNESelectorParent(GNEFrame* frameParent);

    /// @brief destructor
    ~GNESelectorParent();

    /// @brief get currently parent additional selected
    std::string getIdSelected() const;

    /// @brief select manually a element of the list
    void setIDSelected(const std::string& id);

    /// @brief Show list of GNESelectorParent Module
    void showSelectorParentModule(const std::vector<SumoXMLTag>& parentTags);

    /// @brief hide GNESelectorParent Module
    void hideSelectorParentModule();

    /// @brief Refresh list of Additional Parents Module
    void refreshSelectorParentModule();

private:
    /// @brief pointer to Frame Parent
    GNEFrame* myFrameParent;

    /// @brief current parent additional tag
    std::vector<SumoXMLTag> myParentTags;

    /// @brief Label with parent name
    FXLabel* myParentsLabel;

    /// @brief List of parents
    FXList* myParentsList;
};
