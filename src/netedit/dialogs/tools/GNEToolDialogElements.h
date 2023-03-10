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
/// @file    GNEToolDialogElements.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Elements used in GNEToolDialog
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>
#include <string>
#include <vector>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEToolDialog;
class MFXLabelTooltip;
class Option;

// ===========================================================================
// class definitions
// ===========================================================================

/// @brief elements used in Tool Dialogs
class GNEToolDialogElements {

public:
    /// @brief argument
    class Argument : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE_ABSTRACT(GNEToolDialogElements::Argument)

    public:
        /// @brief constructor
        Argument(GNEToolDialog* toolDialogParent, const std::string &parameter, const std::string &defaultValue, const std::string &description);

        /// @brief destructor
        ~Argument();

        /// @brief get parameter label
        MFXLabelTooltip* getParameterLabel() const;

        /// @brief reset to default value
        virtual void reset() = 0;

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when user changes Z value
        long XXXX(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief default constructor
        Argument();

        /// @brief get value
        virtual const std::string getValue() const = 0;

        /// @brief parameter
        const std::string myParameter;

        /// @brief argument name
        const std::string myDefaultValue;

    private:
        /// @brief parameter label
        MFXLabelTooltip* myParameterLabel = nullptr;

        /// @brief Invalidated copy constructor.
        Argument(const Argument&) = delete;

        /// @brief Invalidated assignment operator.
        Argument& operator=(const Argument&) = delete;
    };

    /// @brief header
    class HeaderArgument : public Argument {

    public:
        /// @brief constructor
        HeaderArgument(GNEToolDialog* toolDialogParent);

        /// @brief reset to default value
        void reset();

    protected:
        /// @brief get value
        const std::string getValue() const;
    
    private:
        /// @brief Invalidated copy constructor.
        HeaderArgument(const HeaderArgument&) = delete;

        /// @brief Invalidated assignment operator.
        HeaderArgument& operator=(const HeaderArgument&) = delete;
    };

    /// @brief filename argument
    class FileNameArgument : public Argument {
        /// @brief FOX-declaration
        FXDECLARE(GNEToolDialogElements::FileNameArgument)

    public:
        /// @brief constructor
        FileNameArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option);
    
        /// @brief reset to default value
        void reset();

        /// @brief Called when user set filename
        long onCmdSetFilenameValue(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX need this
        FileNameArgument();

        /// @brief get value
        const std::string getValue() const;

    private:
        /// @brief filename button
        FXButton* myFilenameButton = nullptr;

        /// @brief filename textField
        FXTextField* myFilenameTextField = nullptr;

        /// @brief Invalidated copy constructor.
        FileNameArgument(const FileNameArgument&) = delete;

        /// @brief Invalidated assignment operator.
        FileNameArgument& operator=(const FileNameArgument&) = delete;
    };

    /// @brief string argument
    class StringArgument : public Argument {
        /// @brief FOX-declaration
        FXDECLARE(GNEToolDialogElements::StringArgument)

    public:
        /// @brief constructor
        StringArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option);

        /// @brief reset to default value
        void reset();

        /// @brief Called when user set string value
        long onCmdSetStringValue(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX need this
        StringArgument();

        /// @brief get value
        const std::string getValue() const;

        /// @brief string textField
        FXTextField* myStringTextField = nullptr;

    private:
        /// @brief Invalidated copy constructor.
        StringArgument(const StringArgument&) = delete;

        /// @brief Invalidated assignment operator.
        StringArgument& operator=(const StringArgument&) = delete;
    };

    /// @brief int argument
    class IntArgument : public Argument {
        /// @brief FOX-declaration
        FXDECLARE(GNEToolDialogElements::IntArgument)

    public:
        /// @brief constructor
        IntArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option);

        /// @brief reset to default value
        void reset();

        /// @brief Called when user set int value
        long onCmdSetIntValue(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX need this
        IntArgument();

        /// @brief get value
        const std::string getValue() const;

        /// @brief int textField
        FXTextField* myIntTextField = nullptr;

    private:
        /// @brief Invalidated copy constructor.
        IntArgument(const IntArgument&) = delete;

        /// @brief Invalidated assignment operator.
        IntArgument& operator=(const IntArgument&) = delete;
    };

    /// @brief float argument
    class FloatArgument : public Argument {
        /// @brief FOX-declaration
        FXDECLARE(GNEToolDialogElements::FloatArgument)

    public:
        /// @brief constructor
        FloatArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option);
    
        /// @brief reset to default value
        void reset();

        /// @brief Called when user set float value
        long onCmdSetFloatValue(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX need this
        FloatArgument();

        /// @brief get value
        const std::string getValue() const;

        /// @brief float textField
        FXTextField* myFloatTextField = nullptr;

    private:
        /// @brief Invalidated copy constructor.
        FloatArgument(const IntArgument&) = delete;

        /// @brief Invalidated assignment operator.
        FloatArgument& operator=(const IntArgument&) = delete;
    };


    /// @brief bool argument
    class BoolArgument : public Argument {
        /// @brief FOX-declaration
        FXDECLARE(GNEToolDialogElements::BoolArgument)

    public:
        /// @brief constructor
        BoolArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option);

        /// @brief reset to default value
        void reset();

        /// @brief Called when user set float value
        long onCmdSetBoolValue(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX need this
        BoolArgument();

        /// @brief get value
        const std::string getValue() const;

        /// @brief float textField
        FXTextField* myBoolTextField = nullptr;

    private:
        /// @brief Invalidated copy constructor.
        BoolArgument(const IntArgument&) = delete;

        /// @brief Invalidated assignment operator.
        BoolArgument& operator=(const IntArgument&) = delete;
    };
};
