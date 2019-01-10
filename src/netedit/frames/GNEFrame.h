/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// Abstract class for lateral frames in NetEdit
/****************************************************************************/
#ifndef GNEFrame_h
#define GNEFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBTrafficLightLogic.h>
#include <utils/gui/div/GUISelectedStorage.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <netedit/GNEAttributeCarrier.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEViewNet;
class GNEUndoList;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEFrame
 * Abstract class for lateral frames in NetEdit
 */
class GNEFrame : public FXVerticalFrame {

public:

    // ===========================================================================
    // class ACHierarchy
    // ===========================================================================

    class ACHierarchy : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrame::ACHierarchy)

    public:
        /// @brief constructor
        ACHierarchy(GNEFrame* frameParent);

        /// @brief destructor
        ~ACHierarchy();

        /// @brief show ACHierarchy
        void showACHierarchy(GNEAttributeCarrier* AC);

        /// @brief hide ACHierarchy
        void hideACHierarchy();

        /// @brief refresh ACHierarchy
        void refreshACHierarchy();

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user press right click over an item of list of childs
        long onCmdShowChildMenu(FXObject*, FXSelector, void* data);

        /// @brief called when user select option "center item" of child Menu
        long onCmdCenterItem(FXObject*, FXSelector, void*);

        /// @brief called when user select option "inspect item" of child menu
        long onCmdInspectItem(FXObject*, FXSelector, void*);

        /// @brief called when user select option "delte item" of child menu
        long onCmdDeleteItem(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        ACHierarchy() {}

        // @brief create pop-up menu in the positions X-Y for the attribute carrier ac
        void createPopUpMenu(int X, int Y, GNEAttributeCarrier* ac);

        /// @brief show child of current attributeCarrier
        FXTreeItem* showAttributeCarrierParents();

        /// @brief show child of current attributeCarrier
        void showAttributeCarrierChilds(GNEAttributeCarrier* AC, FXTreeItem* itemParent);

        /// @brief add item into list
        FXTreeItem* addACIntoList(GNEAttributeCarrier* AC, FXTreeItem* itemParent);

    private:
        /// @brief Frame Parent
        GNEFrame* myFrameParent;

        /// @brief Attribute carrier
        GNEAttributeCarrier* myAC;

        /// @brief tree list to show the childs of the element to erase
        FXTreeList* myTreelist;

        /// @brief map used to save the Tree items with their AC
        std::map<FXTreeItem*, GNEAttributeCarrier*> myTreeItemToACMap;

        /// @brief set used to save tree items without AC assigned, the Incoming/Outcoming connections
        std::set<FXTreeItem*> myTreeItemsConnections;

        /// @brief pointer to current right clicked Attribute Carrier
        GNEAttributeCarrier* myRightClickedAC;
    };

    // ===========================================================================
    // class GenericParametersEditor
    // ===========================================================================

    class GenericParametersEditor : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrame::GenericParametersEditor)

    public:
        /// @brief constructor
        GenericParametersEditor(GNEFrame* frameParent);

        /// @brief destructor
        ~GenericParametersEditor();

        /// @brief show netedit attributes editor (used for edit generic parameters of an existent AC)
        void showGenericParametersEditor(GNEAttributeCarrier* AC);

        /// @brief show netedit attributes editor (used for edit generic parameters of an existent list of AC)
        void showGenericParametersEditor(std::vector<GNEAttributeCarrier*> ACs);

        /// @brief hide netedit attributes editor
        void hideGenericParametersEditor();

        /// @brief refresh netedit attributes
        void refreshGenericParametersEditor();

        /// @brief get generic parameters as string
        std::string getGenericParametersStr() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user clicks over add generic parameter
        long onCmdEditGenericParameter(FXObject*, FXSelector, void*);

        /// @brief Called when user udpate the generic parameter text field
        long onCmdSetGenericParameter(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        GenericParametersEditor() {}

    private:
        /// @brief edited Attribute Carrier
        GNEAttributeCarrier* myAC;

        /// @brief list of edited ACs
        std::vector<GNEAttributeCarrier*> myACs;

        /// @brief pointer to current vector of generic parameters
        std::vector<std::pair<std::string, std::string> >* myGenericParameters;

        /// @brief pointer to inspector frame parent
        GNEFrame* myFrameParent;

        /// @brief text field for write generic parameter
        FXTextField* myTextFieldGenericParameter;

        /// @brief button for add generic parameter
        FXButton* myEditGenericParameterButton;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     * @brief frameLabel label of the frame
     */
    GNEFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet, const std::string& frameLabel);

    /// @brief destructor
    ~GNEFrame();

    /// @brief focus upper element of frame
    void focusUpperElement();

    /**@brief show Frame
     * @note some GNEFrames needs a re-implementation
     */
    virtual void show();

    /**@brief hide Frame
     * @note some GNEFrames needs a re-implementation
     */
    virtual void hide();

    /// @brief set width of GNEFrame
    void setFrameWidth(int newWidth);

    /// @brief get view net
    GNEViewNet* getViewNet() const;

    /// @brief get the label for the frame's header
    FXLabel* getFrameHeaderLabel() const;

    /// @brief get font of the header's frame
    FXFont* getFrameHeaderFont() const;

protected:
    /// @brief FOX needs this
    GNEFrame() {}

    /// @brief Open help attributes dialog
    void openHelpAttributesDialog(SumoXMLTag tag) const;

    /// @brief View Net for changes
    GNEViewNet* myViewNet;

    /// @brief Vertical frame that holds all widgets of frame
    FXVerticalFrame* myContentFrame;

    /// @brief fame for header elements
    FXHorizontalFrame* myHeaderFrame;

    /// @brief fame for left header elements
    FXHorizontalFrame* myHeaderLeftFrame;

    /// @brief fame for right header elements
    FXHorizontalFrame* myHeaderRightFrame;

private:
    /// @brief scroll windows that holds the content frame
    FXScrollWindow* myScrollWindowsContents;

    /// @brief Font for the Header
    FXFont* myFrameHeaderFont;

    /// @brief the label for the frame's header
    FXLabel* myFrameHeaderLabel;

    /// @brief Invalidated copy constructor.
    GNEFrame(const GNEFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFrame& operator=(const GNEFrame&) = delete;
};


#endif

/****************************************************************************/
