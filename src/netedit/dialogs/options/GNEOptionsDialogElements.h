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
/// @file    GNEOptionsDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2023
///
// A Dialog for setting options (see OptionsCont)
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXLabelTooltip.h>
#include <string>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEOptionsDialog;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEOptionsDialog
 * @briefA Dialog for setting options (see OptionsCont)
 */
class GNEOptionsDialogElements {
    
public:

    class InputOption : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE_ABSTRACT(GNEOptionsDialogElements::InputOption)

    public:
        /// @brief constructor
        InputOption(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
                    const std::string& name, const std::string& description);

        /// @brief adjust input name size
        void adjustNameSize();

        /// @brief get topic
        const std::string &getTopic() const;

        /// @brief get name (Lower)
        const std::string getNameLower() const;

        /// @brief get description  (Lower)
        const std::string getDescriptionLower() const;

        /// @brief called when user set value in textField/button/checkBox
        virtual long onCmdSetOption(FXObject*, FXSelector, void*) = 0;

        /// @brief called when user press reset button
        virtual long onCmdResetOption(FXObject*, FXSelector, void*) = 0;

        /// @brief update reset button
        long onUpdResetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(InputOption)

        /// @brief GNEOptionsDialog parent
        GNEOptionsDialog* myGUIDialogOptions = nullptr;

        /// @brief topic
        const std::string myTopic;

        /// @brief name
        const std::string myName;

        /// @brief description
        const std::string myDescription;

        /// @brief content frame
        FXHorizontalFrame* myContentFrame = nullptr;

    private:
        /// @brief tooltip label for name
        MFXLabelTooltip* myNameLabel = nullptr;

        // @brief reset button
        FXButton* myResetButton = nullptr; 
    };

    class InputString : public InputOption {

    public:
        /// @brief constructor
        InputString(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
                    const std::string& name, const std::string& description);

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief text field
        FXTextField* myStringTextField = nullptr;
    };

    class InputStringVector : public InputOption {

    public:
        /// @brief constructor
        InputStringVector(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
                          const std::string& name, const std::string& description);

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief text field
        FXTextField* myStringVectorTextField = nullptr;
    };

    class InputBool : public InputOption {

    public:
        /// @brief constructor
        InputBool(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
                  const std::string& name, const std::string& description);

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief menu check
        FXCheckButton* myCheckButton = nullptr; 
    };

    class InputInt : public InputOption {

    public:
        /// @brief
        InputInt(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
                 const std::string& name, const std::string& description);

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief text field
        FXTextField* myIntTextField = nullptr;
    };

    class InputIntVector : public InputOption {

    public:
        /// @brief
        InputIntVector(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
                       const std::string& name, const std::string& description);

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief text field
        FXTextField* myIntVectorTextField = nullptr;
    };

    class InputFloat : public InputOption {

    public:
        /// @brief constructor
        InputFloat(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
                   const std::string& name, const std::string& description);

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    private:
        /// @brief text field
        FXTextField* myFloatTextField = nullptr;
    };

    class InputFilename : public InputOption {
        /// @brief FOX-declaration
        FXDECLARE(GNEOptionsDialogElements::InputFilename)

    public:
        /// @brief constructor
        InputFilename(GNEOptionsDialog* GUIDialogOptions, FXComposite* parent, const std::string& topic,
                      const std::string& name, const std::string& description);

        /// @brief called when user press open dialog button
        long onCmdOpenDialog(FXObject*, FXSelector, void*);

        /// @brief called when user set value in textField/button/checkBox
        long onCmdSetOption(FXObject*, FXSelector, void*);

        /// @brief called when user press reset button
        long onCmdResetOption(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        InputFilename();

    private:
        /// @brief open filename button
        FXButton* myOpenFilenameButton = nullptr;

        /// @brief text field
        FXTextField* myFilenameTextField = nullptr;
    };
};
