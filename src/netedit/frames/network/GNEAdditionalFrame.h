/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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

#include <netedit/frames/GNEAttributesCreator.h>
#include <netedit/frames/GNEConsecutiveSelector.h>
#include <netedit/frames/GNEFrame.h>
#include <netedit/frames/GNENeteditAttributes.h>
#include <netedit/frames/GNENetworkSelector.h>
#include <netedit/frames/GNESelectorParent.h>
#include <netedit/frames/GNETagSelector.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEAdditionalFrame
 * The Widget for setting internal attributes of additional elements
 */
class GNEAdditionalFrame : public GNEFrame {

public:
    // ===========================================================================
    // class EdgeTypeSelector
    // ===========================================================================

    class E2MultilaneLegendModule : public MFXGroupBoxModule {

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

    class HelpCreationModule : public MFXGroupBoxModule {

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

    /// @brief get edges selector
    GNENetworkSelector* getEdgesSelector() const;

    /// @brief get edges selector
    GNENetworkSelector* getLanesSelector() const;

    /// @brief get consecutive lane selector
    GNEConsecutiveSelector* getConsecutiveLaneSelector() const;

    /// @brief get Netedit parameter
    GNENeteditAttributes* getNeteditAttributes() const;

    /// @brief create path
    bool createPath(const bool useLastRoute);

protected:
    /// @brief SumoBaseObject used for create additional
    CommonXMLStructure::SumoBaseObject* myBaseAdditional = nullptr;

    /// @brief Tag selected in GNETagSelector
    void tagSelected();

private:
    // @brief create baseAdditionalObject
    bool createBaseAdditionalObject(const GNETagProperties& tagProperty);

    /// @brief build common additional attributes
    bool buildAdditionalCommonAttributes(const GNETagProperties& tagValues);

    /// @brief build additional over an edge (parent of lane)
    bool buildAdditionalOverEdge(GNELane* lane, const GNETagProperties& tagValues);

    /// @brief build additional over a single lane
    bool buildAdditionalOverLane(GNELane* lane, const GNETagProperties& tagValues);

    /// @brief build additional over view
    bool buildAdditionalOverView(const GNETagProperties& tagValues);

    /// @brief item selector
    GNETagSelector* myAdditionalTagSelector = nullptr;

    /// @brief internal additional attributes
    GNEAttributesCreator* myAdditionalAttributes = nullptr;

    /// @brief Netedit parameter
    GNENeteditAttributes* myNeteditAttributes = nullptr;

    /// @brief Module for select a single parent additional
    GNESelectorParent* mySelectorAdditionalParent = nullptr;

    /// @brief Module for select edges
    GNENetworkSelector* myEdgesSelector = nullptr;

    /// @brief Module for select lanes
    GNENetworkSelector* myLanesSelector = nullptr;

    /// @brief Module for select consecutive lanes
    GNEConsecutiveSelector* myConsecutiveLaneSelector = nullptr;

    /// @brief Module for show additional help legend
    HelpCreationModule* myHelpCreationModule = nullptr;

    /// @brief Module for show E2 Detector legend
    E2MultilaneLegendModule* myE2MultilaneLegendModule = nullptr;
};
