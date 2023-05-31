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
/// @file    GUIDesigns.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
///
// File with the definitions of standard style of FXObjects in SUMO
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <utils/foxtools/fxheader.h>

// ===========================================================================
// Class declaration
// ===========================================================================

class MFXMenuCheckIcon;

// ===========================================================================
// Definitions
// ===========================================================================

/// @brief define a standard height for all elements (Change it carefully)
#define GUIDesignHeight 23

/// @brief define the default frames area width
#define GUIDesignFramesAreaDefaultWidth 220

/// @brief right margin for frame area
#define GUIDesignFrameAreaMarging 2

/// @brief elements with big images
#define GUIDesignBigSizeElement 138

/// @name FXTextFields
/// @{
/// @brief text field extended over Frame with thick frame
#define GUIDesignTextField                              (FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief text field extended over Frame with thick frame (int)
#define GUIDesignTextFieldRestricted(type)              (FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT | type), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief text field with fixed width
#define GUIDesignTextFieldFixed(width)                  (FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, width, GUIDesignHeight, 2, 2, 2, 2

/// @brief text field with fixed width
#define GUIDesignTextFieldFixedRestricted(width, type)  (FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | type), 0, 0, width, GUIDesignHeight, 2, 2, 2, 2

/// @brief text field with min width (used in TLS table)
#define GUIDesignTextFieldTLSTable                      (FRAME_THICK | LAYOUT_FILL_X | LAYOUT_MIN_WIDTH | LAYOUT_FIX_HEIGHT | JUSTIFY_CENTER_Y | JUSTIFY_LEFT), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief Num of column of text field
#define GUIDesignTextFieldNCol          1

/// @}


/// @name FXButtons
/// @{
/// @brief button extended over over Frame with thick and raise frame
#define GUIDesignButton                         (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief button extended over over column with thick and raise frame
#define GUIDesignButtonAttribute                (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 100, GUIDesignHeight, 2, 2, 2, 2

/// @brief little rectangular button used in frames (For example, in "help" buttons)
#define GUIDesignButtonRectangular              (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | JUSTIFY_CENTER_X | JUSTIFY_CENTER_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 46, GUIDesignHeight, 2, 2, 2, 2

/// @brief button only with icon
#define GUIDesignButtonIcon                     (FRAME_THICK | FRAME_RAISED | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, GUIDesignHeight, GUIDesignHeight, 2, 2, 2, 2

/// @brief button only with icon
#define GUIDesignButtonIconRectangular          (FRAME_THICK | FRAME_RAISED | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 46, GUIDesignHeight, 2, 2, 2, 2

/// @brief button rectangular with thick and raise frame with a width of 100
#define GUIDesignButtonRectangular100           (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | JUSTIFY_CENTER_X | JUSTIFY_CENTER_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 100, GUIDesignHeight, 2, 2, 2, 2

/// @brief button rectangular with thick and raise frame with a width of 150
#define GUIDesignButtonRectangular150           (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | JUSTIFY_CENTER_X | JUSTIFY_CENTER_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 150, GUIDesignHeight, 2, 2, 2, 2

/// @brief button used in GroupBoxModule
#define GUIDesignButtonMFXGroupBoxModule        (FRAME_LINE | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, GUIDesignHeight - 1, GUIDesignHeight - 1, 2, 2, 2, 2

/// @brief button used in GroupBoxModule for extend
#define GUIDesignButtonMFXGroupBoxModuleExtend  (FRAME_LINE | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 60, GUIDesignHeight - 1, 2, 2, 2, 2

/// @brief button rectangular with thick and raise frame with a width of 100
#define GUIDesignButtonStatusBarFixed           (ICON_BEFORE_TEXT | JUSTIFY_CENTER_X | JUSTIFY_CENTER_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 100, GUIDesignHeight, 2, 2, 2, 2

/// @brief checkable button only with icon used in TLSTable
#define GUIDesignTLSTableCheckableButtonIcon    (MENUBUTTON_RIGHT | FRAME_THICK | FRAME_RAISED | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, GUIDesignHeight, GUIDesignHeight, 0, 0, 0, 0

/// @brief checkable button placed in popup (for example, locate buttons)
#define GUIDesignButtonPopup                    (ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED)

/// @}


