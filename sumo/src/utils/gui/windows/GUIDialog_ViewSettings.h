#ifndef GUIDialog_ViewSettings_h
#define GUIDialog_ViewSettings_h
//---------------------------------------------------------------------------//
//                        GUIDialog_ViewSettings.h -
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
// $Log$
// Revision 1.8  2006/12/21 13:23:56  dkrajzew
// added visualization of tls/junction link indices
//
// Revision 1.7  2006/12/18 08:24:58  dkrajzew
// made several visualization things optional
//
// Revision 1.6  2006/11/28 12:10:45  dkrajzew
// got rid of FXEX-Mutex (now using the one supplied in FOX)
//
// Revision 1.5  2006/09/18 10:18:23  dkrajzew
// debugging
//
// Revision 1.4  2006/07/06 05:55:59  dkrajzew
// added the "show blinker" option
//
// Revision 1.3  2006/01/31 11:02:37  dkrajzew
// added the possibility to exaggerate pois
//
// Revision 1.2  2006/01/19 13:36:38  dkrajzew
// debugging the resize bug
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

#include <fx.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/MFXAddEditTypedTable.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class BaseSchemeInfoSource;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIDialog_ViewSettings
 * The dialog to change the application (gui) settings.
 */
class GUIDialog_ViewSettings : public FXDialogBox
{
    // is a FOX-object with an own mapping
    FXDECLARE(GUIDialog_ViewSettings)
public:
    /// constructor
    GUIDialog_ViewSettings( FXMainWindow* mainWindow,
        GUISUMOAbstractView *parent,
        GUISUMOAbstractView::VisualizationSettings *settings,
        BaseSchemeInfoSource *laneEdgeModeSource,
        BaseSchemeInfoSource *vehicleModeSource,
        std::vector<GUISUMOAbstractView::Decal> *decals,
        FXMutex *decalsLock);

    /// destructor
    ~GUIDialog_ViewSettings();

    /// Called on OK-button pressure
    long onCmdOk(FXObject*,FXSelector,void*);

    /// Called on Cancel-button pressure
    long onCmdCancel(FXObject*,FXSelector,void*);

    /// Called on Cancel-button pressure
    long onCmdSave(FXObject*,FXSelector,void*);

    long onCmdColorChange(FXObject*,FXSelector,void*);
    long onCmdEditTable(FXObject*,FXSelector,void*data);

    long onChgNameChange(FXObject*,FXSelector,void*);
    long onCmdNameChange(FXObject*,FXSelector,void*);

protected:
    RGBColor convert(const FXColor c);
    FXColor convert(const RGBColor &c);

    void rebuildColorMatrices(bool doCreate=false);
    void rebuildList();

private:
    FXMainWindow* myMainWindow;
    GUISUMOAbstractView *myParent;
    GUISUMOAbstractView::VisualizationSettings *mySettings;
    BaseSchemeInfoSource *myLaneColoringInfoSource;
    BaseSchemeInfoSource *myVehicleColoringInfoSource;
    std::vector<GUISUMOAbstractView::Decal> *myDecals;
    FXMutex *myDecalsLock;
    FXComboBox *mySchemeName;

    FXColorWell *myBackgroundColor;
    FXVerticalFrame *myDecalsFrame;
    MFXAddEditTypedTable *myDecalsTable;

    FXComboBox *myLaneEdgeColorMode;
    FXVerticalFrame *myLaneColorSettingFrame;
    FXColorWell *mySingleLaneColor, *myMinLaneColor, *myMaxLaneColor;
    FXCheckButton *myShowLaneBorders;
    FXCheckButton *myShowLaneDecals;
    FXCheckButton *myShowRails;

    FXComboBox *myVehicleColorMode;
    FXVerticalFrame *myVehicleColorSettingFrame;
    FXColorWell *mySingleVehicleColor, *myMinVehicleColor, *myMaxVehicleColor;
    FXRealSpinDial *myVehicleMinSizeDialer, *myVehicleUpscaleDialer;
    FXCheckButton *myShowBlinker, *myShowC2CRadius, *myShowLaneChangePreference, *myShowVehicleName;

    FXCheckButton *myShowTLIndex, *myShowJunctionIndex;

    FXRealSpinDial *myDetectorMinSizeDialer, *myDetectorUpscaleDialer;

    FXRealSpinDial *myPOIMinSizeDialer, *myPOIUpscaleDialer;

    FXCheckButton *myShowLane2Lane;
    FXCheckButton *myAntialiase;
    FXCheckButton *myDither;


    GUISUMOAbstractView::VisualizationSettings myBackup;

protected:
    /// default constructor (needed by FOX)
    GUIDialog_ViewSettings() { }

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

