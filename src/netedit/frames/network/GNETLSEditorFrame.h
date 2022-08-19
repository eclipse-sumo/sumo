/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNETLSEditorFrame.h
/// @author  Jakob Erdmann
/// @date    May 2011
///
// The Widget for modifying traffic lights
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <netbuild/NBTrafficLightLogic.h>
#include <netedit/frames/GNEOverlappedInspection.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBTrafficLightDefinition;
class NBLoadedSUMOTLDef;
class NBOwnTLDef;
class GNEInternalLane;
class GNETLSTable;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETLSEditorFrame
 * The Widget for modifying Traffic Lights (TLS)
 */
class GNETLSEditorFrame : public GNEFrame {

public:
    // ===========================================================================
    // class TLSJunction
    // ===========================================================================

    class TLSJunction : public MFXGroupBoxModule {

    public:
        /// @brief constructor
        TLSJunction(GNETLSEditorFrame* tlsEditorParent);

        /// @brief destructor
        ~TLSJunction();

        /// @brief get current modified junction
        GNEJunction* getCurrentJunction() const;

        /// @brief set current junction
        void setCurrentJunction(GNEJunction* junction);

        /// @brief update descrition
        void updateJunctionDescription() const;

    private:
        /// @brief label for junction ID
        FXLabel* myJunctionIDLabel;

        /// @brief text field for junction ID
        FXTextField* myJunctionIDTextField;

        /// @brief label for program ID
        FXLabel* myTLSIDLabel;

        /// @brief text field for junction ID
        FXTextField* myTLSIDTextField;

        /// @brief the junction of the tls is being modified
        GNEJunction* myCurrentJunction;
    };

    // ===========================================================================
    // class TLSDefinition
    // ===========================================================================

