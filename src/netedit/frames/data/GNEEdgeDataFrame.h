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
class GNEDataSet;

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
    // class DataSetSelector
    // ===========================================================================

    class DataSetSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEEdgeDataFrame::DataSetSelector)

    public:
        /// @brief constructor
        DataSetSelector(GNEEdgeDataFrame* edgeDataFrameParent);

        /// @brief destructor
        ~DataSetSelector();

        /// @brief refresh interval selector
        void refreshDataSetSelector(const GNEDataSet *currentDataSet);

        /// @brief get current select data set ID
        GNEDataSet *getDataSet() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user press button "create dataSet"
        long onCmdCreateDataSet(FXObject*, FXSelector, void*);

        /// @brief Called when the user set a new data set ID
        long onCmdSetNewDataSetID(FXObject*, FXSelector, void*);

        /// @brief Called when the user select an existent data set
        long onCmdSelectDataSet(FXObject* obj, FXSelector, void*);

        /// @brief Called when the user select check button
        long onCmdSelectCheckButton(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        FOX_CONSTRUCTOR(DataSetSelector)

    private:
        /// @brief pointer to edgeData frame Parent
        GNEEdgeDataFrame* myEdgeDataFrameParent;

        /// @brief check button to create a new dataSet
        FXCheckButton *myNewDataSetCheckButton;

        /// @brief horizontal frame new id
        FXHorizontalFrame *myHorizontalFrameNewID;

        /// @brief interval new id
        FXTextField *myNewDataSetIDTextField; 

        /// @brief create dataSet button
        FXButton* myCreateDataSetButton;

        /// @brief comboBox with intervals
        FXComboBox* myDataSetsComboBox;
    };

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

        /// @brief get current select data set ID
        GNEDataInterval *getDataInterval() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user press button "create interval"
        long onCmdCreateInterval(FXObject*, FXSelector, void*);

        /// @brief Called when the user select an interval in the list
        long onCmdSelectInterval(FXObject*, FXSelector, void*);

        /// @brief Called when the user changes begin or end
        long onCmdSetIntervalAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when the user select check button
        long onCmdSelectCheckButton(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        FOX_CONSTRUCTOR(IntervalSelector)

    private:
        /// @brief add item into list
        FXTreeItem* addListItem(GNEDataInterval* dataInterval, FXTreeItem* itemParent = nullptr);

        /// @brief pointer to edgeData frame Parent
        GNEEdgeDataFrame* myEdgeDataFrameParent;

        /// @brief check button to create a new interval
        FXCheckButton *myNewIntervalCheckButton;

        /// @brief interval begin horizontal frame
        FXHorizontalFrame *myHorizontalFrameBegin; 

        /// @brief interval begin text field
        FXTextField *myBeginTextField; 

        /// @brief interval end horizontal frame
        FXHorizontalFrame *myHorizontalFrameEnd; 

        /// @brief interval end
        FXTextField *myEndTextField; 
        
        /// @brief create interval button
        FXButton* myCreateIntervalButton;

        /// @brief tree list to show the interval list
        FXTreeList* myIntervalsTreelist;

        /// @brief map used to save the Tree items with their GNEDataInterval
        std::map<FXTreeItem*, GNEDataInterval*> myTreeItemIntervalMap;
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
    /// @brief dataSet selector modul
    DataSetSelector *myDataSetSelector;

    /// @brief interval selector modul
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
