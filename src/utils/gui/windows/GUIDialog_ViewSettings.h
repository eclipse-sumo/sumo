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
/// @file    GUIDialog_ViewSettings.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Wed, 21. Dec 2005
///
// The dialog to change the view (gui) settings.
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/div/GUIPersistentWindowPos.h>
#include <utils/foxtools/MFXDecalsTable.h>


// ===========================================================================
// class declarations
// ===========================================================================

class MFXComboBoxIcon;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDialog_ViewSettings
 * @brief The dialog to change the view (gui) settings.
 *
 * @todo Check whether saving/loading settings should be done via XML
 */
class GUIDialog_ViewSettings : public FXDialogBox, public GUIPersistentWindowPos {
    /// @brief FOX Declaration
    FXDECLARE(GUIDialog_ViewSettings)

public:
    /// @brief NamePanel
    class NamePanel {

    public:
        /// @brief constructor
        NamePanel(FXMatrix* parent, GUIDialog_ViewSettings* target,
                  const std::string& title,
                  const GUIVisualizationTextSettings& settings);

        /// @brief get settings
        GUIVisualizationTextSettings getSettings();

        /// @brief update
        void update(const GUIVisualizationTextSettings& settings);

        /// @brief check button
        FXCheckButton* myCheck = nullptr;

        /// @brief size dial
        FXRealSpinner* mySizeDial = nullptr;

        /// @brief color well
        FXColorWell* myColorWell = nullptr;

        /// @brief BGColor well
        FXColorWell* myBGColorWell = nullptr;

        /// @brief draw only for selected?
        FXCheckButton* mySelectedCheck = nullptr;

        /// @brief const size check
        FXCheckButton* myConstSizeCheck = nullptr;

        /// @brief Matrix
        FXMatrix* myMatrix0 = nullptr;
    };

    /// @brief SizePanel
    class SizePanel : public FXObject {
        /// @brief FOX Declaration
        FXDECLARE(SizePanel)

    public:
        /// @brief constructor
        SizePanel(FXMatrix* parent, GUIDialog_ViewSettings* target,
                  const GUIVisualizationSizeSettings& settings, GUIGlObjectType type);

        /// @brief get settings
        GUIVisualizationSizeSettings getSettings();

        /// @brief update
        void update(const GUIVisualizationSizeSettings& settings);

        /// @name FOX-callbacks
        /// @{
        /// @brief Called if something (color, width, etc.) has been changed
        long onCmdSizeChange(FXObject* obj, FXSelector sel, void* ptr);

        /// @}

        /// @brief min size dial
        FXRealSpinner* myMinSizeDial = nullptr;

        /// @brief exaggerate dial
        FXRealSpinner* myExaggerateDial = nullptr;

        /// @brief check button
        FXCheckButton* myCheck = nullptr;

        /// @brief check selected button
        FXCheckButton* myCheckSelected = nullptr;

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(SizePanel)

    private:
        /// @brief pointer to dialog viewSettings
        GUIDialog_ViewSettings* myDialogViewSettings = nullptr;

        /// @brief GLObject type associated with this size
        GUIGlObjectType myType = GLO_NETWORK;
    };

    /** @brief Constructor
     * @param[in] parent The view to report changed settings to
     * @param[in, out] settings The current settings that can be changed
     */
    GUIDialog_ViewSettings(GUISUMOAbstractView* parent, GUIVisualizationSettings* settings);

    /// @brief FOX need this
    GUIDialog_ViewSettings() : myBackup("DUMMY") {}

    /// @brief Destructor
    ~GUIDialog_ViewSettings();

    /// @brief show view settings dialog
    void show();

    /// @brief get GUISUMOAbstractView parent
    GUISUMOAbstractView* getSUMOAbstractView();

    /** @brief Sets current settings (called if reopened)
     * @param[in, out] settings The current settings that can be changed
     */
    void setCurrent(GUIVisualizationSettings* settings);

    /// @name FOX-callbacks
    /// @{

    /// @brief Called if the OK-button was pressed
    long onCmdOk(FXObject*, FXSelector, void*);

