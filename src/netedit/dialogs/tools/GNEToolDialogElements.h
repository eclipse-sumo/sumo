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
        virtual ~Argument();

        /// @brief get label
        virtual FXLabel* getNameLabel() = 0;

        /// @brief reset to default value
        virtual void reset() = 0;

        /// @brief get argument name
        const std::string getName() const;

        /// @brief get argument options
        const Option* getOption() const;

    private:
        /// @brief argument name
        const std::string myName;

        /// @brief argument options
        const Option* myOption;

        /// @brief default constructor
        Argument();

        /// @brief Invalidated copy constructor.
        Argument(const Argument&) = delete;

        /// @brief Invalidated assignment operator.
        Argument& operator=(const Argument&) = delete;
    };

    /// @brief filename
    class FileNameArgument : protected FXHorizontalFrame, public Argument {

    public:
        /// @brief constructor
        FileNameArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option);
        
        /// @brief get label
        FXLabel* getNameLabel();

        /// @brief reset argument values
        void reset();

        /// get argument (parameter and value)
        std::string getArgument() const;

    private:
        /// @brief label name
        FXLabel* myNameLabel = nullptr;

        /// @brief filename button
        FXButton* myFilenameButton = nullptr;

        /// @brief filename Textfield
        FXTextField* myFilenameTextField = nullptr;

        /// @brief Invalidated copy constructor.
        FileNameArgument(const FileNameArgument&) = delete;

        /// @brief Invalidated assignment operator.
        FileNameArgument& operator=(const FileNameArgument&) = delete;
    };

    /// @brief filename
    class StringArgument : protected FXHorizontalFrame, public Argument {

    public:
        /// @brief constructor
        StringArgument(GNEToolDialog* toolDialogParent, const std::string name, const Option* option);

        /// @brief get label
        FXLabel* getNameLabel();

        /// @brief reset argument values
        void reset();

        /// get argument (parameter and value)
        std::string getArgument() const;

    private:
        /// @brief label name
        FXLabel* myNameLabel = nullptr;

        /// @brief string Textfield
        FXTextField* myStringTextField = nullptr;

        /// @brief Invalidated copy constructor.
        StringArgument(const StringArgument&) = delete;

        /// @brief Invalidated assignment operator.
        StringArgument& operator=(const StringArgument&) = delete;
    };
};
