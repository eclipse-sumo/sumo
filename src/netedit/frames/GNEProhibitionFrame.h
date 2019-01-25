/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEProhibitionFrame.h
/// @author  Mirko Barthauer (Technische Universitaet Braunschweig)
/// @date    May 2018
/// @version $Id$
///
// The Widget for remove network-elements
/****************************************************************************/
#ifndef GNEProhibitionFrame_h
#define GNEProhibitionFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GNEFrame.h"

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

    /// @brief FOX-declaration
    FXDECLARE(GNEProhibitionFrame)

public:

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEProhibitionFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEProhibitionFrame();

    /**@brief handle prohibitions and set the relative colouring
     * @param objectsUnderCursor collection of objects under cursor after click over view
     */
    void handleProhibitionClick(const GNEViewNet::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief show prohibition frame
    void show();

    /// @brief hide prohibition frame
    void hide();

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user presses the OK-Button saves any prohibition modifications
    long onCmdOK(FXObject*, FXSelector, void*);

    /// @brief Called when the user presses the Cancel-button discards any prohibition modifications
    long onCmdCancel(FXObject*, FXSelector, void*);

protected:
    /// @brief FOX needs this
    GNEProhibitionFrame() {}

private:
    /// @brief the prohibition status of a connection
    enum ConnStatus {
        UNDEFINED,
        PROHIBITED,
        PROHIBITING
    };

    /// @brief build prohibition
    void buildProhibition(GNEConnection* conn, bool mayDefinitelyPass, bool allowConflict, bool toggle);

    /// @brief Groupbox for description
    FXGroupBox* myGroupBoxDescription;

    /// @brief the label that shows the currently selected connection
    FXLabel* myConnDescriptionLabel;

    /// @brief group box for legend
    FXGroupBox* myGroupBoxLegend;

    /// @brief selected connection label
    FXLabel* mySelectedLabel;

    /// @brief "undefined" yielding label
    FXLabel* myUndefinedLabel;

    /// @brief "prohibited" label
    FXLabel* myProhibitedLabel;

    /// @brief "prohibiting" label
    FXLabel* myProhibitingLabel;

    /// @brief "Cancel" button
    FXButton* myCancelButton;

    /// @brief "OK" button
    //FXButton* mySaveButton;

    /// @brief the connection which prohibits
    GNEConnection* myCurrentConn;

    /// @brief the set of connections which
    std::set<GNEConnection*> myConcernedConns;

    /// @brief color for selected connection whose prohibition shall be shown
    static RGBColor selectedColor;

    /// @brief color for non-conflicting pairs of connections
    static RGBColor undefinedColor;

    /// @brief color for waiting connections
    static RGBColor prohibitedColor;

    /// @brief color for connections with precedence
    static RGBColor prohibitingColor;

    /// @brief color for unregulated conflicts
    static RGBColor unregulatedConflictColor;

    /// @brief color for mutual conflicts
    static RGBColor mutualConflictColor;

    /// @brief update description
    void updateDescription() const;
};


#endif

/****************************************************************************/

