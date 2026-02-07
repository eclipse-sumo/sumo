/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEOptionsEditorRow.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// Row used in GNEOptionsEditor to edit options
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <utils/foxtools/fxheader.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEOptionsEditor;
class MFXLabelTooltip;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEOptionsEditorRow {

public:

    /// @brief input option
    class OptionRow : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE_ABSTRACT(GNEOptionsEditorRow::OptionRow)

    public:
        /// @brief constructor
        OptionRow(GNEOptionsEditor* optionsEditor, FXComposite* parent, const std::string& topic,
                  const std::string& name, const std::string& description, const std::string& defaultValue,
                  const bool editable);

        /// @brief adjust input name size
        void adjustNameSize();

        /// @brief get topic
        const std::string& getTopic() const;

        /// @brief get name (Lower)
        const std::string getNameLower() const;

        /// @brief get description  (Lower)
        const std::string getDescriptionLower() const;

        /// @brief update option (used after load options)
        virtual void updateOption() = 0;

        /// @brief restore option (used for setting original options)
        virtual void restoreOption() = 0;

        /// @brief called when user set value in textField/button/checkBox
        virtual long onCmdSetOption(FXObject*, FXSelector, void*) = 0;

        /// @brief called when user press reset button
        virtual long onCmdResetOption(FXObject*, FXSelector, void*) = 0;

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(OptionRow)

        /// @brief GNEOptionsEditor parent
        GNEOptionsEditor* myOptionsEditor = nullptr;

        /// @brief topic
        const std::string myTopic;

        /// @brief name
        const std::string myName;

        /// @brief description
        const std::string myDescription;

        /// @brief default value
        const std::string myDefaultValue;

        /// @brief content frame
        FXHorizontalFrame* myContentFrame = nullptr;

        /// @brief editable
        const bool myEditable = true;

        /// @brief update reset button
        void updateResetButton();

    private:
        /// @brief get value
        virtual std::string getValue() const = 0;

        /// @brief tooltip label for name
        MFXLabelTooltip* myNameLabel = nullptr;

        // @brief reset button
        FXButton* myResetButton = nullptr;
    };

    /// @brief input string
    class OptionString : public OptionRow {

    public:
        /// @brief constructor
        OptionString(GNEOptionsEditor* optionsEditor, FXComposite* parent, const std::string& topic,
                     const std::string& name, const std::string& description, const std::string& defaultValue,
                     const bool editable);

        /// @brief update option
        void updateOption();

        /// @brief restore option
        void restoreOption();

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief get value
        std::string getValue() const;

        /// @brief text field
        FXTextField* myStringTextField = nullptr;
    };

    /// @brief input string vector
    class OptionStringVector : public OptionRow {

    public:
        /// @brief constructor
        OptionStringVector(GNEOptionsEditor* optionsEditor, FXComposite* parent, const std::string& topic,
                           const std::string& name, const std::string& description, const std::string& defaultValue,
                           const bool editable);

        /// @brief update option
        void updateOption();

        /// @brief restore option
        void restoreOption();

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief get value
        std::string getValue() const;

        /// @brief text field
        FXTextField* myStringVectorTextField = nullptr;
    };

    /// @brief input bool
    class OptionBool : public OptionRow {

    public:
        /// @brief constructor
        OptionBool(GNEOptionsEditor* optionsEditor, FXComposite* parent, const std::string& topic,
                   const std::string& name, const std::string& description, const std::string& defaultValue,
                   const bool editable);

        /// @brief update option
        void updateOption();

        /// @brief restore option
        void restoreOption();

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief get value
        std::string getValue() const;

        /// @brief menu check
        FXCheckButton* myCheckButton = nullptr;
    };

    /// @brief input int
    class OptionInt : public OptionRow {

    public:
        /// @brief
        OptionInt(GNEOptionsEditor* optionsEditor, FXComposite* parent, const std::string& topic,
                  const std::string& name, const std::string& description, const std::string& defaultValue,
                  const bool editable);

        /// @brief update option
        void updateOption();

        /// @brief restore option
        void restoreOption();

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief get value
        std::string getValue() const;

        /// @brief text field
        FXTextField* myIntTextField = nullptr;
    };

    /// @brief input int vector
    class OptionIntVector : public OptionRow {

    public:
        /// @brief
        OptionIntVector(GNEOptionsEditor* optionsEditor, FXComposite* parent, const std::string& topic,
                        const std::string& name, const std::string& description, const std::string& defaultValue,
                        const bool editable);

        /// @brief update option
        void updateOption();

        /// @brief restore option
        void restoreOption();

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief get value
        std::string getValue() const;

        /// @brief text field
        FXTextField* myIntVectorTextField = nullptr;
    };

    /// @brief input float
    class OptionFloat : public OptionRow {

    public:
        /// @brief constructor
        OptionFloat(GNEOptionsEditor* optionsEditor, FXComposite* parent, const std::string& topic,
                    const std::string& name, const std::string& description, const std::string& defaultValue,
                    const bool editable);

        /// @brief update option
        void updateOption();

        /// @brief restore option
        void restoreOption();

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief get value
        std::string getValue() const;

        /// @brief parse float xx to xx.00
        std::string parseFloat(const std::string& value) const;

        /// @brief text field
        FXTextField* myFloatTextField = nullptr;
    };

    /// @brief input float
    class OptionTime : public OptionRow {

    public:
        /// @brief constructor
        OptionTime(GNEOptionsEditor* optionsEditor, FXComposite* parent, const std::string& topic,
                   const std::string& name, const std::string& description, const std::string& defaultValue,
                   const bool editable);

        /// @brief update option
        void updateOption();

        /// @brief restore option
        void restoreOption();

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief get value
        std::string getValue() const;

        /// @brief parse float xx to xx.00
        std::string parseTime(const std::string& value) const;

        /// @brief text field
        FXTextField* myTimeTextField = nullptr;
    };

    /// @brief input filename
    class OptionFilename : public OptionRow {
        /// @brief FOX-declaration
        FXDECLARE(GNEOptionsEditorRow::OptionFilename)

    public:
        /// @brief constructor
        OptionFilename(GNEOptionsEditor* optionsEditor, FXComposite* parent, const std::string& topic,
                       const std::string& name, const std::string& description, const std::string& defaultValue,
                       const bool editable);

        /// @brief update option
        void updateOption();

        /// @brief restore option
        void restoreOption();

        /// @brief called when user press open dialog button
        long onCmdOpenDialog(FXObject*, FXSelector, void*);

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        OptionFilename();

    private:
        /// @brief get value
        std::string getValue() const;

        /// @brief open filename button
        FXButton* myOpenFilenameButton = nullptr;

        /// @brief text field
        FXTextField* myFilenameTextField = nullptr;
    };
};