    /// @brief Called if the Cancel-button was pressed
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief Called if something (color, width, etc.) has been changed
    long onCmdColorChange(FXObject*, FXSelector, void*);

    /// @brief Called if the name of the scheme was changed
    long onCmdNameChange(FXObject*, FXSelector, void*);

    /// @brief Called if the settings shall be saved into the registry
    long onCmdSaveSetting(FXObject*, FXSelector, void* data);

    /// @brief Called when updating the button that allows to save the settings into the registry
    long onUpdSaveSetting(FXObject*, FXSelector, void* data);

    /// @brief Called if the settings shall be deleted
    long onCmdDeleteSetting(FXObject*, FXSelector, void* data);

    /// @brief Called when updating the button that allows to delete settings
    long onUpdDeleteSetting(FXObject*, FXSelector, void* data);

    /// @brief Called if the settings shall be exported into a file
    long onCmdExportSetting(FXObject*, FXSelector, void* data);

    /// @brief Called when updating the button that allows to export settings into a file
    long onUpdExportSetting(FXObject*, FXSelector, void* data);

    /// @brief Called if the settings shall be read from a file
    long onCmdImportSetting(FXObject*, FXSelector, void* data);

    /// @brief Called when updating the button that allows to read settings from a file
    long onUpdImportSetting(FXObject*, FXSelector, void* data);

    /// @brief Called if the decal shall be loaded from a file
    long onCmdLoadDecal(FXObject*, FXSelector, void* data);

    /// @brief Called if the decals shall be loaded from a file
    long onCmdLoadXMLDecals(FXObject*, FXSelector, void* data);

    /// @brief Called if the decals shall be saved to a file
    long onCmdSaveXMLDecals(FXObject*, FXSelector, void* data);

    /// @brief Called if the decals shall be cleared
    long onCmdClearDecals(FXObject*, FXSelector, void* data);
    /// @}

    /** @brief Returns the name of the currently chosen scheme
     * @return The name of the edited (chosen) scheme)
     */
    std::string getCurrentScheme() const;

    /** @brief Sets the named scheme as the current
     * @param[in] The name of the scheme that shall be set as current
     */
    void setCurrentScheme(const std::string&);

protected:
    /// @brief The parent view (which settings are changed)
    GUISUMOAbstractView* myParent = nullptr;

    /// @brief The current settings
    GUIVisualizationSettings* mySettings = nullptr;

    /// @brief A backup of the settings (used if the "Cancel" button is pressed)
    GUIVisualizationSettings myBackup;

    /// @name Dialog elements
    /// @{

    FXComboBox* mySchemeName = nullptr;
    FXCheckButton* myShowGrid = nullptr;
    FXRealSpinner* myGridXSizeDialer = nullptr;
    FXRealSpinner* myGridYSizeDialer = nullptr;

    FXColorWell* myBackgroundColor = nullptr;
    MFXDecalsTable* myDecalsTable = nullptr;

    /// @brief selection colors
    FXColorWell* mySelectionColor = nullptr;
    FXColorWell* mySelectedEdgeColor = nullptr;
    FXColorWell* mySelectedLaneColor = nullptr;
    FXColorWell* mySelectedConnectionColor = nullptr;
    FXColorWell* mySelectedProhibitionColor = nullptr;
    FXColorWell* mySelectedCrossingColor = nullptr;
    FXColorWell* mySelectedAdditionalColor = nullptr;
    FXColorWell* mySelectedRouteColor = nullptr;
    FXColorWell* mySelectedVehicleColor = nullptr;
    FXColorWell* mySelectedPersonColor = nullptr;
    FXColorWell* mySelectedPersonPlanColor = nullptr;
    FXColorWell* mySelectedEdgeDataColor = nullptr;

