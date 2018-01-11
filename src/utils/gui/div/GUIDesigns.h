/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIDesigns.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id$
///
// File with the definitions of standard style of FXObjects in SUMO
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
#define GUIDesignTextField                  (FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief text field extended over Frame with thick frame and limited to Integers
#define GUIDesignTextFieldInt               (FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT | TEXTFIELD_INTEGER), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief text field extended over Frame with thick frame and limited to Doubles/doubles
#define GUIDesignTextFieldReal              (FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT | TEXTFIELD_REAL), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief Num of column of text field
#define GUIDesignTextFieldNCol              1
/// @}


/// @name FXButtons
/// @{
/// @brief button extended over over Frame with thick and raise frame
#define GUIDesignButton                     (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief button extended over over column with thick and raise frame
#define GUIDesignButtonAttribute            (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 100, 23, 2, 2, 2, 2

/// @brief little button rectangular     (46x23) used in frames     (For example, in "help" buttons)
#define GUIDesignButtonRectangular          (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | JUSTIFY_CENTER_X | JUSTIFY_CENTER_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 46, 23, 2, 2, 2, 2

/// @brief button only with icon     (23x23)
#define GUIDesignButtonIcon                 (FRAME_THICK | FRAME_RAISED | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 23, 23, 2, 2, 2, 2

/// @brief button only with icon     (46x23)
#define GUIDesignButtonIconRectangular      (FRAME_THICK | FRAME_RAISED | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 46, 23, 2, 2, 2, 2

/// @brief button used in breakpoint editor
#define GUIDesignButtonBreakpoint           (ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED), 0, 0, 0, 0, 4, 4, 3, 3
/// @}


/// @name FXButtons for toolbar
/// @{
/// @brief little button with text placed in navigation toolbar
#define GUIDesignButtonToolbarText          (BUTTON_TOOLBAR | ICON_BEFORE_TEXT | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_Y | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23

/// @brief little button with icon placed in navigation toolbar
#define GUIDesignButtonToolbar              (BUTTON_TOOLBAR | ICON_BEFORE_TEXT | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 23, 23

/// @brief little button with text and icon
#define GUIDesignButtonToolbarWithText      (BUTTON_TOOLBAR | ICON_BEFORE_TEXT | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FIX_HEIGHT | JUSTIFY_LEFT), 0, 0, 0, 23

/// @brief little checkable button with icon placed in navigation toolbar used specify for Locator
#define GUIDesignButtonToolbarLocator       (BUTTON_TOOLBAR | MENUBUTTON_RIGHT | LAYOUT_TOP | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 23, 23

/// @brief little checkable button with icon placed in navigation toolbar
#define GUIDesignButtonToolbarCheckable     (BUTTON_NORMAL | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 23, 23
/// @}


/// @name FXButtons for dialogs
/// @{
/// @brief OK Button
#define GUIDesignButtonOK                   (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 75, 23, 2, 2, 2, 2

/// @brief Accept Button
#define GUIDesignButtonAccept               (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 75, 23, 2, 2, 2, 2

/// @brief Cancel Button
#define GUIDesignButtonCancel               (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 75, 23, 2, 2, 2, 2

/// @brief Reset Button
#define GUIDesignButtonReset                (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 75, 23, 2, 2, 2, 2
/// @}


/// @name FXCheckButtons
/// @{
/// @brief CheckButton for Frames without thick extended over the frame
#define GUIDesignCheckButton                (CHECKBUTTON_NORMAL | LAYOUT_CENTER_Y)

/// @brief checkButton without thick extended over the frame used for attributes
#define GUIDesignCheckButtonAttribute       (CHECKBUTTON_NORMAL | JUSTIFY_CENTER_Y | LAYOUT_FIX_HEIGHT | JUSTIFY_LEFT), 0, 0, 0, 23, 2, 2, 2, 2
/// @}


/// @name MDI Buttons
/// @{
/// @brief MDIButton oriented to left
#define GUIDesignMDIButtonLeft              (LAYOUT_LEFT)

/// @brief MDIButton oriented to right
#define GUIDesignMDIButtonRight             (FRAME_RAISED | LAYOUT_RIGHT)
/// @}


