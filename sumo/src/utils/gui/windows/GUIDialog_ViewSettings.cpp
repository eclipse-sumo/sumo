/****************************************************************************/
/// @file    GUIDialog_ViewSettings.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Wed, 21. Dec 2005
/// @version $Id$
///
// The dialog to change the view (gui) settings.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fstream>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/RGBColor.h>
#include <utils/foxtools/MFXAddEditTypedTable.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/TplConvert.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/importio/LineReader.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include "GUIDialog_ViewSettings.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_ViewSettings) GUIDialog_ViewSettingsMap[] = {
    FXMAPFUNC(SEL_CHANGED,  MID_SIMPLE_VIEW_COLORCHANGE,    GUIDialog_ViewSettings::onCmdColorChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_COLORCHANGE,    GUIDialog_ViewSettings::onCmdColorChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_NAMECHANGE,     GUIDialog_ViewSettings::onCmdNameChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_OK,                GUIDialog_ViewSettings::onCmdOk),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_CANCEL,            GUIDialog_ViewSettings::onCmdCancel),
    FXMAPFUNC(SEL_CHANGED,  MFXAddEditTypedTable::ID_TEXT_CHANGED,  GUIDialog_ViewSettings::onCmdEditTable),

    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_SAVE,    GUIDialog_ViewSettings::onCmdSaveSetting),
    FXMAPFUNC(SEL_UPDATE,  MID_SIMPLE_VIEW_SAVE,    GUIDialog_ViewSettings::onUpdSaveSetting),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_DELETE,    GUIDialog_ViewSettings::onCmdDeleteSetting),
    FXMAPFUNC(SEL_UPDATE,  MID_SIMPLE_VIEW_DELETE,    GUIDialog_ViewSettings::onUpdDeleteSetting),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_EXPORT,    GUIDialog_ViewSettings::onCmdExportSetting),
    FXMAPFUNC(SEL_UPDATE,  MID_SIMPLE_VIEW_EXPORT,    GUIDialog_ViewSettings::onUpdExportSetting),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_IMPORT,    GUIDialog_ViewSettings::onCmdImportSetting),
    FXMAPFUNC(SEL_UPDATE,  MID_SIMPLE_VIEW_IMPORT,    GUIDialog_ViewSettings::onUpdImportSetting),

    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_LOAD_DECALS,    GUIDialog_ViewSettings::onCmdLoadDecals),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_SAVE_DECALS,    GUIDialog_ViewSettings::onCmdSaveDecals),

};


