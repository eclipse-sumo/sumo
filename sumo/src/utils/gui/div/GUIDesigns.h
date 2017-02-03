/****************************************************************************/
/// @file    GUIDesigns.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id$
///
/// File with the definitions of standard style of FXObjects in SUMO
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIDesigns_h
#define GUIDesigns_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>

// ===========================================================================
// Definitions
// ===========================================================================

/// @name FXTextFields
/// @{
    /// @brief text field extended over Frame with thick frame
    #define GUIDesignTextField              (FRAME_THICK | LAYOUT_FILL_X)

    /// @brief text field extended over Frame with thick frame and limited to Integers
    #define GUIDesignTextFieldInt  (FRAME_THICK | LAYOUT_FILL_X | TEXTFIELD_INTEGER)

    /// @brief text field extended over Frame with thick frame and limited to Doubles/SUMOReals
    #define GUIDesignTextFieldReal (FRAME_THICK | LAYOUT_FILL_X | TEXTFIELD_REAL)

    /// @brief text field extended over the matrix column with thick frame and limited to integers
    #define GUIDesignTextFieldAttributeInt  (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | TEXTFIELD_INTEGER)

    /// @brief text field extended over the matrix column with thick frame and limited to real
    #define GUIDesignTextFieldAttributeReal (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | TEXTFIELD_REAL)

    /// @brief text field extended over the matrix column with thick frame
    #define GUIDesignTextFieldAttributeStr  (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X)

    /// @brief Num of column of text field
    #define GUIDesignTextFieldNCol          1
/// @}

/// @name FXButtons
/// @{
    /// @brief button extended over over Frame with thick and raise frame 
    #define GUIDesignButton                 (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | LAYOUT_FILL_X) , 0, 0, 0, 0, 4, 4, 3, 3

    /// @brief button extended over over column with thick and raise frame 
    #define GUIDesignButtonAttribute        (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X)

    /// @brief button with size limited by text, centered over Frame with thick and raise frame 
    #define GUIDesignButtonDialog           (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | LAYOUT_CENTER_X | JUSTIFY_NORMAL)

    /// @brief little button for help in frames
    #define GUIDesignButtonHelp             (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | JUSTIFY_NORMAL)

    /// @brief little button with text placed in navigation toolbar
    #define GUIDesignButtonToolbarText      (BUTTON_TOOLBAR | ICON_ABOVE_TEXT | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_Y | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23
    
    /// @brief little button with icon placed in navigation toolbar
    #define GUIDesignButtonToolbar          (BUTTON_TOOLBAR | ICON_ABOVE_TEXT | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 23, 23

    /// @brief little checkable button with icon placed in navigation toolbar used specify for Locator
    #define GUIDesignButtonToolbarLocator   (BUTTON_TOOLBAR | MENUBUTTON_RIGHT | LAYOUT_TOP | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 23, 23

    /// @brief little checkable button with icon placed in navigation toolbar
    #define GUIDesignButtonToolbarCheckable (BUTTON_NORMAL | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 23, 23

    /// @brief OK Button
    #define GUIDesignButtonOK               (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 75, 23, 2, 2, 2, 2

    /// @brief Accept Button
    #define GUIDesignButtonAccept           (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 75, 23, 2, 2, 2, 2
    
    /// @brief Cancel Button
    #define GUIDesignButtonCancel           (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 75, 23, 2, 2, 2, 2

    /// @brief Reset Button
    #define GUIDesignButtonReset            (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 75, 23, 2, 2, 2, 2

/// @}
    
/// @name MDI Buttons
/// @{
    /// @brief MDIButton oriented to left
    #define GUIDesignMDIButtonLeft          (LAYOUT_LEFT)

    /// @brief MDIButton oriented to right
    #define GUIDesignMDIButtonRight         (FRAME_RAISED | LAYOUT_RIGHT)
/// @}

/// @name FXMenu Checks
/// @{
    /// @brief MenuCheck for Frames extended over the frame
    #define GUIDesignMenuCheck              (LAYOUT_LEFT | ICON_BEFORE_TEXT)

    /// @brief MenuCheck button for attributes extended over the matrix column
    #define GUIDesignMenuCheckAttribute     (LAYOUT_LEFT | LAYOUT_FILL_COLUMN | ICON_BEFORE_TEXT)
/// @}

