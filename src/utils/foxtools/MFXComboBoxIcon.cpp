/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
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
/// @file    MFXComboBoxIcon.cpp
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    2018-12-19
///
//
/****************************************************************************/

/* =========================================================================
 * included modules
 * ======================================================================= */
#include "MFXComboBoxIcon.h"
#include "MFXListItemIcon.h"


#define ICON_SPACING        4   // Spacing between icon and label (2 + 2)
#define ICON_SIZE           16
#define COMBOBOX_INS_MASK   (COMBOBOX_REPLACE | COMBOBOX_INSERT_BEFORE | COMBOBOX_INSERT_AFTER | COMBOBOX_INSERT_FIRST | COMBOBOX_INSERT_LAST)
#define COMBOBOX_MASK       (COMBOBOX_STATIC | COMBOBOX_INS_MASK)

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(MFXComboBoxIcon) MFXComboBoxIconMap[] = {
    FXMAPFUNC(SEL_FOCUS_UP,         0,                              MFXComboBoxIcon::onFocusUp),
    FXMAPFUNC(SEL_FOCUS_DOWN,       0,                              MFXComboBoxIcon::onFocusDown),
    FXMAPFUNC(SEL_FOCUS_SELF,       0,                              MFXComboBoxIcon::onFocusSelf),
    FXMAPFUNC(SEL_UPDATE,           MFXComboBoxIcon::ID_TEXT,       MFXComboBoxIcon::onUpdFmText),
    FXMAPFUNC(SEL_CLICKED,          MFXComboBoxIcon::ID_LIST,       MFXComboBoxIcon::onListClicked),
    FXMAPFUNC(SEL_COMMAND,          MFXComboBoxIcon::ID_LIST,       MFXComboBoxIcon::onListClicked),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,  MFXComboBoxIcon::ID_TEXT,       MFXComboBoxIcon::onTextButton),
    FXMAPFUNC(SEL_MOUSEWHEEL,       MFXComboBoxIcon::ID_TEXT,       MFXComboBoxIcon::onMouseWheel),
    FXMAPFUNC(SEL_CHANGED,          MFXComboBoxIcon::ID_TEXT,       MFXComboBoxIcon::onTextChanged),
    FXMAPFUNC(SEL_COMMAND,          MFXComboBoxIcon::ID_TEXT,       MFXComboBoxIcon::onTextCommand),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_SETVALUE,          MFXComboBoxIcon::onFwdToText),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_SETINTVALUE,       MFXComboBoxIcon::onFwdToText),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_SETREALVALUE,      MFXComboBoxIcon::onFwdToText),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_SETSTRINGVALUE,    MFXComboBoxIcon::onFwdToText),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_GETINTVALUE,       MFXComboBoxIcon::onFwdToText),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_GETREALVALUE,      MFXComboBoxIcon::onFwdToText),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_GETSTRINGVALUE,    MFXComboBoxIcon::onFwdToText),
};

// Object implementation
FXIMPLEMENT(MFXComboBoxIcon,    FXPacker,   MFXComboBoxIconMap, ARRAYNUMBER(MFXComboBoxIconMap))

// ===========================================================================
// member method definitions
// ===========================================================================

