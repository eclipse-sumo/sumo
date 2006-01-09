//---------------------------------------------------------------------------//
//                        GUIDialog_ViewSettings.cpp -
//  The application-settings dialog
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 21. Dec 2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "GUIDialog_ViewSettings.h"
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/images/GUIImageGlobals.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/foxtools/MFXUtils.h>

#include <utils/gui/drawer/GUIColoringSchemesMap.h>
#include <utils/foxtools/MFXAddEditTypedTable.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/gui/drawer/GUICompleteSchemeStorage.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIDialog_ViewSettings) GUIDialog_ViewSettingsMap[]=
{
    FXMAPFUNC(SEL_CHANGED,  MID_SIMPLE_VIEW_COLORCHANGE,    GUIDialog_ViewSettings::onCmdColorChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_COLORCHANGE,    GUIDialog_ViewSettings::onCmdColorChange),
    FXMAPFUNC(SEL_CHANGED,  MID_SIMPLE_VIEW_NAMECHANGE,     GUIDialog_ViewSettings::onChgNameChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SIMPLE_VIEW_NAMECHANGE,     GUIDialog_ViewSettings::onCmdNameChange),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_OK,                GUIDialog_ViewSettings::onCmdOk),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_CANCEL,            GUIDialog_ViewSettings::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_SAVE,              GUIDialog_ViewSettings::onCmdSave),
    FXMAPFUNC(SEL_CHANGED,  MFXAddEditTypedTable::ID_TEXT_CHANGED,  GUIDialog_ViewSettings::onCmdEditTable),
};


