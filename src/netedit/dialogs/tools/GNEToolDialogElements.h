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
        Argument(const std::string &name_, const Option* option_);

        /// @brief destructor
        ~Argument();

        /// @brief reset to default value
        virtual void reset() = 0;

    protected:
        /// @brief argument name
        const std::string name;

        /// @brief default value
        const Option* option;

    private:
        /// @brief default constructor
        Argument();

        /// @brief Invalidated copy constructor.
        Argument(const Argument&) = delete;

        /// @brief Invalidated assignment operator.
        Argument& operator=(const Argument&) = delete;
    };

    /// @brief filename
    class FileNameArgument : protected FXVerticalFrame, protected Argument {

    public:
        /// @brief constructor
        FileNameArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option);

        /// get argument (parameter and value)
        std::string getArgument() const;

        /// @brief reset argument values
        void reset();

    protected:
        /// @brief filename button
        FXButton* myFilenameButton = nullptr;

        /// @brief filename Textfield
        FXTextField* myFilenameTextField = nullptr;

    private:
        /// @brief Invalidated copy constructor.
        FileNameArgument(const FileNameArgument&) = delete;

        /// @brief Invalidated assignment operator.
        FileNameArgument& operator=(const FileNameArgument&) = delete;
    };

    /// @brief separator
    class Separator : protected FXVerticalFrame {

    public:
        /// @brief constructor
        Separator(FXComposite* parent, const std::string name);

    private:
        /// @brief Invalidated copy constructor.
        Separator(const Separator&) = delete;

        /// @brief Invalidated assignment operator.
        Separator& operator=(const Separator&) = delete;
    };
};