/// @name FXMenuChecks
/// @{
/// @brief design for radio button
#define GUIDesignRadioButton                (LAYOUT_LEFT | LAYOUT_FILL_X | ICON_BEFORE_TEXT | JUSTIFY_LEFT)

/// @brief design for radio button with fixed height
#define GUIDesignRadioButtonAttribute       (LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT | JUSTIFY_LEFT | JUSTIFY_CENTER_Y | ICON_BEFORE_TEXT), 0, 0, 0, 23, 2, 2, 2, 2
/// @}


/// @name FXLabels
/// @{
/// @brief label extended over frame without thick and with text justify to left and height of 23
#define GUIDesignLabelLeft                  (JUSTIFY_LEFT | LAYOUT_FILL_X | ICON_BEFORE_TEXT | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief label extended over frame without thick and with text justify to right and height of 23
#define GUIDesignLabelRight                 (JUSTIFY_RIGHT | LAYOUT_FILL_X | ICON_BEFORE_TEXT | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief label extended over frame without thickand  with text justify to center and height of 23
#define GUIDesignLabelCenter                (JUSTIFY_NORMAL | LAYOUT_FILL_X | ICON_BEFORE_TEXT | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief label extended over frame with thick and with text justify to left and height of 23
#define GUIDesignLabelThick                 (FRAME_THICK | LAYOUT_FILL_X | ICON_BEFORE_TEXT | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief label extended over frame with thick and with text justify to left and height of 23
#define GUIDesignLabelLeftThick             (FRAME_THICK | JUSTIFY_LEFT | LAYOUT_FILL_X | ICON_BEFORE_TEXT | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief label extended over frame with thick and with text justify to right and height of 23
#define GUIDesignLabelRightThick            (FRAME_THICK | JUSTIFY_RIGHT | LAYOUT_FILL_X | ICON_BEFORE_TEXT | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief label extended over frame with thick and with text justify to center and height of 23
#define GUIDesignLabelCenterThick           (FRAME_THICK | JUSTIFY_NORMAL | LAYOUT_FILL_X | ICON_BEFORE_TEXT | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief label extended over the matrix column with thick frame and height of 23
#define GUIDesignLabelAttribute             (FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | ICON_BEFORE_TEXT), 0, 0, 100, 23, 2, 2, 2, 2

/// @brief label extended over over frame without thickand with text justify to center and withouht vertical spaces
#define GUIDesignLabelAboutInfo             (JUSTIFY_NORMAL | LAYOUT_FILL_X | ICON_BEFORE_TEXT), 0, 0, 0, 0, 2, 2, 0, 0

/// @brief label ticked filled only with an icon of 32x32
#define GUIDesignLabelIcon32x32Thicked      (FRAME_THICK | ICON_BEFORE_TEXT | LAYOUT_FIX_HEIGHT | LAYOUT_FIX_WIDTH), 0, 0, 24, 23, 2, 2, 2, 2

/// @brief label ticked filled used for VClasses. It uses icons of 64x32 pixels
#define GUIDesignLabelIcon64x32Thicked      (FRAME_THICK | ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 94, 46, 2, 2, 2, 2

/// @brief design for label with icon of 64x64 px
#define GUIDesignLabelIcon64x64noSpacing    (LAYOUT_CENTER_Y | LAYOUT_CENTER_X | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT ), 0, 0, 64, 64, 0, 0, 0, 0

/// @brief label ticked filled extended over frame used for VClasses. can be used by icons of 64x32 pixels
#define GUIDesignLabelIconExtendedx46Ticked (FRAME_THICK | ICON_BEFORE_TEXT | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, 46, 2, 2, 2, 2

/// @brief label extended over frame without thick and with text justify to left, used to show information in frames
#define GUIDesignLabelFrameInformation      (JUSTIFY_LEFT | LAYOUT_FILL_X | ICON_BEFORE_TEXT), 0, 0, 0, 0, 2, 2, 2, 2
/// @}


/// @name FXComboBox
/// @{
/// @brief Combo box editable extended over the all frame
#define GUIDesignComboBox                   (FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief Combo box static     (not editable)
#define GUIDesignComboBoxStatic             (COMBOBOX_STATIC | FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23, 2, 2, 2, 2

