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
/// @file    GNEFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
///
// Abstract class for lateral frames in NetEdit
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEFrameAttributeModules.h"

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEFrame
 * Abstract class for lateral frames in NetEdit
 */
class GNEFrame : public FXVerticalFrame {

public:
    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     * @brief frameLabel label of the frame
     */
    GNEFrame(GNEViewParent* viewParent, GNEViewNet* viewNet, const std::string& frameLabel);

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
    void setFrameWidth(const int newWidth);

    /// @brief get view net
    GNEViewNet* getViewNet() const;

    /// @brief get vertical frame that holds all widgets of frame
    FXVerticalFrame* getContentFrame() const;

    /// @brief get the label for the frame's header
    FXLabel* getFrameHeaderLabel() const;

    /// @brief get font of the header's frame
    FXFont* getFrameHeaderFont() const;

    /// @brief get scrollBar width (zero if is hidden)
    int getScrollBarWidth() const;

    /// @brief Open help attributes dialog
    void openHelpAttributesDialog(const GNEAttributeCarrier* AC) const;

    /// @brief function called after undo/redo in the current frame (can be reimplemented in frame children)
    virtual void updateFrameAfterUndoRedo();

    /// @brief function called after setting new width in current frame (can be reimplemented in frame children)
    virtual void frameWidthUpdated();

    /// @name functions called by moduls that can be reimplemented in frame children (note: reimplement as protected, just for safety)
    /// @{

    /// @brief Tag selected in GNETagSelector
    virtual void tagSelected();

    /// @brief selected demand element in DemandElementSelector
    virtual void demandElementSelected();

    /// @brief build a shaped element using the drawed shape
    virtual bool shapeDrawed();

    /// @brief function called after set a valid attribute in AttributeCreator/AttributeEditor/ParametersEditor/...
    virtual void attributeUpdated(SumoXMLAttr attribute);

    /// @brief open GNEAttributesCreator extended dialog
    virtual void attributesEditorExtendedDialogOpened();

    /// @brief open GNEAttributesCreator extended dialog
    virtual void selectedOverlappedElement(GNEAttributeCarrier* AC);

    /// @brief create path between two elements
    virtual bool createPath(const bool useLastRoute);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEFrame)

    /// @brief View Net
    GNEViewNet* myViewNet = nullptr;

    /// @brief Vertical frame that holds all widgets of frame
    FXVerticalFrame* myContentFrame = nullptr;

    /// @brief fame for header elements
    FXHorizontalFrame* myHeaderFrame = nullptr;

    /// @brief fame for left header elements
    FXHorizontalFrame* myHeaderLeftFrame = nullptr;

    /// @brief fame for right header elements
    FXHorizontalFrame* myHeaderRightFrame = nullptr;

    /// @brief get predefinedTagsMML
    const std::vector<std::string>& getPredefinedTagsMML() const;

    /// @brief build rainbow in frame modul
    static FXLabel* buildRainbow(FXComposite* parent);

private:
    /// @brief scroll windows that holds the content frame
    FXScrollWindow* myScrollWindowsContents = nullptr;

    /// @brief static Font for the Header (it's common for all headers, then create only one time)
    static FXFont* myFrameHeaderFont;

    /// @brief the label for the frame's header
    FXLabel* myFrameHeaderLabel = nullptr;

    /// @brief Map of attribute ids to their (readable) string-representation (needed for SUMOSAXAttributesImpl_Cached)
    std::vector<std::string> myPredefinedTagsMML;

    /// @brief Invalidated copy constructor.
    GNEFrame(const GNEFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFrame& operator=(const GNEFrame&) = delete;
};