MFXComboBoxIcon::MFXComboBoxIcon(FXComposite* p, FXint cols, const bool haveIcons, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb):
    FXPacker(p, opts, x, y, w, h, 0, 0, 0, 0, 0, 0),
    myHaveIcons(haveIcons) {
    flags |= FLAG_ENABLED;
    target = tgt;
    message = sel;
    myIconLabel = new FXLabel(this, "", nullptr, 0, 0, 0, 0, 0, pl, pr, pt, pb);
    if (!myHaveIcons) {
        myIconLabel->hide();
    }
    myTextFieldIcon = new MFXTextFieldIcon(this, cols, this, MFXComboBoxIcon::ID_TEXT, 0, 0, 0, 0, 0, pl, pr, pt, pb);
    if (options & COMBOBOX_STATIC) {
        myTextFieldIcon->setEditable(FALSE);
    }
    myPane = new FXPopup(this, FRAME_LINE);
    myList = new MFXListIcon(myPane, this, MFXComboBoxIcon::ID_LIST, LIST_BROWSESELECT | LIST_AUTOSELECT | LAYOUT_FILL_X | LAYOUT_FILL_Y | SCROLLERS_TRACK | HSCROLLER_NEVER);
    if (options & COMBOBOX_STATIC) {
        myList->setScrollStyle(SCROLLERS_TRACK | HSCROLLING_OFF);
    }
    myButton = new FXMenuButton(this, FXString::null, NULL, myPane, FRAME_RAISED | FRAME_THICK | MENUBUTTON_DOWN | MENUBUTTON_ATTACH_RIGHT, 0, 0, 0, 0, 0, 0, 0, 0);
    myButton->setXOffset(border);
    myButton->setYOffset(border);
    flags &= ~FLAG_UPDATE;  // Never GUI update
}


MFXComboBoxIcon::~MFXComboBoxIcon() {
    delete myPane;
    myPane = (FXPopup*) - 1L;
    myIconLabel = (FXLabel*) - 1L;
    myTextFieldIcon = (MFXTextFieldIcon*) - 1L;
    myButton = (FXMenuButton*) - 1L;
    myList = (MFXListIcon*) - 1L;
}


void
MFXComboBoxIcon::create() {
    FXPacker::create();
    myPane->create();
}


void
MFXComboBoxIcon::detach() {
    FXPacker::detach();
    myPane->detach();
}


void
MFXComboBoxIcon::destroy() {
    myPane->destroy();
    FXPacker::destroy();
}


void
MFXComboBoxIcon::enable() {
    if (!isEnabled()) {
        FXPacker::enable();
        myIconLabel->enable();
        myTextFieldIcon->enable();
        myButton->enable();
    }
}


void
MFXComboBoxIcon::disable() {
    if (isEnabled()) {
        FXPacker::disable();
        myIconLabel->disable();
        myTextFieldIcon->disable();
        myButton->disable();
    }
}


FXint
MFXComboBoxIcon::getDefaultWidth() {
    FXint ww, pw;
    if (myIconLabel->shown()) {
        ww = myIconLabel->getDefaultWidth() + myTextFieldIcon->getDefaultWidth() + myButton->getDefaultWidth() + (border << 1);
    } else {
        ww = myTextFieldIcon->getDefaultWidth() + myButton->getDefaultWidth() + (border << 1);
    }
    pw = myPane->getDefaultWidth();
    return FXMAX(ww, pw);
}


FXint
MFXComboBoxIcon::getDefaultHeight() {
    FXint th, bh;
    th = myTextFieldIcon->getDefaultHeight();
    bh = myButton->getDefaultHeight();
    return FXMAX(th, bh) + (border << 1);
}


void
MFXComboBoxIcon::layout() {
    const FXint itemHeight = height - (border << 1);
    const FXint iconSize = myHaveIcons ? itemHeight : 0;
    const FXint buttonWidth = myButton->getDefaultWidth();
    const FXint textWidth = width - buttonWidth - iconSize - (border << 1);
    myIconLabel->position(border, border, iconSize, iconSize);
    myTextFieldIcon->position(border + iconSize, border, textWidth, itemHeight);
    myButton->position(border + textWidth + iconSize, border, buttonWidth, itemHeight);

    int size = -1;
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItemWidth(i) > size) {
            size = myList->getItemWidth(i);
        }
    }
    myPane->resize(size + 17, myPane->getDefaultHeight());
    flags &= ~FLAG_DIRTY;
}


FXbool
MFXComboBoxIcon::isEditable() const {
    return myTextFieldIcon->isEditable();
}


void
MFXComboBoxIcon::setEditable(FXbool edit) {
    myTextFieldIcon->setEditable(edit);
}


FXString
MFXComboBoxIcon::getText() const {
    return myTextFieldIcon->getText();
}


