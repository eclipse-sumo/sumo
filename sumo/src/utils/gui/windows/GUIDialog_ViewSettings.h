/****************************************************************************/
/// @file    GUIDialog_ViewSettings.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 21. Dec 2005
/// @version $Id$
///
// The view-settings dialog
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
#ifndef GUIDialog_ViewSettings_h
#define GUIDialog_ViewSettings_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/foxtools/MFXAddEditTypedTable.h>
#include <utils/foxtools/MFXMutex.h>


// ===========================================================================
// class declarations
// ===========================================================================
class BaseSchemeInfoSource;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDialog_ViewSettings
 * @brief The dialog to change the view (gui) settings.
 */
class GUIDialog_ViewSettings : public FXDialogBox
{
    // is a FOX-object with an own mapping
    FXDECLARE(GUIDialog_ViewSettings)
public:
    /// constructor
    GUIDialog_ViewSettings(GUISUMOAbstractView *parent,
                           GUISUMOAbstractView::VisualizationSettings *settings,
                           BaseSchemeInfoSource *laneEdgeModeSource,
                           BaseSchemeInfoSource *vehicleModeSource,
                           std::vector<GUISUMOAbstractView::Decal> *decals,
                           MFXMutex *decalsLock);

    /// destructor
    ~GUIDialog_ViewSettings();

    /// Called on OK-button pressure
    long onCmdOk(FXObject*,FXSelector,void*);

    /// Called on Cancel-button pressure
    long onCmdCancel(FXObject*,FXSelector,void*);

    long onCmdColorChange(FXObject*,FXSelector,void*);
    long onCmdEditTable(FXObject*,FXSelector,void*data);

    long onChgNameChange(FXObject*,FXSelector,void*);
    long onCmdNameChange(FXObject*,FXSelector,void*);

    long onCmdSaveSetting(FXObject*,FXSelector,void*data);
    long onUpdSaveSetting(FXObject*,FXSelector,void*data);
    long onCmdDeleteSetting(FXObject*,FXSelector,void*data);
    long onUpdDeleteSetting(FXObject*,FXSelector,void*data);
    long onCmdExportSetting(FXObject*,FXSelector,void*data);
    long onUpdExportSetting(FXObject*,FXSelector,void*data);
    long onCmdImportSetting(FXObject*,FXSelector,void*data);
    long onUpdImportSetting(FXObject*,FXSelector,void*data);

    std::string getCurrentScheme() const;
    void setCurrentScheme(const std::string &);

protected:
    RGBColor convert(const FXColor c);
    FXColor convert(const RGBColor &c);

    void rebuildColorMatrices(bool doCreate=false);
    void rebuildList();

    void writeSettings();
    void saveSettings(const std::string &file);
    void loadSettings(const std::string &file);

private:
    GUISUMOAbstractView *myParent;
    GUISUMOAbstractView::VisualizationSettings *mySettings;
    BaseSchemeInfoSource *myLaneColoringInfoSource;
    BaseSchemeInfoSource *myVehicleColoringInfoSource;
    std::vector<GUISUMOAbstractView::Decal> *myDecals;
    MFXMutex *myDecalsLock;
    FXComboBox *mySchemeName;
    FXCheckButton *myShowGrid;
    FXRealSpinDial *myGridXSizeDialer, *myGridYSizeDialer;

    FXColorWell *myBackgroundColor;
    FXVerticalFrame *myDecalsFrame;
    MFXAddEditTypedTable *myDecalsTable;

    FXComboBox *myLaneEdgeColorMode;
    FXVerticalFrame *myLaneColorSettingFrame;
    FXColorWell *mySingleLaneColor, *myMinLaneColor, *myMaxLaneColor;
    FXCheckButton *myShowLaneBorders, *myShowLaneDecals, *myShowRails, *myShowEdgeName;
    FXRealSpinDial *myEdgeNameSizeDialer;
    FXColorWell *myEdgeNameColor;

    FXComboBox *myVehicleColorMode;
    FXVerticalFrame *myVehicleColorSettingFrame;
    FXColorWell *mySingleVehicleColor, *myMinVehicleColor, *myMaxVehicleColor;
    FXRealSpinDial *myVehicleMinSizeDialer, *myVehicleUpscaleDialer;
    FXCheckButton *myShowBlinker, *myShowC2CRadius, *myShowLaneChangePreference, *myShowVehicleName;
    FXRealSpinDial *myVehicleNameSizeDialer;
    FXColorWell *myVehicleNameColor;

    FXCheckButton *myShowTLIndex, *myShowJunctionIndex;
    FXCheckButton *myShowJunctionName;
    FXRealSpinDial *myJunctionNameSizeDialer;
    FXColorWell *myJunctionNameColor;

    FXRealSpinDial *myDetectorMinSizeDialer, *myDetectorUpscaleDialer, *myAddNameSizeDialer;
    FXCheckButton *myShowAddName;
    //FXColorWell *myDetectorNameColor;

    FXRealSpinDial *myPOIMinSizeDialer, *myPOIUpscaleDialer, *myPOINameSizeDialer;
    FXCheckButton *myShowPOIName;
    FXColorWell *myPOINameColor;

    FXCheckButton *myShowLane2Lane;
    FXCheckButton *myAntialiase;
    FXCheckButton *myDither;
    FXCheckButton *myShowSizeLegend;

    GUISUMOAbstractView::VisualizationSettings myBackup;

protected:
    /// default constructor (needed by FOX)
    GUIDialog_ViewSettings() { }

};


#endif

/****************************************************************************/

