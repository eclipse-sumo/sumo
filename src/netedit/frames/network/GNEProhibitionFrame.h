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
/// @file    GNEProhibitionFrame.h
/// @author  Mirko Barthauer (Technische Universitaet Braunschweig)
/// @date    May 2018
///
// The Widget for remove network-elements
/****************************************************************************/
#pragma once
#include <config.h>
#include <netedit/frames/GNEFrame.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEConnection;
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEProhibitionFrame
 * The Widget for editing connection foes
 */
class GNEProhibitionFrame : public GNEFrame {

public:

    /// @brief the prohibition status of a connection
    enum ConnStatus {
        UNDEFINED,
        PROHIBITED,
        PROHIBITING
    };

    // ===========================================================================
    // class RelativeToConnection
    // ===========================================================================

    class RelativeToConnection : public MFXGroupBoxModule {

    public:
        /// @brief constructor
        RelativeToConnection(GNEProhibitionFrame* prohibitionFrameParent);

        /// @brief destructor
        ~RelativeToConnection();

        /// @brief update description
        void updateDescription() const;

    private:
        /// @brief pointer to prohibition frame parent
        GNEProhibitionFrame* myProhibitionFrameParent;

        /// @brief the label that shows the currently selected connection
        FXLabel* myConnDescriptionLabel;
    };

    // ===========================================================================
    // class Legend
    // ===========================================================================

    class Legend : public MFXGroupBoxModule {

    public:
        /// @brief constructor
        Legend(GNEProhibitionFrame* prohibitionFrameParent);

        /// @brief destructor
        ~Legend();

        /// @brief get color for non-conflicting pairs of connections
        const RGBColor& getUndefinedColor() const;

        /// @brief get color for waiting connections
        const RGBColor& getProhibitedColor() const;

        /// @brief get color for connections with precedence
        const RGBColor& getProhibitingColor() const;

        /// @brief get color for unregulated conflicts
        const RGBColor& getUnregulatedConflictColor() const;

        /// @brief get color for mutual conflicts
        const RGBColor& getMutualConflictColor() const;

    private:
        /// @brief color for non-conflicting pairs of connections
        RGBColor myUndefinedColor;

        /// @brief color for waiting connections
        RGBColor myProhibitedColor;

        /// @brief color for connections with precedence
        RGBColor myProhibitingColor;

        /// @brief color for unregulated conflicts
        RGBColor myUnregulatedConflictColor;

        /// @brief color for mutual conflicts
        RGBColor myMutualConflictColor;
    };

    // ===========================================================================
    // class Selection (in the future will be "Modification")
    // ===========================================================================

    class Selection : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEProhibitionFrame::Selection)

    public:
        /// @brief constructor
        Selection(GNEProhibitionFrame* prohibitionFrameParent);

        /// @brief destructor
        ~Selection();

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when the user presses the OK-Button saves any prohibition modifications
        long onCmdOK(FXObject*, FXSelector, void*);

        /// @brief Called when the user presses the Cancel-button discards any prohibition modifications
        long onCmdCancel(FXObject*, FXSelector, void*);

        /// @brief Called when the user presses the OK-Button saves any prohibition modifications
        long onCmdRequireConnection(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(Selection)

    private:
        /// @brief pointer to prohibition frame parent
        GNEProhibitionFrame* myProhibitionFrameParent = nullptr;

        /// @brief "Save" button
        MFXButtonTooltip* mySaveButton = nullptr;

        /// @brief "Cancel" button
        MFXButtonTooltip* myCancelButton = nullptr;
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEProhibitionFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEProhibitionFrame();

    /**@brief handle prohibitions and set the relative coloring
     * @param viewObjects collection of objects under cursor after click over view
     */
    void handleProhibitionClick(const GNEViewNetHelper::ViewObjectsSelector& viewObjects);

    /// @brief show prohibition frame
    void show();

    /// @brief hide prohibition frame
    void hide();

    /// @brief get selection module
    GNEProhibitionFrame::Selection* getSelectionModul() const;

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEProhibitionFrame)

    /// @brief build prohibition
    void buildProhibition(GNEConnection* conn, bool mayDefinitelyPass, bool allowConflict, bool toggle);

private:
    /// @brief Relative To Connection
    RelativeToConnection* myRelativeToConnection;

    /// @brief prohibition legend
    GNEProhibitionFrame::Legend* myLegend = nullptr;

    /// @brief Selection
    GNEProhibitionFrame::Selection* mySelectionModul = nullptr;

    /// @brief the connection which prohibits
    GNEConnection* myCurrentConn = nullptr;

    /// @brief the set of connections which
    std::set<GNEConnection*> myConcernedConns;
};