/// @name FXLabels
/// @{
    /// @brief label extended over over frame without thick and with text justify to left
    #define GUIDesignLabelLeft              (JUSTIFY_LEFT | LAYOUT_FILL_X | ICON_BEFORE_TEXT)

    /// @brief label extended over over frame without thick and with text justify to right
    #define GUIDesignLabelRight             (JUSTIFY_RIGHT | LAYOUT_FILL_X | ICON_BEFORE_TEXT)

    /// @brief label extended over over frame without thickand  with text justify to center
    #define GUIDesignLabelCenter            (JUSTIFY_NORMAL | LAYOUT_FILL_X | ICON_BEFORE_TEXT)

    /// @brief label extended over over frame with thick and with text justify to left
    #define GUIDesignLabelThick             (FRAME_THICK | LAYOUT_FILL_X | ICON_BEFORE_TEXT)

    /// @brief label extended over over frame with thick and with text justify to left
    #define GUIDesignLabelLeftThick         (FRAME_THICK | JUSTIFY_LEFT | LAYOUT_FILL_X | ICON_BEFORE_TEXT)

    /// @brief label extended over over frame with thick and with text justify to right
    #define GUIDesignLabelRightThick        (FRAME_THICK | JUSTIFY_RIGHT | LAYOUT_FILL_X | ICON_BEFORE_TEXT)

    /// @brief label extended over over frame with thick and with text justify to center
    #define GUIDesignLabelCenterThick       (FRAME_THICK | JUSTIFY_NORMAL | LAYOUT_FILL_X | ICON_BEFORE_TEXT)

    /// @brief label extended over the matrix column with thick frame
    #define GUIDesignLabelAttribute         (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | ICON_BEFORE_TEXT)
/// @}

/// @name FXComboBox
/// @{
    /// @brief Combo box editable extended over the all frame
    #define GUIDesignComboBox               (FRAME_THICK | LAYOUT_FILL_X)

    /// @brief Combo box static (not editable)
    #define GUIDesignComboBoxStatic         (COMBOBOX_STATIC | FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | LAYOUT_CENTER_Y)

    /// @brief Combo box static (cannot be edited) extended over the matrix column
    #define GUIDesignComboBoxAttribute      (COMBOBOX_NORMAL | FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X)

    /// @brief number of column of every combo box
    #define GUIDesignComboBoxNCol           1
/// @}

/// @name FXPackers
/// @{
    /// @brief Horizontal frame extended over frame
    #define GUIDesignHorizontalFrame        (LAYOUT_FILL_X | LAYOUT_LEFT)

    /// @brief Vertical frame extended over frame
    #define GUIDesignVerticalFrame          (LAYOUT_FILL_Y | LAYOUT_LEFT)

    /// @brief Horizontal frame used in status bar
    #define GUIDesignHorizontalFrameStatusBar   (LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y | LAYOUT_RIGHT | FRAME_SUNKEN), 0, 0, 20, 0, 0, 0, 0, 0, 0, 0
    
    /// @brief Matrix design extended over Frame packed by columns
    #define GUIDesignMatrix                     (LAYOUT_FILL_X | LAYOUT_FILL_Y | MATRIX_BY_COLUMNS | PACK_UNIFORM_WIDTH)
    
    /// @brief Matrix design extended over Frame and used for represent attributes and their values
    #define GUIDesignMatrixAttributes           (LAYOUT_FILL_X | MATRIX_BY_COLUMNS | PACK_UNIFORM_WIDTH)

    /// @brief Group box design extended over frame
    #define GUIDesignGroupBoxFrame              (LAYOUT_FILL_X | FRAME_GROOVE | GROUPBOX_TITLE_CENTER)

    /// @brief FXMatrix used to pack values in Viewsettings
    #define GUIDesignMatrixViewSettings         (LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS), 0, 0, 0, 0, 10, 10, 10, 10, 5, 5
/// @}


/// @name General design for netedit frames
/// @{
    /// @brief design for frames area
    #define GUIDesignFrameArea              (FRAME_SUNKEN | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0
    
    /// @brief design for viewn area
    #define GUIDesignViewnArea              (FRAME_SUNKEN | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0
    
    /// @brief design for the content frame of every frame
    #define GUIDesignContentsScrollWindow   (LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y)

    /// @brief design for the content frame of every frame
    #define GUIDesignContentsFrame          (LAYOUT_FILL_X | LAYOUT_FILL_Y)
    
    /// @brief design for all frames
    #define GUIDesignFrame                  (LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
/// @}