    /// @brief additional colors
    FXColorWell* myBusStopColor = nullptr;
    FXColorWell* myBusStopColorSign = nullptr;
    FXColorWell* myTrainStopColor = nullptr;
    FXColorWell* myTrainStopColorSign = nullptr;
    FXColorWell* myContainerStopColor = nullptr;
    FXColorWell* myContainerStopColorSign = nullptr;
    FXColorWell* myChargingStationColor = nullptr;
    FXColorWell* myChargingStationColorSign = nullptr;
    FXColorWell* myStopColor = nullptr;
    FXColorWell* myWaypointColor = nullptr;
    FXColorWell* myVehicleTripsColor = nullptr;
    FXColorWell* myStopPersonsColor = nullptr;
    FXColorWell* myPersonTripColor = nullptr;
    FXColorWell* myWalkColor = nullptr;
    FXColorWell* myRideColor = nullptr;
    FXColorWell* myStopContainersColor = nullptr;
    FXColorWell* myTransportColor = nullptr;
    FXColorWell* myTranshipColor = nullptr;

    /// @brief demand widths

    FXRealSpinner* myTripWidth = nullptr;
    FXRealSpinner* myPersonTripWidth = nullptr;
    FXRealSpinner* myWalkWidth = nullptr;
    FXRealSpinner* myRideWidth = nullptr;
    FXRealSpinner* myTransportWidth = nullptr;
    FXRealSpinner* myTranshipWidth = nullptr;

    /// @brief ... lane colorer
    MFXComboBoxIcon* myLaneEdgeColorMode = nullptr;
    FXVerticalFrame* myLaneColorSettingFrame = nullptr;
    std::vector<FXColorWell*> myLaneColors;
    std::vector<FXRealSpinner*> myLaneThresholds;
    std::vector<FXButton*> myLaneButtons;
    FXCheckButton* myLaneColorInterpolation = nullptr;
    FXButton* myLaneColorRainbow = nullptr;
    FXCheckButton* myLaneColorRainbowCheck = nullptr;
    FXRealSpinner* myLaneColorRainbowThreshold = nullptr;
    FXCheckButton* myLaneColorRainbowCheck2 = nullptr;
    FXRealSpinner* myLaneColorRainbowThreshold2 = nullptr;
    FXButton* myJunctionColorRainbow = nullptr;
    FXComboBox* myParamKey = nullptr;
    FXComboBox* myScalingParamKey = nullptr;
    FXComboBox* myMeanDataID = nullptr;

    /// @brief ... lane scaler
    MFXComboBoxIcon* myLaneEdgeScaleMode = nullptr;
    FXVerticalFrame* myLaneScaleSettingFrame = nullptr;
    std::vector<FXRealSpinner*> myLaneScales;
    std::vector<FXRealSpinner*> myLaneScaleThresholds;
    std::vector<FXButton*> myLaneScaleButtons;
    FXCheckButton* myLaneScaleInterpolation = nullptr;

    FXCheckButton* myShowLaneBorders = nullptr;
    FXCheckButton* myShowBikeMarkings = nullptr;
    FXCheckButton* myShowLaneDecals = nullptr;
    FXCheckButton* myRealisticLinkRules = nullptr;
    FXCheckButton* myShowLinkRules = nullptr;
    FXCheckButton* myShowRails = nullptr;
    FXCheckButton* mySecondaryShape = nullptr;
    FXCheckButton* myHideMacroConnectors = nullptr;
    FXCheckButton* myShowLaneDirection = nullptr;
    FXCheckButton* myShowSublanes = nullptr;
    FXCheckButton* mySpreadSuperposed = nullptr;
    FXRealSpinner* myLaneWidthUpscaleDialer = nullptr;
    FXRealSpinner* myLaneMinWidthDialer = nullptr;

    /// @brief Vehicles
    MFXComboBoxIcon* myVehicleColorMode, *myVehicleShapeDetail = nullptr;
    FXVerticalFrame* myVehicleColorSettingFrame = nullptr;
    std::vector<FXColorWell*> myVehicleColors;
    std::vector<FXRealSpinner*> myVehicleThresholds;
    std::vector<FXButton*> myVehicleButtons;
    FXCheckButton* myVehicleColorInterpolation = nullptr;