void
MFXComboBoxIcon::setNumColumns(FXint cols) {
    myTextFieldIcon->setNumColumns(cols);
}


FXint
MFXComboBoxIcon::getNumColumns() const {
    return myTextFieldIcon->getNumColumns();
}


FXint
MFXComboBoxIcon::getNumItems() const {
    return myList->getNumItems();
}


FXint
MFXComboBoxIcon::getNumVisible() const {
    return myList->getNumVisible();
}


void
MFXComboBoxIcon::setText(FXString text) {
    myTextFieldIcon->setText(text);
}


void
MFXComboBoxIcon::setNumVisible(FXint nvis) {
    myList->setNumVisible(nvis);
}


FXbool
MFXComboBoxIcon::isItemCurrent(FXint index) const {
    return myList->isItemCurrent(index);
}


void
MFXComboBoxIcon::setCurrentItem(FXint index, FXbool notify) {
    FXint current = myList->getCurrentItem();
    if (current != index) {
        myList->setCurrentItem(index);
        myList->makeItemVisible(index);
        if (0 <= index) {
            // cast MFXListItemIcon
            const MFXListItemIcon* item = dynamic_cast<MFXListItemIcon*>(myList->getItem(index));
            // set icon and background color
            if (item) {
                myTextFieldIcon->setText(item->getText());
                myTextFieldIcon->setBackColor(item->getBackGroundColor());
                myIconLabel->setIcon(item->getIcon());
                myIconLabel->setBackColor(item->getBackGroundColor());
            } else {
                myTextFieldIcon->resetTextField();
                myTextFieldIcon->setBackColor(FXRGB(255, 255, 255));
                myIconLabel->setIcon(nullptr);
                myIconLabel->setBackColor(FXRGB(255, 255, 255));
            }
        } else {
            myTextFieldIcon->resetTextField();
        }
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_COMMAND, message), (void*)getText().text());
        }
    }
}


FXint
MFXComboBoxIcon::getCurrentItem() const {
    return myList->getCurrentItem();
}


FXString
MFXComboBoxIcon::getItem(FXint index) const {
    return myList->getItem(index)->getText();
}


FXint
MFXComboBoxIcon::setIconItem(FXint index, const FXString& text, FXIcon* icon, FXColor bgColor, void* ptr) {
    if (index < 0 || myList->getNumItems() <= index) {
        fxerror("%s::setItem: index out of range.\n", getClassName());
    }
    myList->setItem(index, text, NULL, ptr);
    if (isItemCurrent(index)) {
        myTextFieldIcon->setText(text);
        myTextFieldIcon->setBackColor(bgColor);
        myIconLabel->setIcon(icon);
        myIconLabel->setBackColor(bgColor);
    }
    recalc();
    return index;
}


FXint
MFXComboBoxIcon::insertIconItem(FXint index, const FXString& text, FXIcon* icon, FXColor bgColor, void* ptr) {
    if (index < 0 || myList->getNumItems() < index) {
        fxerror("%s::insertItem: index out of range.\n", getClassName());
    }
    myList->insertItem(index, text, NULL, ptr);
    if (isItemCurrent(index)) {
        myTextFieldIcon->setText(text);
        myTextFieldIcon->setBackColor(bgColor);
        myIconLabel->setIcon(icon);
        myIconLabel->setBackColor(bgColor);
    }
    recalc();
    return index;
}


FXint
MFXComboBoxIcon::appendIconItem(const FXString& text, FXIcon* icon, FXColor bgColor, void* ptr) {
    FXint index = myList->appendItem(new MFXListItemIcon(text, icon, bgColor, ptr));
    if (isItemCurrent(getNumItems() - 1)) {
        myTextFieldIcon->setText(text);
        myTextFieldIcon->setBackColor(bgColor);
        myIconLabel->setIcon(icon);
        myIconLabel->setBackColor(bgColor);
    }
    recalc();
    return index;
}