/// @brief Combo box static     (cannot be edited) extended over the matrix column
#define GUIDesignComboBoxAttribute          (COMBOBOX_NORMAL | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, 23, 2, 2, 2, 2


/// @brief number of column of every combo box
#define GUIDesignComboBoxNCol               1
/// @}


/// @name FXPackers
/// @{
/// @brief Horizontal frame extended over frame parent
#define GUIDesignHorizontalFrame            (LAYOUT_FILL_X)

/// @brief Vertical frame extended over frame parent
#define GUIDesignVerticalFrame              (LAYOUT_FILL_Y)

/// @brief Horizontal frame used for pack icons
#define GUIDesignHorizontalFrameIcons       (LAYOUT_FILL_X | FRAME_THICK), 0, 0, 0, 0, 0, 0, 0, 0, 2, 2

/// @brief Horizontal frame used in status bar
#define GUIDesignHorizontalFrameStatusBar   (LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y | LAYOUT_RIGHT | FRAME_SUNKEN), 0, 0, 20, 0, 0, 0, 0, 0, 0, 0

/// @brief Matrix design extended over Frame packed by columns
#define GUIDesignMatrix                     (LAYOUT_FILL_X | LAYOUT_FILL_Y | MATRIX_BY_COLUMNS | PACK_UNIFORM_WIDTH)

/// @brief Group box design extended over frame
#define GUIDesignGroupBoxFrame              (LAYOUT_FILL_X | FRAME_GROOVE | GROUPBOX_TITLE_CENTER)

/// @brief FXMatrix used to pack values in Viewsettings
#define GUIDesignMatrixViewSettings         (LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS), 0, 0, 0, 0, 10, 10, 10, 10, 5, 5

/// @brief Matrix design extended over Frame and used for represent attributes and their values (Deprecated)
#define GUIDesignMatrixAttributes           (LAYOUT_FILL_X | MATRIX_BY_COLUMNS | PACK_UNIFORM_WIDTH)
/// @}


/// @name General design for netedit frames
/// @{
/// @brief design for frames area
#define GUIDesignFrameArea                  (FRAME_SUNKEN | LAYOUT_SIDE_TOP | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y)

/// @brief design for viewn area
#define GUIDesignViewnArea                  (FRAME_SUNKEN | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0

/// @brief design for the content frame of every frame
#define GUIDesignContentsScrollWindow       (LAYOUT_FILL_X | LAYOUT_FILL_Y | HSCROLLER_NEVER), 0, 0, 0, 0

/// @brief design for auxiliar (Without borders) frames used to pack another frames extended in all directions
#define GUIDesignAuxiliarFrame              (LAYOUT_FILL_X | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/// @brief design for auxiliar (Without borders) horizontal frame used to pack another frames
#define GUIDesignAuxiliarHorizontalFrame    (LAYOUT_FILL_X), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/// @brief design for auxiliar (Without borders) horizontal frame used to pack another frames
#define GUIDesignAuxiliarVerticalFrame      (LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/// @brief design for horizontal frame used to pack another frames with a uniform width
#define GUIDesignUniformHorizontalFrame     (LAYOUT_FILL_X | LAYOUT_FILL_Y | PACK_UNIFORM_WIDTH)

/// @brief design for the main content frame of every frame/dialog
#define GUIDesignContentsFrame              (LAYOUT_FILL_X | LAYOUT_FILL_Y)
/// @}


/// @name designs for toolbars
/// @{
/// @brief design for all toolbars and menubars
#define GUIDesignBar                        (LAYOUT_SIDE_TOP | LAYOUT_FILL_X | FRAME_RAISED)

/// @brief design used in status bar
#define GUIDesignStatusBar                  (LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | FRAME_RAISED)

/// @brief design for toolbar grip     (used to change the position of toolbar with mouse)
#define GUIDesignToolBarGrip                (TOOLBARGRIP_DOUBLE)

/// @brief design for first toolbar shell positioned in the next position of dock
#define GUIDesignToolBarShell1              (LAYOUT_DOCK_NEXT | LAYOUT_SIDE_TOP | FRAME_RAISED)

/// @brief design for first toolbar shell positioned in the same  position of dock
#define GUIDesignToolBarShell2              (LAYOUT_DOCK_SAME | LAYOUT_SIDE_TOP | FRAME_RAISED)