    /// @brief vehicle scaler
    MFXComboBoxIcon* myVehicleScaleMode = nullptr;
    FXVerticalFrame* myVehicleScaleSettingFrame = nullptr;
    std::vector<FXRealSpinner*> myVehicleScales;
    std::vector<FXRealSpinner*> myVehicleScaleThresholds;
    std::vector<FXButton*> myVehicleScaleButtons;
    FXCheckButton* myVehicleScaleInterpolation = nullptr;

    FXCheckButton* myShowBlinker = nullptr;
    FXCheckButton* myShowMinGap = nullptr;
    FXCheckButton* myShowBrakeGap = nullptr;
    FXCheckButton* myShowBTRange = nullptr;
    FXCheckButton* myShowRouteIndex = nullptr;
    FXCheckButton* myScaleLength = nullptr;
    FXCheckButton* myDrawReversed = nullptr;
    FXCheckButton* myShowParkingInfo = nullptr;
    /*FXCheckButton* myShowLaneChangePreference = nullptr;*/

    FXComboBox* myVehicleParamKey = nullptr;
    FXComboBox* myVehicleTextParamKey = nullptr;

    /// @brief Persons
    MFXComboBoxIcon* myPersonColorMode, *myPersonShapeDetail = nullptr;
    FXVerticalFrame* myPersonColorSettingFrame = nullptr;
    std::vector<FXColorWell*> myPersonColors;
    std::vector<FXRealSpinner*> myPersonThresholds;
    std::vector<FXButton*> myPersonButtons;
    FXCheckButton* myPersonColorInterpolation = nullptr;

    /// @brief Containers
    MFXComboBoxIcon* myContainerColorMode, *myContainerShapeDetail = nullptr;
    FXVerticalFrame* myContainerColorSettingFrame = nullptr;
    std::vector<FXColorWell*> myContainerColors;
    std::vector<FXRealSpinner*> myContainerThresholds;
    std::vector<FXButton*> myContainerButtons;
    FXCheckButton* myContainerColorInterpolation = nullptr;
    FXRealSpinner* myContainerMinSizeDialer, *myContainerUpscaleDialer = nullptr;

    /// @brief junctions
    MFXComboBoxIcon* myJunctionColorMode = nullptr;
    FXVerticalFrame* myJunctionColorSettingFrame = nullptr;
    std::vector<FXColorWell*> myJunctionColors;
    std::vector<FXRealSpinner*> myJunctionThresholds;
    std::vector<FXButton*> myJunctionButtons;
    FXCheckButton* myJunctionColorInterpolation = nullptr;

    /// @brief POIs
    MFXComboBoxIcon* myPOIColorMode, *myPOIShapeDetail = nullptr;
    FXVerticalFrame* myPOIColorSettingFrame = nullptr;
    std::vector<FXColorWell*> myPOIColors;
    std::vector<FXRealSpinner*> myPOIThresholds;
    std::vector<FXButton*> myPOIButtons;
    FXCheckButton* myPOIColorInterpolation = nullptr;
    FXComboBox* myPOITextParamKey = nullptr;
    FXSpinner* myPoiDetail = nullptr;

    /// @brief Polygons
    MFXComboBoxIcon* myPolyColorMode, *myPolyShapeDetail = nullptr;
    FXVerticalFrame* myPolyColorSettingFrame = nullptr;
    std::vector<FXColorWell*> myPolyColors;
    std::vector<FXRealSpinner*> myPolyThresholds;
    std::vector<FXButton*> myPolyButtons;
    FXCheckButton* myPolyColorInterpolation = nullptr;

    /// @brief Data
    MFXComboBoxIcon* myDataColorMode = nullptr;
    FXVerticalFrame* myDataColorSettingFrame = nullptr;
    std::vector<FXColorWell*> myDataColors;
    std::vector<FXRealSpinner*> myDataThresholds;
    std::vector<FXButton*> myDataButtons;
    FXCheckButton* myDataColorInterpolation = nullptr;
    FXComboBox* myDataParamKey = nullptr;
    FXRealSpinner* myEdgeRelationUpscaleDialer = nullptr;
    FXRealSpinner* myTazRelationUpscaleDialer = nullptr;
    FXButton* myDataColorRainbow = nullptr;
    FXCheckButton* myDataColorRainbowCheck = nullptr;
    FXRealSpinner* myDataColorRainbowThreshold = nullptr;