/// @name FXButtons for toolbar
/// @{
/// @brief little button with text placed in navigation toolbar
#define GUIDesignButtonToolbarText      (BUTTON_TOOLBAR | ICON_BEFORE_TEXT | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_Y | LAYOUT_FIX_HEIGHT), 0, 0, 0, GUIDesignHeight

/// @brief little button with icon placed in navigation toolbar
#define GUIDesignButtonToolbar          (BUTTON_TOOLBAR | ICON_BEFORE_TEXT | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, GUIDesignHeight, GUIDesignHeight

/// @brief little button with text and icon
#define GUIDesignButtonToolbarWithText  (BUTTON_TOOLBAR | ICON_BEFORE_TEXT | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FIX_HEIGHT | JUSTIFY_LEFT), 0, 0, 0, GUIDesignHeight

/// @brief little checkable button with icon placed in navigation toolbar used specify for Locator
#define GUIDesignButtonToolbarLocator   (BUTTON_TOOLBAR | MENUBUTTON_RIGHT | LAYOUT_TOP | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, GUIDesignHeight, GUIDesignHeight

/// @}


/// @name FXButtons for toolbar
/// @{
/// @brief MFX Checkable Button with icon placed in navigation toolbar
#define GUIDesignMFXCheckableButtonSquare       (BUTTON_NORMAL | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, GUIDesignHeight, GUIDesignHeight

/// @brief checkable button with icon placed in navigation toolbar for supermodes
#define GUIDesignMFXCheckableButtonSupermode    (BUTTON_NORMAL | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 100, GUIDesignHeight

/// @brief checkable button extended over frame
#define GUIDesignMFXCheckableButton             (BUTTON_NORMAL | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, GUIDesignHeight

/// @brief checkable button extended over frame
#define GUIDesignMFXCheckableButtonBig          (BUTTON_NORMAL | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, GUIDesignBigSizeElement, GUIDesignBigSizeElement

/// @}

/// @name FXButtons for dialogs
/// @{
/// @brief OK Button
#define GUIDesignButtonOK                   (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 75, GUIDesignHeight, 2, 2, 2, 2

/// @brief Accept Button
#define GUIDesignButtonAccept               (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 75, GUIDesignHeight, 2, 2, 2, 2

/// @brief Configuration Button
#define GUIDesignButtonConfiguration        (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 140, GUIDesignHeight, 2, 2, 2, 2

/// @brief Cancel Button
#define GUIDesignButtonCancel               (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 75, GUIDesignHeight, 2, 2, 2, 2

/// @brief Reset Button
#define GUIDesignButtonReset                (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 75, GUIDesignHeight, 2, 2, 2, 2

/// @brief Advanced Button
#define GUIDesignButtonAdvanced             (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 110, GUIDesignHeight, 2, 2, 2, 2

/// @brief overwrite Button
#define GUIDesignButtonOverwrite            (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 100, GUIDesignHeight, 2, 2, 2, 2

/// @brief Focus Button
#define GUIDesignButtonFocus                (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_NONE), 0, 0, 0, 0, 0, 0, 0, 0

/// @brief Button with custom width (used in GNEGeometryPointDialog)
#define GUIDesignButtonCustomWidth(width)   (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, width, GUIDesignHeight, 2, 2, 2, 2

///  @brief button for saving elements in GUISaveDialog
#define GUIDesignSaveDialogButtonInitial    (BUTTON_INITIAL | BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_CENTER_X | LAYOUT_FIX_WIDTH), 0, 0, 150, 0, 15, 15, VERT_PAD, VERT_PAD

///  @brief button for saving elements in GUISaveDialog
#define GUIDesignSaveDialogButton           (BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_CENTER_X | LAYOUT_FIX_WIDTH), 0, 0, 150, 0, 15, 15, VERT_PAD, VERT_PAD

/// @}


/// @name FXCheckButtons
/// @{

/// @brief checkButton placed in left position
#define GUIDesignCheckButton                (CHECKBUTTON_NORMAL | JUSTIFY_CENTER_Y | LAYOUT_FIX_HEIGHT | JUSTIFY_LEFT), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief checkButton placed in left position with double size
#define GUIDesignCheckButtonExtraHeight     (CHECKBUTTON_NORMAL | JUSTIFY_CENTER_Y | LAYOUT_FIX_HEIGHT | JUSTIFY_LEFT), 0, 0, 0, GUIDesignHeight + 10, 2, 2, 2, 2