FXIMPLEMENT(GUIDialog_ViewSettings, FXDialogBox, GUIDialog_ViewSettingsMap, ARRAYNUMBER(GUIDialog_ViewSettingsMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDialog_ViewSettings::GUIDialog_ViewSettings(FXMainWindow* mainWindow,
            GUISUMOAbstractView *parent,
            GUISUMOAbstractView::VisualizationSettings *settings,
            BaseSchemeInfoSource *laneEdgeModeSource,
            BaseSchemeInfoSource *vehicleModeSource,
            std::vector<GUISUMOAbstractView::Decal> *decals,
            FXEX::FXMutex *decalsLock)
    : FXDialogBox( parent, "Application Settings" ),
    myMainWindow(mainWindow), myParent(parent), mySettings(settings),
    myLaneColoringInfoSource(laneEdgeModeSource),
    myVehicleColoringInfoSource(vehicleModeSource),
    myDecals(decals), myDecalsLock(decalsLock)
{
    myBackup = (*mySettings);

    FXVerticalFrame *contentFrame =
        new FXVerticalFrame(this, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
        0,0,0,0, 0,0,0,0, 5,5);
    //
    mySchemeName = new FXComboBox(contentFrame, 20, this, MID_SIMPLE_VIEW_NAMECHANGE, COMBOBOX_INSERT_LAST|FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP);
    const std::vector<std::string> &names = gSchemeStorage.getNames();
    for(std::vector<std::string>::const_iterator i=names.begin(); i!=names.end(); ++i) {
        mySchemeName->appendItem((*i).c_str());
    }
    //
    FXTabBook *tabbook =
        new FXTabBook(contentFrame,0,0,PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_RIGHT);
    {
        // tab for the background
        FXTabItem *tab1 = new FXTabItem(tabbook,"Background",NULL);
        FXVerticalFrame *frame1 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);
        FXMatrix *m1 =
            new FXMatrix(frame1,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                0,0,0,0, 10,10,10,10, 5,5);
        new FXLabel(m1, "Color", 0, LAYOUT_CENTER_Y);
        myBackgroundColor = new FXColorWell(m1, convert(settings->backgroundColor),
            this, MID_SIMPLE_VIEW_COLORCHANGE,
            LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
            0, 0, 100, 0,   0, 0, 0, 0);
        new FXHorizontalSeparator(frame1,SEPARATOR_GROOVE|LAYOUT_FILL_X);
        new FXLabel(frame1, "Decals:");
        myDecalsFrame = new FXVerticalFrame(frame1);
    }
    {
        FXTabItem *tab2 = new FXTabItem(tabbook,"Streets",NULL);
        FXVerticalFrame *frame2 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);
        FXSplitter *s2 = new FXSplitter(frame2);
        FXMatrix *m21 =
            new FXMatrix(s2,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                0,0,0,0, 10,10,10,10, 5,5);
        new FXLabel(m21, "Color by", 0, LAYOUT_CENTER_Y);
        myLaneEdgeColorMode = new FXComboBox(m21, 30, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP);
        laneEdgeModeSource->fill(*myLaneEdgeColorMode);
        /*
        c1->appendItem("single color");
        c1->appendItem("average speed (lane)");
        c1->appendItem("average speed (edge)");
        c1->appendItem("density (lane)");
        c1->appendItem("density (edge)");
        c1->appendItem("allowed speed (lane)");
        c1->appendItem("type (edge)");
        c1->appendItem("allowed - average speed (lane)");
        c1->appendItem("allowed - average / allowed speed (lane)");
        c1->appendItem("by allowed vehicle classes (lane)");
        c1->appendItem("LOS (lane)");
        c1->appendItem("LOS (edge)");
        */
        myLaneEdgeColorMode->setNumVisible(10);
        myLaneColorSettingFrame = new FXVerticalFrame(s2);
        new FXHorizontalSeparator(frame2,SEPARATOR_GROOVE|LAYOUT_FILL_X);
        FXMatrix *m22 =
            new FXMatrix(frame2,1,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                0,0,0,0, 10,10,10,10, 5,5);
        myShowLaneBorders = new FXCheckButton(m22, "Show lane borders", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowLaneBorders->setCheck(mySettings->laneShowBorders);
        myShowLaneDecals = new FXCheckButton(m22, "Show link decals", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowLaneDecals->setCheck(mySettings->showLinkDecals);
    }
    {
        if(myVehicleColoringInfoSource!=0) {
            FXTabItem *tab3 = new FXTabItem(tabbook,"Vehicles",NULL);
            FXVerticalFrame *frame3 =
                new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);
            FXMatrix *m31 =
                new FXMatrix(frame3,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                    0,0,0,0, 10,10,10,10, 5,5);
            new FXLabel(m31, "Show As", 0, LAYOUT_CENTER_Y);
            FXComboBox *c31 = new FXComboBox(m31, 20, 0, 0, FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP);
            c31->appendItem("triangles");
            c31->appendItem("boxes");
            c31->appendItem("simple shapes");
            c31->appendItem("complex shapes");
            c31->setNumVisible(4);
            c31->disable();
            new FXHorizontalSeparator(frame3,SEPARATOR_GROOVE|LAYOUT_FILL_X);
            FXSplitter *s3 = new FXSplitter(frame3);
            FXMatrix *m32 =
                new FXMatrix(s3,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                    0,0,0,0, 10,10,10,10, 5,5);
            new FXLabel(m32, "Color By", 0, LAYOUT_CENTER_Y);
            myVehicleColorMode = new FXComboBox(m32, 20, this, MID_SIMPLE_VIEW_COLORCHANGE, FRAME_SUNKEN|LAYOUT_LEFT|LAYOUT_TOP);
            myVehicleColoringInfoSource->fill(*myVehicleColorMode);
            myVehicleColorMode->setNumVisible(10);
            myVehicleColorSettingFrame =
                new FXVerticalFrame(s3, 0, 0,0,0,0, 10,10,10,10, 5,2);
            new FXHorizontalSeparator(frame3,SEPARATOR_GROOVE|LAYOUT_FILL_X);
            FXMatrix *m33 =
                new FXMatrix(frame3,1,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                    0,0,0,0, 10,10,10,10, 5,5);
            FXCheckButton *tmpc = new FXCheckButton(m33, "Show blinker", 0 ,0);
            tmpc->disable();
            tmpc = new FXCheckButton(m33, "Show breaking lights", 0 ,0);
            tmpc->disable();
            tmpc = new FXCheckButton(m33, "Show needed headway", 0 ,0);
            tmpc->disable();
            new FXHorizontalSeparator(frame3,SEPARATOR_GROOVE|LAYOUT_FILL_X);
            FXMatrix *m34 =
                new FXMatrix(frame3,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                    0,0,0,0, 10,10,10,10, 5,5);
            new FXLabel(m34, "Minimum size to show", 0, LAYOUT_CENTER_Y);
            myVehicleMinSizeDialer =
                new FXRealSpinDial(m34, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                    LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
            myVehicleMinSizeDialer->setValue(mySettings->minVehicleSize);
            new FXLabel(m34, "Exaggerate by", 0, LAYOUT_CENTER_Y);
            myVehicleUpscaleDialer =
                new FXRealSpinDial(m34, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                    LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
            myVehicleUpscaleDialer->setRange(1, 100);
            myVehicleUpscaleDialer->setValue(mySettings->vehicleExaggeration);
        }
    }
    {
        FXTabItem *tab4 = new FXTabItem(tabbook,"Nodes",NULL);
        FXVerticalFrame *frame4 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);
    }
    {
        FXTabItem *tab5 = new FXTabItem(tabbook,"Detectors/Trigger",NULL);
        FXVerticalFrame *frame5 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);
        FXMatrix *m51 =
            new FXMatrix(frame5,2,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                0,0,0,0, 10,10,10,10, 5,5);
        new FXLabel(m51, "Minimum size to show", 0, LAYOUT_CENTER_Y);
        myDetectorMinSizeDialer =
            new FXRealSpinDial(m51, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myDetectorMinSizeDialer->setValue(mySettings->minAddSize);
        new FXLabel(m51, "Exaggerate by", 0, LAYOUT_CENTER_Y);
        myDetectorUpscaleDialer =
            new FXRealSpinDial(m51, 10, this, MID_SIMPLE_VIEW_COLORCHANGE,
                LAYOUT_TOP|FRAME_SUNKEN|FRAME_THICK);
        myDetectorUpscaleDialer->setRange(1, 100);
        myDetectorUpscaleDialer->setValue(mySettings->addExaggeration);
    }
    {
        FXTabItem *tab6 = new FXTabItem(tabbook,"Further",NULL);
        FXVerticalFrame *frame6 =
            new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED, 0,0,0,0, 0,0,0,0, 2,2);
        FXMatrix *m61 =
            new FXMatrix(frame6,1,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                0,0,0,0, 10,10,10,10, 5,5);
        myShowLane2Lane = new FXCheckButton(m61, "Show lane to lane connections", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myShowLane2Lane->setCheck(mySettings->showLane2Lane);
        new FXHorizontalSeparator(frame6,SEPARATOR_GROOVE|LAYOUT_FILL_X);
        FXMatrix *m62 =
            new FXMatrix(frame6,1,LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                0,0,0,0, 10,10,10,10, 5,5);
        myAntialiase = new FXCheckButton(m62, "Antialiase", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myAntialiase->setCheck(mySettings->antialiase);
        myDither = new FXCheckButton(m62, "Dither", this, MID_SIMPLE_VIEW_COLORCHANGE);
        myDither->setCheck(mySettings->dither);
    }
    FXHorizontalFrame *f2 = new FXHorizontalFrame(contentFrame, LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0, 10,10,5,5);
    FXButton *initial=new FXButton(f2,"&Use",NULL,this,MID_SETTINGS_OK,BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    new FXButton(f2,"&Discard",NULL,this,MID_SETTINGS_CANCEL,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    new FXButton(f2,"&Save",NULL,this,MID_SETTINGS_SAVE,BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK|LAYOUT_TOP|LAYOUT_LEFT|LAYOUT_CENTER_X,0,0,0,0, 30,30,4,4);
    initial->setFocus();

    rebuildColorMatrices(false);
}


GUIDialog_ViewSettings::~GUIDialog_ViewSettings()
{
    myParent->remove(this);
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
GUIDialog_ViewSettings::onCmdSave(FXObject*,FXSelector,void*)
{
    string name = mySchemeName->getItemText(mySchemeName->getCurrentItem()).text();
    if(gSchemeStorage.contains(name)) {
        if(MBOX_CLICKED_NO==FXMessageBox::question(this,MBOX_YES_NO,"Overwrite Settings?","Overwrite existing settings '%s'?", mySchemeName->getItemText(mySchemeName->getCurrentItem()))) {
            return 1;
        }
    }
    gSchemeStorage.add(name, (*mySettings));
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
    FXString dataS = (char*) data; // !!!unicode
    // check whether this item has bben added twice
    if(dataS==mySchemeName->getItemText(mySchemeName->getNumItems()-1)) {
        for(int i=0; i<mySchemeName->getNumItems()-1; ++i) {
            if(dataS==mySchemeName->getItemText(i)) {
                mySchemeName->removeItem(i);
            }
        }
    }
    myBackup = gSchemeStorage.get(dataS.text());
    (*mySettings) = gSchemeStorage.get(dataS.text());

    myBackgroundColor->setRGBA(convert(mySettings->backgroundColor));

    myLaneEdgeColorMode->setCurrentItem(mySettings->laneEdgeMode);
    myShowLaneBorders->setCheck(mySettings->laneShowBorders);
    myShowLaneDecals->setCheck(mySettings->showLinkDecals);

    myVehicleColorMode->setCurrentItem(mySettings->vehicleMode);
    myVehicleUpscaleDialer->setValue(mySettings->vehicleExaggeration);
    myVehicleMinSizeDialer->setValue(mySettings->minVehicleSize);

    myDetectorUpscaleDialer->setValue(mySettings->addExaggeration);
    myDetectorMinSizeDialer->setValue(mySettings->minAddSize);

    myShowLane2Lane->setCheck(mySettings->showLane2Lane);
    myAntialiase->setCheck(mySettings->antialiase);
    myDither->setCheck(mySettings->dither);

    // lanes
    switch(myLaneColoringInfoSource->getColorSetType(myBackup.laneEdgeMode)) {
    case CST_SINGLE:
        mySingleLaneColor->setRGBA(convert(mySettings->singleLaneColor));
        myLaneColoringInfoSource->getColorerInterface(myBackup.laneEdgeMode)->resetColor(myBackup.singleLaneColor);
        break;
    case CST_MINMAX:
        myMinLaneColor->setRGBA(convert(mySettings->minLaneColor));
        myMaxLaneColor->setRGBA(convert(mySettings->maxLaneColor));
        myLaneColoringInfoSource->getColorerInterface(myBackup.laneEdgeMode)->resetColor(myBackup.minLaneColor, myBackup.maxLaneColor);
        break;
    default:
        break;
    }
    // vehicles
    if(myVehicleColoringInfoSource!=0) {
        switch(myVehicleColoringInfoSource->getColorSetType(myBackup.vehicleMode)) {
        case CST_SINGLE:
            mySingleVehicleColor->setRGBA(convert(mySettings->singleVehicleColor));
            myVehicleColoringInfoSource->getColorerInterface(myBackup.vehicleMode)->resetColor(myBackup.singleVehicleColor);
            break;
        case CST_MINMAX:
            myMinVehicleColor->setRGBA(convert(mySettings->minVehicleColor));
            myMaxVehicleColor->setRGBA(convert(mySettings->maxVehicleColor));
            myVehicleColoringInfoSource->getColorerInterface(myBackup.vehicleMode)->resetColor(myBackup.minVehicleColor, myBackup.maxVehicleColor);
            break;
        default:
            break;
        }
    }
    update();
    myParent->update();
    return 1;
}



long
GUIDialog_ViewSettings::onCmdColorChange(FXObject*,FXSelector,void*)
{
    int prevLaneMode = mySettings->laneEdgeMode;
    int prevVehicleMode = mySettings->vehicleMode;

    mySettings->backgroundColor = convert(myBackgroundColor->getRGBA());

    mySettings->laneEdgeMode = myLaneEdgeColorMode->getCurrentItem();
    mySettings->laneShowBorders = myShowLaneBorders->getCheck()!=0;
    mySettings->showLinkDecals = myShowLaneDecals->getCheck()!=0;

    mySettings->vehicleMode = myVehicleColorMode->getCurrentItem();
    mySettings->vehicleExaggeration = (SUMOReal) myVehicleUpscaleDialer->getValue();
    mySettings->minVehicleSize = (SUMOReal) myVehicleMinSizeDialer->getValue();

    mySettings->addExaggeration = (SUMOReal) myDetectorUpscaleDialer->getValue();
    mySettings->minAddSize = (SUMOReal) myDetectorMinSizeDialer->getValue();

    mySettings->showLane2Lane = myShowLane2Lane->getCheck()!=0;
    mySettings->antialiase = myAntialiase->getCheck()!=0;
    mySettings->dither = myDither->getCheck()!=0;


    if(mySettings->laneEdgeMode!=prevLaneMode||mySettings->vehicleMode!=prevVehicleMode) {
        rebuildColorMatrices(true);
    }
    // lanes
    switch(myLaneColoringInfoSource->getColorSetType(mySettings->laneEdgeMode)) {
    case CST_SINGLE:
        mySettings->singleLaneColor = convert(mySingleLaneColor->getRGBA());
        myLaneColoringInfoSource->getColorerInterface(mySettings->laneEdgeMode)->resetColor(mySettings->singleLaneColor);
        break;
    case CST_MINMAX:
        mySettings->minLaneColor = convert(myMinLaneColor->getRGBA());
        mySettings->maxLaneColor = convert(myMaxLaneColor->getRGBA());
        myLaneColoringInfoSource->getColorerInterface(mySettings->laneEdgeMode)->resetColor(mySettings->minLaneColor, mySettings->maxLaneColor);
        break;
    default:
        break;
    }
    // vehicles
    if(myVehicleColoringInfoSource!=0) {
        switch(myVehicleColoringInfoSource->getColorSetType(mySettings->vehicleMode)) {
        case CST_SINGLE:
            mySettings->singleVehicleColor = convert(mySingleVehicleColor->getRGBA());
            myVehicleColoringInfoSource->getColorerInterface(mySettings->vehicleMode)->resetColor(mySettings->singleVehicleColor);
            break;
        case CST_MINMAX:
            mySettings->minVehicleColor = convert(myMinVehicleColor->getRGBA());
            mySettings->maxVehicleColor = convert(myMaxVehicleColor->getRGBA());
            myVehicleColoringInfoSource->getColorerInterface(mySettings->vehicleMode)->resetColor(mySettings->minVehicleColor, mySettings->maxVehicleColor);
            break;
        default:
            break;
        }
    }
    myParent->update();
    return 1;
}


RGBColor
GUIDialog_ViewSettings::convert(const FXColor c)
{
    return RGBColor(
        (SUMOReal) FXREDVAL(c) / (SUMOReal) 255.,
        (SUMOReal) FXGREENVAL(c) / (SUMOReal) 255.,
        (SUMOReal) FXBLUEVAL(c) / (SUMOReal) 255.);
}


FXColor
GUIDialog_ViewSettings::convert(const RGBColor &c)
{
    return FXRGB(c.red()*255., c.green()*255., c.blue()*255.);
}


void
GUIDialog_ViewSettings::rebuildList()
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
    for(k=0; k<8; k++) {
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
    size_t row = 0;
    std::vector<GUISUMOAbstractView::Decal>::iterator j;
    for(j=myDecals->begin(); j!=myDecals->end(); ++j) {
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
    for(k=0; k<8; k++) {
        myDecalsTable->setItemText(row, k, " ");
    }
    //
}


void
GUIDialog_ViewSettings::rebuildColorMatrices(bool doCreate)
{
    {
        // decals
        myDecalsTable = new MFXAddEditTypedTable(myDecalsFrame, this, MID_TABLE,
            LAYOUT_FIX_WIDTH|LAYOUT_FILL_Y, 0,0, 470);
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
    }
    {
        // lane
        MFXUtils::deleteChildren(myLaneColorSettingFrame);
        FXMatrix *m = new FXMatrix(myLaneColorSettingFrame,2,
            LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
            0,0,0,0,10,10,10,10, 5,3);
        mySingleLaneColor = 0;
        myMinLaneColor = 0;
        myMaxLaneColor = 0;
        switch(myLaneColoringInfoSource->getColorSetType(mySettings->laneEdgeMode)) {
        case CST_SINGLE:
            mySettings->singleLaneColor = myLaneColoringInfoSource->getColorerInterface(mySettings->laneEdgeMode)->getSingleColor();
            new FXLabel(m , "Color", 0, LAYOUT_CENTER_Y);
            mySingleLaneColor = new FXColorWell(m , convert(mySettings->singleLaneColor),
                this, MID_SIMPLE_VIEW_COLORCHANGE,
                LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                0, 0, 100, 0,   0, 0, 0, 0);
            break;
        case CST_MINMAX:
            mySettings->minLaneColor = myLaneColoringInfoSource->getColorerInterface(mySettings->laneEdgeMode)->getMinColor();
            mySettings->maxLaneColor = myLaneColoringInfoSource->getColorerInterface(mySettings->laneEdgeMode)->getMaxColor();
            new FXLabel(m , "min Color", 0, LAYOUT_CENTER_Y);
            myMinLaneColor = new FXColorWell(m , convert(mySettings->minLaneColor),
                this, MID_SIMPLE_VIEW_COLORCHANGE,
                LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                0, 0, 100, 0,   0, 0, 0, 0);
            new FXLabel(m , "max Color", 0, LAYOUT_CENTER_Y);
            myMaxLaneColor = new FXColorWell(m , convert(mySettings->maxLaneColor),
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
        if(doCreate) {
            m->create();
        }
    }
    {
        if(myVehicleColoringInfoSource!=0) {
            // vehicles
            MFXUtils::deleteChildren(myVehicleColorSettingFrame);
            FXMatrix *m = new FXMatrix(myVehicleColorSettingFrame,2,
                LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT|MATRIX_BY_COLUMNS,
                0,0,0,0,10,10,10,10, 5,3);
            mySingleVehicleColor = 0;
            myMinVehicleColor = 0;
            myMaxVehicleColor = 0;
            switch(myVehicleColoringInfoSource->getColorSetType(mySettings->vehicleMode)) {
            case CST_SINGLE:
                mySettings->singleVehicleColor = myVehicleColoringInfoSource->getColorerInterface(mySettings->vehicleMode)->getSingleColor();
                new FXLabel(m , "Color", 0, LAYOUT_CENTER_Y);
                mySingleVehicleColor = new FXColorWell(m , convert(mySettings->singleVehicleColor),
                    this, MID_SIMPLE_VIEW_COLORCHANGE,
                    LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                    0, 0, 100, 0,   0, 0, 0, 0);
                break;
            case CST_MINMAX:
                mySettings->minVehicleColor = myVehicleColoringInfoSource->getColorerInterface(mySettings->vehicleMode)->getMinColor();
                mySettings->maxVehicleColor = myVehicleColoringInfoSource->getColorerInterface(mySettings->vehicleMode)->getMaxColor();
                new FXLabel(m , "min Color", 0, LAYOUT_CENTER_Y);
                myMinVehicleColor = new FXColorWell(m , convert(mySettings->minVehicleColor),
                    this, MID_SIMPLE_VIEW_COLORCHANGE,
                    LAYOUT_FIX_WIDTH|LAYOUT_CENTER_Y|LAYOUT_SIDE_TOP|FRAME_SUNKEN|FRAME_THICK|ICON_AFTER_TEXT,
                    0, 0, 100, 0,   0, 0, 0, 0);
                new FXLabel(m , "max Color", 0, LAYOUT_CENTER_Y);
                myMaxVehicleColor = new FXColorWell(m , convert(mySettings->maxVehicleColor),
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
            if(doCreate) {
                m->create();
            }
        }
    }
}


long
GUIDialog_ViewSettings::onCmdEditTable(FXObject*,FXSelector,void*data)
{
    MFXEditedTableItem *i = (MFXEditedTableItem*) data;
    string value = i->item->getText().text();
    // check whether the inserted value is empty
    if(value.find_first_not_of(" ")==string::npos) {
        return 1;
    }
    GUISUMOAbstractView::Decal d;
    int row = i->row;
    if(row==myDecals->size()) {
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

    switch(i->col) {
    case 0:
        d.filename = value;
        break;
    case 1:
        try {
            d.left = TplConvert<char>::_2SUMOReal(value.c_str());
            SUMOReal width = d.right - d.left;
            myDecalsTable->setItemText(i->row, 5, toString(width).c_str());
        } catch (NumberFormatException) {
            string msg = string("The value must be a float, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 2:
        try {
            d.top = TplConvert<char>::_2SUMOReal(value.c_str());
            SUMOReal height = d.bottom - d.top;
            myDecalsTable->setItemText(i->row, 6, toString(height).c_str());
        } catch (NumberFormatException) {
            string msg = string("The value must be a float, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 3:
        try {
            d.right = TplConvert<char>::_2SUMOReal(value.c_str());
            SUMOReal width = d.right - d.left;
            myDecalsTable->setItemText(i->row, 5, toString(width).c_str());
        } catch (NumberFormatException) {
            string msg = string("The value must be a float, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 4:
        try {
            d.bottom = TplConvert<char>::_2SUMOReal(value.c_str());
            SUMOReal height = d.bottom - d.top;
            myDecalsTable->setItemText(i->row, 6, toString(height).c_str());
        } catch (NumberFormatException) {
            string msg = string("The value must be a float, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 5:
        try {
            SUMOReal width = TplConvert<char>::_2SUMOReal(value.c_str());
            d.right = width + d.left;
            myDecalsTable->setItemText(i->row, 3, toString(d.right).c_str());
        } catch (NumberFormatException) {
            string msg = string("The value must be a float, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 6:
        try {
            SUMOReal height = TplConvert<char>::_2SUMOReal(value.c_str());
            d.bottom = height + d.top;
            myDecalsTable->setItemText(i->row, 4, toString(d.bottom).c_str());
        } catch (NumberFormatException) {
            string msg = string("The value must be a float, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    case 7:
        try {
            d.rot = TplConvert<char>::_2SUMOReal(value.c_str());
        } catch (NumberFormatException) {
            string msg = string("The value must be a float, is:") + value;
            FXMessageBox::error(this, MBOX_OK, "Number format error", msg.c_str());
        }
        break;
    default:
        throw 1;
    }
    (*myDecals)[row] = d;
    if(!i->updateOnly) {
        rebuildList();
    }
    myParent->update();
    return 1;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


