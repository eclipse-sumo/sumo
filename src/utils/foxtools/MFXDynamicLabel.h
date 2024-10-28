/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    MFXDynamicLabel.h
/// @author  Mirko Barthauer
/// @date    31.03.2023
///
// Text label with dynamic multi-line text (inserts line breaks on the fly)
/****************************************************************************/

#include <string>

#include "fxheader.h"


/// @brief A list item which allows for custom coloring
class MFXDynamicLabel : public FXLabel {
    /// @brief FOX declaration
    FXDECLARE(MFXDynamicLabel)

public:
    /// @brief enum for events
    enum {
        MID_LABEL_WIDTHCHANGE,
    };

    /// @brief Construct label with given text and icon
    MFXDynamicLabel(FXComposite* p, const FXString& text, FXIcon* ic = 0, FXuint opts = LABEL_NORMAL, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0, FXint pl = DEFAULT_PAD, FXint pr = DEFAULT_PAD, FXint pt = DEFAULT_PAD, FXint pb = DEFAULT_PAD, std::string indent = "- ");

    virtual ~MFXDynamicLabel() {};

    /// @brief overload text label updates to store the original string as backup for when width changes again
    void setText(const FXString& text);

    /// @brief overload to be informed when the label text has to be reformatted due to width changes
    long onUpdate(FXObject* sender, FXSelector, void*);

    /// @brief overload position to be informed when the parent has done the layout
    void position(FXint x, FXint y, FXint w, FXint h);

    FXint getDefaultHeight();

protected:
    /// @brief fox needs this
    MFXDynamicLabel();

private:
    void computeIndentation();
    void reformatLineBreaks(const int curWidth);

    std::string myOriginalString;
    std::string myIndentString;
    int myIndent;
    int myPreviousWidth;
};