/// @brief checkButton without thick extended over the frame used for attributes
#define GUIDesignCheckButtonAttribute       (CHECKBUTTON_NORMAL | JUSTIFY_CENTER_Y |  LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | JUSTIFY_LEFT | ICON_BEFORE_TEXT | FRAME_THICK), 0, 0, 100, GUIDesignHeight, 2, 2, 2, 2

/// @brief checkButton placed in center position with size GUIDesignHeight x GUIDesignHeight
#define GUIDesignCheckButtonIcon            (CHECKBUTTON_NORMAL | JUSTIFY_CENTER_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | JUSTIFY_CENTER_X), 0, 0, GUIDesignHeight, GUIDesignHeight, 2, 2, 2, 2

/// @brief CheckButton for Frames without thick extended over the frame
#define GUIDesignCheckButtonViewSettings    (CHECKBUTTON_NORMAL | LAYOUT_CENTER_Y)

/// @brief design for check button with fixed height (used in fix elements dialogs)
#define GUIDesignCheckButtonFix             (CHECKBUTTON_NORMAL | JUSTIFY_CENTER_Y |  LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | JUSTIFY_LEFT | ICON_BEFORE_TEXT), 0, 0, 200, GUIDesignHeight - 2, 2, 2, 2, 2

/// @}


/// @name MDI Buttons
/// @{
/// @brief MDIButton oriented to left
#define GUIDesignMDIButtonLeft              (LAYOUT_LEFT)

/// @brief MDIButton oriented to right
#define GUIDesignMDIButtonRight             (FRAME_RAISED | LAYOUT_RIGHT)

/// @}


/// @name FXRadiobuttons
/// @{
/// @brief design for radio button
#define GUIDesignRadioButton            (LAYOUT_LEFT | LAYOUT_FILL_X | ICON_BEFORE_TEXT | JUSTIFY_LEFT)

/// @brief design for radio button squared
#define GUIDesignRadioButtonSquared     (RADIOBUTTON_NORMAL | FRAME_THICK |  LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | JUSTIFY_CENTER_X | JUSTIFY_CENTER_Y), 0, 0, GUIDesignHeight, GUIDesignHeight, 2, 2, 2, 2

/// @brief design for radio button with fixed height (used in fix elements dialogs)
#define GUIDesignRadioButtonFix         (RADIOBUTTON_NORMAL | JUSTIFY_CENTER_Y |  LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | JUSTIFY_LEFT | ICON_BEFORE_TEXT), 0, 0, 200, GUIDesignHeight - 2, 2, 2, 2, 2

/// @}


/// @name FXLabels
/// @{
/// @brief label extended over frame
#define GUIDesignLabel(justify)             (LAYOUT_FILL_X | ICON_BEFORE_TEXT | justify | LAYOUT_FIX_HEIGHT), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief label, icon before text, text centered and custom width
#define GUIDesignLabelFixed(width)          (ICON_BEFORE_TEXT | JUSTIFY_CENTER_X | JUSTIFY_CENTER_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, width, GUIDesignHeight, 2, 2, 2, 2

/// @brief label extended over frame with thick and with text justify to left
#define GUIDesignLabelThick(justify)        (FRAME_THICK | LAYOUT_FILL_X | ICON_BEFORE_TEXT | justify  | LAYOUT_FIX_HEIGHT), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief label thicked, icon before text, text centered and custom width
#define GUIDesignLabelThickedFixed(width)   (FRAME_THICK | ICON_BEFORE_TEXT | JUSTIFY_CENTER_X | JUSTIFY_CENTER_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, width, GUIDesignHeight, 2, 2, 2, 2

/// @brief label squared over frame with thick and with text justify to center
#define GUIDesignLabelIconThick             (FRAME_THICK | JUSTIFY_NORMAL | LAYOUT_FILL_X | ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, GUIDesignHeight, GUIDesignHeight, 2, 2, 2, 2

/// @brief label extended over frame without thick and with text justify to left
#define GUIDesignLabelAboutInfo             (JUSTIFY_LEFT | LAYOUT_FILL_X | ICON_BEFORE_TEXT), 0, 0, 0, 0, 2, 2, 0, 0

/// @brief label extended over frame without thick and with text justify to left
#define GUIDesignLabelOverwrite             (JUSTIFY_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y | ICON_BEFORE_TEXT), 0, 0, 0, 0, 2, 2, 2, 2