/// @name designs for toolbars
/// @{
    /// @brief design for all toolbars and menubars
    #define GUIDesignBar                    (LAYOUT_SIDE_TOP | LAYOUT_FILL_X | FRAME_RAISED)

    /// @brief design used in status bar
    #define GUIDesignStatusBar              (LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | FRAME_RAISED)

    /// @brief design for toolbar grip (used to change the position of toolbar with mouse)
    #define GUIDesignToolBarGrip            (TOOLBARGRIP_DOUBLE)

    /// @brief design for first toolbar shell positioned in the next position of dock
    #define GUIDesignToolBarShell1          (LAYOUT_DOCK_NEXT | LAYOUT_SIDE_TOP | FRAME_RAISED)

    /// @brief design for first toolbar shell positioned in the same  position of dock
    #define GUIDesignToolBarShell2          (LAYOUT_DOCK_SAME | LAYOUT_SIDE_TOP | FRAME_RAISED)

    #define GUIDesignToolBarShell3          (FRAME_NORMAL)
/// @}


/// @name designs for separators
/// @{
    /// @brief horizontal separator
    #define GUIDesignHorizontalSeparator    (SEPARATOR_GROOVE | LAYOUT_FILL_X)

    /// @brief vertical separator
    #define GUIDesignVerticalSeparator      (SEPARATOR_GROOVE | LAYOUT_FILL_Y)
/// @}


/// @name designs for splitters
/// @{
    /// @brief general design for  splitters
    #define GUIDesignSplitter               (LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_RAISED | FRAME_THICK | SPLITTER_TRACKING)

    /// @brief MDI Splitter
    #define GUIDesignSplitterMDI            (LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_SUNKEN | FRAME_THICK)
/// @}


/// @name designs for Spin dials
/// @{
    /// @brief desing for standard spin dial
    #define GUIDesignSpinDial               (FRAME_THICK | FRAME_SUNKEN | LAYOUT_TOP | LAYOUT_CENTER_Y)

    /// @brief  spin dial extended over the matrix column with thick frame
    #define GUIDesignSpinDialAttribute      (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | SPIN_NOMAX)
/// @}


/// @name design for about dialogs (SUMO and netedit)
/// @{

    /// @brief design for Frame 1
    #define GUIDesignAboutFrame1            (LAYOUT_TOP | FRAME_NONE | LAYOUT_FILL_X), 0, 0, 0, 0, 0, 0, 1, 1

    /// @brief design for Frame 2
    #define GUIDesignAboutFrame2            (LAYOUT_TOP | LAYOUT_CENTER_X | FRAME_NONE), 0, 0, 0, 0, 0, 0, 1, 1

    /// @brief design for Frame 3
    #define GUIDesignAboutFrame3            (FRAME_NONE), 0, 0, 0, 0, 20, 0, 0, 0

    /// @brief design for label with icon
    #define GUIDesignAboutLabelIcon         (LAYOUT_CENTER_Y | TEXT_OVER_ICON), 5, 0, 45, 0, 12, 0, 0, 0

    /// @brief design for label with description (centered, between two icons)
    #define GUIDesignAboutLabelDescription  (LAYOUT_CENTER_Y | LAYOUT_CENTER_X | JUSTIFY_CENTER_X | LABEL_NORMAL), 0, 0, 0, 0, 0, 0, 0, 0

    /// @brief design for label with information (for example, copyright)
    #define GUIDesignAboutLabelInformation  (LAYOUT_CENTER_X | JUSTIFY_CENTER_X | LABEL_NORMAL), 0, 0, 0, 0, 0, 0, 0, 0

    /// @brief design for label with link to DLR's webpage
    #define GUIDesignAboutLinkLabel         (LAYOUT_CENTER_X | JUSTIFY_CENTER_X | LABEL_NORMAL), 0, 0, 0, 0, 5, 5, 5, 5
/// @}

/// @name design for Tables

    /// @brief design for tables which their rows and columns cannot be selected
    #define GUIDesignTableNoRowColSelectables   (LAYOUT_FILL_X | LAYOUT_FILL_Y | TABLE_NO_ROWSELECT | TABLE_NO_COLSELECT)
/// @}

    /// @brief Tree list used in frames to represent childs of elements
    #define GUIDesignTreeListFrame          (TREELIST_SHOWS_LINES | TREELIST_SHOWS_BOXES | TREELIST_SINGLESELECT | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, 200

    /// @brief CheckButton for Frames extended over the frame
    #define GUIDesignCheckButton            (CHECKBUTTON_NORMAL | LAYOUT_CENTER_Y)

    #define GUIDesignRadioButton            (RADIOBUTTON_NORMAL)

    #define GUIDesignTable                  (LAYOUT_FIX_HEIGHT | LAYOUT_FILL_X)

    #define GUIDesignDialogBox              (DECOR_CLOSE | DECOR_TITLE)

    #define GUIDesignList                   (FRAME_NORMAL | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT | LIST_NORMAL)

#endif

/****************************************************************************/
