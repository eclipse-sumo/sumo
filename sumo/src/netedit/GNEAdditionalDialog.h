/****************************************************************************/
/// @file    GNEAdditionalDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id$
///
/// A abstract class for editing additional elements
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <vector>


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
    /// @brief constructor
    GNEAdditionalDialog(GNEAdditional* parent, int width, int height);

    /// @brief destructor
    ~GNEAdditionalDialog();

    /// @brief change additional dialog header
    void changeAdditionalDialogHeader(const std::string& newHeader);

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

    /// @brief frame for contents
    FXVerticalFrame* myContentFrame;

    /// @brief accept button
    FXButton* myAcceptButton;

    /// @brief cancel button
    FXButton* myCancelButton;

    /// @brief cancel button
    FXButton* myResetButton;

private:
    /// @brief Invalidated copy constructor
    GNEAdditionalDialog(const GNEAdditionalDialog&);

    /// @brief Invalidated assignment operator
    GNEAdditionalDialog& operator=(const GNEAdditionalDialog&);
};

#endif