/// @brief label extended over frame with thick and with text justify to center
#define GUIDesignLabelAboutInfoCenter       (FRAME_THICK | JUSTIFY_NORMAL | LAYOUT_FILL_X | ICON_BEFORE_TEXT), 0, 0, 0, 0, 2, 2, 0, 0

/// @brief label ticked filled only with an icon of 32x32
#define GUIDesignLabelIcon32x32Thicked      (FRAME_THICK | ICON_BEFORE_TEXT | LAYOUT_FIX_HEIGHT | LAYOUT_FIX_WIDTH), 0, 0, 24, GUIDesignHeight, 2, 2, 2, 2

/// @brief label ticked filled used for VClasses. It uses icons of 64x32 pixels
#define GUIDesignLabelIcon64x32Thicked      (FRAME_THICK | ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 94, 46, 2, 2, 2, 2

/// @brief design for label with icon
#define GUIDesignLabelIcon                  (LAYOUT_CENTER_Y | LAYOUT_CENTER_X), 0, 0, 0, 0, 2, 2, 0, 0

/// @brief label ticked filled extended over frame used for VClasses/VShapes. (can be used by icons of 64x32 pixels)
#define GUIDesignLabelTickedIcon180x46      (FRAME_THICK | ICON_BEFORE_TEXT | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT | LAYOUT_FIX_WIDTH), 0, 0, 180, 46, 2, 2, 2, 2

/// @brief label extended over frame without thick and with text justify to left, used to show information in frames
#define GUIDesignLabelFrameInformation      (JUSTIFY_LEFT | LAYOUT_FILL_X | ICON_BEFORE_TEXT), 0, 0, 0, 0, 2, 2, 2, 2

/// @brief label used in statusBar
#define GUIDesignLabelStatusBar             (LAYOUT_CENTER_Y), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief label used in MFXGroupBoxModule
#define GUIDesignLabelMFXGroupBoxModule     (FRAME_GROOVE | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @}


/// @name FXComboBox
/// @{
/// @brief Combo box editable extended over the all frame
#define GUIDesignComboBox                   (FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief Combo box static (not editable)
#define GUIDesignComboBoxStatic             (COMBOBOX_STATIC | FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | LAYOUT_FIX_HEIGHT), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief Combo box static (not editable) extended over the all frame
#define GUIDesignComboBoxStaticExtended     (COMBOBOX_STATIC | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief Combo box static     (cannot be edited) extended over the matrix column
#define GUIDesignComboBoxAttribute          (COMBOBOX_NORMAL | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief comboBox with thick frame, width 180
#define GUIDesignComboBoxWidth180           (COMBOBOX_NORMAL | FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 180, GUIDesignHeight, 2, 2, 2, 2

/// @brief comboBox with thick frame, width 100
#define GUIDesignComboBoxWidth100           (COMBOBOX_NORMAL | FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 100, GUIDesignHeight, 2, 2, 2, 2

/// @brief number of column of every combo box
#define GUIDesignComboBoxNCol               1

/// @}


/// @name FXPackers
/// @{
/// @brief Horizontal frame extended over X-frame parent used in MFXGroupBoxModule
#define GUIDesignGroupBoxModule             (LAYOUT_FIX_WIDTH), 0, 0, 0, 0, 4, 4, 4, 4, 4, 4

/// @brief Horizontal frame extended over XY-frame parent used in MFXGroupBoxModule
#define GUIDesignGroupBoxModuleExtendY      (LAYOUT_FILL_X | LAYOUT_FILL_Y), 0, 0, 0, 0, 4, 4, 4, 4, 4, 4

/// @brief Thick frame extended over frame parent
#define GUIDesignFrameThick                 (FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y), 0, 0, 0, 0, 2, 2, 2, 2, 2, 2

/// @brief Horizontal frame extended over frame parent
#define GUIDesignHorizontalFrame            (LAYOUT_FILL_X), 0, 0, 0, 0, 4, 4, 4, 4, 4, 4

/// @brief Vertical frame extended over frame parent
#define GUIDesignVerticalFrame              (LAYOUT_FILL_Y), 0, 0, 0, 0, 4, 4, 4, 4, 4, 4

/// @brief Horizontal frame used for pack icons
#define GUIDesignHorizontalFrameIcons       (LAYOUT_FILL_X | FRAME_THICK), 0, 0, 0, 0, 0, 0, 0, 0, 2, 2

