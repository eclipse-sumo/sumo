/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEAdditionalFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// The Widget for add additional elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <netedit/GNEViewNetHelper.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributesEditor;
class GNEConsecutiveSelector;
class GNESelectorParent;
class GNETagSelector;
class GNEViewObjectSelector;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEAdditionalFrame : public GNEFrame {

public:
    // ===========================================================================
    // class EdgeTypeSelector
    // ===========================================================================

    class E2MultilaneLegendModule : public GNEGroupBoxModule {

    public:
        /// @brief constructor
        E2MultilaneLegendModule(GNEFrame* frameParent);

        /// @brief destructor
        ~E2MultilaneLegendModule();

        /// @brief show Legend modul
        void showE2MultilaneLegend();

        /// @brief hide Legend modul
        void hideE2MultilaneLegend();
    };

    // ===========================================================================
    // class HelpCreationModule
    // ===========================================================================

    class HelpCreationModule : public GNEGroupBoxModule {

    public:
        /// @brief constructor
        HelpCreationModule(GNEFrame* frameParent);

        /// @brief destructor
        ~HelpCreationModule();

        /// @brief show Legend modul
        void showHelpCreationModule(SumoXMLTag XMLTag);

        /// @brief hide Legend modul
        void hideHelpCreationModule();

    private:
        /// @brief add translated string
        std::string addTLString(const std::string& str);

        /// @brief help label
        FXLabel* myHelpLabel = nullptr;

        /// @brief map with tags and their associated help
        std::map<SumoXMLTag, std::string> myHelpMap;
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEAdditionalFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEAdditionalFrame();

    /// @brief show Frame
    void show();

    /**@brief add additional element
     * @param viewObjects collection of objects under cursor after click over view
     * @return true if additional was successfully added
     */
    bool addAdditional(const GNEViewNetHelper::ViewObjectsSelector& viewObjects);

    /// @brief get additional tag selector
    GNETagSelector* getAdditionalTagSelector() const;

    /// @brief get consecutive lane selector
    GNEConsecutiveSelector* getConsecutiveLaneSelector() const;

    /// @brief get attribtues editor
    GNEAttributesEditor* getAttributesEditor() const;

    /// @brief get module for select view objects
    GNEViewObjectSelector* getViewObjetsSelector() const;

    /// @brief create path
    bool createPath(const bool useLastRoute);

protected:
    /// @brief SumoBaseObject used for create additional
    CommonXMLStructure::SumoBaseObject* myBaseAdditional = nullptr;

    /// @brief Tag selected in GNETagSelector
    void tagSelected();

    /// @brief last clicked position
    Position myLastClickedPosition;

private:
    // @brief reste base additional object
    void resetBaseAdditionalObject();

    // @brief init baseAdditionalObject
    bool initBaseAdditionalObject(const GNETagProperties* tagProperty, const GNEViewNetHelper::ViewObjectsSelector& viewObjects);

    /// @brief item selector
    GNETagSelector* myAdditionalTagSelector = nullptr;

    /// @brief additional attributes editor
    GNEAttributesEditor* myAdditionalAttributesEditor = nullptr;

    /// @brief Module for select a single parent additional
    GNESelectorParent* mySelectorAdditionalParent = nullptr;

    /// @brief Module for select view objects
    GNEViewObjectSelector* myViewObjetsSelector = nullptr;

    /// @brief Module for select consecutive lanes
    GNEConsecutiveSelector* myConsecutiveLaneSelector = nullptr;

    /// @brief Module for show additional help legend
    HelpCreationModule* myHelpCreationModule = nullptr;

    /// @brief Module for show E2 Detector legend
    E2MultilaneLegendModule* myE2MultilaneLegendModule = nullptr;
};