FXIMPLEMENT(GUIDialog_ViewSettings, FXDialogBox, GUIDialog_ViewSettingsMap, ARRAYNUMBER(GUIDialog_ViewSettingsMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_ViewSettings::GUIDialog_ViewSettings(GUISUMOAbstractView* parent,
        GUIVisualizationSettings* settings,
        std::vector<GUISUMOAbstractView::Decal>* decals,
        MFXMutex* decalsLock) :
    FXDialogBox(parent, "View Settings", DECOR_TITLE | DECOR_BORDER, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
    myParent(parent), mySettings(settings),
    myDecals(decals), myDecalsLock(decalsLock), myDecalsTable(0) {
    myBackup = (*mySettings);

    FXVerticalFrame* contentFrame =
        new FXVerticalFrame(this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y,
                            0, 0, 0, 0, 0, 0, 0, 0, 2, 2);
    //
    {
        FXHorizontalFrame* frame0 =
            new FXHorizontalFrame(contentFrame, FRAME_THICK, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2);
        mySchemeName = new FXComboBox(frame0, 20, this, MID_SIMPLE_VIEW_NAMECHANGE, COMBOBOX_INSERT_LAST | FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_CENTER_Y | COMBOBOX_STATIC);
        const std::vector<std::string>& names = gSchemeStorage.getNames();
        for (std::vector<std::string>::const_iterator i = names.begin(); i != names.end(); ++i) {
            size_t index = mySchemeName->appendItem((*i).c_str());
            if ((*i) == mySettings->name) {
                mySchemeName->setCurrentItem((FXint) index);
            }
        }
        mySchemeName->setNumVisible(5);

        new FXButton(frame0, "\t\tSave the setting to registry",
                     GUIIconSubSys::getIcon(ICON_SAVEDB), this, MID_SIMPLE_VIEW_SAVE,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
        new FXButton(frame0, "\t\tRemove the setting from registry",
                     GUIIconSubSys::getIcon(ICON_REMOVEDB), this, MID_SIMPLE_VIEW_DELETE,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
        new FXButton(frame0, "\t\tExport setting to file",
                     GUIIconSubSys::getIcon(ICON_SAVE), this, MID_SIMPLE_VIEW_EXPORT,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);
        new FXButton(frame0, "\t\tLoad setting from file",
                     GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, MID_SIMPLE_VIEW_IMPORT,
                     ICON_ABOVE_TEXT | BUTTON_TOOLBAR | FRAME_RAISED | LAYOUT_TOP | LAYOUT_LEFT);

    }
    //
    FXTabBook* tabbook =
        new FXTabBook(contentFrame, 0, 0, TABBOOK_LEFTTABS | PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT | LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_RIGHT,
                      0, 0, 0, 0, 0, 0, 0, 0);
    {
        // tab for the background
        new FXTabItem(tabbook, "Background", NULL, TAB_LEFT_NORMAL, 0, 0, 0, 0, 4, 8, 4, 4);
        FXVerticalFrame* frame1 =
            new FXVerticalFrame(tabbook, FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2);

        FXMatrix* m11 =
            new FXMatrix(frame1, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        new FXLabel(m11, "Color", 0, LAYOUT_CENTER_Y);
        myBackgroundColor = new FXColorWell(m11, MFXUtils::getFXColor(settings->backgroundColor),
                                            this, MID_SIMPLE_VIEW_COLORCHANGE,
                                            LAYOUT_FIX_WIDTH | LAYOUT_CENTER_Y | LAYOUT_SIDE_TOP | FRAME_SUNKEN | FRAME_THICK | ICON_AFTER_TEXT,
                                            0, 0, 100, 0,   0, 0, 0, 0);

        new FXHorizontalSeparator(frame1, SEPARATOR_GROOVE | LAYOUT_FILL_X);

        FXVerticalFrame* frame11 =
            new FXVerticalFrame(frame1, LAYOUT_FILL_Y, 0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        new FXLabel(frame11, "Decals:");
        myDecalsFrame = new FXVerticalFrame(frame11);
        FXHorizontalFrame* frame111 = new FXHorizontalFrame(frame11, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH, 0, 0, 0, 0, 10, 10, 5, 5);
        new FXButton(frame111, "&Load Decals", NULL, this, MID_SIMPLE_VIEW_LOAD_DECALS, BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X, 0, 0, 0, 0, 20, 20, 4, 4);
        new FXButton(frame111, "&Save Decals", NULL, this, MID_SIMPLE_VIEW_SAVE_DECALS, BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X, 0, 0, 0, 0, 20, 20, 4, 4);

        new FXHorizontalSeparator(frame1, SEPARATOR_GROOVE | LAYOUT_FILL_X);

        FXMatrix* m12 =
            new FXMatrix(frame1, 2, LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        myShowGrid =
            new FXCheckButton(m12, "Show grid", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y | CHECKBUTTON_NORMAL);
        myShowGrid->setCheck(mySettings->showGrid);
        new FXLabel(m12, "");
        FXMatrix* m121 =
            new FXMatrix(m12, 2, LAYOUT_CENTER_Y | LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
        new FXLabel(m121, "x-spacing", 0, LAYOUT_CENTER_Y);
        myGridXSizeDialer =
            new FXRealSpinDial(m121, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_CENTER_Y | LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myGridXSizeDialer->setRange(1, 10000);
        myGridXSizeDialer->setValue(mySettings->gridXSize);
        FXMatrix* m122 =
            new FXMatrix(m12, 2, LAYOUT_CENTER_Y | LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
        new FXLabel(m122, "y-spacing", 0, LAYOUT_CENTER_Y);
        myGridYSizeDialer =
            new FXRealSpinDial(m122, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_CENTER_Y | LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myGridYSizeDialer->setRange(1, 10000);
        myGridYSizeDialer->setValue(mySettings->gridXSize);
    }
    {
        // tab for the streets
        new FXTabItem(tabbook, "Streets", NULL, TAB_LEFT_NORMAL, 0, 0, 0, 0, 4, 8, 4, 4);

        FXVerticalFrame* frame2 =
            new FXVerticalFrame(tabbook, FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2);
        FXMatrix* m21 =
            new FXMatrix(frame2, 3, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 2, 5, 5);
        new FXLabel(m21, "Color", 0, LAYOUT_CENTER_Y);
        myLaneEdgeColorMode = new FXComboBox(m21, 30, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC);
        myLaneEdgeColorMode->setNumVisible(10);
        myLaneColorInterpolation = new FXCheckButton(m21, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y | CHECKBUTTON_NORMAL);
        FXScrollWindow* genScroll = new FXScrollWindow(frame2, LAYOUT_FILL_X | LAYOUT_SIDE_TOP | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_HEIGHT, 0, 0, 0, 80);
        myLaneColorSettingFrame =
            new FXVerticalFrame(genScroll, LAYOUT_FILL_X | LAYOUT_FILL_Y,  0, 0, 0, 0, 10, 10, 2, 8, 5, 2);
//we should insert a FXScrollWindow around the frame2
#ifdef HAVE_INTERNAL
        if (GUIVisualizationSettings::UseMesoSim) {
            mySettings->edgeColorer.fill(*myLaneEdgeColorMode);
        } else {
#endif
            mySettings->laneColorer.fill(*myLaneEdgeColorMode);
#ifdef HAVE_INTERNAL
        }
#endif

        new FXHorizontalSeparator(frame2, SEPARATOR_GROOVE | LAYOUT_FILL_X);
        FXMatrix* m22 =
            new FXMatrix(frame2, 2, LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        myShowLaneBorders = new FXCheckButton(m22, "Show lane borders", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowLaneBorders->setCheck(mySettings->laneShowBorders);
        new FXLabel(m22, " ", 0, LAYOUT_CENTER_Y);
        myShowLaneDecals = new FXCheckButton(m22, "Show link decals", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowLaneDecals->setCheck(mySettings->showLinkDecals);
        new FXLabel(m22, " ", 0, LAYOUT_CENTER_Y);
        myShowRails = new FXCheckButton(m22, "Show rails", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowRails->setCheck(mySettings->showRails);
        new FXLabel(m22, " ", 0, LAYOUT_CENTER_Y);
        myHideMacroConnectors = new FXCheckButton(m22, "Hide macro connectors", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myHideMacroConnectors->setCheck(mySettings->hideConnectors);
        new FXLabel(m22, " ", 0, LAYOUT_CENTER_Y);
        new FXLabel(m22, "Exaggerate width by", 0, LAYOUT_CENTER_Y);
        myLaneWidthUpscaleDialer =
            new FXRealSpinDial(m22, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myLaneWidthUpscaleDialer->setRange(0, 10000);
        myLaneWidthUpscaleDialer->setValue(mySettings->laneWidthExaggeration);

        // edge name
        myEdgeNamePanel = new NamePanel(m22, this, "Show edge name", mySettings->edgeName);
        myStreetNamePanel = new NamePanel(m22, this, "Show street name", mySettings->streetName);
        myInternalEdgeNamePanel = new NamePanel(m22, this, "Show internal edge name", mySettings->internalEdgeName);
    }
    {
        // vehicles
        new FXTabItem(tabbook, "Vehicles", NULL, TAB_LEFT_NORMAL, 0, 0, 0, 0, 4, 8, 4, 4);
        FXVerticalFrame* frame3 =
            new FXVerticalFrame(tabbook, FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2);

        FXMatrix* m31 =
            new FXMatrix(frame3, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 2, 5, 5);
        new FXLabel(m31, "Show As", 0, LAYOUT_CENTER_Y);
        myVehicleShapeDetail = new FXComboBox(m31, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC);
        myVehicleShapeDetail->appendItem("'triangles'");
        myVehicleShapeDetail->appendItem("'boxes'");
        myVehicleShapeDetail->appendItem("'simple shapes'");
        myVehicleShapeDetail->appendItem("'raster images'");
        myVehicleShapeDetail->setNumVisible(4);
        myVehicleShapeDetail->setCurrentItem(settings->vehicleQuality);

        new FXHorizontalSeparator(frame3, SEPARATOR_GROOVE | LAYOUT_FILL_X);

        FXMatrix* m32 =
            new FXMatrix(frame3, 3, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 2, 5, 5);
        new FXLabel(m32, "Color", 0, LAYOUT_CENTER_Y);
        myVehicleColorMode = new FXComboBox(m32, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC);
        mySettings->vehicleColorer.fill(*myVehicleColorMode);
        myVehicleColorMode->setNumVisible(10);
        myVehicleColorInterpolation = new FXCheckButton(m32, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y | CHECKBUTTON_NORMAL);

        FXScrollWindow* genScroll = new FXScrollWindow(frame3, LAYOUT_FILL_X | LAYOUT_SIDE_TOP | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_HEIGHT, 0, 0, 0, 80);
        myVehicleColorSettingFrame =
            new FXVerticalFrame(genScroll, LAYOUT_FILL_X | LAYOUT_FILL_Y,  0, 0, 0, 0, 10, 10, 2, 8, 5, 2);

        new FXHorizontalSeparator(frame3, SEPARATOR_GROOVE | LAYOUT_FILL_X);

        FXMatrix* m33 =
            new FXMatrix(frame3, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        myShowBlinker = new FXCheckButton(m33, "Show blinker / brake lights", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowBlinker->setCheck(mySettings->showBlinker);
        new FXLabel(m33, " ", 0, LAYOUT_CENTER_Y);
        myShowMinGap = new FXCheckButton(m33, "Show minimum gap", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowMinGap->setCheck(mySettings->drawMinGap);
        new FXLabel(m33, " ", 0, LAYOUT_CENTER_Y);
        /*
        myShowLaneChangePreference = new FXCheckButton(m33, "Show lane change preference", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowLaneChangePreference->setCheck(mySettings->drawLaneChangePreference);
        new FXLabel(m33, " ", 0, LAYOUT_CENTER_Y);
        */
        myVehicleNamePanel = new NamePanel(m33, this, "Show vehicle name", mySettings->vehicleName);
        /*
        FXCheckButton *tmpc = new FXCheckButton(m33, "Show braking lights", 0 ,0);
        tmpc->disable();
        tmpc = new FXCheckButton(m33, "Show needed headway", 0 ,0);
        tmpc->disable();
        */

        new FXHorizontalSeparator(frame3, SEPARATOR_GROOVE | LAYOUT_FILL_X);

        FXMatrix* m34 =
            new FXMatrix(frame3, 2, LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        FXMatrix* m341 =
            new FXMatrix(m34, 2, LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
        new FXLabel(m341, "Minimum size", 0, LAYOUT_CENTER_Y);
        myVehicleMinSizeDialer =
            new FXRealSpinDial(m341, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myVehicleMinSizeDialer->setValue(mySettings->minVehicleSize);
        FXMatrix* m342 =
            new FXMatrix(m34, 2, LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
        new FXLabel(m342, "Exaggerate by", 0, LAYOUT_CENTER_Y);
        myVehicleUpscaleDialer =
            new FXRealSpinDial(m342, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myVehicleUpscaleDialer->setRange(0, 10000);
        myVehicleUpscaleDialer->setValue(mySettings->vehicleExaggeration);
    }

    {
        // persons
        new FXTabItem(tabbook, "Persons", NULL, TAB_LEFT_NORMAL, 0, 0, 0, 0, 4, 8, 4, 4);
        FXVerticalFrame* frame3 =
            new FXVerticalFrame(tabbook, FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2);

        FXMatrix* m101 =
            new FXMatrix(frame3, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 2, 5, 5);
        new FXLabel(m101, "Show As", 0, LAYOUT_CENTER_Y);
        myPersonShapeDetail = new FXComboBox(m101, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC);
        myPersonShapeDetail->appendItem("'triangles'");
        myPersonShapeDetail->appendItem("'boxes'");
        myPersonShapeDetail->appendItem("'simple shapes'");
        myPersonShapeDetail->appendItem("'raster images'");
        myPersonShapeDetail->setNumVisible(4);
        myPersonShapeDetail->setCurrentItem(settings->personQuality);

        new FXHorizontalSeparator(frame3, SEPARATOR_GROOVE | LAYOUT_FILL_X);

        FXMatrix* m102 =
            new FXMatrix(frame3, 3, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 2, 5, 5);
        new FXLabel(m102, "Color", 0, LAYOUT_CENTER_Y);
        myPersonColorMode = new FXComboBox(m102, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC);
        mySettings->personColorer.fill(*myPersonColorMode);
        myPersonColorMode->setNumVisible(10);
        myPersonColorInterpolation = new FXCheckButton(m102, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y | CHECKBUTTON_NORMAL);

        FXScrollWindow* genScroll = new FXScrollWindow(frame3, LAYOUT_FILL_X | LAYOUT_SIDE_TOP | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_HEIGHT, 0, 0, 0, 80);
        myPersonColorSettingFrame =
            new FXVerticalFrame(genScroll, LAYOUT_FILL_X | LAYOUT_FILL_Y,  0, 0, 0, 0, 10, 10, 2, 8, 5, 2);

        new FXHorizontalSeparator(frame3, SEPARATOR_GROOVE | LAYOUT_FILL_X);

        FXMatrix* m103 =
            new FXMatrix(frame3, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        myPersonNamePanel = new NamePanel(m103, this, "Show person name", mySettings->personName);

        new FXHorizontalSeparator(frame3, SEPARATOR_GROOVE | LAYOUT_FILL_X);

        FXMatrix* m104 =
            new FXMatrix(frame3, 2, LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        FXMatrix* m1041 =
            new FXMatrix(m104, 2, LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
        new FXLabel(m1041, "Minimum size", 0, LAYOUT_CENTER_Y);
        myPersonMinSizeDialer =
            new FXRealSpinDial(m1041, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myPersonMinSizeDialer->setValue(mySettings->minPersonSize);
        FXMatrix* m1042 =
            new FXMatrix(m104, 2, LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
        new FXLabel(m1042, "Exaggerate by", 0, LAYOUT_CENTER_Y);
        myPersonUpscaleDialer =
            new FXRealSpinDial(m1042, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myPersonUpscaleDialer->setRange(0, 10000);
        myPersonUpscaleDialer->setValue(mySettings->personExaggeration);
    }
    {
        // nodes
        new FXTabItem(tabbook, "Junctions", NULL, TAB_LEFT_NORMAL, 0, 0, 0, 0, 4, 8, 4, 4);
        FXVerticalFrame* frame4 =
            new FXVerticalFrame(tabbook, FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2);
        FXMatrix* m41 =
            new FXMatrix(frame4, 3, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 2, 5, 5);
        new FXLabel(m41, "Color", 0, LAYOUT_CENTER_Y);
        myJunctionColorMode = new FXComboBox(m41, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC);
        mySettings->junctionColorer.fill(*myJunctionColorMode);
        myJunctionColorMode->setNumVisible(2);
        myJunctionColorInterpolation = new FXCheckButton(m41, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y | CHECKBUTTON_NORMAL);

        FXScrollWindow* genScroll = new FXScrollWindow(frame4, LAYOUT_FILL_X | LAYOUT_SIDE_TOP | FRAME_RAISED | FRAME_THICK | LAYOUT_FIX_HEIGHT, 0, 0, 0, 80);
        myJunctionColorSettingFrame =
            new FXVerticalFrame(genScroll, LAYOUT_FILL_X | LAYOUT_FILL_Y,  0, 0, 0, 0, 10, 10, 2, 8, 5, 2);

        new FXHorizontalSeparator(frame4, SEPARATOR_GROOVE | LAYOUT_FILL_X);
        FXMatrix* m42 =
            new FXMatrix(frame4, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        myShowTLIndex = new FXCheckButton(m42, "Show link tls index", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowTLIndex->setCheck(mySettings->drawLinkTLIndex);
        new FXLabel(m42, " ", 0, LAYOUT_CENTER_Y);
        myShowJunctionIndex = new FXCheckButton(m42, "Show link junction index", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowJunctionIndex->setCheck(mySettings->drawLinkJunctionIndex);
        new FXLabel(m42, " ", 0, LAYOUT_CENTER_Y);
        myShowLane2Lane = new FXCheckButton(m42, "Show lane to lane connections", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowLane2Lane->setCheck(mySettings->showLane2Lane);
        new FXLabel(m42, " ", 0, LAYOUT_CENTER_Y);
        myJunctionNamePanel = new NamePanel(m42, this, "Show junction name", mySettings->junctionName);
        myInternalJunctionNamePanel = new NamePanel(m42, this, "Show internal junction name", mySettings->internalJunctionName);
    } {
        new FXTabItem(tabbook, "Detectors/Trigger", NULL, TAB_LEFT_NORMAL, 0, 0, 0, 0, 4, 8, 4, 4);
        FXVerticalFrame* frame5 =
            new FXVerticalFrame(tabbook, FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2);

        FXMatrix* m51 =
            new FXMatrix(frame5, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        myAddNamePanel = new NamePanel(m51, this, "Show detector name", mySettings->addName);
        new FXHorizontalSeparator(frame5 , SEPARATOR_GROOVE | LAYOUT_FILL_X);

        FXMatrix* m52 =
            new FXMatrix(frame5, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        FXMatrix* m521 =
            new FXMatrix(m52, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
        new FXLabel(m521, "Minimum size", 0, LAYOUT_CENTER_Y);
        myDetectorMinSizeDialer =
            new FXRealSpinDial(m521, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        FXMatrix* m522 =
            new FXMatrix(m52, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
        myDetectorMinSizeDialer->setValue(mySettings->minAddSize);
        new FXLabel(m522, "Exaggerate by", 0, LAYOUT_CENTER_Y);
        myDetectorUpscaleDialer =
            new FXRealSpinDial(m522, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myDetectorUpscaleDialer->setRange(0, 10000);
        myDetectorUpscaleDialer->setValue(mySettings->addExaggeration);


        /*
        new FXLabel(m522, "Color", 0, LAYOUT_CENTER_Y);
        myDetectorNameColor = new FXColorWell(m522, MFXUtils::getFXColor(settings->addNameColor),
                            this, MID_SIMPLE_VIEW_COLORCHANGE,
                            LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                            0, 0, 100, 0,   0, 0, 0, 0);
                            */
    } {
        new FXTabItem(tabbook, "POIs", NULL, TAB_LEFT_NORMAL, 0, 0, 0, 0, 4, 8, 4, 4);
        FXVerticalFrame* frame6 =
            new FXVerticalFrame(tabbook, FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2);

        FXMatrix* m61 =
            new FXMatrix(frame6, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        myPOINamePanel = new NamePanel(m61, this, "Show poi names", mySettings->poiName);
        new FXHorizontalSeparator(frame6 , SEPARATOR_GROOVE | LAYOUT_FILL_X);

        FXMatrix* m62 =
            new FXMatrix(frame6, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        FXMatrix* m621 =
            new FXMatrix(m62, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
        new FXLabel(m621, "Minimum size to show", 0, LAYOUT_CENTER_Y);
        myPOIMinSizeDialer =
            new FXRealSpinDial(m621, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        FXMatrix* m622 =
            new FXMatrix(m62, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
        myPOIMinSizeDialer->setValue(mySettings->minPOISize);
        new FXLabel(m622, "Exaggerate by", 0, LAYOUT_CENTER_Y);
        myPOIUpscaleDialer =
            new FXRealSpinDial(m622, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myPOIUpscaleDialer->setRange(0, 10000);
        myPOIUpscaleDialer->setValue(mySettings->addExaggeration);


    } {
        new FXTabItem(tabbook, "Polygons", NULL, TAB_LEFT_NORMAL, 0, 0, 0, 0, 4, 8, 4, 4);
        FXVerticalFrame* frame9 =
            new FXVerticalFrame(tabbook, FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2);

        FXMatrix* m91 =
            new FXMatrix(frame9, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        myPolyNamePanel = new NamePanel(m91, this, "Show polygon names", mySettings->polyName);
        new FXHorizontalSeparator(frame9 , SEPARATOR_GROOVE | LAYOUT_FILL_X);

        FXMatrix* m92 =
            new FXMatrix(frame9, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        FXMatrix* m921 =
            new FXMatrix(m92, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
        new FXLabel(m921, "Minimum size to show", 0, LAYOUT_CENTER_Y);
        myPolyMinSizeDialer =
            new FXRealSpinDial(m921, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        FXMatrix* m922 =
            new FXMatrix(m92, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
        myPolyMinSizeDialer->setValue(mySettings->minPolySize);
        new FXLabel(m922, "Exaggerate by", 0, LAYOUT_CENTER_Y);
        myPolyUpscaleDialer =
            new FXRealSpinDial(m922, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        myPolyUpscaleDialer->setRange(0, 10000);
        myPolyUpscaleDialer->setValue(mySettings->addExaggeration);


    }{
        new FXTabItem(tabbook, "Legend", NULL, TAB_LEFT_NORMAL, 0, 0, 0, 0, 4, 8, 4, 4);
        FXVerticalFrame* frame7 =
            new FXVerticalFrame(tabbook, FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2);

        FXMatrix* m72 =
            new FXMatrix(frame7, 2, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        myShowSizeLegend = new FXCheckButton(m72, "Show Size Legend", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowSizeLegend->setCheck(mySettings->showSizeLegend);
        new FXLabel(m72, "");
    } {
        new FXTabItem(tabbook, "openGL", NULL, TAB_LEFT_NORMAL, 0, 0, 0, 0, 4, 8, 4, 4);
        FXVerticalFrame* frame8 =
            new FXVerticalFrame(tabbook, FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2);

        FXMatrix* m82 =
            new FXMatrix(frame8, 1, LAYOUT_FILL_X | LAYOUT_TOP | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                         0, 0, 0, 0, 10, 10, 10, 10, 5, 5);
        myAntialiase = new FXCheckButton(m82, "Antialiase", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myAntialiase->setCheck(mySettings->antialiase);
        myDither = new FXCheckButton(m82, "Dither", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myDither->setCheck(mySettings->dither);
    }
    FXHorizontalFrame* f2 = new FXHorizontalFrame(contentFrame, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH, 0, 0, 0, 0, 10, 10, 5, 5);
    FXButton* initial = new FXButton(f2, "&Use", NULL, this, MID_SETTINGS_OK, BUTTON_INITIAL | BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X, 0, 0, 0, 0, 30, 30, 4, 4);
    new FXButton(f2, "&Discard", NULL, this, MID_SETTINGS_CANCEL, BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X, 0, 0, 0, 0, 30, 30, 4, 4);
    initial->setFocus();

    rebuildColorMatrices(false);
    setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
}


GUIDialog_ViewSettings::~GUIDialog_ViewSettings() {
    myParent->remove(this);
    delete myEdgeNamePanel;
    delete myInternalEdgeNamePanel;
    delete myStreetNamePanel;
    delete myJunctionNamePanel;
    delete myInternalJunctionNamePanel;
    delete myVehicleNamePanel;
    delete myAddNamePanel;
    delete myPOINamePanel;
    delete myPolyNamePanel;
}


void
GUIDialog_ViewSettings::setCurrent(GUIVisualizationSettings* settings) {
    mySettings = settings;
    myBackup = (*settings);
    onCmdNameChange(0, 0, 0);
}


long
GUIDialog_ViewSettings::onCmdOk(FXObject*, FXSelector, void*) {
    hide();
    return 1;
}


long
GUIDialog_ViewSettings::onCmdCancel(FXObject*, FXSelector, void*) {
    hide();
    (*mySettings) = myBackup;
    return 1;
}


long
GUIDialog_ViewSettings::onCmdNameChange(FXObject*, FXSelector, void* data) {
    if (data != 0) {
        FXString dataS = (char*) data; // !!!unicode
        // check whether this item has been added twice
        if (dataS == mySchemeName->getItemText(mySchemeName->getNumItems() - 1)) {
            for (int i = 0; i < mySchemeName->getNumItems() - 1; ++i) {
                if (dataS == mySchemeName->getItemText(i)) {
                    mySchemeName->removeItem(i);
                }
            }
        }
        myBackup = gSchemeStorage.get(dataS.text());
        mySettings = &gSchemeStorage.get(dataS.text());
    }
    rebuildColorMatrices(true);

    myBackgroundColor->setRGBA(MFXUtils::getFXColor(mySettings->backgroundColor));

    myLaneEdgeColorMode->setCurrentItem((FXint) mySettings->getLaneEdgeMode());
    myShowLaneBorders->setCheck(mySettings->laneShowBorders);
    myShowLaneDecals->setCheck(mySettings->showLinkDecals);
    myShowRails->setCheck(mySettings->showRails);
    myEdgeNamePanel->update(mySettings->edgeName);
    myInternalEdgeNamePanel->update(mySettings->internalEdgeName);
    myStreetNamePanel->update(mySettings->streetName);
    myHideMacroConnectors->setCheck(mySettings->hideConnectors);
    myLaneWidthUpscaleDialer->setValue(mySettings->laneWidthExaggeration);

    myVehicleColorMode->setCurrentItem((FXint) mySettings->vehicleColorer.getActive());
    myVehicleShapeDetail->setCurrentItem(mySettings->vehicleQuality);
    myVehicleUpscaleDialer->setValue(mySettings->vehicleExaggeration);
    myVehicleMinSizeDialer->setValue(mySettings->minVehicleSize);
    myShowBlinker->setCheck(mySettings->showBlinker);
    myShowMinGap->setCheck(mySettings->drawMinGap);
    /*
    myShowLaneChangePreference->setCheck(mySettings->drawLaneChangePreference);
    */
    myVehicleNamePanel->update(mySettings->vehicleName);

    myPersonColorMode->setCurrentItem((FXint) mySettings->personColorer.getActive());
    myPersonShapeDetail->setCurrentItem(mySettings->personQuality);
    myPersonUpscaleDialer->setValue(mySettings->personExaggeration);
    myPersonMinSizeDialer->setValue(mySettings->minPersonSize);
    myPersonNamePanel->update(mySettings->personName);

    myJunctionColorMode->setCurrentItem((FXint) mySettings->junctionColorer.getActive());
    myShowTLIndex->setCheck(mySettings->drawLinkTLIndex);
    myShowJunctionIndex->setCheck(mySettings->drawLinkJunctionIndex);
    myJunctionNamePanel->update(mySettings->junctionName);
    myInternalJunctionNamePanel->update(mySettings->internalJunctionName);

    myDetectorUpscaleDialer->setValue(mySettings->addExaggeration);
    myDetectorMinSizeDialer->setValue(mySettings->minAddSize);
    myAddNamePanel->update(mySettings->addName);

    myPOIUpscaleDialer->setValue(mySettings->poiExaggeration);
    myPOIMinSizeDialer->setValue(mySettings->minPOISize);
    myPOINamePanel->update(mySettings->poiName);

    myPolyUpscaleDialer->setValue(mySettings->polyExaggeration);
    myPolyMinSizeDialer->setValue(mySettings->minPolySize);
    myPolyNamePanel->update(mySettings->polyName);

    myShowLane2Lane->setCheck(mySettings->showLane2Lane);
    myAntialiase->setCheck(mySettings->antialiase);
    myDither->setCheck(mySettings->dither);
    myShowSizeLegend->setCheck(mySettings->showSizeLegend);

    myParent->setColorScheme(mySettings->name);
    update();
    myParent->update();
    return 1;
}


bool
GUIDialog_ViewSettings::updateColorRanges(FXObject* sender, std::vector<FXColorWell*>::const_iterator colIt,
        std::vector<FXColorWell*>::const_iterator colEnd,
        std::vector<FXRealSpinDial*>::const_iterator threshIt,
        std::vector<FXRealSpinDial*>::const_iterator threshEnd,
        std::vector<FXButton*>::const_iterator buttonIt,
        GUIColorScheme& scheme) {
    size_t pos = 0;
    while (colIt != colEnd) {
        if (scheme.isFixed()) {
            if (sender == *colIt) {
                scheme.setColor(pos, MFXUtils::getRGBColor((*colIt)->getRGBA()));
            }
        } else {
            if (sender == *threshIt) {
                const SUMOReal val = (*threshIt)->getValue();
                double lo, hi;
                if (pos != 0) {
                    threshIt--;
                    (*threshIt)->getRange(lo, hi);
                    (*threshIt)->setRange(lo, val);
                    threshIt++;
                }
                threshIt++;
                if (threshIt != threshEnd) {
                    (*threshIt)->getRange(lo, hi);
                    (*threshIt)->setRange(val, hi);
                }
                scheme.setThreshold(pos, val);
                return false;
            }
            if (sender == *colIt) {
                scheme.setColor(pos, MFXUtils::getRGBColor((*colIt)->getRGBA()));
                return false;
            }
            if (sender == *buttonIt) {
                if (pos == 0) {
                    scheme.addColor(MFXUtils::getRGBColor((*colIt)->getRGBA()), (*threshIt)->getValue());
                } else {
                    scheme.removeColor(pos);
                }
                return true;
            }
            ++threshIt;
            ++buttonIt;
        }
        ++colIt;
        pos++;
    }
    return false;
}


long
GUIDialog_ViewSettings::onCmdColorChange(FXObject* sender, FXSelector, void* /*val*/) {
    GUIVisualizationSettings tmpSettings = *mySettings;
    size_t prevLaneMode = mySettings->getLaneEdgeMode();
    size_t prevVehicleMode = mySettings->vehicleColorer.getActive();
    size_t prevPersonMode = mySettings->personColorer.getActive();
    size_t prevJunctionMode = mySettings->junctionColorer.getActive();
    bool doRebuildColorMatrices = false;

    tmpSettings.name = mySettings->name;
    tmpSettings.backgroundColor = MFXUtils::getRGBColor(myBackgroundColor->getRGBA());
    tmpSettings.showGrid = (myShowGrid->getCheck() != FALSE);
    tmpSettings.gridXSize = (SUMOReal) myGridXSizeDialer->getValue();
    tmpSettings.gridYSize = (SUMOReal) myGridYSizeDialer->getValue();

#ifdef HAVE_INTERNAL
    if (GUIVisualizationSettings::UseMesoSim) {
        tmpSettings.edgeColorer.setActive(myLaneEdgeColorMode->getCurrentItem());
    } else {
#endif
        tmpSettings.laneColorer.setActive(myLaneEdgeColorMode->getCurrentItem());
#ifdef HAVE_INTERNAL
    }
#endif
    tmpSettings.laneShowBorders = (myShowLaneBorders->getCheck() != FALSE);
    tmpSettings.showLinkDecals = (myShowLaneDecals->getCheck() != FALSE);
    tmpSettings.showRails = (myShowRails->getCheck() != FALSE);
    tmpSettings.edgeName = myEdgeNamePanel->getSettings();
    tmpSettings.internalEdgeName = myInternalEdgeNamePanel->getSettings();
    tmpSettings.streetName = myStreetNamePanel->getSettings();
    tmpSettings.hideConnectors = (myHideMacroConnectors->getCheck() != FALSE);
    tmpSettings.laneWidthExaggeration = (SUMOReal) myLaneWidthUpscaleDialer->getValue();

    tmpSettings.vehicleColorer.setActive(myVehicleColorMode->getCurrentItem());
    tmpSettings.vehicleQuality = myVehicleShapeDetail->getCurrentItem();
    tmpSettings.vehicleExaggeration = (SUMOReal) myVehicleUpscaleDialer->getValue();
    tmpSettings.minVehicleSize = (SUMOReal) myVehicleMinSizeDialer->getValue();
    tmpSettings.showBlinker = (myShowBlinker->getCheck() != FALSE);
    tmpSettings.drawMinGap = (myShowMinGap->getCheck() != FALSE);
    /*
    tmpSettings.drawLaneChangePreference = (myShowLaneChangePreference->getCheck() != FALSE);
    */
    tmpSettings.vehicleName = myVehicleNamePanel->getSettings();

    tmpSettings.personColorer.setActive(myPersonColorMode->getCurrentItem());
    tmpSettings.personQuality = myPersonShapeDetail->getCurrentItem();
    tmpSettings.personExaggeration = (SUMOReal) myPersonUpscaleDialer->getValue();
    tmpSettings.minPersonSize = (SUMOReal) myPersonMinSizeDialer->getValue();
    tmpSettings.personName = myPersonNamePanel->getSettings();

    tmpSettings.junctionColorer.setActive(myJunctionColorMode->getCurrentItem());
    tmpSettings.drawLinkTLIndex = (myShowTLIndex->getCheck() != FALSE);
    tmpSettings.drawLinkJunctionIndex = (myShowJunctionIndex->getCheck() != FALSE);
    tmpSettings.junctionName = myJunctionNamePanel->getSettings();
    tmpSettings.internalJunctionName = myInternalJunctionNamePanel->getSettings();

    tmpSettings.addExaggeration = (SUMOReal) myDetectorUpscaleDialer->getValue();
    tmpSettings.minAddSize = (SUMOReal) myDetectorMinSizeDialer->getValue();
    tmpSettings.addName = myAddNamePanel->getSettings();

    tmpSettings.poiExaggeration = (SUMOReal) myPOIUpscaleDialer->getValue();
    tmpSettings.minPOISize = (SUMOReal) myPOIMinSizeDialer->getValue();
    tmpSettings.poiName = myPOINamePanel->getSettings();

    tmpSettings.polyExaggeration = (SUMOReal) myPolyUpscaleDialer->getValue();
    tmpSettings.minPolySize = (SUMOReal) myPolyMinSizeDialer->getValue();
    tmpSettings.polyName = myPolyNamePanel->getSettings();

    tmpSettings.showLane2Lane = (myShowLane2Lane->getCheck() != FALSE);
    tmpSettings.antialiase = (myAntialiase->getCheck() != FALSE);
    tmpSettings.dither = (myDither->getCheck() != FALSE);
    tmpSettings.showSizeLegend = (myShowSizeLegend->getCheck() != FALSE);

    // lanes
    if (tmpSettings.getLaneEdgeMode() == prevLaneMode) {
        if (updateColorRanges(sender, myLaneColors.begin(), myLaneColors.end(),
                              myLaneThresholds.begin(), myLaneThresholds.end(), myLaneButtons.begin(),
                              tmpSettings.getLaneEdgeScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myLaneColorInterpolation) {
            tmpSettings.getLaneEdgeScheme().setInterpolated(myLaneColorInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // vehicles
    if (tmpSettings.vehicleColorer.getActive() == prevVehicleMode) {
        if (updateColorRanges(sender, myVehicleColors.begin(), myVehicleColors.end(),
                              myVehicleThresholds.begin(), myVehicleThresholds.end(), myVehicleButtons.begin(),
                              tmpSettings.vehicleColorer.getScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myVehicleColorInterpolation) {
            tmpSettings.vehicleColorer.getScheme().setInterpolated(myVehicleColorInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // persons
    if (tmpSettings.personColorer.getActive() == prevPersonMode) {
        if (updateColorRanges(sender, myPersonColors.begin(), myPersonColors.end(),
                              myPersonThresholds.begin(), myPersonThresholds.end(), myPersonButtons.begin(),
                              tmpSettings.personColorer.getScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myPersonColorInterpolation) {
            tmpSettings.personColorer.getScheme().setInterpolated(myPersonColorInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // junctions
    if (tmpSettings.junctionColorer.getActive() == prevJunctionMode) {
        if (updateColorRanges(sender, myJunctionColors.begin(), myJunctionColors.end(),
                              myJunctionThresholds.begin(), myJunctionThresholds.end(), myJunctionButtons.begin(),
                              tmpSettings.junctionColorer.getScheme())) {
            doRebuildColorMatrices = true;
        }
        if (sender == myJunctionColorInterpolation) {
            tmpSettings.junctionColorer.getScheme().setInterpolated(myJunctionColorInterpolation->getCheck() != FALSE);
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }

    if (tmpSettings == *mySettings) {
        return 1;
    }

    int index = mySchemeName->getCurrentItem();
    if (index < (int) gSchemeStorage.getNumInitialSettings()) {
        // one of the initial settings is modified
        // every time this happens we create a new scheme
        int suffix = 1;
        while (gSchemeStorage.contains("custom_" + toString(suffix))) {
            suffix++;
        }
        tmpSettings.name = "custom_" + toString(suffix);
        // the newly created settings must be entered in several places:
        // - the comboBox mySchemeName of this dialog
        // - the comboBox of the parent view (set as active)
        // - the comboBox of all other views (only append) XXX @todo
        index = mySchemeName->appendItem(tmpSettings.name.c_str());
        mySchemeName->setCurrentItem(index);
        myParent->getColoringSchemesCombo().appendItem(tmpSettings.name.c_str());
    }
    myParent->getColoringSchemesCombo().setCurrentItem(
        myParent->getColoringSchemesCombo().findItem(tmpSettings.name.c_str()));
    gSchemeStorage.add(tmpSettings); // overwrites existing
    mySettings = &gSchemeStorage.get(tmpSettings.name);
    myParent->setColorScheme(tmpSettings.name);

    if (doRebuildColorMatrices) {
        rebuildColorMatrices(true);
    }
    myParent->forceRefresh();
    getApp()->forceRefresh();
    return 1;
}


void
GUIDialog_ViewSettings::loadSettings(const std::string& file) {
    GUISettingsHandler handler(file);
    std::string settingsName = handler.addSettings(myParent);
    if (settingsName != "") {
        FXint index = mySchemeName->appendItem(settingsName.c_str());
        mySchemeName->setCurrentItem(index);
        mySettings = &gSchemeStorage.get(settingsName);
    }
}


void
GUIDialog_ViewSettings::saveDecals(const std::string& file) const {
    try {
        OutputDevice& dev = OutputDevice::getDevice(file);
        dev << "<decals>\n";
        std::vector<GUISUMOAbstractView::Decal>::iterator j;
        for (j = myDecals->begin(); j != myDecals->end(); ++j) {
            GUISUMOAbstractView::Decal& d = *j;
            dev << "    <decal filename=\"" << d.filename
                << "\" centerX=\"" << d.centerX
                << "\" centerY=\"" << d.centerY
                << "\" centerZ=\"" << d.centerZ
                << "\" width=\"" << d.width
                << "\" height=\"" << d.height
                << "\" altitude=\"" << d.altitude
                << "\" rotation=\"" << d.rot
                << "\" tilt=\"" << d.tilt
                << "\" roll=\"" << d.roll
                << "\" layer=\"" << d.layer
                << "\"/>\n";
        }
        dev << "</decals>\n";
        dev.close();
    } catch (IOError& e) {
        FXMessageBox::error(myParent, MBOX_OK, "Storing failed!", "%s", e.what());
    }
}


void
GUIDialog_ViewSettings::loadDecals(const std::string& file) {
    myDecalsLock->lock();
    GUISettingsHandler handler(file);
    if (handler.hasDecals()) {
        (*myDecals) = handler.getDecals();
    }
    rebuildList();
    myParent->update();
    myDecalsLock->unlock();
}


long
GUIDialog_ViewSettings::onCmdSaveSetting(FXObject*, FXSelector, void* /*data*/) {
    int index = mySchemeName->getCurrentItem();
    if (index < (int) gSchemeStorage.getNumInitialSettings()) {
        return 1;
    }
    // get the name
    std::string name = "";
    while (name.length() == 0) {
        FXDialogBox dialog(this, "Enter a name", DECOR_TITLE | DECOR_BORDER);
        FXVerticalFrame* content = new FXVerticalFrame(&dialog, LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 10, 10, 10, 10, 10, 10);
        new FXLabel(content, "Please enter an alphanumeric name: ", NULL, LAYOUT_FILL_X | JUSTIFY_LEFT);
        FXTextField* text = new FXTextField(content, 40, &dialog, FXDialogBox::ID_ACCEPT, TEXTFIELD_ENTER_ONLY | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X);
        new FXHorizontalSeparator(content, SEPARATOR_GROOVE | LAYOUT_FILL_X);
        FXHorizontalFrame* buttons = new FXHorizontalFrame(content, LAYOUT_FILL_X | PACK_UNIFORM_WIDTH, 0, 0, 0, 0, 0, 0, 0, 0);
        new FXButton(buttons, "&OK", NULL, &dialog, FXDialogBox::ID_ACCEPT, BUTTON_INITIAL | BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_RIGHT);
        new FXButton(buttons, "&Cancel", NULL, &dialog, FXDialogBox::ID_CANCEL, BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_RIGHT, 0, 0, 0, 0, 20, 20);
        dialog.create();
        text->setFocus();
        if (!dialog.execute()) {
            return 1;
        }
        name = text->getText().text();
        for (size_t i = 0; i < name.length(); ++i) {
            if (name[i] != '_' && (name[i] < 'a' || name[i] > 'z') && (name[i] < 'A' || name[i] > 'Z') && (name[i] < '0' || name[i] > '9')) {
                name = "";
                break;
            }
        }
    }
    GUIVisualizationSettings tmpSettings = *mySettings;
    gSchemeStorage.remove(mySettings->name);
    tmpSettings.name = name;
    gSchemeStorage.add(tmpSettings);
    mySchemeName->setItemText(index, tmpSettings.name.c_str());
    myParent->getColoringSchemesCombo().setItemText(index, tmpSettings.name.c_str());
    myParent->setColorScheme(tmpSettings.name);
    mySettings = &gSchemeStorage.get(name);
    myBackup = *mySettings;
    gSchemeStorage.writeSettings(getApp());
    return 1;
}


long
GUIDialog_ViewSettings::onUpdSaveSetting(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   mySchemeName->getCurrentItem() < (int) gSchemeStorage.getNumInitialSettings()
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIDialog_ViewSettings::onCmdDeleteSetting(FXObject*, FXSelector, void* /*data*/) {
    int index = mySchemeName->getCurrentItem();
    if (index < (int) gSchemeStorage.getNumInitialSettings()) {
        return 1;
    }
    std::string name = mySchemeName->getItem(index).text();
    gSchemeStorage.remove(name);
    mySchemeName->removeItem(index);
    onCmdNameChange(0, 0, (void*) mySchemeName->getItem(0).text());
    gSchemeStorage.writeSettings(getApp());
    return 1;
}


long
GUIDialog_ViewSettings::onUpdDeleteSetting(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   mySchemeName->getCurrentItem() < (int) gSchemeStorage.getNumInitialSettings()
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIDialog_ViewSettings::onCmdExportSetting(FXObject*, FXSelector, void* /*data*/) {
    FXFileDialog opendialog(this, "Export view settings");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (!opendialog.execute() || !MFXUtils::userPermitsOverwritingWhenFileExists(this, opendialog.getFilename())) {
        return 1;
    }
    try {
        OutputDevice& dev = OutputDevice::getDevice(opendialog.getFilename().text());
        mySettings->save(dev);
        dev.close();
    } catch (IOError& e) {
        FXMessageBox::error(this, MBOX_OK, "Storing failed!", "%s", e.what());
    }
    return 1;
}


long
GUIDialog_ViewSettings::onUpdExportSetting(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   mySchemeName->getCurrentItem() < (int) gSchemeStorage.getNumInitialSettings()
                   ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIDialog_ViewSettings::onCmdImportSetting(FXObject*, FXSelector, void* /*data*/) {
    FXFileDialog opendialog(this, "Import view settings");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        loadSettings(opendialog.getFilename().text());
    }
    return 1;
}


long
GUIDialog_ViewSettings::onCmdLoadDecals(FXObject*, FXSelector, void* /*data*/) {
    FXFileDialog opendialog(this, "Load Decals");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        loadDecals(opendialog.getFilename().text());
    }
    return 1;
}


long
GUIDialog_ViewSettings::onCmdSaveDecals(FXObject*, FXSelector, void* /*data*/) {
    FXFileDialog opendialog(this, "Save Decals");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (!opendialog.execute() || !MFXUtils::userPermitsOverwritingWhenFileExists(this, opendialog.getFilename())) {
        return 1;
    }
    saveDecals(opendialog.getFilename().text());
    return 1;
}




long
GUIDialog_ViewSettings::onUpdImportSetting(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), ptr);
    return 1;
}


void
GUIDialog_ViewSettings::rebuildList() {
    myDecalsTable->clearItems();
    // set table attributes
    myDecalsTable->setTableSize(10, 7);
    myDecalsTable->setColumnText(0, "picture file");
    myDecalsTable->setColumnText(1, "center x");
    myDecalsTable->setColumnText(2, "center y");
    myDecalsTable->setColumnText(3, "width");
    myDecalsTable->setColumnText(4, "height");
    myDecalsTable->setColumnText(5, "rotation");
    myDecalsTable->setColumnText(6, "layer");
    FXHeader* header = myDecalsTable->getColumnHeader();
    header->setHeight(getApp()->getNormalFont()->getFontHeight() + getApp()->getNormalFont()->getFontAscent());
    int k;
    for (k = 0; k < 7; k++) {
        header->setItemJustify(k, JUSTIFY_CENTER_X | JUSTIFY_TOP);
        header->setItemSize(k, 60);
    }
    header->setItemSize(0, 150);
    // insert already known decals information into table
    FXint row = 0;
    std::vector<GUISUMOAbstractView::Decal>::iterator j;
    for (j = myDecals->begin(); j != myDecals->end(); ++j) {
        GUISUMOAbstractView::Decal& d = *j;
        myDecalsTable->setItemText(row, 0, d.filename.c_str());
        myDecalsTable->setItemText(row, 1, toString<SUMOReal>(d.centerX).c_str());
        myDecalsTable->setItemText(row, 2, toString<SUMOReal>(d.centerY).c_str());
        myDecalsTable->setItemText(row, 3, toString<SUMOReal>(d.width).c_str());
        myDecalsTable->setItemText(row, 4, toString<SUMOReal>(d.height).c_str());
        myDecalsTable->setItemText(row, 5, toString<SUMOReal>(d.rot).c_str());
        myDecalsTable->setItemText(row, 6, toString<SUMOReal>(d.layer).c_str());
        row++;
    }
    // insert dummy last field
    for (k = 0; k < 7; k++) {
        myDecalsTable->setItemText(row, k, " ");
    }
}


FXMatrix*
GUIDialog_ViewSettings::rebuildColorMatrix(FXVerticalFrame* frame,
        std::vector<FXColorWell*>& colors,
        std::vector<FXRealSpinDial*>& thresholds,
        std::vector<FXButton*>& buttons,
        FXCheckButton* interpolation,
        GUIColorScheme& scheme) {
    MFXUtils::deleteChildren(frame);
    FXMatrix* m = new FXMatrix(frame, 3,
                               LAYOUT_FILL_X | MATRIX_BY_COLUMNS,
                               0, 0, 0, 0, 10, 10, 0, 0, 5, 3);
    colors.clear();
    thresholds.clear();
    buttons.clear();
    const bool fixed = scheme.isFixed();
    std::vector<RGBColor>::const_iterator colIt = scheme.getColors().begin();
    std::vector<SUMOReal>::const_iterator threshIt = scheme.getThresholds().begin();
    std::vector<std::string>::const_iterator nameIt = scheme.getNames().begin();
    FX::FXString buttonText = "Add";
    while (colIt != scheme.getColors().end()) {
        colors.push_back(new FXColorWell(m , MFXUtils::getFXColor(*colIt),
                                         this, MID_SIMPLE_VIEW_COLORCHANGE,
                                         LAYOUT_FIX_WIDTH | LAYOUT_CENTER_Y | FRAME_SUNKEN | FRAME_THICK | ICON_AFTER_TEXT,
                                         0, 0, 100, 0,   0, 0, 0, 0));
        if (fixed) {
            new FXLabel(m, nameIt->c_str());
            new FXLabel(m, "");
        } else {
            const int dialerOptions = scheme.allowsNegativeValues() ? SPINDIAL_NOMIN : 0;
            FXRealSpinDial* threshDialer = new FXRealSpinDial(m, 10, this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y | LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK | SPINDIAL_NOMAX | dialerOptions);
            threshDialer->setValue(*threshIt);
            thresholds.push_back(threshDialer);
            buttons.push_back(new FXButton(m, buttonText, NULL, this, MID_SIMPLE_VIEW_COLORCHANGE, BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X, 0, 0, 0, 0, 20, 20, 4, 4));
            buttonText = "Remove";
        }
        colIt++;
        threshIt++;
        nameIt++;
    }
    interpolation->setCheck(scheme.isInterpolated());
    if (fixed) {
        interpolation->disable();
    } else {
        if (colors.size() > 1) {
            interpolation->enable();
            if (interpolation->getCheck() != FALSE) {
                thresholds.front()->enable();
            } else {
                thresholds.front()->disable();
            }
        } else {
            interpolation->disable();
            thresholds.front()->disable();
        }
    }
    return m;
}


void
GUIDialog_ViewSettings::rebuildColorMatrices(bool doCreate) {
    // decals
    delete myDecalsTable;
    myDecalsTable = new MFXAddEditTypedTable(myDecalsFrame, this, MID_TABLE,
            LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH/*|LAYOUT_FIX_HEIGHT*/, 0, 0, 470, 0);
    myDecalsTable->setVisibleRows(5);
    myDecalsTable->setVisibleColumns(7);
    myDecalsTable->setTableSize(5, 7);
    myDecalsTable->setBackColor(FXRGB(255, 255, 255));
    myDecalsTable->getRowHeader()->setWidth(0);
    for (int i = 1; i <= 5; ++i) {
        myDecalsTable->setCellType(i, CT_REAL);
        myDecalsTable->setNumberCellParams(i, -10000000, 10000000, 1, 10, 100, "%.2f");
    }
    rebuildList();
    if (doCreate) {
        myDecalsTable->create();
    }
    FXMatrix* m = rebuildColorMatrix(myLaneColorSettingFrame, myLaneColors, myLaneThresholds, myLaneButtons, myLaneColorInterpolation, mySettings->getLaneEdgeScheme());
    if (doCreate) {
        m->create();
    }
    myLaneColorSettingFrame->getParent()->recalc();
    m = rebuildColorMatrix(myVehicleColorSettingFrame, myVehicleColors, myVehicleThresholds, myVehicleButtons, myVehicleColorInterpolation, mySettings->vehicleColorer.getScheme());
    if (doCreate) {
        m->create();
    }
    myPersonColorSettingFrame->getParent()->recalc();
    m = rebuildColorMatrix(myPersonColorSettingFrame, myPersonColors, myPersonThresholds, myPersonButtons, myPersonColorInterpolation, mySettings->personColorer.getScheme());
    if (doCreate) {
        m->create();
    }
    myPersonColorSettingFrame->getParent()->recalc();
    m = rebuildColorMatrix(myJunctionColorSettingFrame, myJunctionColors, myJunctionThresholds, myJunctionButtons, myJunctionColorInterpolation, mySettings->junctionColorer.getScheme());
    if (doCreate) {
        m->create();
    }
    myJunctionColorSettingFrame->getParent()->recalc();
    layout();
    update();
}


long
GUIDialog_ViewSettings::onCmdEditTable(FXObject*, FXSelector, void* data) {
    MFXEditedTableItem* i = (MFXEditedTableItem*) data;
    std::string value = i->item->getText().text();
    // check whether the inserted value is empty
    if (value.find_first_not_of(" ") == std::string::npos) {
        return 1;
    }
    GUISUMOAbstractView::Decal d;
    int row = i->row;
    // check whether we add a new entry or edit an existing entry
    if (row == static_cast<int>(myDecals->size())) {
        d.filename = "";
        d.centerX = SUMOReal(myParent->getGridWidth() / 2.);
        d.centerY = SUMOReal(myParent->getGridHeight() / 2.);
        d.width = 0.;
        d.height = 0.;
        d.initialised = false;
        d.rot = 0;
        d.layer = 0;
        myDecalsLock->lock();
        myDecals->push_back(d);
        myDecalsLock->unlock();
    } else if (row > static_cast<int>(myDecals->size())) {
        // ignore clicks two lines below existing entries
        return 1;
    } else {
        d = (*myDecals)[row];
    }

    switch (i->col) {
        case 0:
            if (d.initialised && d.filename != value) {
                d.initialised = false;
            }
            d.filename = value;
            break;
        case 1:
            try {
                d.centerX = TplConvert::_2SUMOReal(value.c_str());
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a float, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        case 2:
            try {
                d.centerY = TplConvert::_2SUMOReal(value.c_str());
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a float, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        case 3:
            try {
                d.width = TplConvert::_2SUMOReal(value.c_str());
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a float, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        case 4:
            try {
                d.height = TplConvert::_2SUMOReal(value.c_str());
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a float, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        case 5:
            try {
                d.rot = TplConvert::_2SUMOReal(value.c_str());
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a float, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        case 6:
            try {
                d.layer = TplConvert::_2SUMOReal(value.c_str());
            } catch (NumberFormatException&) {
                std::string msg = "The value must be a float, is:" + value;
                FXMessageBox::error(this, MBOX_OK, "Number format error", "%s", msg.c_str());
            }
            break;
        default:
            throw 1;
    }
    (*myDecals)[row] = d;
    if (!i->updateOnly) {
        rebuildList();
    }
    myParent->update();
    return 1;
}


std::string
GUIDialog_ViewSettings::getCurrentScheme() const {
    return mySchemeName->getItem(mySchemeName->getCurrentItem()).text();
}


void
GUIDialog_ViewSettings::setCurrentScheme(const std::string& name) {
    if (name.c_str() == mySchemeName->getItemText(mySchemeName->getCurrentItem())) {
        return;
    }
    for (int i = 0; i < mySchemeName->getNumItems(); ++i) {
        if (name.c_str() == mySchemeName->getItemText(i)) {
            mySchemeName->setCurrentItem(i);
            onCmdNameChange(0, 0, (void*)name.c_str());
            return;
        }
    }
}

GUIDialog_ViewSettings::NamePanel::NamePanel(
    FXMatrix* parent,
    GUIDialog_ViewSettings* target,
    const std::string& title,
    const GUIVisualizationTextSettings& settings) {
    myCheck = new FXCheckButton(parent, title.c_str(), target, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y | CHECKBUTTON_NORMAL);
    myCheck->setCheck(settings.show);
    new FXLabel(parent, "");
    FXMatrix* m1 = new FXMatrix(parent, 2, LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                                0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
    new FXLabel(m1, "Size", 0, LAYOUT_CENTER_Y);
    mySizeDial = new FXRealSpinDial(m1, 10, target, MID_SIMPLE_VIEW_COLORCHANGE,
                                    LAYOUT_CENTER_Y | LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
    mySizeDial->setRange(10, 1000);
    mySizeDial->setValue(settings.size);
    FXMatrix* m2 = new FXMatrix(parent, 2, LAYOUT_FILL_X | LAYOUT_BOTTOM | LAYOUT_LEFT | MATRIX_BY_COLUMNS,
                                0, 0, 0, 0, 10, 10, 0, 0, 5, 5);
    new FXLabel(m2, "Color", 0, LAYOUT_CENTER_Y);
    myColorWell = new FXColorWell(m2, MFXUtils::getFXColor(settings.color),
                                  target, MID_SIMPLE_VIEW_COLORCHANGE,
                                  LAYOUT_FIX_WIDTH | LAYOUT_CENTER_Y | LAYOUT_SIDE_TOP | FRAME_SUNKEN | FRAME_THICK | ICON_AFTER_TEXT,
                                  0, 0, 100, 0,   0, 0, 0, 0);
}

GUIVisualizationTextSettings
GUIDialog_ViewSettings::NamePanel::getSettings() {
    return GUIVisualizationTextSettings(myCheck->getCheck() != FALSE,
                                        mySizeDial->getValue(), MFXUtils::getRGBColor(myColorWell->getRGBA()));
}


void
GUIDialog_ViewSettings::NamePanel::update(const GUIVisualizationTextSettings& settings) {
    myCheck->setCheck(settings.show);
    mySizeDial->setValue(settings.size);
    myColorWell->setRGBA(MFXUtils::getFXColor(settings.color));
}
/****************************************************************************/