/// @brief Horizontal frame used in status bar
#define GUIDesignHorizontalFrameStatusBar   (LAYOUT_FILL_Y | LAYOUT_RIGHT | FRAME_SUNKEN | LAYOUT_FIX_HEIGHT), 0, 0, 20, GUIDesignHeight, 0, 0, 0, 0, 0, 0

/// @brief Matrix design extended over Frame packed by columns
#define GUIDesignMatrix                     (LAYOUT_FILL_X | LAYOUT_FILL_Y | MATRIX_BY_COLUMNS | PACK_UNIFORM_WIDTH)

/// @brief Group box design extended over frame
#define GUIDesignGroupBoxFrame              (LAYOUT_FILL_X | FRAME_GROOVE | GROUPBOX_TITLE_CENTER)

/// @brief Group box design extended over frame (X and Y)
#define GUIDesignGroupBoxFrameFill          (LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_GROOVE | GROUPBOX_TITLE_CENTER)

/// @brief Group box design for elements of width 100
#define GUIDesignGroupBoxFrame100           (LAYOUT_FIX_WIDTH | FRAME_GROOVE | GROUPBOX_TITLE_CENTER), 0, 0, 112, 0, 4, 4, 4, 4, 4, 4

/// @brief FXMatrix used to pack values in Viewsettings
#define GUIDesignMatrixViewSettings         (LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS), 0, 0, 0, 0, 10, 10, 10, 10, 5, 5

/// @brief Matrix design extended over Frame and used for represent attributes and their values (Deprecated)
#define GUIDesignMatrixAttributes           (LAYOUT_FILL_X | MATRIX_BY_COLUMNS | PACK_UNIFORM_WIDTH)

/// @brief Matrix for pack GLTypes (used in GNESelectorFrame)
#define GUIDesignMatrixLockGLTypes          (LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/// @}


/// @name General design for netedit frames
/// @{
/// @brief design for frames area
#define GUIDesignFrameArea                      (FRAME_SUNKEN | LAYOUT_SIDE_TOP | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y), 0, 0, 0, 0, DEFAULT_SPACING, GUIDesignFrameAreaMarging, DEFAULT_SPACING, DEFAULT_SPACING

/// @brief design for view area
#define GUIDesignViewnArea                      (FRAME_SUNKEN | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0

/// @brief design for scroll windows extended over Y and y
#define GUIDesignScrollWindow                   (LAYOUT_FILL_X | LAYOUT_FILL_Y | HSCROLLER_NEVER | VSCROLLER_ALWAYS), 0, 0, 0, 0

/// @brief design for scroll windows extended over Y and fix width
#define GUIDesignScrollWindowFixed              (LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y | HSCROLLER_NEVER | VSCROLLER_ALWAYS), 0, 0, 0, 0

/// @brief design for the content scroll of UndoList
#define GUIDesignContentsScrollUndoList         (LAYOUT_FILL_X | LAYOUT_FILL_Y | HSCROLLER_NEVER), 0, 0, 0, 0

/// @brief design for the main content frame of every frame/dialog
#define GUIDesignChildWindowContentFrame        (FRAME_SUNKEN | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y)

/// @brief design for the main content frame of every frame/dialog
#define GUIDesignContentsFrame                  (LAYOUT_FILL_X | LAYOUT_FILL_Y)

/// @brief design for auxiliar (Without borders) frame extended in all directions
#define GUIDesignAuxiliarFrame                  (LAYOUT_FILL_X | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/// @brief design for auxiliar (Without borders) fixed
#define GUIDesignAuxiliarFrameFixed             (LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/// @brief design for auxiliar (Without borders) frame with fixed width and extended height
#define GUIDesignAuxiliarFrameFixedWidth(width) (LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y), 0, 0, width, 0, 0, 0, 0, 0, 0, 0

/// @brief design for auxiliar (Without borders) horizontal frame used to pack another frames
#define GUIDesignAuxiliarHorizontalFrame        (LAYOUT_FILL_X), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/// @brief design for auxiliar (Without borders) horizontal frame used to pack another frames  uniform
#define GUIDesignAuxiliarHorizontalFrameUniform (LAYOUT_FILL_X | PACK_UNIFORM_WIDTH), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/// @brief design for auxiliar (Without borders) horizontal frame used to pack another frames
#define GUIDesignAuxiliarVerticalFrame          (LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/// @brief design for horizontal frame used to pack another frames with a uniform width
#define GUIDesignUniformHorizontalFrame         (LAYOUT_FILL_X | LAYOUT_FILL_Y | PACK_UNIFORM_WIDTH)

