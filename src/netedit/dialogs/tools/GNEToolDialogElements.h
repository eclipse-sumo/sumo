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
    class Argument {

    public:
        /// @brief constructor
        Argument(const std::string &parameter, const std::string &defaultValue);

        /// @brief destructor
        virtual ~Argument();

        /// @brief reset to default value
        virtual void reset() = 0;

        /// @brief get parameter label
        virtual MFXLabelTooltip* getParameterLabel() const = 0;

    protected:
        /// @brief default constructor
        Argument();

        /// @brief parameter
        const std::string myParameter;

        /// @brief argument name
        const std::string myDefaultValue;

    private:
        /// @brief Invalidated copy constructor.
        Argument(const Argument&) = delete;

        /// @brief Invalidated assignment operator.
        Argument& operator=(const Argument&) = delete;
    };

    /// @brief header
    class HeaderArgument : protected FXHorizontalFrame, public Argument {

    public:
        /// @brief constructor
        HeaderArgument(GNEToolDialog* toolDialogParent);

        /// @brief reset argument values
        void reset();

        /// @brief get parameter label
        MFXLabelTooltip* getParameterLabel() const;

        /// get argument (parameter and value)
        std::string getArgument() const;

    private:
        /// @brief parameter label
        MFXLabelTooltip* myParameterLabel = nullptr;

        /// @brief filename button
        FXButton* myFilenameButton = nullptr;

        /// @brief filename Textfield
        FXTextField* myFilenameTextField = nullptr;

        /// @brief Invalidated copy constructor.
        HeaderArgument(const HeaderArgument&) = delete;

        /// @brief Invalidated assignment operator.
        HeaderArgument& operator=(const HeaderArgument&) = delete;
    };

    /// @brief filename
    class FileNameArgument : protected FXHorizontalFrame, public Argument {
        /// @brief FOX-declaration
        FXDECLARE(GNEToolDialogElements::FileNameArgument)

    public:
        /// @brief constructor
        FileNameArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option);

        /// @brief reset argument values
        void reset();

        /// @brief get parameter label
        MFXLabelTooltip* getParameterLabel() const;

        /// get argument (parameter and value)
        std::string getArgument() const;
    
        /// @name FOX-callbacks
        /// @{

        /// @brief Called when user changes Z value
        long onCmdSetValue(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FileNameArgument();

    private:
        /// @brief parameter label
        MFXLabelTooltip* myParameterLabel = nullptr;

        /// @brief filename button
        FXButton* myFilenameButton = nullptr;

        /// @brief filename Textfield
        FXTextField* myFilenameTextField = nullptr;

        /// @brief Invalidated copy constructor.
        FileNameArgument(const FileNameArgument&) = delete;

        /// @brief Invalidated assignment operator.
        FileNameArgument& operator=(const FileNameArgument&) = delete;
    };

    /// @brief string
    class StringArgument : protected FXHorizontalFrame, public Argument {
        /// @brief FOX-declaration
        FXDECLARE(GNEToolDialogElements::StringArgument)

    public:
        /// @brief constructor
        StringArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option, const int type = 0);

        /// @brief reset argument values
        void reset();

        /// @brief get parameter label
        MFXLabelTooltip* getParameterLabel() const;

        /// get argument (parameter and value)
        std::string getArgument() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when user changes Z value
        long onCmdSetValue(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        StringArgument();

        /// @brief string Textfield
        FXTextField* myStringTextField = nullptr;

    private:
        /// @brief parameter label
        MFXLabelTooltip* myParameterLabel = nullptr;

        /// @brief Invalidated copy constructor.
        StringArgument(const StringArgument&) = delete;

        /// @brief Invalidated assignment operator.
        StringArgument& operator=(const StringArgument&) = delete;
    };

    /// @brief int
    class IntArgument : public StringArgument {
        /// @brief FOX-declaration
        FXDECLARE(GNEToolDialogElements::IntArgument)

    public:
        /// @brief constructor
        IntArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option);

    protected:
        /// @brief FOX need this
        IntArgument();

    private:
        /// @brief Invalidated copy constructor.
        IntArgument(const IntArgument&) = delete;

        /// @brief Invalidated assignment operator.
        IntArgument& operator=(const IntArgument&) = delete;
    };

    /// @brief float
    class FloatArgument : public StringArgument {
        /// @brief FOX-declaration
        FXDECLARE(GNEToolDialogElements::FloatArgument)

    public:
        /// @brief constructor
        FloatArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option);
    
    protected:
        /// @brief FOX need this
        FloatArgument();

    private:
        /// @brief Invalidated copy constructor.
        FloatArgument(const IntArgument&) = delete;

        /// @brief Invalidated assignment operator.
        FloatArgument& operator=(const IntArgument&) = delete;
    };
};