    /// @brief buttons
    FXCheckButton* myShowLane2Lane = nullptr;
    FXCheckButton* myDrawJunctionShape = nullptr;
    FXCheckButton* myDrawCrossingsAndWalkingAreas = nullptr;
    FXCheckButton* myShowSizeLegend = nullptr;
    FXCheckButton* myShowColorLegend = nullptr;
    FXCheckButton* myShowVehicleColorLegend = nullptr;

    /// @brief 3D
    FXCheckButton* myShow3DTLSLinkMarkers = nullptr;
    FXCheckButton* myShow3DTLSDomes = nullptr;
    FXCheckButton* myShow3DHeadUpDisplay = nullptr;
    FXCheckButton* myGenerate3DTLSModels = nullptr;
    FXSpinner* myLight3DFactor = nullptr;
    //FXColorWell* myAmbient3DLight = nullptr;
    //FXColorWell* myDiffuse3DLight = nullptr;
    FXColorWell* mySkyColor = nullptr;

    /// @brief openGL
    FXCheckButton* myDither = nullptr;
    FXCheckButton* myFPS = nullptr;
    FXCheckButton* myDrawBoundaries = nullptr;
    FXCheckButton* myForceDrawForPositionSelection = nullptr;
    FXCheckButton* myForceDrawForRectangleSelection = nullptr;
    FXCheckButton* myDisableDottedContours = nullptr;
    FXButton* myRecalculateBoundaries = nullptr;

    /// @brief name panels
    NamePanel* myEdgeNamePanel = nullptr;
    NamePanel* myInternalEdgeNamePanel = nullptr;
    NamePanel* myCwaEdgeNamePanel = nullptr;
    NamePanel* myStreetNamePanel = nullptr;
    NamePanel* myEdgeValuePanel = nullptr;
    NamePanel* myEdgeScaleValuePanel = nullptr;
    NamePanel* myJunctionIndexPanel = nullptr;
    NamePanel* myTLIndexPanel = nullptr;
    NamePanel* myJunctionIDPanel = nullptr;
    NamePanel* myJunctionNamePanel = nullptr;
    NamePanel* myInternalJunctionNamePanel = nullptr;
    NamePanel* myTLSPhaseIndexPanel = nullptr;
    NamePanel* myTLSPhaseNamePanel = nullptr;
    NamePanel* myVehicleNamePanel = nullptr;
    NamePanel* myVehicleValuePanel = nullptr;
    NamePanel* myVehicleScaleValuePanel = nullptr;
    NamePanel* myVehicleTextPanel = nullptr;
    NamePanel* myPersonNamePanel = nullptr;
    NamePanel* myPersonValuePanel = nullptr;
    NamePanel* myContainerNamePanel = nullptr;
    NamePanel* myAddNamePanel = nullptr;
    NamePanel* myAddFullNamePanel = nullptr;
    NamePanel* myPOINamePanel = nullptr;
    NamePanel* myPOITypePanel = nullptr;
    NamePanel* myPOITextPanel = nullptr;
    NamePanel* myPolyNamePanel = nullptr;
    NamePanel* myPolyTypePanel = nullptr;
    NamePanel* myDataValuePanel = nullptr;
    NamePanel* myGeometryIndicesPanel = nullptr;

    /// @brief size panels
    SizePanel* myVehicleSizePanel = nullptr;
    SizePanel* myPersonSizePanel = nullptr;
    SizePanel* myContainerSizePanel = nullptr;
    SizePanel* myPOISizePanel = nullptr;
    SizePanel* myPolySizePanel = nullptr;
    SizePanel* myAddSizePanel = nullptr;
    SizePanel* myJunctionSizePanel = nullptr;

    /// @brief load/save-menu
    FXCheckButton* mySaveViewPort = nullptr;
    FXCheckButton* mySaveDelay = nullptr;
    FXCheckButton* mySaveDecals = nullptr;
    FXCheckButton* mySaveBreakpoints = nullptr;

    /// @}