/// @brief design for CollapsableFrame (Used in MFXGroupBoxModule)
#define GUIDesignCollapsableFrame               (LAYOUT_FILL_X | LAYOUT_FILL_Y), 0, 0, 0, 0, 2, 2, 2, 2, 2, 2

/// @brief design for auxiliar vertical frames with fixed width (used in TLSTable and DecalsTable)
#define GUIDesignAuxiliarFrameFixWidth          (LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/// @brief design for auxiliar vertical frames with fixed height (DecalsTable)
#define GUIDesignAuxiliarFrameFixHeight         (LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

/// @}


/// @name designs for toolbars
/// @{
/// @brief design for top toolbar (file, edit, processing...)
#define GUIDesignToolbarMenuBar             (LAYOUT_SIDE_TOP | LAYOUT_FILL_X | FRAME_RAISED)

/// @brief design for top toolbar (file, edit, processing...) used only in netedit (due supermodes buttons)
#define GUIDesignToolbarMenuBarNetedit      (LAYOUT_SIDE_TOP | FRAME_RAISED)

/// @brief design for default toolbar
#define GUIDesignToolBar                    (FRAME_NORMAL)

/// @brief design for toolbar
#define GUIDesignToolBarRaisedSame          (LAYOUT_DOCK_SAME | FRAME_RAISED)

/// @brief design for first toolbar shell positioned in the next position of dock
#define GUIDesignToolBarRaisedNext          (LAYOUT_DOCK_NEXT | FRAME_RAISED)

/// @brief design used in status bar
#define GUIDesignStatusBar                  (LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | FRAME_RAISED)

/// @brief design for toolbar grip (used to change the position of toolbar with mouse)
#define GUIDesignToolBarGrip                (TOOLBARGRIP_DOUBLE)

/// @brief design for first toolbar shell positioned in the next-top position of dock
#define GUIDesignToolBarRaisedNextTop       (LAYOUT_DOCK_NEXT | LAYOUT_SIDE_TOP | FRAME_RAISED)

/// @brief design for first toolbar shell positioned in the same  position of dock
#define GUIDesignToolBarRaisedSameTop       (LAYOUT_DOCK_SAME | LAYOUT_SIDE_TOP | FRAME_RAISED)

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
#define GUIDesignSplitter       (LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_RAISED | FRAME_THICK | SPLITTER_TRACKING)

/// @brief MDI Splitter
#define GUIDesignSplitterMDI    (LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_SUNKEN | FRAME_THICK)

/// @}


/// @name designs for Spin dials
/// @{
/// @brief design for standard spin dial
#define GUIDesignSpinDial               (FRAME_THICK | FRAME_SUNKEN | LAYOUT_TOP | LAYOUT_CENTER_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 100, GUIDesignHeight, 0, 0, 0, 0

/// @brief design for standard spin dial
#define GUIDesignSpinDialViewPortZoom   (FRAME_THICK | FRAME_SUNKEN | LAYOUT_TOP | LAYOUT_CENTER_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 100, GUIDesignHeight, 0, 0, 0, 0

/// @brief design for standard spin dial
#define GUIDesignSpinDialViewPort       (FRAME_THICK | FRAME_SUNKEN | LAYOUT_TOP | LAYOUT_CENTER_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | SPIN_NOMIN | SPIN_NOMAX), 0, 0, 100, GUIDesignHeight, 0, 0, 0, 0

/// @brief design for spin dial attribute
#define GUIDesignSpinDialAttribute       (FRAME_THICK | FRAME_SUNKEN | LAYOUT_TOP | LAYOUT_CENTER_Y | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT | SPIN_NOMIN | SPIN_NOMAX), 0, 0, 0, GUIDesignHeight, 0, 0, 0, 0

/// @brief design for standard spin dial
#define GUIDesignSpinDialDecalsTable    (FRAME_THICK | FRAME_SUNKEN | JUSTIFY_CENTER_X | JUSTIFY_CENTER_Y | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | SPIN_NOMIN | SPIN_NOMAX), 0, 0, 100, GUIDesignHeight, 0, 0, 0, 0

/// @}


