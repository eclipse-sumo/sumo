/****************************************************************************/
/// @file    GUIDialog_ViewSettings.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 21. Dec 2005
/// @version $Id$
///
// The dialog to change the view (gui) settings.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include <utils/gui/drawer/GUIColoringSchemesMap.h>
#include <utils/foxtools/MFXAddEditTypedTable.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/TplConvert.h>
#include <utils/gui/drawer/GUICompleteSchemeStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/importio/LineReader.h>
#include <utils/iodevices/OutputDevice.h>

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
    FXMAPFUNC(SEL_CHANGED,  MID_SIMPLE_VIEW_NAMECHANGE,     GUIDialog_ViewSettings::onChgNameChange),
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

};


FXIMPLEMENT(GUIDialog_ViewSettings, FXDialogBox, GUIDialog_ViewSettingsMap, ARRAYNUMBER(GUIDialog_ViewSettingsMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_ViewSettings::GUIDialog_ViewSettings(
    GUISUMOAbstractView *parent,
    GUISUMOAbstractView::VisualizationSettings *settings,
    BaseSchemeInfoSource *laneEdgeModeSource,
    BaseSchemeInfoSource *vehicleModeSource,
    std::vector<GUISUMOAbstractView::Decal> *decals,
    MFXMutex *decalsLock) throw()
        : FXDialogBox(parent, "View Settings"),
        myParent(parent), mySettings(settings),
        myLaneColoringInfoSource(laneEdgeModeSource),
        myVehicleColoringInfoSource(vehicleModeSource),
        myDecals(decals), myDecalsLock(decalsLock), myDecalsTable(0)
{
    myBackup = (*mySettings);

    FXVerticalFrame *contentFrame =
        new FXVerticalFrame(this, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
                            0,0,0,0, 0,0,0,0, 5,5);
    //
    {
        FXHorizontalFrame *frame0 =
            new FXHorizontalFrame(contentFrame,FRAME_THICK, 0,0,0,0, 0,0,0,0, 2,2);
        mySchemeName = new FXComboBox(frame0, 20, this, MID_SIMPLE_VIEW_NAMECHANGE, COMBOBOX_INSERT_LAST|FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_CENTER_Y|COMBOBOX_STATIC);
        const std::vector<std::string> &names = gSchemeStorage.getNames();
        for (std::vector<std::string>::const_iterator i=names.begin(); i!=names.end(); ++i) {
            mySchemeName->appendItem((*i).c_str());
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
        new FXTabBook(contentFrame,0,0,TABBOOK_LEFTTABS|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_RIGHT);
    {
        // tab for the background
        FXTabItem *tab1 = new FXTabItem(tabbook,"Background",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
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
        FXTabItem *tab2 = new FXTabItem(tabbook,"Streets",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
        FXVerticalFrame *frame2 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);

        //FXSplitter *s2 = new FXSplitter(frame2, LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0, 0,80);
        FXMatrix *m21 =
            new FXMatrix(frame2,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,2, 5,5);
        new FXLabel(m21, "Color by", 0, LAYOUT_CENTER_Y);
        myLaneEdgeColorMode = new FXComboBox(m21, 30, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP|COMBOBOX_STATIC);
        laneEdgeModeSource->fill(*myLaneEdgeColorMode);
        myLaneEdgeColorMode->setNumVisible(10);
        myLaneEdgeColorMode->setCurrentItem(settings->laneEdgeMode);

        myLaneColorSettingFrame =
            new FXVerticalFrame(frame2, LAYOUT_FILL_Y,  0,0,0,0, 10,10,2,8, 5,2);

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
    }
    {
        if (myVehicleColoringInfoSource!=0) {
            FXTabItem *tab3 = new FXTabItem(tabbook,"Vehicles",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
            FXVerticalFrame *frame3 =
                new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);

            FXMatrix *m31 =
                new FXMatrix(frame3,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                             0,0,0,0, 10,10,10,2, 5,5);
            new FXLabel(m31, "Show As", 0, LAYOUT_CENTER_Y);
            FXComboBox *c31 = new FXComboBox(m31, 20, 0, 0, FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP|COMBOBOX_STATIC);
            c31->appendItem("triangles");
            c31->appendItem("boxes");
            c31->appendItem("simple shapes");
            c31->appendItem("complex shapes");
            c31->setNumVisible(4);
            c31->disable();

            new FXHorizontalSeparator(frame3,SEPARATOR_GROOVE|LAYOUT_FILL_X);

            FXMatrix *m32 =
                new FXMatrix(frame3,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                             0,0,0,0, 10,10,10,2, 5,5);
            new FXLabel(m32, "Color By", 0, LAYOUT_CENTER_Y);
            myVehicleColorMode = new FXComboBox(m32, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP|COMBOBOX_STATIC);
            myVehicleColoringInfoSource->fill(*myVehicleColorMode);
            myVehicleColorMode->setNumVisible(10);
            myVehicleColorMode->setCurrentItem(settings->vehicleMode);

            myVehicleColorSettingFrame =
                new FXVerticalFrame(frame3, LAYOUT_FILL_Y,  0,0,0,0, 10,10,2,8, 5,2);

            new FXHorizontalSeparator(frame3,SEPARATOR_GROOVE|LAYOUT_FILL_X);

            FXMatrix *m33 =
                new FXMatrix(frame3,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                             0,0,0,0, 10,10,10,10, 5,5);
            myShowBlinker = new FXCheckButton(m33, "Show blinker", this, MID_SIMPLE_VIEW_COLORCHANGE);
            myShowBlinker->setCheck(mySettings->showBlinker);
            new FXLabel(m33, " ", 0, LAYOUT_CENTER_Y);
            myShowC2CRadius = new FXCheckButton(m33, "Show C2C radius", this, MID_SIMPLE_VIEW_COLORCHANGE);
            myShowC2CRadius->setCheck(mySettings->drawcC2CRadius);
            new FXLabel(m33, " ", 0, LAYOUT_CENTER_Y);
            myShowLaneChangePreference = new FXCheckButton(m33, "Show lane change preference", this, MID_SIMPLE_VIEW_COLORCHANGE);
            myShowLaneChangePreference->setCheck(mySettings->drawLaneChangePreference);
            new FXLabel(m33, " ", 0, LAYOUT_CENTER_Y);
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
            FXCheckButton *tmpc = new FXCheckButton(m33, "Show breaking lights", 0 ,0);
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
        }
    }
    {
        FXTabItem *tab4 = new FXTabItem(tabbook,"Nodes",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
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
    }
    {
        FXTabItem *tab5 = new FXTabItem(tabbook,"Detectors/Trigger",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
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
    }
    {
        FXTabItem *tab6 = new FXTabItem(tabbook,"POIs",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
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


    }
    {
        FXTabItem *tab7 = new FXTabItem(tabbook,"Legend",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
        FXVerticalFrame *frame7 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);

        FXMatrix *m72 =
            new FXMatrix(frame7,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                         0,0,0,0, 10,10,10,10, 5,5);
        myShowSizeLegend = new FXCheckButton(m72, "Show Size Legend", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowSizeLegend->setCheck(mySettings->showSizeLegend);
        new FXLabel(m72, "");
    }
    {
        FXTabItem *tab8 = new FXTabItem(tabbook,"openGL",NULL,TAB_LEFT_NORMAL, 0,0,0,0, 4,8,4,4);
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


GUIDialog_ViewSettings::~GUIDialog_ViewSettings() throw()
{
    myParent->remove(this);
}


void
GUIDialog_ViewSettings::setCurrent(GUISUMOAbstractView::VisualizationSettings *settings) throw()
{
    mySettings = settings;
    myBackup = (*settings);
    onCmdNameChange(0, 0, 0);
}


long
GUIDialog_ViewSettings::onCmdOk(FXObject*,FXSelector,void*)
{
    hide();
    return 1;
}


long
GUIDialog_ViewSettings::onCmdCancel(FXObject*,FXSelector,void*)
{
    hide();
    (*mySettings) = myBackup;
    return 1;
}


long
GUIDialog_ViewSettings::onChgNameChange(FXObject*,FXSelector,void*)
{
    return 1;
}


long
GUIDialog_ViewSettings::onCmdNameChange(FXObject*,FXSelector,void*data)
{
    if(data!=0) {
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

    myLaneEdgeColorMode->setCurrentItem(mySettings->laneEdgeMode);
    myShowLaneBorders->setCheck(mySettings->laneShowBorders);
    myShowLaneDecals->setCheck(mySettings->showLinkDecals);
    myShowRails->setCheck(mySettings->showRails);
    myShowEdgeName->setCheck(mySettings->drawEdgeName);
    myEdgeNameSizeDialer->setValue(mySettings->edgeNameSize);
    myEdgeNameColor->setRGBA(convert(mySettings->edgeNameColor));

    myVehicleColorMode->setCurrentItem(mySettings->vehicleMode);
    myVehicleUpscaleDialer->setValue(mySettings->vehicleExaggeration);
    myVehicleMinSizeDialer->setValue(mySettings->minVehicleSize);
    myShowBlinker->setCheck(mySettings->showBlinker);
    myShowC2CRadius->setCheck(mySettings->drawcC2CRadius);
    myShowLaneChangePreference->setCheck(mySettings->drawLaneChangePreference);
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

    // lanes
    switch (myLaneColoringInfoSource->getColorSetType(myBackup.laneEdgeMode)) {
    case CST_SINGLE:
        mySingleLaneColor->setRGBA(convert(
                                       mySettings->laneColorings[mySettings->laneEdgeMode][0]));
        break;
    case CST_MINMAX:
        myMinLaneColor->setRGBA(convert(
                                    mySettings->laneColorings[mySettings->laneEdgeMode][0]));
        myMaxLaneColor->setRGBA(convert(
                                    mySettings->laneColorings[mySettings->laneEdgeMode][1]));
        break;
    default:
        break;
    }
    // vehicles
    if (myVehicleColoringInfoSource!=0) {
        switch (myVehicleColoringInfoSource->getColorSetType(myBackup.vehicleMode)) {
        case CST_SINGLE:
            mySingleVehicleColor->setRGBA(convert(
                                              mySettings->vehicleColorings[mySettings->vehicleMode][0]));
            break;
        case CST_MINMAX:
            myMinVehicleColor->setRGBA(convert(
                                           mySettings->vehicleColorings[mySettings->vehicleMode][0]));
            myMaxVehicleColor->setRGBA(convert(
                                           mySettings->vehicleColorings[mySettings->vehicleMode][1]));
            break;
        default:
            break;
        }
    }
    myParent->setColorScheme((char*) mySettings->name.c_str());
    update();
    myParent->update();
    return 1;
}



long
GUIDialog_ViewSettings::onCmdColorChange(FXObject*sender,FXSelector,void*val)
{
    GUISUMOAbstractView::VisualizationSettings tmpSettings = *mySettings;
    int prevLaneMode = mySettings->laneEdgeMode;
    int prevVehicleMode = mySettings->vehicleMode;

    tmpSettings.name = mySettings->name;
    tmpSettings.backgroundColor = convert(myBackgroundColor->getRGBA());
    tmpSettings.showGrid = myShowGrid->getCheck()!=0;
    tmpSettings.gridXSize = (SUMOReal) myGridXSizeDialer->getValue();
    tmpSettings.gridYSize = (SUMOReal) myGridYSizeDialer->getValue();

    tmpSettings.laneEdgeMode = myLaneEdgeColorMode->getCurrentItem();
    tmpSettings.laneShowBorders = myShowLaneBorders->getCheck()!=0;
    tmpSettings.showLinkDecals = myShowLaneDecals->getCheck()!=0;
    tmpSettings.showRails = myShowRails->getCheck()!=0;
    tmpSettings.drawEdgeName = myShowEdgeName->getCheck()!=0;
    tmpSettings.edgeNameSize = (SUMOReal) myEdgeNameSizeDialer->getValue();
    tmpSettings.edgeNameColor = convert(myEdgeNameColor->getRGBA());

    if (myVehicleColoringInfoSource!=0) {
        tmpSettings.vehicleMode = myVehicleColorMode->getCurrentItem();
        tmpSettings.vehicleExaggeration = (SUMOReal) myVehicleUpscaleDialer->getValue();
        tmpSettings.minVehicleSize = (SUMOReal) myVehicleMinSizeDialer->getValue();
        tmpSettings.showBlinker = myShowBlinker->getCheck()!=0;
        tmpSettings.drawcC2CRadius = myShowC2CRadius->getCheck()!=0;
        tmpSettings.drawLaneChangePreference = myShowLaneChangePreference->getCheck()!=0;
        tmpSettings.drawVehicleName = myShowVehicleName->getCheck()!=0;
        tmpSettings.vehicleNameSize = (SUMOReal) myVehicleNameSizeDialer->getValue();
        tmpSettings.vehicleNameColor = convert(myVehicleNameColor->getRGBA());
    }

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
    if (tmpSettings.laneEdgeMode==prevLaneMode) {
        switch (myLaneColoringInfoSource->getColorSetType(tmpSettings.laneEdgeMode)) {
        case CST_SINGLE:
            if (sender==mySingleLaneColor) {
                tmpSettings.laneColorings[tmpSettings.laneEdgeMode][0] = convert((FXColor)(FXuval) val);
                myLaneColoringInfoSource->getColorerInterface(tmpSettings.laneEdgeMode)->resetColor(
                    tmpSettings.laneColorings[tmpSettings.laneEdgeMode][0]);
            }
            break;
        case CST_MINMAX:
            if (sender==myMinLaneColor) {
                tmpSettings.laneColorings[tmpSettings.laneEdgeMode][0] = convert((FXColor)(FXuval) val);
            }
            if (sender==myMaxLaneColor) {
                tmpSettings.laneColorings[tmpSettings.laneEdgeMode][1] = convert((FXColor)(FXuval) val);
            }
            myLaneColoringInfoSource->getColorerInterface(tmpSettings.laneEdgeMode)->resetColor(
                tmpSettings.laneColorings[tmpSettings.laneEdgeMode][0],
                tmpSettings.laneColorings[tmpSettings.laneEdgeMode][1]);
            break;
        default:
            break;
        }
    }
    // vehicles
    if (myVehicleColoringInfoSource!=0&&tmpSettings.vehicleMode==prevVehicleMode) {
        switch (myVehicleColoringInfoSource->getColorSetType(tmpSettings.vehicleMode)) {
        case CST_SINGLE:
            if (sender==mySingleVehicleColor) {
                tmpSettings.vehicleColorings[tmpSettings.vehicleMode][0] = convert((FXColor)(FXuval) val);
            }
            myVehicleColoringInfoSource->getColorerInterface(tmpSettings.vehicleMode)->resetColor(
                tmpSettings.vehicleColorings[tmpSettings.vehicleMode][0]);
            break;
        case CST_MINMAX:
            if (sender==myMinVehicleColor) {
                tmpSettings.vehicleColorings[tmpSettings.vehicleMode][0] = convert((FXColor)(FXuval) val);
            }
            if (sender==myMaxVehicleColor) {
                tmpSettings.vehicleColorings[tmpSettings.vehicleMode][1] = convert((FXColor)(FXuval) val);
            }
            myVehicleColoringInfoSource->getColorerInterface(tmpSettings.vehicleMode)->resetColor(
                tmpSettings.vehicleColorings[tmpSettings.vehicleMode][0],
                tmpSettings.vehicleColorings[tmpSettings.vehicleMode][1]);
            break;
        default:
            break;
        }
    }

    if (tmpSettings==*mySettings) {
        return 1;
    }

    if (tmpSettings.name[0]!='*') {
        tmpSettings.name = '*' + tmpSettings.name;
    }
    gSchemeStorage.add(tmpSettings);
    int index = mySchemeName->getCurrentItem();
    if (index<3) { // !!!!
        index = mySchemeName->appendItem(tmpSettings.name.c_str());
        gSchemeStorage.add(tmpSettings);
        mySchemeName->setCurrentItem(index);
        myParent->getColoringSchemesCombo().appendItem(tmpSettings.name.c_str());
        myParent->getColoringSchemesCombo().setCurrentItem(index);
        myParent->setColorScheme((char*) tmpSettings.name.c_str());
    } else {
        mySchemeName->setItemText(index, tmpSettings.name.c_str());
        myParent->getColoringSchemesCombo().setItemText(index, tmpSettings.name.c_str());
        myParent->setColorScheme((char*) tmpSettings.name.c_str());
    }
    mySettings = &gSchemeStorage.get(tmpSettings.name);

    if (mySettings->laneEdgeMode!=prevLaneMode||mySettings->vehicleMode!=prevVehicleMode) {
        rebuildColorMatrices(true);
    }
    myParent->forceRefresh();
    getApp()->forceRefresh();
    return 1;
}


void
GUIDialog_ViewSettings::writeSettings() throw()
{
    const std::map<std::string, GUISUMOAbstractView::VisualizationSettings> &items = gSchemeStorage.getItems();
    const std::vector<std::string> &names = gSchemeStorage.getNames();
    getApp()->reg().writeIntEntry("VisualizationSettings", "settingNo", (FXint) names.size()-3);//!!!
    size_t gidx = 0;
    for (std::vector<std::string>::const_iterator i=names.begin()+3; i!=names.end(); ++i, ++gidx) {
        size_t k, index;
        std::map<int, std::vector<RGBColor> >::const_iterator j;

        const string &name = (*i);
        const GUISUMOAbstractView::VisualizationSettings &item = items.find(name)->second;

        string sname = "visset#" + toString(gidx);

        getApp()->reg().writeStringEntry("VisualizationSettings", sname.c_str(), item.name.c_str());//def.c_str());

        getApp()->reg().writeIntEntry(sname.c_str(), "antialiase", item.antialiase ? 1 : 0);
        getApp()->reg().writeIntEntry(sname.c_str(), "dither", item.dither ? 1 : 0);

        getApp()->reg().writeIntEntry(sname.c_str(), "backgroundColor", convert(item.backgroundColor));
        getApp()->reg().writeIntEntry(sname.c_str(), "showGrid", item.showGrid ? 1 : 0);
        getApp()->reg().writeRealEntry(sname.c_str(), "gridXSize", item.gridXSize);
        getApp()->reg().writeRealEntry(sname.c_str(), "gridYSize", item.gridYSize);

        getApp()->reg().writeIntEntry(sname.c_str(), "laneEdgeMode", item.laneEdgeMode);
        getApp()->reg().writeIntEntry(sname.c_str(), "laneShowBorders", item.laneShowBorders ? 1 : 0);
        getApp()->reg().writeIntEntry(sname.c_str(), "showLinkDecals", item.showLinkDecals ? 1 : 0);
        getApp()->reg().writeIntEntry(sname.c_str(), "laneEdgeExaggMode", item.laneEdgeExaggMode);
        getApp()->reg().writeRealEntry(sname.c_str(), "minExagg", item.minExagg);
        getApp()->reg().writeRealEntry(sname.c_str(), "maxExagg", item.maxExagg);
        getApp()->reg().writeIntEntry(sname.c_str(), "showRails", item.showRails ? 1 : 0);
        getApp()->reg().writeRealEntry(sname.c_str(), "edgeNameSize", item.edgeNameSize);
        getApp()->reg().writeIntEntry(sname.c_str(), "edgeNameColor", convert(item.edgeNameColor));
        getApp()->reg().writeIntEntry(sname.c_str(), "noLaneCols", (int) item.laneColorings.size());
        for (j=item.laneColorings.begin(), index=0; j!=item.laneColorings.end(); ++j, ++index) {
            getApp()->reg().writeIntEntry(sname.c_str(), ("nlcN" + toString(index)).c_str(), (int) index);
            getApp()->reg().writeIntEntry(sname.c_str(), ("nlcS" + toString(index)).c_str(), (int)(*j).second.size());
            for (k=0; k<(*j).second.size(); ++k) {
                getApp()->reg().writeIntEntry(sname.c_str(), ("nlcC" + toString(index) + "_" + toString(k)).c_str(), convert((*j).second[k]));
            }
        }

        getApp()->reg().writeIntEntry(sname.c_str(), "vehicleMode", item.vehicleMode);
        getApp()->reg().writeRealEntry(sname.c_str(), "minVehicleSize", item.minVehicleSize);
        getApp()->reg().writeRealEntry(sname.c_str(), "vehicleExaggeration", item.vehicleExaggeration);
        getApp()->reg().writeIntEntry(sname.c_str(), "showBlinker", item.showBlinker ? 1 : 0);
        getApp()->reg().writeIntEntry(sname.c_str(), "drawcC2CRadius", item.drawcC2CRadius ? 1 : 0);
        getApp()->reg().writeIntEntry(sname.c_str(), "drawLaneChangePreference", item.drawLaneChangePreference ? 1 : 0);
        getApp()->reg().writeIntEntry(sname.c_str(), "drawVehicleName", item.drawVehicleName ? 1 : 0);
        getApp()->reg().writeRealEntry(sname.c_str(), "vehicleNameSize", item.vehicleNameSize);
        getApp()->reg().writeIntEntry(sname.c_str(), "vehicleNameColor", convert(item.vehicleNameColor));
        getApp()->reg().writeIntEntry(sname.c_str(), "noVehCols", (int) item.vehicleColorings.size());
        for (j=item.vehicleColorings.begin(), index=0; j!=item.vehicleColorings.end(); ++j, ++index) {
            getApp()->reg().writeIntEntry(sname.c_str(), ("nvcN" + toString(index)).c_str(), (int) index);
            getApp()->reg().writeIntEntry(sname.c_str(), ("nvcS" + toString(index)).c_str(), (int)(*j).second.size());
            for (k=0; k<(*j).second.size(); ++k) {
                getApp()->reg().writeIntEntry(sname.c_str(), ("nvcC" + toString(index) + "_" + toString(k)).c_str(), convert((*j).second[k]));
            }
        }

        getApp()->reg().writeIntEntry(sname.c_str(), "junctionMode", item.junctionMode);
        getApp()->reg().writeIntEntry(sname.c_str(), "drawLinkTLIndex", item.drawLinkTLIndex ? 1 : 0);
        getApp()->reg().writeIntEntry(sname.c_str(), "drawLinkJunctionIndex", item.drawLinkJunctionIndex ? 1 : 0);
        getApp()->reg().writeIntEntry(sname.c_str(), "drawJunctionName", item.drawJunctionName ? 1 : 0);
        getApp()->reg().writeRealEntry(sname.c_str(), "junctionNameSize", item.junctionNameSize);
        getApp()->reg().writeIntEntry(sname.c_str(), "junctionNameColor", convert(item.junctionNameColor));

        getApp()->reg().writeIntEntry(sname.c_str(), "showLane2Lane", item.showLane2Lane ? 1 : 0);

        getApp()->reg().writeIntEntry(sname.c_str(), "addMode", item.addMode);
        getApp()->reg().writeRealEntry(sname.c_str(), "minAddSize", item.minAddSize);
        getApp()->reg().writeIntEntry(sname.c_str(), "addExaggeration", item.addExaggeration ? 1 : 0);
        getApp()->reg().writeIntEntry(sname.c_str(), "drawAddName", item.drawAddName ? 1 : 0);
        getApp()->reg().writeRealEntry(sname.c_str(), "addNameSize", item.addNameSize);

        getApp()->reg().writeRealEntry(sname.c_str(), "poiExaggeration", item.poiExaggeration);
        getApp()->reg().writeRealEntry(sname.c_str(), "minPOISize", item.minPOISize);
        getApp()->reg().writeIntEntry(sname.c_str(), "drawPOIName", item.drawPOIName ? 1 : 0);
        getApp()->reg().writeRealEntry(sname.c_str(), "poiNameSize", item.poiNameSize);
        getApp()->reg().writeIntEntry(sname.c_str(), "poiNameColor", convert(item.poiNameColor));

        getApp()->reg().writeIntEntry(sname.c_str(), "showSizeLegend", item.showSizeLegend ? 1 : 0);
    }
}


void
GUIDialog_ViewSettings::saveSettings(const std::string &file) throw()
{
    size_t index, k;
    std::map<int, std::vector<RGBColor> >::const_iterator j;
    try {
        OutputDevice &dev = OutputDevice::getDevice(file);
        dev << "name " << mySettings->name << '\n';

        dev << "antialiase " << mySettings->antialiase << '\n';
        dev << "dither " << mySettings->dither << '\n';

        dev << "backgroundColor " << mySettings->backgroundColor << '\n';
        dev << "showGrid " << mySettings->showGrid << '\n';
        dev << "gridXSize " << mySettings->gridXSize << '\n';
        dev << "gridYSize " << mySettings->gridYSize << '\n';

        dev << "laneEdgeMode " << mySettings->laneEdgeMode << '\n';
        dev << "laneShowBorders " << mySettings->laneShowBorders << '\n';
        dev << "showLinkDecals " << mySettings->showLinkDecals << '\n';
        dev << "laneEdgeExaggMode " << mySettings->laneEdgeExaggMode << '\n';
        dev << "minExagg " << mySettings->minExagg << '\n';
        dev << "maxExagg " << mySettings->maxExagg << '\n';
        dev << "showRails " << mySettings->showRails << '\n';
        dev << "edgeNameSize " << mySettings->edgeNameSize << '\n';
        dev << "edgeNameColor " << mySettings->edgeNameColor << '\n';
        for (j=mySettings->laneColorings.begin(), index=0; j!=mySettings->laneColorings.end(); ++j, ++index) {
            for (k=0; k<(*j).second.size(); ++k) {
                dev << "nlcC " << toString(index) << " " << (*j).second[k] << '\n';
            }
        }

        dev << "vehicleMode " << mySettings->vehicleMode << '\n';
        dev << "minVehicleSize " << mySettings->minVehicleSize << '\n';
        dev << "vehicleExaggeration " << mySettings->vehicleExaggeration << '\n';
        dev << "showBlinker " << mySettings->showBlinker << '\n';
        dev << "drawcC2CRadius " << mySettings->drawcC2CRadius << '\n';
        dev << "drawLaneChangePreference " << mySettings->drawLaneChangePreference << '\n';
        dev << "drawVehicleName " << mySettings->drawVehicleName << '\n';
        dev << "vehicleNameSize " << mySettings->vehicleNameSize << '\n';
        dev << "vehicleNameColor " << mySettings->vehicleNameColor << '\n';
        for (j=mySettings->vehicleColorings.begin(), index=0; j!=mySettings->vehicleColorings.end(); ++j, ++index) {
            for (k=0; k<(*j).second.size(); ++k) {
                dev << "nvcC " << toString(index) << " " << (*j).second[k] << '\n';
            }
        }

        dev << "junctionMode " << mySettings->junctionMode << '\n';
        dev << "drawLinkTLIndex " << mySettings->drawLinkTLIndex << '\n';
        dev << "drawLinkJunctionIndex " << mySettings->drawLinkJunctionIndex << '\n';
        dev << "drawJunctionName " << mySettings->drawJunctionName << '\n';
        dev << "junctionNameSize " << mySettings->junctionNameSize << '\n';
        dev << "junctionNameColor " << mySettings->junctionNameColor << '\n';

        dev << "showLane2Lane " << mySettings->showLane2Lane << '\n';

        dev << "addMode " << mySettings->addMode << '\n';
        dev << "minAddSize " << mySettings->minAddSize << '\n';
        dev << "addExaggeration " << mySettings->addExaggeration << '\n';
        dev << "drawAddName " << mySettings->drawAddName << '\n';
        dev << "addNameSize " << mySettings->addNameSize << '\n';

        dev << "poiExaggeration " << mySettings->poiExaggeration << '\n';
        dev << "minPOISize " << mySettings->minPOISize << '\n';
        dev << "drawPOIName " << mySettings->drawPOIName << '\n';
        dev << "poiNameSize " << mySettings->poiNameSize << '\n';
        dev << "poiNameColor " << mySettings->poiNameColor << '\n';

        dev << "showSizeLegend " << mySettings->showSizeLegend << '\n';

        dev.close();
    } catch (IOError &e) {
        FXMessageBox::error(this, MBOX_OK, "Storing failed!", e.what());
    }
}


void
GUIDialog_ViewSettings::loadSettings(const std::string &file) throw()
{
    GUISUMOAbstractView::VisualizationSettings setting = gSchemeStorage.getItems().begin()->second;
    LineReader lr(file);
    while (lr.hasMore()) {
        string line = lr.readLine();
        string name = line.substr(0, line.find(' '));
        string val = StringUtils::prune(line.substr(line.find(' ')));
        if (name=="name") setting.name = val;

        if (name=="antialiase") setting.antialiase = TplConvert<char>::_2bool(val.c_str());
        if (name=="dither") setting.dither = TplConvert<char>::_2bool(val.c_str());

        if (name=="backgroundColor") setting.backgroundColor = RGBColor::parseColor(val);
        if (name=="showGrid") setting.showGrid = TplConvert<char>::_2bool(val.c_str());
        if (name=="gridXSize") setting.gridXSize = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="gridYSize") setting.gridYSize = TplConvert<char>::_2SUMOReal(val.c_str());

        if (name=="laneEdgeMode") setting.laneEdgeMode = TplConvert<char>::_2int(val.c_str());
        if (name=="laneShowBorders") setting.laneShowBorders = TplConvert<char>::_2bool(val.c_str());
        if (name=="showLinkDecals") setting.showLinkDecals = TplConvert<char>::_2bool(val.c_str());
        if (name=="laneEdgeExaggMode") setting.laneEdgeExaggMode = TplConvert<char>::_2int(val.c_str());
        if (name=="minExagg") setting.minExagg = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="maxExagg") setting.maxExagg = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="showRails") setting.name = TplConvert<char>::_2bool(val.c_str());
        if (name=="edgeNameSize") setting.edgeNameSize = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="edgeNameColor") setting.edgeNameColor = RGBColor::parseColor(val);
        if (name=="nlcC") {
            string iS = val.substr(0, val.find(' '));
            string vS = StringUtils::prune(val.substr(val.find(' ')));
            size_t index = TplConvert<char>::_2int(iS.c_str());
            if (setting.laneColorings.find(index)==setting.laneColorings.end()) {
                setting.laneColorings[index] = vector<RGBColor>();
            }
            setting.laneColorings[index].push_back(RGBColor::parseColor(vS));
        }

        if (name=="vehicleMode") setting.vehicleMode = TplConvert<char>::_2int(val.c_str());
        if (name=="minVehicleSize") setting.minVehicleSize = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="vehicleExaggeration") setting.vehicleExaggeration = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="showBlinker") setting.showBlinker = TplConvert<char>::_2bool(val.c_str());
        if (name=="drawcC2CRadius") setting.drawcC2CRadius = TplConvert<char>::_2bool(val.c_str());
        if (name=="drawLaneChangePreference") setting.drawLaneChangePreference = TplConvert<char>::_2bool(val.c_str());
        if (name=="drawVehicleName") setting.drawVehicleName = TplConvert<char>::_2bool(val.c_str());
        if (name=="vehicleNameSize") setting.vehicleNameSize = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="vehicleNameColor") setting.vehicleNameColor = RGBColor::parseColor(val);
        if (name=="nvcC") {
            string iS = val.substr(0, val.find(' '));
            string vS = StringUtils::prune(val.substr(val.find(' ')));
            size_t index = TplConvert<char>::_2int(iS.c_str());
            if (setting.vehicleColorings.find(index)==setting.vehicleColorings.end()) {
                setting.vehicleColorings[index] = vector<RGBColor>();
            }
            setting.vehicleColorings[index].push_back(RGBColor::parseColor(vS));
        }

        if (name=="junctionMode") setting.junctionMode = TplConvert<char>::_2int(val.c_str());
        if (name=="drawLinkTLIndex") setting.drawLinkTLIndex = TplConvert<char>::_2bool(val.c_str());
        if (name=="drawLinkJunctionIndex") setting.drawLinkJunctionIndex = TplConvert<char>::_2bool(val.c_str());
        if (name=="drawJunctionName") setting.drawJunctionName = TplConvert<char>::_2bool(val.c_str());
        if (name=="junctionNameSize") setting.junctionNameSize = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="junctionNameColor") setting.junctionNameColor = RGBColor::parseColor(val);

        if (name=="showLane2Lane") setting.showLane2Lane = TplConvert<char>::_2bool(val.c_str());

        if (name=="addMode") setting.addMode = TplConvert<char>::_2int(val.c_str());
        if (name=="minAddSize") setting.minAddSize = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="addExaggeration") setting.addExaggeration = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="drawAddName") setting.drawAddName = TplConvert<char>::_2bool(val.c_str());
        if (name=="addNameSize") setting.addNameSize = TplConvert<char>::_2SUMOReal(val.c_str());

        if (name=="poiExaggeration") setting.poiExaggeration = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="minPOISize") setting.minPOISize = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="drawPOIName") setting.drawPOIName = TplConvert<char>::_2bool(val.c_str());
        if (name=="poiNameSize") setting.poiNameSize = TplConvert<char>::_2SUMOReal(val.c_str());
        if (name=="poiNameColor") setting.poiNameColor = RGBColor::parseColor(val);

        if (name=="showSizeLegend") setting.showSizeLegend = TplConvert<char>::_2bool(val.c_str());

    }
    FXint index = mySchemeName->appendItem(setting.name.c_str());
    gSchemeStorage.add(setting);
    mySchemeName->setCurrentItem(index);
    myParent->getColoringSchemesCombo().appendItem(setting.name.c_str());
    myParent->getColoringSchemesCombo().setCurrentItem(index);
    myParent->setColorScheme((char*) setting.name.c_str());
    mySettings = &gSchemeStorage.get(setting.name);
}


