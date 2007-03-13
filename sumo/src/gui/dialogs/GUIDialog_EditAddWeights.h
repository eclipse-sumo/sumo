/****************************************************************************/
/// @file    GUIDialog_EditAddWeights.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 16 Jun 2004
/// @version $Id$
///
// A dialog for editing additional weights
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
#ifndef GUIDialog_EditAddWeights_h
#define GUIDialog_EditAddWeights_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fx.h>
#include <gui/GUIAddWeightsStorage.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIApplicationWindow;
class MFXAddEditTypedTable;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIDialog_EditAddWeights
 * Instances of this class are windows that display the list of instances
 * from a given artifact like vehicles, edges or junctions and allow
 * one of their items
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

    /// Called when the user presses the Deselect-button
    long onCmdDeselect(FXObject*,FXSelector,void*);

    /// Called when the user presses the Clear-button
    long onCmdClear(FXObject*,FXSelector,void*);

    /// Called when the user presses the Close-button
    long onCmdClose(FXObject*,FXSelector,void*);

    /// Called when the table was changed
    long onCmdEditTable(FXObject*,FXSelector,void*);

    /// Called to determine whether the Save-button shall be enabled
    long onUpdSave(FXObject*,FXSelector,void*);

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
        explicit time_sorter()
        { }

        int operator()(const GUIAddWeight &p1, const GUIAddWeight &p2)
        {
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
    GUIDialog_EditAddWeights()
    { }

};


#endif

/****************************************************************************/

