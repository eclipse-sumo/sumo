/****************************************************************************/
/// @file    GNEDialog_About.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id: GNEDialog_About.h 3359 2014-02-22 09:38:00Z behr_mi $
///
// The "About" - dialog for NETEDIT, (adapted from GUIDialog_AboutSUMO)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEDialog_About_h
#define GNEDialog_About_h


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
// class definitions
// ===========================================================================
/**
 * @class GNEDialog_About
 * @brief The application's "About" - dialog
 */
class GNEDialog_About : public FXDialogBox {
public:
    /** @brief Constructor
     * @param[in] parent The parent window
     */
    GNEDialog_About(FXWindow* parent);


    /// @brief Destructor
    ~GNEDialog_About() ;


    /** @brief Creates the widget (and the icons)
     */
    void create();


private:
    /// @brief Icons for the widget
    FXIcon* myDLRIcon;

    /// @brief Font for the widget
    FXFont* myHeadlineFont;

};


#endif

/****************************************************************************/

