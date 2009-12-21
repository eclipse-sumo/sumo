/****************************************************************************/
/// @file    GUIDialog_ViewSettings.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 21. Dec 2005
/// @version $Id$
///
// The dialog to change the view (gui) settings.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include "GUIDialog_ViewSettings.h"
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/images/GUIImageGlobals.h>
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_ViewSettings) GUIDialog_ViewSettingsMap[]= {
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
GUIDialog_ViewSettings::GUIDialog_ViewSettings(
    GUISUMOAbstractView *parent,
    GUIVisualizationSettings *settings,
    std::vector<GUISUMOAbstractView::Decal> *decals,
    MFXMutex *decalsLock) throw()
        : FXDialogBox(parent, "View Settings", DECOR_TITLE|DECOR_BORDER, 0,0,0,0, 0,0,0,0, 0,0),
        myParent(parent), mySettings(settings),
        myDecals(decals), myDecalsLock(decalsLock), myDecalsTable(0) {
    myBackup = (*mySettings);

    FXVerticalFrame *contentFrame =
        new FXVerticalFrame(this, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
                            0,0,0,0, 0,0,0,0, 2,2);
    //
    {
        FXHorizontalFrame *frame0 =
            new FXHorizontalFrame(contentFrame,FRAME_THICK, 0,0,0,0, 0,0,0,0, 2,2);
        mySchemeName = new FXComboBox(frame0, 20, this, MID_SIMPLE_VIEW_NAMECHANGE, COMBOBOX_INSERT_LAST|FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_CENTER_Y|COMBOBOX_STATIC);
        const std::vector<std::string> &names = gSchemeStorage.getNames();
        for (std::vector<std::string>::const_iterator i=names.begin(); i!=names.end(); ++i) {
            size_t index = mySchemeName->appendItem((*i).c_str());
            if ((*i) == mySettings->name) {
                mySchemeName->setCurrentItem(index);
            }
        }
        mySchemeName->setNumVisible(5);

        new FXButton(frame0,"\t\tSave the setting to registry",
                     GUIIconSubSys::getIcon(ICON_SAVEDB), this, MID_SIMPLE_VIEW_SAVE,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(frame0,"\t\tRemove the setting from registry",
                     GUIIconSubSys::getIcon(ICON_REMOVEDB), this, MID_SIMPLE_VIEW_DELETE,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(frame0,"\t\tExport setting to file",
                     GUIIconSubSys::getIcon(ICON_SAVE), this, MID_SIMPLE_VIEW_EXPORT,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);
        new FXButton(frame0,"\t\tLoad setting from file",
                     GUIIconSubSys::getIcon(ICON_OPEN_CONFIG), this, MID_SIMPLE_VIEW_IMPORT,
                     ICON_ABOVE_TEXT|BUTTON_TOOLBAR|FRAME_RAISED|LAYOUT_TOP|LAYOUT_LEFT);

    }
    //
    FXTabBook *tabbook =
        new FXTabBook(contentFrame,0,0,TABBOOK_LEFTTABS|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_RIGHT,
                      0,0,0,0, 0,0,0,0);
    {
        // tab for the background
        new FXTabItem(tabbook,"Background",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
        FXVerticalFrame *frame1 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);

        FXMatrix *m11 =
            new FXMatrix(frame1,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
        new FXLabel(m11, "Color", 0, LAYOUT_CENTER_Y);
        myBackgroundColor = new FXColorWell(m11, convert(settings->backgroundColor),
                                            this, MID_SIMPLE_VIEW_COLORCHANGE,
                                            LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                            0, 0, 100, 0,   0, 0, 0, 0);

        new FXHorizontalSeparator(frame1,SEPARATOR_GROOVE|LAYOUT_FILL_X);

        FXVerticalFrame *frame11 =
            new FXVerticalFrame(frame1,LAYOUT_FILL_Y, 0,0,0,0, 10,10,10,10, 5,5);
        new FXLabel(frame11, "Decals:");
        myDecalsFrame = new FXVerticalFrame(frame11);
        FXHorizontalFrame *frame111 = new FXHorizontalFrame(frame11, LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0, 10,10,5,5);
        new FXButton(frame111,"&Load Decals",NULL,this,MID_SIMPLE_VIEW_LOAD_DECALS,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 20,20,4,4);
        new FXButton(frame111,"&Save Decals",NULL,this,MID_SIMPLE_VIEW_SAVE_DECALS,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 20,20,4,4);

        new FXHorizontalSeparator(frame1,SEPARATOR_GROOVE|LAYOUT_FILL_X);

        FXMatrix *m12 =
            new FXMatrix(frame1,2,LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
        myShowGrid =
            new FXCheckButton(m12, "Show grid", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y|CHECKBUTTON_NORMAL);
        myShowGrid->setCheck(mySettings->showGrid);
        new FXLabel(m12, "");
        FXMatrix *m121 =
            new FXMatrix(m12,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m121, "x-spacing", 0, LAYOUT_CENTER_Y);
        myGridXSizeDialer =
            new FXRealSpinDial(m121, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_CENTER_Y|LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myGridXSizeDialer->setRange(1, 10000);
        myGridXSizeDialer->setValue(mySettings->gridXSize);
        FXMatrix *m122 =
            new FXMatrix(m12,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m122, "y-spacing", 0, LAYOUT_CENTER_Y);
        myGridYSizeDialer =
            new FXRealSpinDial(m122, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_CENTER_Y|LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myGridYSizeDialer->setRange(1, 10000);
        myGridYSizeDialer->setValue(mySettings->gridXSize);
    }
    {
        // tab for the streets
        new FXTabItem(tabbook,"Streets",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);

        FXVerticalFrame *frame2 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);
        FXMatrix *m21 =
            new FXMatrix(frame2,3,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,2, 5,5);
        new FXLabel(m21, "Color by", 0, LAYOUT_CENTER_Y);
        myLaneEdgeColorMode = new FXComboBox(m21, 30, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP|COMBOBOX_STATIC);
        myLaneEdgeColorMode->setNumVisible(10);
        myLaneColorInterpolation = new FXCheckButton(m21, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y|CHECKBUTTON_NORMAL);
        FXScrollWindow *genScroll = new FXScrollWindow(frame2, LAYOUT_FILL_X|LAYOUT_SIDE_TOP|FRAME_RAISED|FRAME_THICK|LAYOUT_FIX_HEIGHT, 0,0,0,80);
        myLaneColorSettingFrame =
            new FXVerticalFrame(genScroll, LAYOUT_FILL_X|LAYOUT_FILL_Y,  0,0,0,0, 10,10,2,8, 5,2);
//we should insert a FXScrollWindow around the frame2
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            mySettings->edgeColorer.fill(*myLaneEdgeColorMode);
        } else {
#endif
            mySettings->laneColorer.fill(*myLaneEdgeColorMode);
#ifdef HAVE_MESOSIM
        }
#endif

        new FXHorizontalSeparator(frame2,SEPARATOR_GROOVE|LAYOUT_FILL_X);
        FXMatrix *m22 =
            new FXMatrix(frame2,2,LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
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
        myShowEdgeName = new FXCheckButton(m22, "Show edge name", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y|CHECKBUTTON_NORMAL);
        myShowEdgeName->setCheck(mySettings->drawEdgeName);
        new FXLabel(m22, "");
        FXMatrix *m221 =
            new FXMatrix(m22,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m221, "Size", 0, LAYOUT_CENTER_Y);
        myEdgeNameSizeDialer =
            new FXRealSpinDial(m221, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_CENTER_Y|LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myEdgeNameSizeDialer->setRange(10, 1000);
        myEdgeNameSizeDialer->setValue(mySettings->edgeNameSize);
        FXMatrix *m222 =
            new FXMatrix(m22,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m222, "Color", 0, LAYOUT_CENTER_Y);
        myEdgeNameColor = new FXColorWell(m222, convert(settings->edgeNameColor),
                                          this, MID_SIMPLE_VIEW_COLORCHANGE,
                                          LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                          0, 0, 100, 0,   0, 0, 0, 0);
        myShowInternalEdgeName = new FXCheckButton(m22, "Show internal edge name", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y|CHECKBUTTON_NORMAL);
        myShowInternalEdgeName->setCheck(mySettings->drawInternalEdgeName);
        new FXLabel(m22, "");
        FXMatrix *m223 =
            new FXMatrix(m22,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m223, "Size", 0, LAYOUT_CENTER_Y);
        myInternalEdgeNameSizeDialer =
            new FXRealSpinDial(m223, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_CENTER_Y|LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myInternalEdgeNameSizeDialer->setRange(10, 1000);
        myInternalEdgeNameSizeDialer->setValue(mySettings->internalEdgeNameSize);
        FXMatrix *m224 =
            new FXMatrix(m22,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m224, "Color", 0, LAYOUT_CENTER_Y);
        myInternalEdgeNameColor = new FXColorWell(m224, convert(settings->internalEdgeNameColor),
                this, MID_SIMPLE_VIEW_COLORCHANGE,
                LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                0, 0, 100, 0,   0, 0, 0, 0);
    }
    {
        new FXTabItem(tabbook,"Vehicles",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
        FXVerticalFrame *frame3 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);

        FXMatrix *m31 =
            new FXMatrix(frame3,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,2, 5,5);
        new FXLabel(m31, "Show As", 0, LAYOUT_CENTER_Y);
        myVehicleShapeDetail = new FXComboBox(m31, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP|COMBOBOX_STATIC);
        myVehicleShapeDetail->appendItem("'triangles'");
        myVehicleShapeDetail->appendItem("'boxes'");
        myVehicleShapeDetail->appendItem("'simple shapes'");
        myVehicleShapeDetail->setNumVisible(3);
        myVehicleShapeDetail->setCurrentItem(settings->vehicleQuality);

        new FXHorizontalSeparator(frame3,SEPARATOR_GROOVE|LAYOUT_FILL_X);

        FXMatrix *m32 =
            new FXMatrix(frame3,3,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,2, 5,5);
        new FXLabel(m32, "Color by", 0, LAYOUT_CENTER_Y);
        myVehicleColorMode = new FXComboBox(m32, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP|COMBOBOX_STATIC);
        mySettings->vehicleColorer.fill(*myVehicleColorMode);
        myVehicleColorMode->setNumVisible(10);
        myVehicleColorInterpolation = new FXCheckButton(m32, "Interpolate", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y|CHECKBUTTON_NORMAL);

        FXScrollWindow *genScroll = new FXScrollWindow(frame3, LAYOUT_FILL_X|LAYOUT_SIDE_TOP|FRAME_RAISED|FRAME_THICK|LAYOUT_FIX_HEIGHT, 0,0,0,80);
        myVehicleColorSettingFrame =
            new FXVerticalFrame(genScroll, LAYOUT_FILL_X|LAYOUT_FILL_Y,  0,0,0,0, 10,10,2,8, 5,2);

        new FXHorizontalSeparator(frame3,SEPARATOR_GROOVE|LAYOUT_FILL_X);

        FXMatrix *m33 =
            new FXMatrix(frame3,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
        myShowBlinker = new FXCheckButton(m33, "Show blinker", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowBlinker->setCheck(mySettings->showBlinker);
        new FXLabel(m33, " ", 0, LAYOUT_CENTER_Y);
        /*
        myShowC2CRadius = new FXCheckButton(m33, "Show C2C radius", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowC2CRadius->setCheck(mySettings->drawcC2CRadius);
        new FXLabel(m33, " ", 0, LAYOUT_CENTER_Y);
        myShowLaneChangePreference = new FXCheckButton(m33, "Show lane change preference", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowLaneChangePreference->setCheck(mySettings->drawLaneChangePreference);
        new FXLabel(m33, " ", 0, LAYOUT_CENTER_Y);
        */
        myShowVehicleName = new FXCheckButton(m33, "Show vehicle name", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y|CHECKBUTTON_NORMAL);
        myShowVehicleName->setCheck(mySettings->drawVehicleName);
        new FXLabel(m33, "");
        FXMatrix *m331 =
            new FXMatrix(m33,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m331, "Size", 0, LAYOUT_CENTER_Y);
        myVehicleNameSizeDialer =
            new FXRealSpinDial(m331, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_CENTER_Y|CHECKBUTTON_NORMAL|FRAME_SUNKEN|FRAME_THICK);
        myVehicleNameSizeDialer->setRange(10, 1000);
        myVehicleNameSizeDialer->setValue(mySettings->vehicleNameSize);
        FXMatrix *m332 =
            new FXMatrix(m33,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m332, "Color", 0, LAYOUT_CENTER_Y);
        myVehicleNameColor = new FXColorWell(m332, convert(settings->vehicleNameColor),
                                             this, MID_SIMPLE_VIEW_COLORCHANGE,
                                             LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                             0, 0, 100, 0,   0, 0, 0, 0);
        /*
        FXCheckButton *tmpc = new FXCheckButton(m33, "Show braking lights", 0 ,0);
        tmpc->disable();
        tmpc = new FXCheckButton(m33, "Show needed headway", 0 ,0);
        tmpc->disable();
        */

        new FXHorizontalSeparator(frame3,SEPARATOR_GROOVE|LAYOUT_FILL_X);

        FXMatrix *m34 =
            new FXMatrix(frame3,2,LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
        FXMatrix *m341 =
            new FXMatrix(m34,2,LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m341, "Minimum size", 0, LAYOUT_CENTER_Y);
        myVehicleMinSizeDialer =
            new FXRealSpinDial(m341, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myVehicleMinSizeDialer->setValue(mySettings->minVehicleSize);
        FXMatrix *m342 =
            new FXMatrix(m34,2,LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m342, "Exaggerate by", 0, LAYOUT_CENTER_Y);
        myVehicleUpscaleDialer =
            new FXRealSpinDial(m342, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myVehicleUpscaleDialer->setRange(1, 100);
        myVehicleUpscaleDialer->setValue(mySettings->vehicleExaggeration);
    } {
        new FXTabItem(tabbook,"Nodes",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
        FXVerticalFrame *frame4 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);

        FXMatrix *m33 =
            new FXMatrix(frame4,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
        myShowTLIndex = new FXCheckButton(m33, "Show link tls index", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowTLIndex->setCheck(mySettings->drawLinkTLIndex);
        new FXLabel(m33, " ", 0, LAYOUT_CENTER_Y);
        myShowJunctionIndex = new FXCheckButton(m33, "Show link junction index", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowJunctionIndex->setCheck(mySettings->drawLinkJunctionIndex);
        new FXLabel(m33, " ", 0, LAYOUT_CENTER_Y);
        myShowLane2Lane = new FXCheckButton(m33, "Show lane to lane connections", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowLane2Lane->setCheck(mySettings->showLane2Lane);
        new FXLabel(m33, " ", 0, LAYOUT_CENTER_Y);

        myShowJunctionName = new FXCheckButton(m33, "Show junction name", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y|CHECKBUTTON_NORMAL);
        myShowJunctionName->setCheck(mySettings->drawJunctionName);
        new FXLabel(m33, "");
        FXMatrix *m331 =
            new FXMatrix(m33,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m331, "Size", 0, LAYOUT_CENTER_Y);
        myJunctionNameSizeDialer =
            new FXRealSpinDial(m331, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               FRAME_SUNKEN|FRAME_THICK|LAYOUT_CENTER_Y);
        myJunctionNameSizeDialer->setRange(10, 1000);
        myJunctionNameSizeDialer->setValue(mySettings->junctionNameSize);
        FXMatrix *m332 =
            new FXMatrix(m33,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m332, "Color", 0, LAYOUT_CENTER_Y);
        myJunctionNameColor = new FXColorWell(m332, convert(settings->junctionNameColor),
                                              this, MID_SIMPLE_VIEW_COLORCHANGE,
                                              LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                              0, 0, 100, 0,   0, 0, 0, 0);
    } {
        new FXTabItem(tabbook,"Detectors/Trigger",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
        FXVerticalFrame *frame5 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);

        FXMatrix *m51 =
            new FXMatrix(frame5,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
        myShowAddName = new FXCheckButton(m51, "Show detector name", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowAddName->setCheck(mySettings->drawAddName);
        new FXLabel(m51, "");
        FXMatrix *m511 =
            new FXMatrix(m51,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m511, "Size", 0, LAYOUT_CENTER_Y);
        myAddNameSizeDialer =
            new FXRealSpinDial(m511, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               FRAME_SUNKEN|FRAME_THICK|LAYOUT_CENTER_Y);
        myAddNameSizeDialer->setRange(10, 1000);
        myAddNameSizeDialer->setValue(mySettings->addNameSize);
        FXMatrix *m512 =
            new FXMatrix(m51,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m512, "");
        new FXLabel(m512, "");

        new FXHorizontalSeparator(frame5 ,SEPARATOR_GROOVE|LAYOUT_FILL_X);

        FXMatrix *m52 =
            new FXMatrix(frame5,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
        FXMatrix *m521 =
            new FXMatrix(m52,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m521, "Minimum size", 0, LAYOUT_CENTER_Y);
        myDetectorMinSizeDialer =
            new FXRealSpinDial(m521, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        FXMatrix *m522 =
            new FXMatrix(m52,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        myDetectorMinSizeDialer->setValue(mySettings->minAddSize);
        new FXLabel(m522, "Exaggerate by", 0, LAYOUT_CENTER_Y);
        myDetectorUpscaleDialer =
            new FXRealSpinDial(m522, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myDetectorUpscaleDialer->setRange(1, 100);
        myDetectorUpscaleDialer->setValue(mySettings->addExaggeration);


        /*
        new FXLabel(m522, "Color", 0, LAYOUT_CENTER_Y);
        myDetectorNameColor = new FXColorWell(m522, convert(settings->addNameColor),
                            this, MID_SIMPLE_VIEW_COLORCHANGE,
                            LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                            0, 0, 100, 0,   0, 0, 0, 0);
                            */
    } {
        new FXTabItem(tabbook,"POIs",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
        FXVerticalFrame *frame6 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);

        FXMatrix *m61 =
            new FXMatrix(frame6,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
        myShowPOIName = new FXCheckButton(m61, "Show poi name", this, MID_SIMPLE_VIEW_COLORCHANGE, LAYOUT_CENTER_Y|CHECKBUTTON_NORMAL);
        myShowPOIName->setCheck(mySettings->drawPOIName);
        new FXLabel(m61, "");
        FXMatrix *m611 =
            new FXMatrix(m61,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m611, "Size", 0, LAYOUT_CENTER_Y);
        myPOINameSizeDialer =
            new FXRealSpinDial(m611, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myPOINameSizeDialer->setRange(10, 1000);
        myPOINameSizeDialer->setValue(mySettings->poiNameSize);
        FXMatrix *m612 =
            new FXMatrix(m61,2,LAYOUT_CENTER_Y|LAYOUT_FILL_X|LAYOUT_BOTTOM|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m612, "Color", 0, LAYOUT_CENTER_Y);
        myPOINameColor = new FXColorWell(m612, convert(settings->poiNameColor),
                                         this, MID_SIMPLE_VIEW_COLORCHANGE,
                                         LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                         0, 0, 100, 0,   0, 0, 0, 0);

        new FXHorizontalSeparator(frame6 ,SEPARATOR_GROOVE|LAYOUT_FILL_X);

        FXMatrix *m62 =
            new FXMatrix(frame6,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
        FXMatrix *m621 =
            new FXMatrix(m62,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        new FXLabel(m621, "Minimum size to show", 0, LAYOUT_CENTER_Y);
        myPOIMinSizeDialer =
            new FXRealSpinDial(m621, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        FXMatrix *m622 =
            new FXMatrix(m62,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,0,0, 5,5);
        myPOIMinSizeDialer->setValue(mySettings->minPOISize);
        new FXLabel(m622, "Exaggerate by", 0, LAYOUT_CENTER_Y);
        myPOIUpscaleDialer =
            new FXRealSpinDial(m622, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                               LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myPOIUpscaleDialer->setRange(1, 1000);
        myPOIUpscaleDialer->setValue(mySettings->addExaggeration);


    } {
        new FXTabItem(tabbook,"Legend",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
        FXVerticalFrame *frame7 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);

        FXMatrix *m72 =
            new FXMatrix(frame7,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
        myShowSizeLegend = new FXCheckButton(m72, "Show Size Legend", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowSizeLegend->setCheck(mySettings->showSizeLegend);
        new FXLabel(m72, "");
    } {
        new FXTabItem(tabbook,"openGL",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
        FXVerticalFrame *frame8 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);

        FXMatrix *m82 =
            new FXMatrix(frame8,1,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
        myAntialiase = new FXCheckButton(m82, "Antialiase", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myAntialiase->setCheck(mySettings->antialiase);
        myDither = new FXCheckButton(m82, "Dither", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myDither->setCheck(mySettings->dither);
    }
    FXHorizontalFrame *f2 = new FXHorizontalFrame(contentFrame, LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0, 10,10,5,5);
    FXButton *initial=new FXButton(f2,"&Use",NULL,this,MID_SETTINGS_OK,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    new FXButton(f2,"&Discard",NULL,this,MID_SETTINGS_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    initial->setFocus();

    rebuildColorMatrices(false);
    setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
}


GUIDialog_ViewSettings::~GUIDialog_ViewSettings() throw() {
    myParent->remove(this);
}


void
GUIDialog_ViewSettings::setCurrent(GUIVisualizationSettings *settings) throw() {
    mySettings = settings;
    myBackup = (*settings);
    onCmdNameChange(0, 0, 0);
}


long
GUIDialog_ViewSettings::onCmdOk(FXObject*,FXSelector,void*) {
    hide();
    return 1;
}


long
GUIDialog_ViewSettings::onCmdCancel(FXObject*,FXSelector,void*) {
    hide();
    (*mySettings) = myBackup;
    return 1;
}


long
GUIDialog_ViewSettings::onCmdNameChange(FXObject*,FXSelector,void*data) {
    if (data!=0) {
        FXString dataS = (char*) data; // !!!unicode
        // check whether this item has been added twice
        if (dataS==mySchemeName->getItemText(mySchemeName->getNumItems()-1)) {
            for (int i=0; i<mySchemeName->getNumItems()-1; ++i) {
                if (dataS==mySchemeName->getItemText(i)) {
                    mySchemeName->removeItem(i);
                }
            }
        }
        myBackup = gSchemeStorage.get(dataS.text());
        mySettings = &gSchemeStorage.get(dataS.text());
    }
    rebuildColorMatrices(true);

    myBackgroundColor->setRGBA(convert(mySettings->backgroundColor));

    myLaneEdgeColorMode->setCurrentItem(mySettings->getLaneEdgeMode());
    myShowLaneBorders->setCheck(mySettings->laneShowBorders);
    myShowLaneDecals->setCheck(mySettings->showLinkDecals);
    myShowRails->setCheck(mySettings->showRails);
    myShowEdgeName->setCheck(mySettings->drawEdgeName);
    myEdgeNameSizeDialer->setValue(mySettings->edgeNameSize);
    myEdgeNameColor->setRGBA(convert(mySettings->edgeNameColor));
    myShowInternalEdgeName->setCheck(mySettings->drawInternalEdgeName);
    myInternalEdgeNameSizeDialer->setValue(mySettings->internalEdgeNameSize);
    myInternalEdgeNameColor->setRGBA(convert(mySettings->internalEdgeNameColor));
    myHideMacroConnectors->setCheck(mySettings->hideConnectors);

    myVehicleColorMode->setCurrentItem(mySettings->vehicleColorer.getActive());
    myVehicleShapeDetail->setCurrentItem(mySettings->vehicleQuality);
    myVehicleUpscaleDialer->setValue(mySettings->vehicleExaggeration);
    myVehicleMinSizeDialer->setValue(mySettings->minVehicleSize);
    myShowBlinker->setCheck(mySettings->showBlinker);
    /*
    myShowC2CRadius->setCheck(mySettings->drawcC2CRadius);
    myShowLaneChangePreference->setCheck(mySettings->drawLaneChangePreference);
    */
    myShowVehicleName->setCheck(mySettings->drawVehicleName);
    myVehicleNameSizeDialer->setValue(mySettings->vehicleNameSize);
    myVehicleNameColor->setRGBA(convert(mySettings->vehicleNameColor));

    myShowTLIndex->setCheck(mySettings->drawLinkTLIndex);
    myShowJunctionIndex->setCheck(mySettings->drawLinkJunctionIndex);
    myShowJunctionName->setCheck(mySettings->drawJunctionName);
    myJunctionNameSizeDialer->setValue(mySettings->junctionNameSize);
    myJunctionNameColor->setRGBA(convert(mySettings->junctionNameColor));

    myDetectorUpscaleDialer->setValue(mySettings->addExaggeration);
    myDetectorMinSizeDialer->setValue(mySettings->minAddSize);
    myShowAddName->setCheck(mySettings->drawAddName);
    myAddNameSizeDialer->setValue(mySettings->addNameSize);
    //myDetectorNameColor->setRGBA(convert(mySettings->addNameColor));

    myPOIUpscaleDialer->setValue(mySettings->poiExaggeration);
    myPOIMinSizeDialer->setValue(mySettings->minPOISize);
    myShowPOIName->setCheck(mySettings->drawPOIName);
    myPOINameSizeDialer->setValue(mySettings->poiNameSize);
    myPOINameColor->setRGBA(convert(mySettings->poiNameColor));

    myShowLane2Lane->setCheck(mySettings->showLane2Lane);
    myAntialiase->setCheck(mySettings->antialiase);
    myDither->setCheck(mySettings->dither);
    myShowSizeLegend->setCheck(mySettings->showSizeLegend);

    myParent->setColorScheme(mySettings->name);
    update();
    myParent->update();
    return 1;
}


long
GUIDialog_ViewSettings::onCmdColorChange(FXObject*sender,FXSelector,void*val) {
    GUIVisualizationSettings tmpSettings = *mySettings;
    size_t prevLaneMode = mySettings->getLaneEdgeMode();
    size_t prevVehicleMode = mySettings->vehicleColorer.getActive();
    bool doRebuildColorMatrices = false;

    tmpSettings.name = mySettings->name;
    tmpSettings.backgroundColor = convert(myBackgroundColor->getRGBA());
    tmpSettings.showGrid = myShowGrid->getCheck()!=0;
    tmpSettings.gridXSize = (SUMOReal) myGridXSizeDialer->getValue();
    tmpSettings.gridYSize = (SUMOReal) myGridYSizeDialer->getValue();

#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        tmpSettings.edgeColorer.setActive(myLaneEdgeColorMode->getCurrentItem());
    } else {
#endif
        tmpSettings.laneColorer.setActive(myLaneEdgeColorMode->getCurrentItem());
#ifdef HAVE_MESOSIM
    }
#endif
    tmpSettings.laneShowBorders = myShowLaneBorders->getCheck()!=0;
    tmpSettings.showLinkDecals = myShowLaneDecals->getCheck()!=0;
    tmpSettings.showRails = myShowRails->getCheck()!=0;
    tmpSettings.drawEdgeName = myShowEdgeName->getCheck()!=0;
    tmpSettings.edgeNameSize = (SUMOReal) myEdgeNameSizeDialer->getValue();
    tmpSettings.edgeNameColor = convert(myEdgeNameColor->getRGBA());
    tmpSettings.drawInternalEdgeName = myShowInternalEdgeName->getCheck()!=0;
    tmpSettings.internalEdgeNameSize = (SUMOReal) myInternalEdgeNameSizeDialer->getValue();
    tmpSettings.internalEdgeNameColor = convert(myInternalEdgeNameColor->getRGBA());
    tmpSettings.hideConnectors = myHideMacroConnectors->getCheck()!=0;

    tmpSettings.vehicleColorer.setActive(myVehicleColorMode->getCurrentItem());
    tmpSettings.vehicleQuality = myVehicleShapeDetail->getCurrentItem();
    tmpSettings.vehicleExaggeration = (SUMOReal) myVehicleUpscaleDialer->getValue();
    tmpSettings.minVehicleSize = (SUMOReal) myVehicleMinSizeDialer->getValue();
    tmpSettings.showBlinker = myShowBlinker->getCheck()!=0;
    /*
    tmpSettings.drawcC2CRadius = myShowC2CRadius->getCheck()!=0;
    tmpSettings.drawLaneChangePreference = myShowLaneChangePreference->getCheck()!=0;
    */
    tmpSettings.drawVehicleName = myShowVehicleName->getCheck()!=0;
    tmpSettings.vehicleNameSize = (SUMOReal) myVehicleNameSizeDialer->getValue();
    tmpSettings.vehicleNameColor = convert(myVehicleNameColor->getRGBA());

    tmpSettings.drawLinkTLIndex = myShowTLIndex->getCheck()!=0;
    tmpSettings.drawLinkJunctionIndex = myShowJunctionIndex->getCheck()!=0;
    tmpSettings.drawJunctionName = myShowJunctionName->getCheck()!=0;
    tmpSettings.junctionNameSize = (SUMOReal) myJunctionNameSizeDialer->getValue();
    tmpSettings.junctionNameColor = convert(myJunctionNameColor->getRGBA());

    tmpSettings.addExaggeration = (SUMOReal) myDetectorUpscaleDialer->getValue();
    tmpSettings.minAddSize = (SUMOReal) myDetectorMinSizeDialer->getValue();
    tmpSettings.drawAddName = myShowAddName->getCheck()!=0;
    tmpSettings.addNameSize = (SUMOReal) myAddNameSizeDialer->getValue();
    //mySettings->addNameColor = convert(myDetectorNameColor->getRGBA());

    tmpSettings.poiExaggeration = (SUMOReal) myPOIUpscaleDialer->getValue();
    tmpSettings.minPOISize = (SUMOReal) myPOIMinSizeDialer->getValue();
    tmpSettings.drawPOIName = myShowPOIName->getCheck()!=0;
    tmpSettings.poiNameSize = (SUMOReal) myPOINameSizeDialer->getValue();
    tmpSettings.poiNameColor = convert(myPOINameColor->getRGBA());

    tmpSettings.showLane2Lane = myShowLane2Lane->getCheck()!=0;
    tmpSettings.antialiase = myAntialiase->getCheck()!=0;
    tmpSettings.dither = myDither->getCheck()!=0;
    tmpSettings.showSizeLegend = myShowSizeLegend->getCheck()!=0;

    // lanes
    if (tmpSettings.getLaneEdgeMode()==prevLaneMode) {
        std::vector<FXColorWell*>::const_iterator colIt = myLaneColors.begin();
        std::vector<FXRealSpinDial*>::const_iterator threshIt = myLaneThresholds.begin();
        std::vector<FXButton*>::const_iterator buttonIt = myLaneButtons.begin();
        GUIColorScheme &scheme = tmpSettings.getLaneEdgeScheme();
        size_t pos = 0;
        while (colIt != myLaneColors.end()) {
            if (scheme.isFixed()) {
                if (sender == *colIt) {
                    scheme.setColor(pos, convert((*colIt)->getRGBA()));
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
                    if (threshIt != myLaneThresholds.end()) {
                        (*threshIt)->getRange(lo, hi);
                        (*threshIt)->setRange(val, hi);
                    }
                    scheme.setThreshold(pos, val);
                    break;
                }
                if (sender == *colIt) {
                    scheme.setColor(pos, convert((*colIt)->getRGBA()));
                    break;
                }
                if (sender == *buttonIt) {
                    if (pos == 0) {
                        scheme.addColor(convert((*colIt)->getRGBA()), (*threshIt)->getValue());
                    } else {
                        scheme.removeColor(pos);
                    }
                    doRebuildColorMatrices = true;
                    break;
                }
                ++threshIt;
                ++buttonIt;
            }
            ++colIt;
            pos++;
        }
        if (sender == myLaneColorInterpolation) {
            scheme.setInterpolated(myLaneColorInterpolation->getCheck());
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }
    // vehicles
    if (tmpSettings.vehicleColorer.getActive()==prevVehicleMode) {
        std::vector<FXColorWell*>::const_iterator colIt = myVehicleColors.begin();
        std::vector<FXRealSpinDial*>::const_iterator threshIt = myVehicleThresholds.begin();
        std::vector<FXButton*>::const_iterator buttonIt = myVehicleButtons.begin();
        GUIColorScheme &scheme = tmpSettings.vehicleColorer.getScheme();
        size_t pos = 0;
        while (colIt != myVehicleColors.end()) {
            if (scheme.isFixed()) {
                if (sender == *colIt) {
                    scheme.setColor(pos, convert((*colIt)->getRGBA()));
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
                    if (threshIt != myVehicleThresholds.end()) {
                        (*threshIt)->getRange(lo, hi);
                        (*threshIt)->setRange(val, hi);
                    }
                    scheme.setThreshold(pos, val);
                    break;
                }
                if (sender == *colIt) {
                    scheme.setColor(pos, convert((*colIt)->getRGBA()));
                    break;
                }
                if (sender == *buttonIt) {
                    if (pos == 0) {
                        scheme.addColor(convert((*colIt)->getRGBA()), (*threshIt)->getValue());
                    } else {
                        scheme.removeColor(pos);
                    }
                    doRebuildColorMatrices = true;
                    break;
                }
                ++threshIt;
                ++buttonIt;
            }
            ++colIt;
            pos++;
        }
        if (sender == myVehicleColorInterpolation) {
            scheme.setInterpolated(myVehicleColorInterpolation->getCheck());
            doRebuildColorMatrices = true;
        }
    } else {
        doRebuildColorMatrices = true;
    }

    if (tmpSettings==*mySettings) {
        return 1;
    }

    if (tmpSettings.name[0]!='*') {
        tmpSettings.name = '*' + tmpSettings.name;
    }
    gSchemeStorage.add(tmpSettings);
    int index = mySchemeName->getCurrentItem();
    if (index<gSchemeStorage.getNumInitialSettings()) {
        index = mySchemeName->appendItem(tmpSettings.name.c_str());
        gSchemeStorage.add(tmpSettings);
        mySchemeName->setCurrentItem(index);
        myParent->getColoringSchemesCombo().appendItem(tmpSettings.name.c_str());
        myParent->getColoringSchemesCombo().setCurrentItem(index);
        myParent->setColorScheme(tmpSettings.name);
    } else {
        mySchemeName->setItemText(index, tmpSettings.name.c_str());
        myParent->getColoringSchemesCombo().setItemText(index, tmpSettings.name.c_str());
        myParent->setColorScheme(tmpSettings.name);
    }
    mySettings = &gSchemeStorage.get(tmpSettings.name);

    if (doRebuildColorMatrices) {
        rebuildColorMatrices(true);
    }
    myParent->forceRefresh();
    getApp()->forceRefresh();
    return 1;
}


void
GUIDialog_ViewSettings::loadSettings(const std::string &file) throw() {
    GUISettingsHandler handler(file);
    std::string settingsName = handler.addSettings(myParent);
    if (settingsName != "") {
        FXint index = mySchemeName->appendItem(settingsName.c_str());
        mySchemeName->setCurrentItem(index);
        mySettings = &gSchemeStorage.get(settingsName);
    }
}


void
GUIDialog_ViewSettings::saveDecals(const std::string &file) const throw() {
    try {
        OutputDevice &dev = OutputDevice::getDevice(file);
        dev << "<decals>\n";
        std::vector<GUISUMOAbstractView::Decal>::iterator j;
        for (j=myDecals->begin(); j!=myDecals->end(); ++j) {
            GUISUMOAbstractView::Decal &d = *j;
            dev << "    <decal filename=\"" << d.filename
            << "\" centerX=\"" << d.centerX
            << "\" centerY=\"" << d.centerY
            << "\" width=\"" << d.width
            << "\" height=\"" << d.height
            << "\" rotation=\"" << d.rot
            << "\"/>\n";
        }
        dev << "</decals>\n";
        dev.close();
    } catch (IOError &e) {
        FXMessageBox::error(myParent, MBOX_OK, "Storing failed!", e.what());
    }
}


void
GUIDialog_ViewSettings::loadDecals(const std::string &file) throw() {
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
GUIDialog_ViewSettings::onCmdSaveSetting(FXObject*,FXSelector,void*data) {
    int index = mySchemeName->getCurrentItem();
    if (index<gSchemeStorage.getNumInitialSettings()) {
        return 1;
    }
    // get the name
    while (true) {
        FXDialogBox dialog(this,"Enter a name",DECOR_TITLE|DECOR_BORDER);
        FXVerticalFrame* content=new FXVerticalFrame(&dialog,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,10,10,10,10,10,10);
        new FXLabel(content,"Please enter an alphanumeric name: ",NULL,LAYOUT_FILL_X|JUSTIFY_LEFT);
        FXTextField *text=new FXTextField(content,40,&dialog,FXDialogBox::ID_ACCEPT,TEXTFIELD_ENTER_ONLY|FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X);
        new FXHorizontalSeparator(content,SEPARATOR_GROOVE|LAYOUT_FILL_X);
        FXHorizontalFrame* buttons=new FXHorizontalFrame(content,LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0,0,0,0,0);
        new FXButton(buttons,"&OK",NULL,&dialog,FXDialogBox::ID_ACCEPT,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT);
        new FXButton(buttons,"&Cancel",NULL,&dialog,FXDialogBox::ID_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_RIGHT,0,0,0,0,20,20);
        dialog.create();
        text->setFocus();
        if (dialog.execute()) {
            string name = text->getText().text();
            bool isAlphaNum = true;
            for (size_t i=0; i<name.length(); ++i) {
                if (name[i]=='_' || (name[i]>='a'&&name[i]<='z') || (name[i]>='A'&&name[i]<='Z') || (name[i]>='0'&&name[i]<='9')) {
                    continue;
                }
                isAlphaNum = false;
            }
            isAlphaNum = isAlphaNum & (name.length()>0);
            if (isAlphaNum) {
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
        } else {
            return 1;
        }
    }
    // save
    return 1;
}


long
GUIDialog_ViewSettings::onUpdSaveSetting(FXObject*sender,FXSelector,void*ptr) {
    sender->handle(this,
                   mySchemeName->getCurrentItem()<gSchemeStorage.getNumInitialSettings()
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIDialog_ViewSettings::onCmdDeleteSetting(FXObject*,FXSelector,void*data) {
    int index = mySchemeName->getCurrentItem();
    if (index<gSchemeStorage.getNumInitialSettings()) {
        return 1;
    }
    string name = mySchemeName->getItem(index).text();
    gSchemeStorage.remove(name);
    mySchemeName->removeItem(index);
    onCmdNameChange(0, 0, (void*) mySchemeName->getItem(0).text());
    gSchemeStorage.writeSettings(getApp());
    return 1;
}


long
GUIDialog_ViewSettings::onUpdDeleteSetting(FXObject*sender,FXSelector,void*ptr) {
    sender->handle(this,
                   mySchemeName->getCurrentItem()<gSchemeStorage.getNumInitialSettings()
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIDialog_ViewSettings::onCmdExportSetting(FXObject*,FXSelector,void*data) {
    FXFileDialog opendialog(this, "Export view settings");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (!opendialog.execute()||!MFXUtils::userPermitsOverwritingWhenFileExists(this, opendialog.getFilename())) {
        return 1;
    }
    try {
        OutputDevice &dev = OutputDevice::getDevice(opendialog.getFilename().text());
        mySettings->save(dev);
        dev.close();
    } catch (IOError &e) {
        FXMessageBox::error(this, MBOX_OK, "Storing failed!", e.what());
    }
    return 1;
}


long
GUIDialog_ViewSettings::onUpdExportSetting(FXObject*sender,FXSelector,void*ptr) {
    sender->handle(this,
                   mySchemeName->getCurrentItem()<gSchemeStorage.getNumInitialSettings()
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIDialog_ViewSettings::onCmdImportSetting(FXObject*,FXSelector,void*data) {
    FXFileDialog opendialog(this, "Import view settings");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        loadSettings(opendialog.getFilename().text());
    }
    return 1;
}


long
GUIDialog_ViewSettings::onCmdLoadDecals(FXObject*,FXSelector,void*data) {
    FXFileDialog opendialog(this, "Load Decals");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory();
        loadDecals(opendialog.getFilename().text());
    }
    return 1;
}


long
GUIDialog_ViewSettings::onCmdSaveDecals(FXObject*,FXSelector,void*data) {
    FXFileDialog opendialog(this, "Save Decals");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (!opendialog.execute()||!MFXUtils::userPermitsOverwritingWhenFileExists(this, opendialog.getFilename())) {
        return 1;
    }
    saveDecals(opendialog.getFilename().text());
    return 1;
}




long
GUIDialog_ViewSettings::onUpdImportSetting(FXObject*sender,FXSelector,void*ptr) {
    sender->handle(this, FXSEL(SEL_COMMAND,ID_ENABLE), ptr);
    return 1;
}


RGBColor
GUIDialog_ViewSettings::convert(const FXColor c) const throw() {
    return RGBColor(
               (SUMOReal) FXREDVAL(c) / (SUMOReal) 255.,
               (SUMOReal) FXGREENVAL(c) / (SUMOReal) 255.,
               (SUMOReal) FXBLUEVAL(c) / (SUMOReal) 255.);
}


FXColor
GUIDialog_ViewSettings::convert(const RGBColor &c) const throw() {
    return FXRGB(c.red()*255., c.green()*255., c.blue()*255.);
}


void
GUIDialog_ViewSettings::rebuildList() throw() {
    myDecalsTable->clearItems();
    // set table attributes
    myDecalsTable->setTableSize(10, 6);
    myDecalsTable->setColumnText(0, "picture file");
    myDecalsTable->setColumnText(1, "center x");
    myDecalsTable->setColumnText(2, "center y");
    myDecalsTable->setColumnText(3, "width");
    myDecalsTable->setColumnText(4, "height");
    myDecalsTable->setColumnText(5, "rotation");
    FXHeader *header = myDecalsTable->getColumnHeader();
    header->setHeight(getApp()->getNormalFont()->getFontHeight()+getApp()->getNormalFont()->getFontAscent());
    int k;
    for (k=0; k<6; k++) {
        header->setItemJustify(k, JUSTIFY_CENTER_X|JUSTIFY_TOP);
        header->setItemSize(k, 60);
    }
    header->setItemSize(0, 150);
    // insert already known decals information into table
    FXint row = 0;
    std::vector<GUISUMOAbstractView::Decal>::iterator j;
    for (j=myDecals->begin(); j!=myDecals->end(); ++j) {
        GUISUMOAbstractView::Decal &d = *j;
        myDecalsTable->setItemText(row, 0, d.filename.c_str());
        myDecalsTable->setItemText(row, 1, toString<SUMOReal>(d.centerX).c_str());
        myDecalsTable->setItemText(row, 2, toString<SUMOReal>(d.centerY).c_str());
        myDecalsTable->setItemText(row, 3, toString<SUMOReal>(d.width).c_str());
        myDecalsTable->setItemText(row, 4, toString<SUMOReal>(d.height).c_str());
        myDecalsTable->setItemText(row, 5, toString<SUMOReal>(d.rot).c_str());
        row++;
    }
    // insert dummy last field
    for (k=0; k<6; k++) {
        myDecalsTable->setItemText(row, k, " ");
    }
}


void
GUIDialog_ViewSettings::rebuildColorMatrices(bool doCreate) throw() {
    // decals
    delete myDecalsTable;
    myDecalsTable = new MFXAddEditTypedTable(myDecalsFrame, this, MID_TABLE,
            LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH/*|LAYOUT_FIX_HEIGHT*/, 0,0, 470, 0);
    myDecalsTable->setVisibleRows(5);
    myDecalsTable->setVisibleColumns(6);
    myDecalsTable->setTableSize(5,6);
    myDecalsTable->setBackColor(FXRGB(255,255,255));
    myDecalsTable->getRowHeader()->setWidth(0);
    for (int i=1; i<5; ++i) {
        myDecalsTable->setCellType(i, CT_REAL);
        myDecalsTable->setNumberCellParams(i, -10000000, 10000000, 1, 10, 100, "%.2f");
    }
    myDecalsTable->setCellType(5, CT_REAL);
    myDecalsTable->setNumberCellParams(5, -10000000, 10000000, .1, 1, 10, "%.2f");
    rebuildList();
    if (doCreate) {
        myDecalsTable->create();
    }
    {
        // lane / edge
        MFXUtils::deleteChildren(myLaneColorSettingFrame);
        FXMatrix *m = new FXMatrix(myLaneColorSettingFrame,3,
                                   LAYOUT_FILL_X|MATRIX_BY_COLUMNS,
                                   0,0,0,0,10,10,0,0, 5,3);
        myLaneColors.clear();
        myLaneThresholds.clear();
        myLaneButtons.clear();
        GUIColorScheme &scheme = mySettings->getLaneEdgeScheme();
        const bool interpolate = scheme.isInterpolated();
        const bool fixed = scheme.isFixed();
        const std::vector<RGBColor> &colors = scheme.getColors();
        std::vector<RGBColor>::const_iterator colIt = colors.begin();
        std::vector<SUMOReal>::const_iterator threshIt = scheme.getThresholds().begin();
        std::vector<std::string>::const_iterator nameIt = scheme.getNames().begin();
        FX::FXString buttonText = "Add";
        while (colIt != colors.end()) {
            myLaneColors.push_back(new FXColorWell(m , convert(*colIt),
                                                   this, MID_SIMPLE_VIEW_COLORCHANGE,
                                                   LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                                   0, 0, 100, 0,   0, 0, 0, 0));
            if (fixed) {
                new FXLabel(m, nameIt->c_str());
                new FXLabel(m, "");
            } else {
                FXRealSpinDial* threshDialer =
                    new FXRealSpinDial(m, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                                       LAYOUT_CENTER_Y|LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK|SPINDIAL_NOMAX);
                threshDialer->setValue(*threshIt);
                myLaneThresholds.push_back(threshDialer);
                myLaneButtons.push_back(new FXButton(m,buttonText,NULL,this,MID_SIMPLE_VIEW_COLORCHANGE,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 20,20,4,4));
                buttonText = "Remove";
            }
            colIt++;
            threshIt++;
            nameIt++;
        }
        myLaneColorInterpolation->setCheck(scheme.isInterpolated());
        if (fixed) {
            myLaneColorInterpolation->disable();
        } else {
            if (colors.size() > 1) {
                myLaneColorInterpolation->enable();
                if (myLaneColorInterpolation->getCheck()) {
                    myLaneThresholds.front()->enable();
                } else {
                    myLaneThresholds.front()->disable();
                }
            } else {
                myLaneColorInterpolation->disable();
                myLaneThresholds.front()->disable();
            }
        }
        if (doCreate) {
            m->create();
        }
        myLaneColorSettingFrame->getParent()->recalc();
    }
    {
        // vehicles
        MFXUtils::deleteChildren(myVehicleColorSettingFrame);
        FXMatrix *m = new FXMatrix(myVehicleColorSettingFrame,3,
                                   LAYOUT_FILL_X|MATRIX_BY_COLUMNS,
                                   0,0,0,0,10,10,0,0, 5,3);
        myVehicleColors.clear();
        myVehicleThresholds.clear();
        myVehicleButtons.clear();
        const bool interpolate = mySettings->vehicleColorer.getScheme().isInterpolated();
        const bool fixed = mySettings->vehicleColorer.getScheme().isFixed();
        const std::vector<RGBColor> &colors = mySettings->vehicleColorer.getScheme().getColors();
        std::vector<RGBColor>::const_iterator colIt = colors.begin();
        std::vector<SUMOReal>::const_iterator threshIt = mySettings->vehicleColorer.getScheme().getThresholds().begin();
        std::vector<std::string>::const_iterator nameIt = mySettings->vehicleColorer.getScheme().getNames().begin();
        FX::FXString buttonText = "Add";
        while (colIt != colors.end()) {
            myVehicleColors.push_back(new FXColorWell(m , convert(*colIt),
                                      this, MID_SIMPLE_VIEW_COLORCHANGE,
                                      LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                      0, 0, 100, 0,   0, 0, 0, 0));
            if (fixed) {
                new FXLabel(m, nameIt->c_str());
                new FXLabel(m, "");
            } else {
                FXRealSpinDial* threshDialer =
                    new FXRealSpinDial(m, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                                       LAYOUT_CENTER_Y|LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK|SPINDIAL_NOMAX);
                threshDialer->setValue(*threshIt);
                myVehicleThresholds.push_back(threshDialer);
                myVehicleButtons.push_back(new FXButton(m,buttonText,NULL,this,MID_SIMPLE_VIEW_COLORCHANGE,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 20,20,4,4));
                buttonText = "Remove";
            }
            colIt++;
            threshIt++;
            nameIt++;
        }
        myVehicleColorInterpolation->setCheck(mySettings->vehicleColorer.getScheme().isInterpolated());
        if (fixed) {
            myVehicleColorInterpolation->disable();
        } else {
            if (colors.size() > 1) {
                myVehicleColorInterpolation->enable();
                if (myVehicleColorInterpolation->getCheck()) {
                    myVehicleThresholds.front()->enable();
                } else {
                    myVehicleThresholds.front()->disable();
                }
            } else {
                myVehicleColorInterpolation->disable();
                myVehicleThresholds.front()->disable();
            }
        }
        if (doCreate) {
            m->create();
        }
        myVehicleColorSettingFrame->getParent()->recalc();
    }
    layout();
    update();
}


long
GUIDialog_ViewSettings::onCmdEditTable(FXObject*,FXSelector,void*data) {
    MFXEditedTableItem *i = (MFXEditedTableItem*) data;
    string value = i->item->getText().text();
    // check whether the inserted value is empty
    if (value.find_first_not_of(" ")==string::npos) {
        return 1;
    }
    GUISUMOAbstractView::Decal d;
    int row = i->row;
    if (row==myDecals->size()) {
        d.filename = "";
        d.centerX = SUMOReal(myParent->getGridWidth() / 2.);
        d.centerY = SUMOReal(myParent->getGridHeight() / 2.);
        d.width = SUMOReal(myParent->getGridWidth());
        d.height = SUMOReal(myParent->getGridHeight());
        d.initialised = false;
        d.rot = 0;
        myDecalsLock->lock();
        myDecals->push_back(d);
        myDecalsLock->unlock();
    } else {
        d = (*myDecals)[row];
    }

    switch (i->col) {
    case 0:
        if (d.initialised&&d.filename!=value) {
            d.initialised = false;
        }
        d.filename = value;
        break;
    case 1:
        try {
            d.centerX = TplConvert<char>::_2SUMOReal(value.c_str());
        } catch (NumberFormatException &) {
            string msg = "The value must be a float, is:" + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 2:
        try {
            d.centerY = TplConvert<char>::_2SUMOReal(value.c_str());
        } catch (NumberFormatException &) {
            string msg = "The value must be a float, is:" + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 3:
        try {
            d.width = TplConvert<char>::_2SUMOReal(value.c_str());
        } catch (NumberFormatException &) {
            string msg = "The value must be a float, is:" + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 4:
        try {
            d.height = TplConvert<char>::_2SUMOReal(value.c_str());
        } catch (NumberFormatException &) {
            string msg = "The value must be a float, is:" + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 5:
        try {
            d.rot = TplConvert<char>::_2SUMOReal(value.c_str());
        } catch (NumberFormatException &) {
            string msg = "The value must be a float, is:" + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
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
GUIDialog_ViewSettings::getCurrentScheme() const throw() {
    return mySchemeName->getItem(mySchemeName->getCurrentItem()).text();
}


void
GUIDialog_ViewSettings::setCurrentScheme(const std::string &name) throw() {
    if (name.c_str()==mySchemeName->getItemText(mySchemeName->getCurrentItem())) {
        return;
    }
    for (int i=0; i<mySchemeName->getNumItems(); ++i) {
        if (name.c_str()==mySchemeName->getItemText(i)) {
            mySchemeName->setCurrentItem(i);
            onCmdNameChange(0,0,(void*)name.c_str());
            return;
        }
    }
}


/****************************************************************************/