bool
MFXComboBoxIcon::setItem(const FXString& text, FXIcon* icon) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        // cast MFXListItemIcon
        const MFXListItemIcon* item = dynamic_cast<MFXListItemIcon*>(myList->getItem(i));
        // set icon and background color
        if (item && (item->getText() == text) && (item->getIcon() == icon)) {
            myTextFieldIcon->setText(item->getText());
            myTextFieldIcon->setBackColor(item->getBackGroundColor());
            myIconLabel->setIcon(item->getIcon());
            myIconLabel->setBackColor(item->getBackGroundColor());
            myTextFieldIcon->setTextColor(FXRGB(0, 0, 0));
            return true;
        }
    }
    return false;
}


void
MFXComboBoxIcon::setCustomText(const FXString text) {
    myTextFieldIcon->setText(text);
    myTextFieldIcon->setTextColor(FXRGB(128, 128, 128));
}


FXint
MFXComboBoxIcon::prependItem(const FXString& text, void* ptr) {
    FXint index = myList->prependItem(text, NULL, ptr);
    if (isItemCurrent(0)) {
        myTextFieldIcon->setText(text);
        myTextFieldIcon->setBackColor(FXRGB(255, 255, 255));
        myIconLabel->setIcon(nullptr);
        myIconLabel->setBackColor(FXRGB(255, 255, 255));
    }
    recalc();
    return index;
}


FXint
MFXComboBoxIcon::moveItem(FXint newindex, FXint oldindex) {
    if (newindex < 0 || myList->getNumItems() <= newindex || oldindex < 0 || myList->getNumItems() <= oldindex) {
        fxerror("%s::moveItem: index out of range.\n", getClassName());
    }
    FXint current = myList->getCurrentItem();
    myList->moveItem(newindex, oldindex);
    if (current != myList->getCurrentItem()) {
        current = myList->getCurrentItem();
        if (0 <= current) {
            myTextFieldIcon->setText(myList->getItemText(current));
        } else {
            myTextFieldIcon->setText(" ");
        }
        myIconLabel->setIcon(nullptr);
        myIconLabel->setBackColor(FXRGB(255, 255, 255));
    }
    recalc();
    return newindex;
}


void
MFXComboBoxIcon::removeItem(FXint index) {
    FXint current = myList->getCurrentItem();
    myList->removeItem(index);
    if (index == current) {
        current = myList->getCurrentItem();
        if (0 <= current) {
            myTextFieldIcon->setText(myList->getItemText(current));
        } else {
            myTextFieldIcon->setText(FXString::null);
        }
        myIconLabel->setIcon(nullptr);
        myIconLabel->setBackColor(FXRGB(255, 255, 255));
    }
    recalc();
}


void
MFXComboBoxIcon::clearItems() {
    myTextFieldIcon->resetTextField();
    myList->clearItems();
    recalc();
}


FXint
MFXComboBoxIcon::findItem(const FXString& text, FXint start, FXuint flgs) const {
    return myList->findItem(text, start, flgs);
}


FXint
MFXComboBoxIcon::findItemByData(const void* ptr, FXint start, FXuint flgs) const {
    return myList->findItemByData(ptr, start, flgs);
}


FXString
MFXComboBoxIcon::getItemText(FXint index) const {
    return myList->getItemText(index);
}


void
MFXComboBoxIcon::setItemData(FXint index, void* ptr) const {
    myList->setItemData(index, ptr);
}


void*
MFXComboBoxIcon::getItemData(FXint index) const {
    return myList->getItemData(index);
}


FXbool
MFXComboBoxIcon::isPaneShown() const {
    return myPane->shown();
}


void
MFXComboBoxIcon::setFont(FXFont* fnt) {
    if (!fnt) {
        fxerror("%s::setFont: NULL font specified.\n", getClassName());
    }
    myTextFieldIcon->setFont(fnt);
    myList->setFont(fnt);
    recalc();
}


FXFont*
MFXComboBoxIcon::getFont() const {
    return myTextFieldIcon->getFont();
}