#define GUIDesignToolBarShell3              (FRAME_NORMAL)
/// @}


/// @name designs for separators
/// @{
/// @brief horizontal separator
#define GUIDesignHorizontalSeparator        (SEPARATOR_GROOVE | LAYOUT_FILL_X)

/// @brief vertical separator
#define GUIDesignVerticalSeparator          (SEPARATOR_GROOVE | LAYOUT_FILL_Y)
/// @}


/// @name designs for splitters
/// @{
/// @brief general design for  splitters
#define GUIDesignSplitter                   (LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_RAISED | FRAME_THICK | SPLITTER_TRACKING)

/// @brief MDI Splitter
#define GUIDesignSplitterMDI                (LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_SUNKEN | FRAME_THICK)
/// @}


/// @name designs for Spin dials
/// @{
/// @brief desing for standard spin dial
#define GUIDesignSpinDial                   (FRAME_THICK | FRAME_SUNKEN | LAYOUT_TOP | LAYOUT_CENTER_Y)
/// @}


/// @name design for view settings dialog
/// @{
/// @rbrief dialog
#define GUIDesignViewSettingsMainDialog         (DECOR_TITLE | DECOR_BORDER | DECOR_RESIZE), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

#define GUIDesignViewSettingsDialog             (DECOR_TITLE | DECOR_BORDER)

/// @brief vertical frames
#define GUIDesignViewSettingsVerticalFrame1     (LAYOUT_FILL_X | LAYOUT_SIDE_TOP | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0, 2, 2

#define GUIDesignViewSettingsVerticalFrame2     (LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0, 2, 2

#define GUIDesignViewSettingsVerticalFrame3     (LAYOUT_FILL_Y), 0, 0, 0, 0, 10, 10, 10, 10, 5, 5

#define GUIDesignViewSettingsVerticalFrame4     (LAYOUT_FILL_X | LAYOUT_FILL_Y),  0, 0, 0, 0, 10, 10, 2, 8, 5, 2

#define GUIDesignViewSettingsVerticalFrame5     (LAYOUT_FILL_X | LAYOUT_FILL_Y), 0, 0, 0, 0, 10, 10, 10, 10, 10, 10

#define GUIDesignViewSettingsVerticalFrame6     (LAYOUT_FILL_X | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0, 2, 2

/// @brief Horizontal frames
#define GUIDesignViewSettingsHorizontalFrame1   (FRAME_THICK), 0, 0, 0, 0, 0, 0, 0, 0, 2, 2

#define GUIDesignViewSettingsHorizontalFrame2   (LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH), 0, 0, 0, 0, 10, 10, 5, 5

#define GUIDesignViewSettingsHorizontalFrame3   (LAYOUT_FILL_X | PACK_UNIFORM_WIDTH), 0, 0, 0, 0, 0, 0, 0, 0

/// @brief Matrix
#define GUIDesignViewSettingsMatrix1            (LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS), 0, 0, 0, 0, 10, 10, 10, 10, 5, 5

#define GUIDesignViewSettingsMatrix2            (LAYOUT_FILL_X | LAYOUT_CENTER_Y | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS), 0, 0, 0, 0, 10, 10, 0, 0, 5, 5

#define GUIDesignViewSettingsMatrix3            (LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS), 0, 0, 0, 0, 10, 10, 10, 2, 5, 5

#define GUIDesignViewSettingsMatrix4            (LAYOUT_FILL_X | MATRIX_BY_COLUMNS), 0, 0, 0, 0, 10, 10, 0, 0, 5, 3

#define GUIDesignViewSettingsMatrix5            (LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS), 0, 0, 0, 0, 10, 10, 0, 0, 5, 5


/// @brief Combo boxs
#define GUIDesignViewSettingsComboBox1          (COMBOBOX_INSERT_LAST | FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_CENTER_Y | COMBOBOX_STATIC)

/// @brief ColorWell
#define GUIDesignViewSettingsColorWell          (LAYOUT_FIX_WIDTH | LAYOUT_CENTER_Y | LAYOUT_SIDE_TOP | FRAME_SUNKEN | FRAME_THICK | ICON_AFTER_TEXT), 0, 0, 100, 0, 0, 0, 0, 0

#define GUIDesignViewSettingsSpinDial1          (LAYOUT_CENTER_Y | LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK)