    /// @brief Frame3D
    FXTabItem* myFrame3D = nullptr;

    /// @brief update color ranges
    bool updateColorRanges(FXObject* sender, std::vector<FXColorWell*>::const_iterator colIt,
                           std::vector<FXColorWell*>::const_iterator colEnd,
                           std::vector<FXRealSpinner*>::const_iterator threshIt,
                           std::vector<FXRealSpinner*>::const_iterator threshEnd,
                           std::vector<FXButton*>::const_iterator buttonIt,
                           GUIColorScheme& scheme);

    /// @brief update scale ranges
    bool updateScaleRanges(FXObject* sender, std::vector<FXRealSpinner*>::const_iterator colIt,
                           std::vector<FXRealSpinner*>::const_iterator colEnd,
                           std::vector<FXRealSpinner*>::const_iterator threshIt,
                           std::vector<FXRealSpinner*>::const_iterator threshEnd,
                           std::vector<FXButton*>::const_iterator buttonIt,
                           GUIScaleScheme& scheme);

    /// @brief Rebuilds manipulators for the current coloring scheme
    FXMatrix* rebuildColorMatrix(FXVerticalFrame* frame,
                                 std::vector<FXColorWell*>& colors,
                                 std::vector<FXRealSpinner*>& thresholds,
                                 std::vector<FXButton*>& buttons,
                                 FXCheckButton* interpolation,
                                 GUIColorScheme& scheme);

    /// @brief Rebuilds manipulators for the current scaling scheme
    FXMatrix* rebuildScaleMatrix(FXVerticalFrame* frame,
                                 std::vector<FXRealSpinner*>& scales,
                                 std::vector<FXRealSpinner*>& thresholds,
                                 std::vector<FXButton*>& buttons,
                                 FXCheckButton* interpolation,
                                 GUIScaleScheme& scheme);

    /** @brief Rebuilds color changing dialogs after choosing another coloring scheme
     * @param[in] doCreate Whether "create" shall be called (only if built the first time)
     */
    void rebuildColorMatrices(bool doCreate = false);

    /** @brief Loads a scheme from a file
     * @param[in] file The name of the file to read the settings from
     */
    void loadSettings(const std::string& file);

    /** @brief Writes the currently used decals into a file
     * @param[in] file The name of the file to write the decals into
     */
    void saveDecals(OutputDevice& dev) const;

    /** @brief Loads decals from a file
     * @param[in] file The name of the file to read the decals from
     */
    void loadDecals(const std::string& file);

    /// @brief reload known vehicle parameters
    void updateVehicleParams();

    /// @brief reload known POI parameters
    void updatePOIParams();

    /// @brief build header
    void buildHeader(FXVerticalFrame* contentFrame);

    /// @brief build frames
    /// @{

    void buildBackgroundFrame(FXTabBook* tabbook);
    void buildStreetsFrame(FXTabBook* tabbook);
    void buildVehiclesFrame(FXTabBook* tabbook);
    void buildPersonsFrame(FXTabBook* tabbook);
    void buildContainersFrame(FXTabBook* tabbook);
    void buildJunctionsFrame(FXTabBook* tabbook);
    void buildAdditionalsFrame(FXTabBook* tabbook);
    void buildDemandFrame(FXTabBook* tabbook);
    void buildPOIsFrame(FXTabBook* tabbook);
    void buildPolygonsFrame(FXTabBook* tabbook);
    void buildSelectionFrame(FXTabBook* tabbook);
    void buildDataFrame(FXTabBook* tabbook);
    void buildLegendFrame(FXTabBook* tabbook);
    void buildOpenGLFrame(FXTabBook* tabbook);
    void build3DFrame(FXTabBook* tabbook);

    ///@}

    /// @brief build buttons
    void buildButtons(FXVerticalFrame* contentFrame);

private:
    /// @brief invalidated copy constructor
    GUIDialog_ViewSettings(const GUIDialog_ViewSettings& s) = delete;

    /// @brief invalidated assignment operator
    GUIDialog_ViewSettings& operator=(const GUIDialog_ViewSettings& s) = delete;
};
