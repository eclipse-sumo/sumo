/****************************************************************************/
/// @file    GNEAdditionalDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
/// A abstract class for editing additional elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEAdditionalDialog_h
#define GNEAdditionalDialog_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>


// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditional;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEAdditionalDialog
 * @brief Dialog to edit sequences, parameters, etc.. of Additionals
 */
class GNEAdditionalDialog : public FXDialogBox {
    /// @brief FOX-declaration abstract
    FXDECLARE_ABSTRACT(GNEAdditionalDialog)

public:
    // Constructor
    GNEAdditionalDialog(GNEAdditional* parent, int width, int height);

    // destructor
    ~GNEAdditionalDialog();

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press accept button
    virtual long onCmdAccept(FXObject* sender, FXSelector sel, void* ptr) = 0;

    /// @brief event after press cancel button
    virtual long onCmdCancel(FXObject* sender, FXSelector sel, void* ptr) = 0;

    /// @brief event after press cancel button
    virtual long onCmdReset(FXObject*, FXSelector, void*) = 0;
    /// @}

protected:
    /// @brief FOX needs this
    GNEAdditionalDialog() {}

    /// @brief Invalidated show dialog for external calls
    void show();

    /// @brief Invalidated show dialog for external calls
    void hide();

    /// @brief Invalidated execute dialog for external calls
    void execute();

    /// @brief frame for contents
    FXVerticalFrame* myContentFrame;

    /// @brief accept button
    FXButton* myAcceptButton;

    /// @brief cancel button
    FXButton* myCancelButton;

    /// @brief cancel button
    FXButton* myResetButton;

private:
    /// @brief update data table
    /// @note must be implemented in all childs
    virtual void updateTable() = 0;

    /// @brief frame for buttons (Not accesible for childs)
    FXHorizontalFrame* myButtonFrame;

    /// @brief Invalidated copy constructor
    GNEAdditionalDialog(const GNEAdditionalDialog&);

    /// @brief Invalidated assignment operator
    GNEAdditionalDialog& operator=(const GNEAdditionalDialog&);
};

#endif
