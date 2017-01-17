/****************************************************************************/
/// @file    GNEDialog_Wizard.h
/// @author  Jakob Erdmann
/// @date    July 2011
/// @version $Id$
///
// A Dialog for setting options (see OptionsCont)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
    /**@brief Constructor
     *
     * @param[in] parent The parent window
     * @param[in] name The title to show
     * @param[in] width The initial width of the dialog
     * @param[in] height The initial height of the dialog
     */
    GNEDialog_Wizard(FXWindow* parent,  const char* name, int width, int height);

    /// @brief Destructor
    ~GNEDialog_Wizard();

    // ===========================================================================
    // Option input classes
    // ===========================================================================
    class InputString : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEDialog_Wizard::InputString)

    public:
        /// @brief constructor
        InputString(FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        InputString() {}

    private:
        /// @brief name
        std::string myName;

        /// @brief text field
        FXTextField* myTextField;
    };

    class InputBool : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEDialog_Wizard::InputBool)

    public:
        /// @brief constructor
        InputBool(FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        InputBool() {}

    private:
        /// @brief name
        std::string myName;
        /// @brief menu check
        FXMenuCheck* myCheck;
    };


    class InputInt : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEDialog_Wizard::InputInt)

    public:
        /// @brief
        InputInt(FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        InputInt() {}

    private:
        /// @brief name
        std::string myName;

        /// @brief text field
        FXTextField* myTextField;
    };

    class InputFloat : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEDialog_Wizard::InputFloat)

    public:
        /// @brief constructor
        InputFloat(FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        InputFloat() {}

    private:
        /// @brief name
        std::string myName;

        /// @brief text field
        FXTextField* myTextField;
    };
};


#endif

/****************************************************************************/