void
MFXComboBoxIcon::setComboStyle(FXuint mode) {
    FXuint opts = (options & ~COMBOBOX_MASK) | (mode & COMBOBOX_MASK);
    if (opts != options) {
        options = opts;
        if (options & COMBOBOX_STATIC) {
            myTextFieldIcon->setEditable(FALSE);                                // Non-editable
            myList->setScrollStyle(SCROLLERS_TRACK | HSCROLLING_OFF);   // No scrolling
        } else {
            myTextFieldIcon->setEditable(TRUE);                                 // Editable
            myList->setScrollStyle(SCROLLERS_TRACK | HSCROLLER_NEVER);  // Scrollable, but no scrollbar
        }
        recalc();
    }
}


FXuint
MFXComboBoxIcon::getComboStyle() const {
    return (options & COMBOBOX_MASK);
}


void
MFXComboBoxIcon::setJustify(FXuint style) {
    myTextFieldIcon->setJustify(style);
}


FXuint
MFXComboBoxIcon::getJustify() const {
    return myTextFieldIcon->getJustify();
}


void
MFXComboBoxIcon::setBackColor(FXColor clr) {
    myTextFieldIcon->setBackColor(clr);
    myIconLabel->setBackColor(clr);
    myList->setBackColor(clr);
}


FXColor
MFXComboBoxIcon::getBackColor() const {
    return myTextFieldIcon->getBackColor();
}


void
MFXComboBoxIcon::setTextColor(FXColor clr) {
    myTextFieldIcon->setTextColor(clr);
    myList->setTextColor(clr);
}


FXColor
MFXComboBoxIcon::getTextColor() const {
    return myTextFieldIcon->getTextColor();
}


void
MFXComboBoxIcon::setSelBackColor(FXColor clr) {
    myTextFieldIcon->setSelBackColor(clr);
    myList->setSelBackColor(clr);
}


FXColor
MFXComboBoxIcon::getSelBackColor() const {
    return myTextFieldIcon->getSelBackColor();
}


void
MFXComboBoxIcon::setSelTextColor(FXColor clr) {
    myTextFieldIcon->setSelTextColor(clr);
    myList->setSelTextColor(clr);
}


FXColor
MFXComboBoxIcon::getSelTextColor() const {
    return myTextFieldIcon->getSelTextColor();
}


void
MFXComboBoxIcon::sortItems() {
    myList->sortItems();
}


FXListSortFunc
MFXComboBoxIcon::getSortFunc() const {
    return myList->getSortFunc();
}


void
MFXComboBoxIcon::setSortFunc(FXListSortFunc func) {
    myList->setSortFunc(func);
}


void
MFXComboBoxIcon::setHelpText(const FXString& txt) {
    myTextFieldIcon->setHelpText(txt);
}


const FXString&
MFXComboBoxIcon::getHelpText() const {
    return myTextFieldIcon->getHelpText();
}


void
MFXComboBoxIcon::setTipText(const FXString& txt) {
    myTextFieldIcon->setTipText(txt);
}


const FXString&
MFXComboBoxIcon::getTipText() const {
    return myTextFieldIcon->getTipText();
}


long
MFXComboBoxIcon::onUpdFmText(FXObject*, FXSelector, void*) {
    return target && !isPaneShown() && target->tryHandle(this, FXSEL(SEL_UPDATE, message), NULL);
}


long
MFXComboBoxIcon::onFwdToText(FXObject* sender, FXSelector sel, void* ptr) {
    return myTextFieldIcon->handle(sender, sel, ptr);
}


long
MFXComboBoxIcon::onListClicked(FXObject*, FXSelector sel, void* ptr) {
    myButton->handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), NULL);
    if (FXSELTYPE(sel) == SEL_COMMAND) {
        // cast MFXListItemIcon
        const MFXListItemIcon* item = dynamic_cast<MFXListItemIcon*>(myList->getItem((FXint)(FXival)ptr));
        // set icon and background color
        if (item) {
            myTextFieldIcon->setText(item->getText());
            myTextFieldIcon->setBackColor(item->getBackGroundColor());
            myIconLabel->setIcon(item->getIcon());
            myIconLabel->setBackColor(item->getBackGroundColor());
        }
        if (!(options & COMBOBOX_STATIC)) {
            // Select if editable
            myTextFieldIcon->selectAll();
        }
        if (target) {
            target->tryHandle(this, FXSEL(SEL_COMMAND, message), (void*)getText().text());
        }
    }
    return 1;
}