/// @name designs for sliders
/// @{
/// @brief design for standard slider
#define GUIDesignSlider (SLIDER_ARROW_UP | SLIDER_TICKS_TOP | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT), 0, 0, 300, GUIDesignHeight, 0, 0, 0

/// @}


/// @name design for view settings dialog
/// @{
/// @brief dialog
#define GUIDesignViewSettingsMainDialog         (DECOR_TITLE | DECOR_BORDER | DECOR_RESIZE), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

#define GUIDesignViewSettingsDialog             (DECOR_TITLE | DECOR_BORDER)

/// @brief vertical frames
#define GUIDesignViewSettingsVerticalFrame1     (LAYOUT_FILL_X | LAYOUT_SIDE_TOP | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0, 2, 2

#define GUIDesignViewSettingsVerticalFrame2     (LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_Y), 0, 0, 0, 0, 0, 0, 0, 0, 2, 2

#define GUIDesignViewSettingsVerticalFrame3     (LAYOUT_FILL_X | LAYOUT_FILL_Y), 0, 0, 0, 0, 10, 10, 10, 10, 5, 5

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

/// @brief Combo boxes
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

/// @brief decals table
#define GUIDesignViewSettingsDecalsTable        (LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH), 0, 0, 550, 0

/// @brief decals cell
#define GUIDesignViewSettingsDecalsCellTable    (JUSTIFY_CENTER_X | JUSTIFY_TOP)

/// @}


/// @name design for dialog boxes
/// @{
/// @brief design for standard dialog box with close button (for example, about dialog)
#define GUIDesignDialogBox                                      (DECOR_CLOSE | DECOR_TITLE)

/// @brief design for standard dialog box that can be stretched (But not shrunk)
#define GUIDesignDialogBoxStretchable                           (DECOR_CLOSE | DECOR_TITLE | DECOR_STRETCHABLE)

/// @brief design for standard dialog box that can be shrunk (But not stretched)
#define GUIDesignDialogBoxShrinkable                            (DECOR_CLOSE | DECOR_TITLE | DECOR_SHRINKABLE)

/// @brief design for standard dialog box (for example, about dialog)
#define GUIDesignDialogBoxResizable                             (DECOR_CLOSE | DECOR_TITLE | DECOR_RESIZE)

/// @brief design for standard dialog box (for example, about dialog)
#define GUIDesignAuxiliarDialogBoxResizable                     (DECOR_CLOSE | DECOR_TITLE | DECOR_RESIZE), 0, 0, 0, 0, 0, 0, 0, 0

/// @brief design for dialog box with specific width and height (for example, additional dialogs)
#define GUIDesignDialogBoxExplicit(width, height)               (DECOR_CLOSE | DECOR_TITLE | LAYOUT_EXPLICIT), 0, 0, width, height, 4, 4, 4, 4, 4, 4

/// @brief design for dialog box with specific width and height that can be stretched (But not shrunk)
#define GUIDesignDialogBoxExplicitStretchable(width, height)    (DECOR_CLOSE | DECOR_TITLE | LAYOUT_EXPLICIT | DECOR_STRETCHABLE), 0, 0, width, height, 4, 4, 4, 4, 4, 4

/// @}


/// @name design for FXTables and their cells
/// @{
/// @brief design for table extended over frame
#define GUIDesignTable                  (LAYOUT_FILL_X | LAYOUT_FILL_Y)

/// @brief design for table extended over frame that cannot be edited
#define GUIDesignTableNotEditable       (LAYOUT_FILL_X | LAYOUT_FILL_Y | TABLE_READONLY)

/// @brief design for table extended over frame but with limited Height
#define GUIDesignTableLimitedHeight     (LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT)

/// @brief design for tables used in additional dialogs
#define GUIDesignTableAdditionals       (LAYOUT_FILL_X | LAYOUT_FILL_Y | VSCROLLER_ALWAYS | HSCROLLER_NEVER | TABLE_NO_ROWSELECT | TABLE_NO_COLSELECT), 0, 0, 0, 0, 2, 2, 2, 2

/// @brief design for tables used in GNEFixDemandElements dialogs
#define GUIDesignTableFixElements       (LAYOUT_FILL_X | LAYOUT_FILL_Y | VSCROLLER_ALWAYS | HSCROLLER_NEVER | TABLE_NO_ROWSELECT | TABLE_NO_COLSELECT), 0, 0, 0, 0, 2, 2, 2, 2

