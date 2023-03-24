/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GUIDialog_Options.h
/// @author  Jakob Erdmann
/// @date    July 2011
///
// A Dialog for setting options (see OptionsCont)
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>


// ===========================================================================
// class declaration
// ===========================================================================

class GUIMainWindow;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDialog_Options
 * @briefA Dialog for setting options (see OptionsCont)
 */
class GUIDialog_Options : public FXDialogBox {
    /// @brief FOX-declaration
    FXDECLARE(GUIDialog_Options)

public:
    /**@brief Constructor for options dialogs
     *
     * @param[in] parent The parent window
     * @param[in] optionsContainer edited option container
     * @param[in] name The title to show
     */
    static int Options(GUIMainWindow *windows, OptionsCont* optionsContainer, const char* titleName);

    /**@brief Constructor for run dialogs
     *
     * @param[in] parent The parent window
     * @param[in] optionsContainer edited option container
     * @param[in] name The title to show
     */
    static int Run(GUIMainWindow *windows, OptionsCont* optionsContainer, const char* titleName);

    /// @brief Destructor
    ~GUIDialog_Options();

    /// @brief called when user press run netgenerate button
    long onCmdRunNetgenerate(FXObject*, FXSelector, void*);

    // ===========================================================================
    // Option input classes
    // ===========================================================================

    class InputString : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GUIDialog_Options::InputString)

    public:
        /// @brief constructor
        InputString(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(InputString)

    private:
        /// @brief GUIDialog_Options parent
        GUIDialog_Options* myGUIDialogOptions = nullptr;

        /// @brief name
        std::string myName;

        /// @brief text field
        FXTextField* myTextField;
    };

    class InputStringVector : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GUIDialog_Options::InputStringVector)

    public:
        /// @brief constructor
        InputStringVector(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(InputStringVector)

    private:
        /// @brief GUIDialog_Options parent
        GUIDialog_Options* myGUIDialogOptions = nullptr;

        /// @brief name
        std::string myName;

        /// @brief text field
        FXTextField* myTextField;
    };

    class InputBool : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GUIDialog_Options::InputBool)

    public:
        /// @brief constructor
        InputBool(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(InputBool)

    private:
        /// @brief GUIDialog_Options parent
        GUIDialog_Options* myGUIDialogOptions = nullptr;

        /// @brief name
        std::string myName;

        /// @brief menu check
        FXMenuCheck* myCheck;
    };

    class InputInt : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GUIDialog_Options::InputInt)

    public:
        /// @brief
        InputInt(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(InputInt)

    private:
        /// @brief GUIDialog_Options parent
        GUIDialog_Options* myGUIDialogOptions = nullptr;

        /// @brief name
        std::string myName;

        /// @brief text field
        FXTextField* myTextField;
    };

    class InputIntVector : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GUIDialog_Options::InputIntVector)

    public:
        /// @brief
        InputIntVector(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(InputIntVector)

    private:
        /// @brief GUIDialog_Options parent
        GUIDialog_Options* myGUIDialogOptions = nullptr;

        /// @brief name
        std::string myName;

        /// @brief text field
        FXTextField* myTextField;
    };

    class InputFloat : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GUIDialog_Options::InputFloat)

    public:
        /// @brief constructor
        InputFloat(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(InputFloat)

    private:
        /// @brief GUIDialog_Options parent
        GUIDialog_Options* myGUIDialogOptions = nullptr;

        /// @brief name
        std::string myName;

        /// @brief text field
        FXTextField* myTextField;
    };

    class InputFilename : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GUIDialog_Options::InputFilename)

    public:
        /// @brief constructor
        InputFilename(GUIDialog_Options* GUIDialogOptions, FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(InputFilename)

    private:
        /// @brief GUIDialog_Options parent
        GUIDialog_Options* myGUIDialogOptions = nullptr;

        /// @brief name
        std::string myName;

        /// @brief text field
        FXTextField* myTextField;
    };

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GUIDialog_Options)

    /// @brief pointer to main window parent
    GUIMainWindow* myMainWindowParent;

    /// @brief refecente to edited Option container
    OptionsCont* myOptionsContainer;

private:
    /**@brief Constructor
     *
     * @param[in] parent The parent window
     * @param[in] optionsContainer edited option container
     * @param[in] name The title to show
     * @param[in] width The initial width of the dialog
     * @param[in] height The initial height of the dialog
     * @param[in] runDialog check if this is a run dialog
     */
    GUIDialog_Options(GUIMainWindow* parent, OptionsCont* optionsContainer, const char* titleName, const bool runDialog);

    /// @brief Invalidated copy constructor.
    GUIDialog_Options(const GUIDialog_Options&) = delete;

    /// @brief Invalidated assignment operator.
    GUIDialog_Options& operator=(const GUIDialog_Options&) = delete;
};