long
MFXComboBoxIcon::onTextButton(FXObject*, FXSelector, void*) {
    if (options & COMBOBOX_STATIC) {
        // Post the myList
        myButton->handle(this, FXSEL(SEL_COMMAND, ID_POST), NULL);
        return 1;
    }
    return 0;
}


long
MFXComboBoxIcon::onTextChanged(FXObject*, FXSelector, void* ptr) {
    return target && target->tryHandle(this, FXSEL(SEL_CHANGED, message), ptr);
}


long
MFXComboBoxIcon::onTextCommand(FXObject*, FXSelector, void* ptr) {
    FXint index = myList->getCurrentItem();
    if (!(options & COMBOBOX_STATIC)) {
        switch (options & COMBOBOX_INS_MASK) {
            case COMBOBOX_REPLACE:
                if (0 <= index) {
                    setIconItem(index, (FXchar*)ptr, nullptr, FXRGB(255, 255, 255), getItemData(index));
                }
                break;
            case COMBOBOX_INSERT_BEFORE:
                if (0 <= index) {
                    insertIconItem(index, (FXchar*)ptr);
                }
                break;
            case COMBOBOX_INSERT_AFTER:
                if (0 <= index) {
                    insertIconItem(index + 1, (FXchar*)ptr);
                }
                break;
            case COMBOBOX_INSERT_FIRST:
                insertIconItem(0, (FXchar*)ptr);
                break;
            case COMBOBOX_INSERT_LAST:
                appendIconItem((FXchar*)ptr);
                break;
        }
    }
    // reset icon and color
    myTextFieldIcon->setBackColor(FXRGB(255, 255, 255));
    myIconLabel->setIcon(nullptr);
    myIconLabel->setBackColor(FXRGB(255, 255, 255));
    return target && target->tryHandle(this, FXSEL(SEL_COMMAND, message), ptr);
}


long
MFXComboBoxIcon::onFocusSelf(FXObject* sender, FXSelector, void* ptr) {
    return myTextFieldIcon->handle(sender, FXSEL(SEL_FOCUS_SELF, 0), ptr);
}


long
MFXComboBoxIcon::onFocusUp(FXObject*, FXSelector, void*) {
    if (isEnabled()) {
        FXint index = getCurrentItem();
        if (index < 0) {
            index = getNumItems() - 1;
        } else if (0 < index) {
            index--;
        }
        if (0 <= index && index < getNumItems()) {
            setCurrentItem(index, TRUE);
        }
        return 1;
    }
    return 0;
}


long
MFXComboBoxIcon::onFocusDown(FXObject*, FXSelector, void*) {
    if (isEnabled()) {
        FXint index = getCurrentItem();
        if (index < 0) {
            index = 0;
        } else if (index < getNumItems() - 1) {
            index++;
        }
        if (0 <= index && index < getNumItems()) {
            setCurrentItem(index, TRUE);
        }
        return 1;
    }
    return 0;
}


long MFXComboBoxIcon::onMouseWheel(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    if (isEnabled()) {
        FXint index = getCurrentItem();
        if (event->code < 0) {
            if (index < 0) {
                index = 0;
            } else if (index < getNumItems() - 1) {
                index++;
            }
        } else if (event->code > 0) {
            if (index < 0) {
                index = getNumItems() - 1;
            } else if (0 < index) {
                index--;
            }
        }
        if (0 <= index && index < getNumItems()) {
            setCurrentItem(index, TRUE);
        }
        return 1;
    }
    return 0;
}


MFXComboBoxIcon::MFXComboBoxIcon() :
    myHaveIcons(false) {}
