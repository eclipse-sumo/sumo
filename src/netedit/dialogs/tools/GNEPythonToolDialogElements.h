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
        Category(FXVerticalFrame* argumentFrame, const std::string& category);

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
        Argument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame, const std::string& parameter, Option* option);

        /// @brief destructor
        ~Argument();

        /// @brief reset to default value
        virtual void reset() = 0;

        /// @brief get parameter label
        MFXLabelTooltip* getParameterLabel() const;

        /// @brief get argument
        const std::string getArgument() const;

        /// @brief check if required attribute is set
        bool requiredAttributeSet() const;

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

        /// @brief tool dialog parent
        GNEPythonToolDialog* myToolDialogParent;

        /// @brief option
        Option* myOption;

        /// @brief default value
        const std::string myDefaultValue;

        /// @brief auxiliar elements frame
        FXHorizontalFrame* myElementsFrame = nullptr;

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
        FileNameArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
                         const std::string name, Option* option);

        /// @brief reset to default value
        void reset();

        /// @brief Called when user press open filename button
        long onCmdOpenFilename(FXObject*, FXSelector, void*);

        /// @brief Called when user set filename
        long onCmdSetValue(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX need this
        FileNameArgument();

        /// @brief constructor for current buttons
        FileNameArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
                         const std::string name, Option* option, const std::string& useCurrent);

        /// @brief get value
        const std::string getValue() const;

        /// @brief filename textField
        FXTextField* myFilenameTextField = nullptr;

    private:
        /// @brief current button
        FXButton* myCurrentButton = nullptr;

        /// @brief filename button
        FXButton* myOpenFilenameButton = nullptr;

        /// @brief Invalidated copy constructor.
        FileNameArgument(const FileNameArgument&) = delete;

        /// @brief Invalidated assignment operator.
        FileNameArgument& operator=(const FileNameArgument&) = delete;
    };

    /// @brief network argument
    class NetworkArgument : public FileNameArgument {
        /// @brief FOX-declaration
        FXDECLARE(GNEPythonToolDialogElements::NetworkArgument)

    public:
        /// @brief constructor
        NetworkArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
                        const std::string name, Option* option);

        /// @brief Called when user press open filename button
        long onCmdOpenFilename(FXObject*, FXSelector, void*);

        /// @brief Called when user press use current button
        long onCmdUseCurrent(FXObject*, FXSelector, void*);

        /// @brief enable or disable use current button
        long onUpdUseCurrent(FXObject* sender, FXSelector, void*);

    protected:
        /// @brief FOX need this
        NetworkArgument();

    private:
        /// @brief Invalidated copy constructor.
        NetworkArgument(const NetworkArgument&) = delete;

        /// @brief Invalidated assignment operator.
        NetworkArgument& operator=(const NetworkArgument&) = delete;
    };

    /// @brief network argument
    class AdditionalArgument : public FileNameArgument {
        /// @brief FOX-declaration
        FXDECLARE(GNEPythonToolDialogElements::AdditionalArgument)

    public:
        /// @brief constructor
        AdditionalArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
                           const std::string name, Option* option);

        /// @brief Called when user press open filename button
        long onCmdOpenFilename(FXObject*, FXSelector, void*);

        /// @brief Called when user press use current button
        long onCmdUseCurrent(FXObject*, FXSelector, void*);

        /// @brief enable or disable use current button
        long onUpdUseCurrent(FXObject* sender, FXSelector, void*);

    protected:
        /// @brief FOX need this
        AdditionalArgument();

    private:
        /// @brief Invalidated copy constructor.
        AdditionalArgument(const AdditionalArgument&) = delete;

        /// @brief Invalidated assignment operator.
        AdditionalArgument& operator=(const AdditionalArgument&) = delete;
    };

    /// @brief network argument
    class RouteArgument : public FileNameArgument {
        /// @brief FOX-declaration
        FXDECLARE(GNEPythonToolDialogElements::RouteArgument)

    public:
        /// @brief constructor
        RouteArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
                      const std::string name, Option* option);

        /// @brief Called when user press open filename button
        long onCmdOpenFilename(FXObject*, FXSelector, void*);

        /// @brief Called when user press use current button
        long onCmdUseCurrent(FXObject*, FXSelector, void*);

        /// @brief enable or disable use current button
        long onUpdUseCurrent(FXObject* sender, FXSelector, void*);

    protected:
        /// @brief FOX need this
        RouteArgument();

    private:
        /// @brief Invalidated copy constructor.
        RouteArgument(const RouteArgument&) = delete;

        /// @brief Invalidated assignment operator.
        RouteArgument& operator=(const RouteArgument&) = delete;
    };

    /// @brief network argument
    class DataArgument : public FileNameArgument {
        /// @brief FOX-declaration
        FXDECLARE(GNEPythonToolDialogElements::DataArgument)

    public:
        /// @brief constructor
        DataArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame,
                     const std::string name, Option* option);

        /// @brief Called when user press open filename button
        long onCmdOpenFilename(FXObject*, FXSelector, void*);

        /// @brief Called when user press use current button
        long onCmdUseCurrent(FXObject*, FXSelector, void*);

        /// @brief enable or disable use current button
        long onUpdUseCurrent(FXObject* sender, FXSelector, void*);

    protected:
        /// @brief FOX need this
        DataArgument();

    private:
        /// @brief Invalidated copy constructor.
        DataArgument(const DataArgument&) = delete;

        /// @brief Invalidated assignment operator.
        DataArgument& operator=(const DataArgument&) = delete;
    };

    /// @brief string argument
    class StringArgument : public Argument {

    public:
        /// @brief constructor
        StringArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame, const std::string name, Option* option);

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
        IntArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame, const std::string name, Option* option);

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
        FloatArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame, const std::string name, Option* option);

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
        BoolArgument(GNEPythonToolDialog* toolDialogParent, FXVerticalFrame* argumentFrame, const std::string name, Option* option);

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
