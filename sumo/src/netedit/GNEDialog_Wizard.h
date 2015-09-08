/****************************************************************************/
/// @file    GNEDialog_Wizard.h
/// @author  Jakob Erdmann
/// @date    July 2011
/// @version $Id$
///
// A Dialog for setting options (see OptionsCont)
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
#ifndef GNEDialog_Wizard_h
#define GNEDialog_Wizard_h


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
 * @class GNEDialog_Wizard
 * @briefA Dialog for setting options (see OptionsCont)
 */
class GNEDialog_Wizard : public FXDialogBox {
public:
    /** @brief Constructor
     *
     * @param[in] parent The parent window
     * @param[in] name The title to show
     * @param[in] width The initial width of the dialog
     * @param[in] height The initial height of the dialog
     */
    GNEDialog_Wizard(FXWindow* parent,  const char* name, int width, int height) ;


    /// @brief Destructor
    ~GNEDialog_Wizard() ;


    // ===========================================================================
    // Option input classes
    // ===========================================================================
    class InputString : public FXHorizontalFrame {
        // FOX-declarations
        FXDECLARE(GNEDialog_Wizard::InputString)
    public:
        InputString(FXComposite* parent, const std::string& name);
        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);
    protected:
        /// @brief FOX needs this
        InputString() {}
    private:
        std::string myName;
        FXTextField* myTextField;
    };

    class InputBool : public FXHorizontalFrame {
        // FOX-declarations
        FXDECLARE(GNEDialog_Wizard::InputBool)
    public:
        InputBool(FXComposite* parent, const std::string& name);
        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);
    protected:
        /// @brief FOX needs this
        InputBool() {}
    private:
        std::string myName;
        FXMenuCheck* myCheck;
    };


};


#endif

/****************************************************************************/