#define GUIDesignViewSettingsSpinDial2          (LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK)

/// @brief Tab books
#define GUIDesignViewSettingsTabBook1           (TABBOOK_LEFTTABS | PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT | LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_RIGHT), 0, 0, 0, 0, 0, 0, 0, 0

#define GUIDesignViewSettingsTabItemBook1       (TAB_LEFT_NORMAL), 0, 0, 0, 0, 4, 8, 4, 4

/// @brief Label
#define GUIDesignViewSettingsLabel1             (LAYOUT_CENTER_Y)

#define GUIDesignViewSettingsLabel2             (LAYOUT_FILL_X | JUSTIFY_LEFT)

/// @brief textFields
#define GUIDesignViewSettingsTextField1         (TEXTFIELD_ENTER_ONLY | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X)

/// @brief Buttons
#define GUIDesignViewSettingsButton1            (BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X), 0, 0, 0, 0, 20, 20, 4, 4

#define GUIDesignViewSettingsButton2            (BUTTON_INITIAL | BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X), 0, 0, 0, 0, 30, 30, 4, 4

#define GUIDesignViewSettingsButton3            (BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X), 0, 0, 0, 0, 30, 30, 4, 4

#define GUIDesignViewSettingsButton4            (BUTTON_INITIAL | BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_RIGHT)

#define GUIDesignViewSettingsButton5            (BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_RIGHT), 0, 0, 0, 0, 20, 20

/// @brief Color wells
#define GUIDesignViewSettingsColorWell1         (LAYOUT_FIX_WIDTH | LAYOUT_CENTER_Y | FRAME_SUNKEN | FRAME_THICK | ICON_AFTER_TEXT), 0, 0, 100, 0, 0, 0, 0, 0

#define GUIDesignViewSettingsColorWell2         (LAYOUT_FIX_WIDTH | LAYOUT_CENTER_Y | LAYOUT_SIDE_TOP | FRAME_SUNKEN | FRAME_THICK | ICON_AFTER_TEXT), 0, 0, 100, 0, 0, 0, 0, 0

/// @brief MFX Add/Edit Typed Table
#define GUIDesignViewSettingsMFXTable           (LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH) /*|LAYOUT_FIX_HEIGHT*/, 0, 0, 470, 0

#define GUIDesignViewSettingsMFXTableJustify    (JUSTIFY_CENTER_X | JUSTIFY_TOP)
/// @}


/// @name design for dialog boxs
/// @{
/// @brief design for standard dialog box     (for example, about dialog)
#define GUIDesignDialogBox                      (DECOR_CLOSE | DECOR_TITLE)

/// @brief design for dialog box with specift width and height     (for example, additional dialogs)
#define GUIDesignDialogBoxExplicit              (DECOR_CLOSE | DECOR_TITLE | LAYOUT_EXPLICIT)
/// @}


/// @name design for FXTables and their cells
/// @{
/// @brief design for table extended over frame
#define GUIDesignTable                          (LAYOUT_FILL_X | LAYOUT_FILL_Y)

/// @brief design for table extended over frame but with limited Height
#define GUIDesignTableLimitedHeight             (LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT)

/// @brief design for tables used in additional dialogs
#define GUIDesignTableAdditionals               (LAYOUT_FILL_X | LAYOUT_FILL_Y | VSCROLLER_ALWAYS | HSCROLLER_NEVER | TABLE_NO_ROWSELECT | TABLE_NO_COLSELECT), 0, 0, 0, 0, 2, 2, 2, 2

/// @brief width of cells that only contains an Icon
#define GUIDesignTableIconCellWidth             23
/// @}

/// @brief Tree list used in frames to represent childs of elements
#define GUIDesignTreeListFrame                  (TREELIST_SHOWS_LINES | TREELIST_SHOWS_BOXES | TREELIST_SINGLESELECT | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, 200

/// @brief design for FXLists
#define GUIDesignList                           (FRAME_NORMAL | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT | LIST_NORMAL)

/// @brief design for FXLists
#define GUIDesignListExtended                   (LIST_NORMAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y)

/// @brief desgin for TabBooks
#define GUIDesignTabBook                        (TABBOOK_LEFTTABS | PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT | LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_RIGHT)

#endif

/****************************************************************************/
