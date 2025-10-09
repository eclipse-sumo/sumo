/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNECrashDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2025
///
// Dialog used for handling crashes produced in Netedit
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXLinkLabel.h>
#include <utils/foxtools/MFXTextFieldIcon.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIDesigns.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4266) // mask warning about hidden member function FX::FXTextCodec::utf2mblen
#endif
#include <FX88591Codec.h>
#include <FXUTF16Codec.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "GNECrashDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Map
FXDEFMAP(GNECrashDialog) GNECrashDialogMap[] = {
    FXMAPFUNC(SEL_CLIPBOARD_REQUEST,    0,  GNECrashDialog::onClipboardRequest),
};

// Object implementation
FXIMPLEMENT(GNECrashDialog, GNEDialog, GNECrashDialogMap, ARRAYNUMBER(GNECrashDialogMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNECrashDialog::GNECrashDialog(GNEApplicationWindow* applicationWindow, const ProcessError& processError) :
    GNEDialog(applicationWindow, TL("Critical error"), GUIIcon::ERROR_SMALL,
              DialogType::ABOUT, GNEDialog::Buttons::OK_COPY_REPORT, OpenType::MODAL, ResizeMode::RESIZABLE, 800, 600),
    myTraceText(processError.getTrace()) {
    // create dialog layout (obtained from FXMessageBox)
    auto contents = new FXVerticalFrame(myContentFrame, LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 10, 10, 10, 10);
    // add information label
    new FXLabel(contents, TL("Netedit found an internal critical error and will be closed:"), NULL, GUIDesignLabel(JUSTIFY_NORMAL));
    // create text field for exception
    myExceptionTextField = new MFXTextFieldIcon(contents, applicationWindow->getStaticTooltipMenu(), GUIIcon::EMPTY, nullptr, 0, GUIDesignTextField);
    myExceptionTextField->setEditable(FALSE);
    myExceptionTextField->setText(processError.what());
    // add information label
    new FXLabel(contents, TL("ErrorTrace:"), NULL, GUIDesignLabel(JUSTIFY_NORMAL));
    FXText* text = new FXText(contents, nullptr, 0, TEXT_WORDWRAP | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    text->setEditable(FALSE);
    text->setText(myTraceText.c_str());
    // disable copy button if trace is empty
    if (myTraceText.size() == 0) {
        myCopyButton->disable();
    }
    // open modal dialog
    openDialog();
}


GNECrashDialog::~GNECrashDialog() {
}


void
GNECrashDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNECrashDialog::onCmdCopy(FXObject*, FXSelector, void*) {
    FXDragType types[4];
    types[0] = stringType;
    types[1] = textType;
    types[2] = utf8Type;
    types[3] = utf16Type;
    // Acquire clipboard in different formats
    acquireClipboard(types, 4);
    return 1;
}


long
GNECrashDialog::onCmdReport(FXObject*, FXSelector, void*) {
    // open new issue
    return MFXLinkLabel::fxexecute("https://github.com/eclipse-sumo/sumo/issues/new");
}


long
GNECrashDialog::onClipboardRequest(FXObject* sender, FXSelector sel, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    // Perhaps the target wants to supply its own data for the clipboard
    if (GNEDialog::onClipboardRequest(sender, sel, ptr)) {
        return 1;
    }
    // Recognize the request?
    if (event->target == stringType || event->target == textType || event->target == utf8Type || event->target == utf16Type) {
        // Return myClippedText text as as UTF-8
        if (event->target == utf8Type) {
            setDNDData(FROM_CLIPBOARD, event->target, myTraceText.c_str());
            return 1;
        }
        // Return myClippedText text translated to 8859-1
        if (event->target == stringType || event->target == textType) {
            FX88591Codec ascii;
            setDNDData(FROM_CLIPBOARD, event->target, ascii.utf2mb(myTraceText.c_str()));
            return 1;
        }
        // Return text of the selection translated to UTF-16
        if (event->target == utf16Type) {
            FXUTF16LECodec unicode;             // FIXME maybe other endianness for unix
            setDNDData(FROM_CLIPBOARD, event->target, unicode.utf2mb(myTraceText.c_str()));
            return 1;
        }
    }
    return 0;
}


/****************************************************************************/
