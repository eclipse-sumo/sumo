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
/// @file    GNEPythonToolDialogElements.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2022
///
// Elements used in GNEPythonToolDialog
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>
#include <string>
#include <vector>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEPythonToolDialog;
class MFXLabelTooltip;
class Option;

// ===========================================================================
// class definitions
// ===========================================================================

/// @brief elements used in Tool Dialogs
class GNEPythonToolDialogElements {

public:
    /// @brief category
    class Category : public FXHorizontalFrame {

    public:
        /// @brief constructor
        Category(GNEPythonToolDialog* toolDialogParent, const std::string &category);

        /// @brief destructor
        ~Category();

    private:
        /// @brief Invalidated copy constructor.
        Category(const Category&) = delete;

        /// @brief Invalidated assignment operator.
        Category& operator=(const Category&) = delete;
    };

    /// @brief argument
    class Argument : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE_ABSTRACT(GNEPythonToolDialogElements::Argument)

    public:
        /// @brief constructor
        Argument(GNEPythonToolDialog* toolDialogParent, const std::string &parameter, Option* option);

        /// @brief destructor
        ~Argument();

        /// @brief reset to default value
        virtual void reset() = 0;

        /// @brief get parameter label
        MFXLabelTooltip* getParameterLabel() const;

        /// @brief get argument
        const std::string getArgument() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when user changes argument value
        virtual long onCmdSetValue(FXObject*, FXSelector, void*) = 0;

        /// @brief Called when user press reset button
        long onCmdResetValue(FXObject*, FXSelector, void*);

        /// @brief Called when user press reset button
        long onUpdResetValue(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief default constructor
        Argument();

        /// @brief get value
        virtual const std::string getValue() const = 0;

        /// @brief option
        Option* myOption;

        /// @brief default value
        const std::string myDefaultValue;

        /// @brief auxliar text field for textfield frames
        FXHorizontalFrame* myAuxiliarTextFieldFrame = nullptr;

    private:
        /// @brief parameter label
        MFXLabelTooltip* myParameterLabel = nullptr;

        /// @brief reset button
        FXButton* myResetButton = nullptr;

        /// @brief Invalidated copy constructor.
        Argument(const Argument&) = delete;

        /// @brief Invalidated assignment operator.
        Argument& operator=(const Argument&) = delete;
    };

    /// @brief filename argument
    class FileNameArgument : public Argument {
        /// @brief FOX-declaration
        FXDECLARE(GNEPythonToolDialogElements::FileNameArgument)

    public:
        /// @brief constructor
        FileNameArgument(GNEPythonToolDialog* toolDialogParent, const std::string name, Option* option);
    
        /// @brief reset to default value
        void reset();

        /// @brief Called when user press open filename button
        long onCmdOpenFilename(FXObject*, FXSelector, void*);

        /// @brief Called when user set filename
        long onCmdSetValue(FXObject*, FXSelector, void*);

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

    public:
        /// @brief constructor
        StringArgument(GNEPythonToolDialog* toolDialogParent, const std::string name, Option* option);

        /// @brief reset to default value
        void reset();

        /// @brief Called when user set string value
        long onCmdSetValue(FXObject*, FXSelector, void*);

    protected:
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

    public:
        /// @brief constructor
        IntArgument(GNEPythonToolDialog* toolDialogParent, const std::string name, Option* option);

        /// @brief reset to default value
        void reset();

        /// @brief Called when user set int value
        long onCmdSetValue(FXObject*, FXSelector, void*);

    protected:
        /// @brief get value
        const std::string getValue() const;

        /// @brief int textField
        FXTextField* myIntTextField = nullptr;

    private:
        /// @brief invalid int in string format
        static const std::string INVALID_INT_STR;

        /// @brief Invalidated copy constructor.
        IntArgument(const IntArgument&) = delete;

        /// @brief Invalidated assignment operator.
        IntArgument& operator=(const IntArgument&) = delete;
    };

    /// @brief float argument
    class FloatArgument : public Argument {

    public:
        /// @brief constructor
        FloatArgument(GNEPythonToolDialog* toolDialogParent, const std::string name, Option* option);
    
        /// @brief reset to default value
        void reset();

        /// @brief Called when user set float value
        long onCmdSetValue(FXObject*, FXSelector, void*);

    protected:
        /// @brief get value
        const std::string getValue() const;

        /// @brief float textField
        FXTextField* myFloatTextField = nullptr;

    private:
        /// @brief invalid float in string format
        static const std::string INVALID_DOUBLE_STR;

        /// @brief Invalidated copy constructor.
        FloatArgument(const IntArgument&) = delete;

        /// @brief Invalidated assignment operator.
        FloatArgument& operator=(const IntArgument&) = delete;
    };


    /// @brief bool argument
    class BoolArgument : public Argument {

    public:
        /// @brief constructor
        BoolArgument(GNEPythonToolDialog* toolDialogParent, const std::string name, Option* option);

        /// @brief reset to default value
        void reset();

        /// @brief Called when user set bool value
        long onCmdSetValue(FXObject*, FXSelector, void*);

    protected:
        /// @brief get value
        const std::string getValue() const;

        /// @brief check button
        FXCheckButton* myCheckButton = nullptr;

    private:
        /// @brief Invalidated copy constructor.
        BoolArgument(const IntArgument&) = delete;

        /// @brief Invalidated assignment operator.
        BoolArgument& operator=(const IntArgument&) = delete;
    };
};