long
GUIDialog_ViewSettings::onCmdSaveSetting(FXObject*,FXSelector,void*data)
{
    int index = mySchemeName->getCurrentItem();
    if (index<3) {
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
                GUISUMOAbstractView::VisualizationSettings tmpSettings = *mySettings;
                gSchemeStorage.remove(mySettings->name);
                tmpSettings.name = name;
                gSchemeStorage.add(tmpSettings);
                mySchemeName->setItemText(index, tmpSettings.name.c_str());
                myParent->getColoringSchemesCombo().setItemText(index, tmpSettings.name.c_str());
                myParent->setColorScheme((char*) tmpSettings.name.c_str());
                mySettings = &gSchemeStorage.get(name);
                myBackup = *mySettings;
                writeSettings();
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
GUIDialog_ViewSettings::onUpdSaveSetting(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
                   mySchemeName->getCurrentItem()<3
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIDialog_ViewSettings::onCmdDeleteSetting(FXObject*,FXSelector,void*data)
{
    int index = mySchemeName->getCurrentItem();
    if (index<3) {
        return 1;
    }
    string name = mySchemeName->getItem(index).text();
    gSchemeStorage.remove(name);
    mySchemeName->removeItem(index);
    onCmdNameChange(0, 0, (void*) mySchemeName->getItem(0).text());
    writeSettings();
    return 1;
}


long
GUIDialog_ViewSettings::onUpdDeleteSetting(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
                   mySchemeName->getCurrentItem()<3
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIDialog_ViewSettings::onCmdExportSetting(FXObject*,FXSelector,void*data)
{
    FXFileDialog opendialog(this, "Export view settings");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.txt");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (!opendialog.execute()||!MFXUtils::userPermitsOverwritingWhenFileExists(this, opendialog.getFilename())) {
        return 1;
    }
    saveSettings(opendialog.getFilename().text());
    return 1;
}


long
GUIDialog_ViewSettings::onUpdExportSetting(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this,
                   mySchemeName->getCurrentItem()<3
                   ? FXSEL(SEL_COMMAND,ID_DISABLE) : FXSEL(SEL_COMMAND,ID_ENABLE),
                   ptr);
    return 1;
}


long
GUIDialog_ViewSettings::onCmdImportSetting(FXObject*,FXSelector,void*data)
{
    FXFileDialog opendialog(this, "Import view settings");
    opendialog.setIcon(GUIIconSubSys::getIcon(ICON_EMPTY));
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.txt");
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
GUIDialog_ViewSettings::onUpdImportSetting(FXObject*sender,FXSelector,void*ptr)
{
    sender->handle(this, FXSEL(SEL_COMMAND,ID_ENABLE), ptr);
    return 1;
}


RGBColor
GUIDialog_ViewSettings::convert(const FXColor c) throw()
{
    return RGBColor(
               (SUMOReal) FXREDVAL(c) / (SUMOReal) 255.,
               (SUMOReal) FXGREENVAL(c) / (SUMOReal) 255.,
               (SUMOReal) FXBLUEVAL(c) / (SUMOReal) 255.);
}


FXColor
GUIDialog_ViewSettings::convert(const RGBColor &c) throw()
{
    return FXRGB(c.red()*255., c.green()*255., c.blue()*255.);
}


void
GUIDialog_ViewSettings::rebuildList() throw()
{
    myDecalsTable->clearItems();

    // set table attributes
    myDecalsTable->setTableSize(10, 8); //!!!
    myDecalsTable->setColumnText(0, "picture file");
    myDecalsTable->setColumnText(1, "left");
    myDecalsTable->setColumnText(2, "top");
    myDecalsTable->setColumnText(3, "right");
    myDecalsTable->setColumnText(4, "bottom");
    myDecalsTable->setColumnText(5, "width");
    myDecalsTable->setColumnText(6, "height");
    myDecalsTable->setColumnText(7, "rotation");
    FXHeader *header = myDecalsTable->getColumnHeader();
    header->setHeight(getApp()->getNormalFont()->getFontHeight()+getApp()->getNormalFont()->getFontAscent());
    int k;
    for (k=0; k<8; k++) {
        header->setItemJustify(k, JUSTIFY_CENTER_X|JUSTIFY_TOP);
    }
    header->setItemSize(0, 150); // !! check if the size will be changed
    header->setItemSize(1, 60); // !! check if the size will be changed
    header->setItemSize(2, 60); // !! check if the size will be changed
    header->setItemSize(3, 60); // !! check if the size will be changed
    header->setItemSize(4, 60); // !! check if the size will be changed
    header->setItemSize(5, 60); // !! check if the size will be changed
    header->setItemSize(6, 60); // !! check if the size will be changed
    header->setItemSize(7, 60); // !! check if the size will be changed

    // insert into table
    FXint row = 0;
    std::vector<GUISUMOAbstractView::Decal>::iterator j;
    for (j=myDecals->begin(); j!=myDecals->end(); ++j) {
        GUISUMOAbstractView::Decal &d = *j;
        myDecalsTable->setItemText(row, 0, d.filename.c_str());
        myDecalsTable->setItemText(row, 1, toString<SUMOReal>(d.left).c_str());
        myDecalsTable->setItemText(row, 2, toString<SUMOReal>(d.top).c_str());
        myDecalsTable->setItemText(row, 3, toString<SUMOReal>(d.right).c_str());
        myDecalsTable->setItemText(row, 4, toString<SUMOReal>(d.bottom).c_str());
        myDecalsTable->setItemText(row, 5, toString<SUMOReal>(d.right - d.left).c_str());
        myDecalsTable->setItemText(row, 6, toString<SUMOReal>(d.bottom-d.top).c_str());
        myDecalsTable->setItemText(row, 7, toString<SUMOReal>(d.rot).c_str());
        row++;
    }
    // insert dummy last field
    for (k=0; k<8; k++) {
        myDecalsTable->setItemText(row, k, " ");
    }
    //
}


void
GUIDialog_ViewSettings::rebuildColorMatrices(bool doCreate) throw()
{
    {
        // decals
        delete myDecalsTable;
        myDecalsTable = new MFXAddEditTypedTable(myDecalsFrame, this, MID_TABLE,
                LAYOUT_FILL_Y|LAYOUT_FIX_WIDTH/*|LAYOUT_FIX_HEIGHT*/, 0,0, 470, 0);
        myDecalsTable->setVisibleRows(5);
        myDecalsTable->setVisibleColumns(8);
        myDecalsTable->setTableSize(5,8);
        myDecalsTable->setBackColor(FXRGB(255,255,255));
        myDecalsTable->getRowHeader()->setWidth(0);
        myDecalsTable->setCellType(1, CT_REAL);
        myDecalsTable->setNumberCellParams(1, -10000000, 10000000,
                                           10, 100, 100000, "%.2f");
        myDecalsTable->setCellType(2, CT_REAL);
        myDecalsTable->setNumberCellParams(2, -10000000, 10000000,
                                           10, 100, 100000, "%.2f");
        myDecalsTable->setCellType(3, CT_REAL);
        myDecalsTable->setNumberCellParams(3, -10000000, 10000000,
                                           10, 100, 100000, "%.2f");
        myDecalsTable->setCellType(4, CT_REAL);
        myDecalsTable->setNumberCellParams(4, -10000000, 10000000,
                                           10, 100, 100000, "%.2f");
        myDecalsTable->setCellType(5, CT_REAL);
        myDecalsTable->setNumberCellParams(5, -10000000, 10000000,
                                           .1, 1, 10, "%.2f");
        myDecalsTable->setCellType(6, CT_REAL);
        myDecalsTable->setNumberCellParams(6, -10000000, 10000000,
                                           .1, 1, 10, "%.2f");
        myDecalsTable->setCellType(7, CT_REAL);
        myDecalsTable->setNumberCellParams(7, -10000000, 10000000,
                                           .1, 1, 10, "%.2f");
        rebuildList();
        if (doCreate) {
            myDecalsTable->create();
        }
    }
    {
        // lane
        MFXUtils::deleteChildren(myLaneColorSettingFrame);
        FXMatrix *m = new FXMatrix(myLaneColorSettingFrame,2,
                                   LAYOUT_FILL_X|MATRIX_BY_COLUMNS|LAYOUT_FIX_HEIGHT,
                                   0,0,0,80,10,10,0,0, 5,3);
        mySingleLaneColor = 0;
        myMinLaneColor = 0;
        myMaxLaneColor = 0;
        switch (myLaneColoringInfoSource->getColorSetType(mySettings->laneEdgeMode)) {
        case CST_SINGLE:
            new FXLabel(m , "Color", 0, LAYOUT_CENTER_Y);
            mySingleLaneColor = new FXColorWell(m , convert(mySettings->laneColorings[mySettings->laneEdgeMode][0]),
                                                this, MID_SIMPLE_VIEW_COLORCHANGE,
                                                LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                                0, 0, 100, 0,   0, 0, 0, 0);
            break;
        case CST_MINMAX:
            new FXLabel(m , "min Color", 0, LAYOUT_CENTER_Y);
            myMinLaneColor = new FXColorWell(m , convert(mySettings->laneColorings[mySettings->laneEdgeMode][0]),
                                             this, MID_SIMPLE_VIEW_COLORCHANGE,
                                             LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                             0, 0, 100, 0,   0, 0, 0, 0);
            new FXLabel(m , "max Color", 0, LAYOUT_CENTER_Y);
            myMaxLaneColor = new FXColorWell(m , convert(mySettings->laneColorings[mySettings->laneEdgeMode][1]),
                                             this, MID_SIMPLE_VIEW_COLORCHANGE,
                                             LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                             0, 0, 100, 0,   0, 0, 0, 0);
            break;
        case CST_GRADIENT:
            break;
        case CST_SET:
        default:
            break;
        }
        if (doCreate) {
            m->create();
        }
    }
    {
        if (myVehicleColoringInfoSource!=0) {
            // vehicles
            MFXUtils::deleteChildren(myVehicleColorSettingFrame);
            FXMatrix *m = new FXMatrix(myVehicleColorSettingFrame,2,
                                       LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS|LAYOUT_FIX_HEIGHT,
                                       0,0,0,80,10,10,0,0, 5,3);
            mySingleVehicleColor = 0;
            myMinVehicleColor = 0;
            myMaxVehicleColor = 0;
            switch (myVehicleColoringInfoSource->getColorSetType(mySettings->vehicleMode)) {
            case CST_SINGLE:
                new FXLabel(m , "Color", 0, LAYOUT_CENTER_Y);
                mySingleVehicleColor = new FXColorWell(m , convert(mySettings->vehicleColorings[mySettings->vehicleMode][0]),
                                                       this, MID_SIMPLE_VIEW_COLORCHANGE,
                                                       LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                                       0, 0, 100, 0,   0, 0, 0, 0);
                break;
            case CST_MINMAX:
                new FXLabel(m , "min Color", 0, LAYOUT_CENTER_Y);
                myMinVehicleColor = new FXColorWell(m , convert(mySettings->vehicleColorings[mySettings->vehicleMode][0]),
                                                    this, MID_SIMPLE_VIEW_COLORCHANGE,
                                                    LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                                    0, 0, 100, 0,   0, 0, 0, 0);
                new FXLabel(m , "max Color", 0, LAYOUT_CENTER_Y);
                myMaxVehicleColor = new FXColorWell(m , convert(mySettings->vehicleColorings[mySettings->vehicleMode][1]),
                                                    this, MID_SIMPLE_VIEW_COLORCHANGE,
                                                    LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                                                    0, 0, 100, 0,   0, 0, 0, 0);
                break;
            case CST_GRADIENT:
                break;
            case CST_SET:
            default:
                break;
            }
            if (doCreate) {
                m->create();
            }
        }
    }
    layout();
    update();
}


long
GUIDialog_ViewSettings::onCmdEditTable(FXObject*,FXSelector,void*data)
{
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
        d.left = 0;
        d.top = 0;
        d.right = myParent->getGridWidth();
        d.bottom = myParent->getGridHeight();
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
        d.filename = value;
        break;
    case 1:
        try {
            d.left = TplConvert<char>::_2SUMOReal(value.c_str());
            SUMOReal width = d.right - d.left;
            myDecalsTable->setItemText(i->row, 5, toString(width).c_str());
        } catch (NumberFormatException &) {
            string msg = "The value must be a float, is:" + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 2:
        try {
            d.top = TplConvert<char>::_2SUMOReal(value.c_str());
            SUMOReal height = d.bottom - d.top;
            myDecalsTable->setItemText(i->row, 6, toString(height).c_str());
        } catch (NumberFormatException &) {
            string msg = "The value must be a float, is:" + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 3:
        try {
            d.right = TplConvert<char>::_2SUMOReal(value.c_str());
            SUMOReal width = d.right - d.left;
            myDecalsTable->setItemText(i->row, 5, toString(width).c_str());
        } catch (NumberFormatException &) {
            string msg = "The value must be a float, is:" + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 4:
        try {
            d.bottom = TplConvert<char>::_2SUMOReal(value.c_str());
            SUMOReal height = d.bottom - d.top;
            myDecalsTable->setItemText(i->row, 6, toString(height).c_str());
        } catch (NumberFormatException &) {
            string msg = "The value must be a float, is:" + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 5:
        try {
            SUMOReal width = TplConvert<char>::_2SUMOReal(value.c_str());
            d.right = width + d.left;
            myDecalsTable->setItemText(i->row, 3, toString(d.right).c_str());
        } catch (NumberFormatException &) {
            string msg = "The value must be a float, is:" + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 6:
        try {
            SUMOReal height = TplConvert<char>::_2SUMOReal(value.c_str());
            d.bottom = height + d.top;
            myDecalsTable->setItemText(i->row, 4, toString(d.bottom).c_str());
        } catch (NumberFormatException &) {
            string msg = "The value must be a float, is:" + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 7:
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
GUIDialog_ViewSettings::getCurrentScheme() const throw()
{
    return mySchemeName->getItem(mySchemeName->getCurrentItem()).text();
}


void
GUIDialog_ViewSettings::setCurrentScheme(const std::string &name) throw()
{
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