    class TLSDefinition : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETLSEditorFrame::TLSDefinition)

    public:
        /// @brief constructor
        TLSDefinition(GNETLSEditorFrame* TLSEditorParent);

        /// @brief destructor
        ~TLSDefinition();

        /// @brief init TLS Definitions
        bool initTLSDefinitions();

        /// @brief clear TLS Definitions
        void clearTLSDefinitions();

        /// @brief get number of programs
        int getNumberOfPrograms() const;

        /// @brief get number of TLS definitions
        int getNumberOfTLSDefinitions() const;

        /// @brief check if current TLS was modified
        bool checkHaveModifications() const;

        /// @brief mark Program as modified
        void markAsModified();

        /// @brief get current definition
        NBTrafficLightDefinition* getCurrentTLSDefinition() const;

        /// @brief get current program ID
        const std::string getCurrentTLSProgramID() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press button create/duplicate TLS Program
        long onCmdCreate(FXObject*, FXSelector, void*);

        /// @brief Called when the user press button delete TLS Program
        long onCmdDelete(FXObject*, FXSelector, void*);

        /// @brief Called when the user press button reset current TLS Program
        long onCmdResetCurrentProgram(FXObject*, FXSelector, void*);

        /// @brief Called when the user press button reset all TLS Programs
        long onCmdResetAll(FXObject*, FXSelector, void*);

        /// @brief Called when the user switchs a TLS
        long onCmdDefSwitchTLSProgram(FXObject*, FXSelector, void*);

        /// @brief Called when occurs an update of create definition
        long onUpdCreateButton(FXObject*, FXSelector, void*);

        /// @brief Called when occurs an update of switch definition
        long onUpdTLSModified(FXObject*, FXSelector, void*);

        /// @brief Called when the user presses the save-Button
        long onCmdSaveChanges(FXObject*, FXSelector, void*);

        /// @brief Called when the user presses the Cancel-button
        long onCmdDiscardChanges(FXObject*, FXSelector, void*);

        /// @brief Called when occurs an update of modified
        long onUpdModified(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(TLSDefinition)

        /// @brief create (or duplicate) TLS in the given junction
        void createTLS(GNEJunction* junction);

    private:
        /// @brief pointer to GNETLSEditorFrame parent
        GNETLSEditorFrame* myTLSEditorParent = nullptr;

        /// @brief the list of Definitions for the current junction
        std::vector<NBTrafficLightDefinition*> myTLSDefinitions;

        /// @brief the comboBox for selecting the tl-definition to edit
        FXComboBox* myProgramComboBox;

        /// @brief whether the current tls was modified
        bool myHaveModifications = false;

        /// @brief button for create new Traffic light program
        FXButton* myCreateButton = nullptr;
    };

    // ===========================================================================
    // class TLSAttributes
    // ===========================================================================

    class TLSAttributes : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETLSEditorFrame::TLSAttributes)

    public:
        /// @brief constructor
        TLSAttributes(GNETLSEditorFrame* TLSEditorParent);

        /// @brief destructor
        ~TLSAttributes();

        /// @brief initializes the definitions and corresponding listbox
        void initTLSAttributes();

        /// @brief clear TLS attributes
        void clearTLSAttributes();

        /// @brief get current offset in string format
        SUMOTime getOffset() const;

        /// @brief set new offset
        void setOffset(const SUMOTime& offset);

        /// @brief is current offset valid
        bool isValidOffset();

        /// @brief get current parameters in string format
        std::string getParameters() const;

        /// @brief set new parameters
        void setParameters(const std::string& parameters);

        /// @brief are current parameter valid
        bool isValidParameters();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user presses the button Guess
        long onCmdGuess(FXObject*, FXSelector, void*);

        /// @brief Called when the user changes the offset of a TLS
        long onCmdSetOffset(FXObject*, FXSelector, void*);

        /// @brief Called when the user changes parameters of a TLS
        long onCmdSetParameters(FXObject*, FXSelector, void*);

        /// @brief Called when the user renames a TLS
        long onCmdDefRename(FXObject*, FXSelector, void*);

        /// @brief Called when the user sub-renames a TLS
        long onCmdDefSubRename(FXObject*, FXSelector, void*);

        /// @brief Called when the user adds a OFF
        long onCmdDefAddOff(FXObject*, FXSelector, void*);

        /// @brief Called when occurs an update of needs TLS definition
        long onUpdNeedsTLSDef(FXObject*, FXSelector, void*);

        /// @brief Called when occurs an update of switch definition
        long onUpdTLSModified(FXObject*, FXSelector, void*);

        /// @brief Called when user press edit parameters button
        long onCmdEditParameters(FXObject*, FXSelector, void* ptr);

        /// @}
    
    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(TLSAttributes)

    private:
        /// @brief pointer to TLSEditorParent
        GNETLSEditorFrame* myTLSEditorParent;

        /// @brief TLS Type text field
        FXTextField* myTLSType;

        /// @brief the TextField for modifying offset
        FXTextField* myOffsetTextField;

        /// @brief button for edit parameters
        FXButton* myButtonEditParameters;

        /// @brief the TextField for modifying parameters
        FXTextField* myParametersTextField;
    };

    // ===========================================================================
    // class TLSPhases
    // ===========================================================================

    class TLSPhases : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETLSEditorFrame::TLSPhases)

    public:
        /// @brief constructor
        TLSPhases(GNETLSEditorFrame* TLSEditorParent);

        /// @brief destructor
        ~TLSPhases();

        /// @brief get TLSEditor Parent
        GNETLSEditorFrame* getTLSEditorParent() const;

        /// @brief get phase table
        GNETLSTable* getPhaseTable() const;

        /**@brief initialies the phase table
         * @param[in] index The index to select
         */
        void initPhaseTable();

        /// @brief change phase value (state, name, next, etc.)
        bool changePhaseValue(const int col, const int row, const std::string &value);

        /// @brief add phase
        void addPhase(const int row, const char c = ' ');

        /// @brief duplicate phase
        void duplicatePhase(const int row);

        /// @brief delete phase
        void removePhase(const int row);

        /// @brief move phase up
        void movePhaseUp(const int row);

        /// @brief move phase down
        void movePhaseDown(const int row);

        /// @brief update TLS coloring
        void updateTLSColoring();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user cleans up states
        long onCmdCleanStates(FXObject*, FXSelector, void*);

        /// @brief Called when the user cleans up states
        long onCmdAddUnusedStates(FXObject*, FXSelector, void*);

        /// @brief Called when the user groups states
        long onCmdGroupStates(FXObject*, FXSelector, void*);

        /// @brief Called when the user ungroups states
        long onCmdUngroupStates(FXObject*, FXSelector, void*);

        /// @brief Called to update the ungroups states button
        long onUpdUngroupStates(FXObject*, FXSelector, void*);

        /// @brief Called to buttons that modify link indices
        long onUpdNeedsSingleDef(FXObject*, FXSelector, void*);

        /// @brief enable buttons, only when a tlLogic is being edited
        long onUpdNeedsDef(FXObject*, FXSelector, void*);

        /// @brief Called when occurs an update of needs definition and phase
        long onUpdNeedsDefAndPhase(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(TLSPhases)

    private:
        /// @brief init static phase table
        void initStaticPhaseTable();

        /// @brief init actuated phase table
        void initActuatedPhaseTable();

        /// @brief init delayBase phase table
        void initDelayBasePhaseTable();

        /// @brief init NEMA phase table
        void initNEMAPhaseTable();

        /// @brief build default phase
        int buildDefaultPhase(const int row);

        /// @brief set duration
        bool setDuration(const int col, const int row, const std::string &value);

        /// @brief set state
        bool setState(const int col, const int row, const std::string &value);

        /// @brief set next
        bool setNext(const int col, const int row, const std::string &value);

        /// @brief set name
        bool setName(const int row, const std::string &value);

        /// @brief set minDur
        bool setMinDur(const int row, const std::string &value);

        /// @brief set maxDur
        bool setMaxDur(const int row, const std::string &value);

        /// @brief set earliestEnd
        bool setEarliestEnd(const int row, const std::string &value);

        /// @brief set latestEnd
        bool setLatestEnd(const int row, const std::string &value);

        /// @brief set vehExt
        bool setVehExt(const int row, const std::string &value);

        /// @brief set yellow
        bool setYellow(const int row, const std::string &value);

        /// @brief set red
        bool setRed(const int row, const std::string &value);

        /// @brief recomputes cycle duration and updates label
        void updateCycleDuration(const int col);

        /// @brief update state size
        void updateStateSize(const int col);

        /// @brief pointer to TLSEditor Parent
        GNETLSEditorFrame* myTLSEditorParent;

        /// @brief table for selecting and rearranging phases and for changing duration
        GNETLSTable* myPhaseTable;
    };

    // ===========================================================================
    // class TLSFile
    // ===========================================================================

    class TLSFile : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETLSEditorFrame::TLSFile)

    public:
        /// @brief constructor
        TLSFile(GNETLSEditorFrame* TLSEditorParent);

        /// @brief destructor
        ~TLSFile();

        /// @name FOX-callbacks
        /// @{
        /// @brief load TLS Program from an additional file
        long onCmdLoadTLSProgram(FXObject*, FXSelector, void*);

        /// @brief save TLS Programm to an additional file
        long onCmdSaveTLSProgram(FXObject*, FXSelector, void*);

        /// @brief enable buttons, only when a tlLogic is being edited
        long onUpdNeedsDef(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(TLSFile)

    private:
        /// @brief pointer to TLSEditor Parent
        GNETLSEditorFrame* myTLSEditorParent;

        /// @brief convert SUMOTime into string
        std::string writeSUMOTime(SUMOTime steps);
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNETLSEditorFrame(GNEViewParent *viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNETLSEditorFrame();

    /// @brief show inspector frame
    void show();

    /// @brief function called after setting new width in current frame
    void frameWidthUpdated();

    /**@brief edits the traffic light for the given clicked junction
     * @param[in] clickedPosition clicked position
     * @param[in] objectsUnderCursor The clicked objects under cursor
     */
    void editTLS(const Position& clickedPosition, const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief check if modifications in TLS was saved
    bool isTLSSaved();

    /// @brief parse TLS Programs from a file
    bool parseTLSPrograms(const std::string& file);

    /// @brief update phase definition for the current traffic light and phase
    void handleChange(GNEInternalLane* lane);

    /// @brief update phase definition for the current traffic light and phase
    void handleMultiChange(GNELane* lane, FXObject* obj, FXSelector sel, void* data);

    /// @brief whether the given edge is controlled by the currently edited tlDef
    bool controlsEdge(GNEEdge* edge) const;

    /// @brief open GNEAttributesCreator extended dialog (can be reimplemented in frame children)
    void selectedOverlappedElement(GNEAttributeCarrier* AC);

    /// @brief get modul for TLS Definition
    GNETLSEditorFrame::TLSDefinition* getTLSDefinition() const;

    /**@brief edits the traffic light for the given junction
     * @param[in] junction The junction of which the traffic light shall be edited
     */
    void editJunction(GNEJunction* junction);

protected:
    /// @brief converts to SUMOTime
    static SUMOTime getSUMOTime(const std::string& value);

    /// @brief converts to SUMOTime
    static const std::string getSteps2Time(const SUMOTime value);

private:
    /// @brief Overlapped Inspection
    GNEOverlappedInspection* myOverlappedInspection = nullptr;

    /// @brief modul for TLS Junction
    GNETLSEditorFrame::TLSJunction* myTLSJunction = nullptr;

    /// @brief modul for TLS Definition
    GNETLSEditorFrame::TLSDefinition* myTLSDefinition = nullptr;

    /// @brief modul for TLS attributes
    GNETLSEditorFrame::TLSAttributes* myTLSAttributes = nullptr;

    /// @brief modul for TLS Phases
    GNETLSEditorFrame::TLSPhases* myTLSPhases = nullptr;

    /// @brief modul for load/Save TLS Programs
    GNETLSEditorFrame::TLSFile* myTLSFile = nullptr;

    /// @brief the internal lanes belonging the the current junction indexed by their tl-index
    std::map<int, std::vector<GNEInternalLane*> > myInternalLanes;

    /// @brief the traffic light definition being edited
    NBLoadedSUMOTLDef* myEditedDef = nullptr;

    /// @brief index of the phase being shown
    int myPhaseIndex = 0;

    /// @brief cleans up previous lanes
    void cleanup();

    /// @brief builds internal lanes for the given tlDef
    void buildInternalLanes(NBTrafficLightDefinition* tlDef);

    /// @brief get certain phase of the current traffic light
    const NBTrafficLightLogic::PhaseDefinition& getPhase(const int index);

    /// @brief convert duration (potentially undefined) to string
    static std::string varDurString(SUMOTime dur);
};