/// @}


/// @name design for Chooser (locator) and breakpoint dialogs
/// @{
/// @brief design for Chooser dialog specifying size
#define GUIDesignChooserDialog                  (DECOR_ALL), 20, 40, 300, 350

/// @brief design for Chooser buttons
#define GUIDesignChooserButtons                 (ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED), 0, 0, 150, GUIDesignHeight, 2, 2, 2, 2

/// @brief design for Chooser TextField
#define GUIDesignChooserTextField               (LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_SUNKEN), 0, 0, 0, GUIDesignHeight, 2, 2, 2, 2

/// @brief design for Chooser List
#define GUIDesignChooserListSingle              (LIST_SINGLESELECT | LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_SUNKEN | FRAME_THICK), 0, 0, 0, 0

/// @brief design for Chooser List
#define GUIDesignChooserListMultiple            (LIST_MULTIPLESELECT | LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_SUNKEN | FRAME_THICK), 0, 0, 0, 0

/// @brief design for Breakpoint table
#define GUIDesignBreakpointTable                (LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_SUNKEN | FRAME_THICK), 0, 0, 0, 0

/// @brief design for Chooser Layout left
#define GUIDesignChooserLayoutLeft              (LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP), 0, 0, 0, 0, 4, 4, 4, 4

/// @brief design for Chooser Layout right
#define GUIDesignChooserLayoutRight             (LAYOUT_TOP), 0, 0, 0, 0, 4, 4, 4, 4

/// @brief design for Chooser  Layout list
#define GUIDesignChooserLayoutList              (LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP | FRAME_THICK | FRAME_SUNKEN), 0, 0, 0, 0, 0, 0, 0, 0

/// @}


/// @name design for MFXTreeListDynamic
/// @{
/// @brief treeListDinamic used in Inspector frame
#define GUIDesignTreeListDinamic                (TREELIST_SHOWS_LINES | TREELIST_SHOWS_BOXES | TREELIST_SINGLESELECT | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT)

/// @}


/// @brief Tree list used in frames to represent elements children
#define GUIDesignTreeListFrame                  (TREELIST_SHOWS_LINES | TREELIST_SHOWS_BOXES | TREELIST_SINGLESELECT | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, 200

/// @brief design for FXLists
#define GUIDesignList                           (LIST_NORMAL | FRAME_NORMAL | LAYOUT_FILL_X)

/// @brief design for FXLists with height fixed
#define GUIDesignListFixedHeight                (LIST_NORMAL | FRAME_NORMAL | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT)

/// @brief design for FXLists that only allow a single selected elements selected and height fixed
#define GUIDesignListSingleElementFixedHeight   (LIST_SINGLESELECT | FRAME_NORMAL | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT), 0, 0, 0, 100

/// @brief design for FXLists
#define GUIDesignListExtended                   (LIST_NORMAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y)

/// @brief design for TabBooks
#define GUIDesignTabBook                        (TABBOOK_LEFTTABS | PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT | LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_RIGHT)

// ===========================================================================
// Implementation
// ===========================================================================

class GUIDesigns {

public:
    /// @brief build menu title
    static FXMenuTitle* buildFXMenuTitle(FXComposite* p, const std::string& text, FXIcon* icon, FXMenuPane* menuPane);

    /// @brief build menu command
    static FXMenuCommand* buildFXMenuCommandShortcut(FXComposite* p, const std::string& text, const std::string& shortcut, const std::string& info, FXIcon* icon, FXObject* tgt, FXSelector sel);

    /// @brief build menu checkbox
    static FXMenuCheck* buildFXMenuCheckbox(FXComposite* p, const std::string& text, const std::string& info, FXObject* tgt, FXSelector sel);

    /// @brief build menu checkbox with icon
    static MFXMenuCheckIcon* buildFXMenuCheckboxIcon(FXComposite* p, const std::string& text, const std::string& shortcut, const std::string& info, FXIcon* icon, FXObject* tgt, FXSelector sel);

    /// @brief build menu command
    static FXMenuCommand* buildFXMenuCommand(FXComposite* p, const std::string& text, FXIcon* icon, FXObject* tgt, FXSelector sel);

    /// @brief build menu command (for recent files)
    static FXMenuCommand* buildFXMenuCommandRecentFile(FXComposite* p, const std::string& text, FXObject* tgt, FXSelector sel);
};
