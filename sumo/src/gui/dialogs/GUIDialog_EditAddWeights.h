/****************************************************************************/
/// @file    GUIDialog_EditAddWeights.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 16 Jun 2004
/// @version $Id$
///
// A dialog for editing additional weights
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
#ifndef GUIDialog_EditAddWeights_h
#define GUIDialog_EditAddWeights_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fx.h>
#include <gui/GUIAddWeightsStorage.h>
#include <utils/xml/SAXWeightsHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIApplicationWindow;
class MFXAddEditTypedTable;
class FloatValueTimeLine;
class MSEdge;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIDialog_EditAddWeights
 * @brief A dialog for editing additional weights
 */
class GUIDialog_EditAddWeights : public FXMainWindow
{
    // FOX-declarations
    FXDECLARE(GUIDialog_EditAddWeights)
public:
    /// constructor
    GUIDialog_EditAddWeights(GUIMainWindow *parent);

    /// destructor
    ~GUIDialog_EditAddWeights();

    /// Called when the user presses the Load-button
    long onCmdLoad(FXObject*,FXSelector,void*);

    /// Called when the user presses the Save-button
    long onCmdSave(FXObject*,FXSelector,void*);

    /// Called when the user presses the Clear-button
    long onCmdClear(FXObject*,FXSelector,void*);

    /// Called when the user presses the Close-button
    long onCmdClose(FXObject*,FXSelector,void*);

    /// Called when the table was changed
    long onCmdEditTable(FXObject*,FXSelector,void*);

    /// Called to determine whether the Save-button shall be enabled
    long onUpdSave(FXObject*,FXSelector,void*);

protected:
    /**
     * @class Retriever_AddWeights
     * @brief Obtains supplementary edge weights from a weights handler and stores them within the edges
     */
    class Retriever_AddWeights
    {
    public:
        /**
         * enum WeightType
         * @brief Enumeration of possible supplementary weights
         */
        enum WeightType {
            /// The given weight is an absolute value
            ABSOLUTE_WEIGHT,
            /// The given weight shall be added to the original
            ADD_WEIGHT,
            /// The given weight shall be multiplied by the original
            MULT_WEIGHT
        };

        /**
         * enum SingleWeightRetriever
         * @brief Retriever of one of the possible supplementary weights
         */
        class SingleWeightRetriever : public SAXWeightsHandler::EdgeFloatTimeLineRetriever
        {
        public:
            /// Constructor
            SingleWeightRetriever(WeightType type, Retriever_AddWeights *parent);

            /// Destructor
            ~SingleWeightRetriever();

            /// Informs the parent about having obtained the given value
            void addEdgeWeight(const std::string &id,
                               SUMOReal val, SUMOTime beg, SUMOTime end);

        private:
            /// The parent
            Retriever_AddWeights *myParent;

            /// The type of the supp weight this retriever gets
            WeightType myType;

        };

        /// Constructor
        Retriever_AddWeights();

        /// Destructor
        ~Retriever_AddWeights();

        void addTypedWeight(WeightType type, const std::string &id,
                            SUMOReal val, SUMOTime beg, SUMOTime end);

        /// Returns the retriever for absolute values
        SingleWeightRetriever &getAbsoluteRetriever();

        /// Returns the retriever for additional values
        SingleWeightRetriever &getAddRetriever();

        /// Returns the retriever for multiplicative values
        SingleWeightRetriever &getMultRetriever();

    private:
        /**
         * struct SuppWeights
         * @brief A set of all three possible supplementary values of an edge
         */
        struct SuppWeights {
            /// The absolute time line
            FloatValueTimeLine *absolute;
            /// The additive time line
            FloatValueTimeLine *add;
            /// The multiplicative time line
            FloatValueTimeLine *mult;
        };

        /// A map from edges to supplementary weights
        std::map<MSEdge*, SuppWeights> myWeights;

        /// The retrievers (one for each type of supplementary weights)
        SingleWeightRetriever *myAbsoluteRetriever, *myAddRetriever,
        *myMultRetriever;

    };


private:
    /// Rebuilds the entry list
    void rebuildList();

    /// Builds a text representation of the items in the list
    std::string encode2XML();

    /**
     * @class time_sorter
     * @brief A comparison of GUIAddWeights by begin time
     */
    class time_sorter
    {
    public:
        /// constructor
        explicit time_sorter() { }

        int operator()(const GUIAddWeight &p1, const GUIAddWeight &p2) {
            return p1.timeBeg<p2.timeBeg;
        }
    };

private:
    /// the list that holds the ids
    MFXAddEditTypedTable *myTable;

    /// the parent window
    GUIMainWindow *myParent;

    /// Information whether the entered values are valid
    bool myEntriesAreValid;

protected:
    /// FOX needs this
    GUIDialog_EditAddWeights() { }

};


#endif

/****************************************************************************/

