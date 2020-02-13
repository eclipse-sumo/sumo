/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEEdgeDataFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// The Widget for add edgeData elements
/****************************************************************************/
#ifndef GNEEdgeDataFrame_h
#define GNEEdgeDataFrame_h


// ===========================================================================
// included modules
// ===========================================================================

#include <netedit/frames/GNEFrame.h>


// ===========================================================================
// class declaration
// ===========================================================================

class GNEDataInterval;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEEdgeDataFrame
 * The Widget for setting internal attributes of additional elements
 */
class GNEEdgeDataFrame : public GNEFrame {

public:
    // ===========================================================================
    // class IntervalSelector
    // ===========================================================================

    class IntervalSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEEdgeDataFrame::IntervalSelector)

    public:
        /// @brief constructor
        IntervalSelector(GNEEdgeDataFrame* edgeDataFrameParent);

        /// @brief destructor
        ~IntervalSelector();

        /// @brief refresh interval selector
        void refreshIntervalSelector();

        /// @brief get new interval ID (or return empty string if isn't valid)
        std::string getIntervalID() const;

        /// @brief check if create data set
        bool getCreateDataSet() const;

        /// @brief get begin
        double getBegin() const;

        /// @brief get end
        double getEnd() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select a interval in ComboBox
        long onCmdSelectInterval(FXObject*, FXSelector, void*);

        /// @brief Called when the user edit an attribute
        long onCmdSetAttribute(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        FOX_CONSTRUCTOR(IntervalSelector)

    private:
        /// @brief pointer to edgeData frame Parent
        GNEEdgeDataFrame* myEdgeDataFrameParent;

        /// @brief comboBox with intervals
        FXComboBox* myIntervalsComboBox;

        /// @brief horizontal frame for new ID
        FXHorizontalFrame *myHorizontalFrameNewID;

        /// @brief interval new id
        FXTextField *myNewIDTextField; 

        /// @brief interval begin
        FXTextField *myBeginTextField; 

        /// @brief interval end
        FXTextField *myEndTextField; 
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEEdgeDataFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEEdgeDataFrame();

    /// @brief show Frame
    void show();

    /**@brief add additional element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @return true if additional was sucesfully added
     */
    bool addEdgeData(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

protected:
    /// @brief IntervalSelector modul
    IntervalSelector *myIntervalSelector;

    /// @brief parameters editor
    GNEFrameAttributesModuls::ParametersEditor *myParametersEditor;

    /// @brief interval selected
    void intervalSelected();

private:
    //
};


#endif

/****************************************************************************/
