/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2021 German Aerospace Center (DLR) and others.
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
/// @file    MFXIconComboBox.cpp
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    2018-12-19
///
//
/****************************************************************************/

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <config.h>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#endif

#include "MFXIconComboBox.h"


#define SIDE_SPACING        6   // Left or right spacing between items
#define ICON_SPACING        4   // Spacing between icon and label
#define COMBOBOX_INS_MASK   (COMBOBOX_REPLACE|COMBOBOX_INSERT_BEFORE|COMBOBOX_INSERT_AFTER|COMBOBOX_INSERT_FIRST|COMBOBOX_INSERT_LAST)
#define COMBOBOX_MASK       (COMBOBOX_STATIC|COMBOBOX_INS_MASK)

// Map
FXDEFMAP(MFXIconComboBox) MFXIconComboBoxMap[] = {
    FXMAPFUNC(SEL_FOCUS_UP,         0,                              MFXIconComboBox::onFocusUp),
    FXMAPFUNC(SEL_FOCUS_DOWN,       0,                              MFXIconComboBox::onFocusDown),
    FXMAPFUNC(SEL_FOCUS_SELF,       0,                              MFXIconComboBox::onFocusSelf),
    FXMAPFUNC(SEL_UPDATE,           MFXIconComboBox::ID_TEXT,       MFXIconComboBox::onUpdFmText),
    FXMAPFUNC(SEL_CLICKED,          MFXIconComboBox::ID_LIST,       MFXIconComboBox::onListClicked),
    FXMAPFUNC(SEL_COMMAND,          MFXIconComboBox::ID_LIST,       MFXIconComboBox::onListClicked),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,  MFXIconComboBox::ID_TEXT,       MFXIconComboBox::onTextButton),
    FXMAPFUNC(SEL_MOUSEWHEEL,       MFXIconComboBox::ID_TEXT,       MFXIconComboBox::onMouseWheel),
    FXMAPFUNC(SEL_CHANGED,          MFXIconComboBox::ID_TEXT,       MFXIconComboBox::onTextChanged),
    FXMAPFUNC(SEL_COMMAND,          MFXIconComboBox::ID_TEXT,       MFXIconComboBox::onTextCommand),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_SETVALUE,          MFXIconComboBox::onFwdToText),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_SETINTVALUE,       MFXIconComboBox::onFwdToText),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_SETREALVALUE,      MFXIconComboBox::onFwdToText),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_SETSTRINGVALUE,    MFXIconComboBox::onFwdToText),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_GETINTVALUE,       MFXIconComboBox::onFwdToText),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_GETREALVALUE,      MFXIconComboBox::onFwdToText),
    FXMAPFUNC(SEL_COMMAND,          FXWindow::ID_GETSTRINGVALUE,    MFXIconComboBox::onFwdToText),
};


// Object implementation
FXIMPLEMENT(MFXIconComboBox,    FXPacker,   MFXIconComboBoxMap, ARRAYNUMBER(MFXIconComboBoxMap))
FXIMPLEMENT(MFXListItem,        FXListItem, nullptr,            0)


MFXListItem::MFXListItem(const FXString& text, FXIcon* ic, FXColor _bgColor, void* ptr):
    FXListItem(text, ic, ptr),
    myBackGroundColor(_bgColor) {
}


void
MFXListItem::draw(const FXList* list, FXDC& dc, FXint xx, FXint yy, FXint ww, FXint hh) {
    // almost the same code as FXListItem::draw except for using custom background color
    FXFont* font = list->getFont();
    FXint ih = 0, th = 0;
    if (icon) {
        ih = icon->getHeight();
    }
    if (!label.empty()) {
        th = font->getFontHeight();
    }
    if (isSelected()) {
        dc.setForeground(list->getSelBackColor());
    } else if (myBackGroundColor != FXRGBA(0, 0, 0, 0)) {
        dc.setForeground(myBackGroundColor);
    } else {
        dc.setForeground(list->getBackColor());
    }
    dc.fillRectangle(xx, yy, ww, hh);
    if (hasFocus()) {
        dc.drawFocusRectangle(xx + 1, yy + 1, ww - 2, hh - 2);
    }
    xx += SIDE_SPACING / 2;
    if (icon) {
        dc.drawIcon(icon, xx, yy + (hh - ih) / 2);
        xx += ICON_SPACING + icon->getWidth();
    }
    if (!label.empty()) {
        dc.setFont(font);
        if (!isEnabled()) {
            dc.setForeground(makeShadowColor(list->getBackColor()));
        } else if (isSelected()) {
            dc.setForeground(list->getSelTextColor());
        } else {
            dc.setForeground(list->getTextColor());
        }
        dc.drawText(xx, yy + (hh - th) / 2 + font->getFontAscent(), label);
    }
}


MFXListItem::MFXListItem() :
    FXListItem("", nullptr),
    myBackGroundColor(FXRGB(255, 255, 255)) {
}


/********************************************************************/


MFXIconComboBox::MFXIconComboBox(FXComposite* p, FXint cols, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb):
    FXPacker(p, opts, x, y, w, h, 0, 0, 0, 0, 0, 0) {
    flags |= FLAG_ENABLED;
    target = tgt;
    message = sel;
    field = new FXTextField(this, cols, this, MFXIconComboBox::ID_TEXT, 0, 0, 0, 0, 0, pl, pr, pt, pb);
    if (options & COMBOBOX_STATIC) {
        field->setEditable(FALSE);
    }
    pane = new FXPopup(this, FRAME_LINE);
    list = new FXList(pane, this, MFXIconComboBox::ID_LIST, LIST_BROWSESELECT | LIST_AUTOSELECT | LAYOUT_FILL_X | LAYOUT_FILL_Y | SCROLLERS_TRACK | HSCROLLER_NEVER);
    if (options & COMBOBOX_STATIC) {
        list->setScrollStyle(SCROLLERS_TRACK | HSCROLLING_OFF);
    }
    button = new FXMenuButton(this, FXString::null, NULL, pane, FRAME_RAISED | FRAME_THICK | MENUBUTTON_DOWN | MENUBUTTON_ATTACH_RIGHT, 0, 0, 0, 0, 0, 0, 0, 0);
    button->setXOffset(border);
    button->setYOffset(border);
    flags &= ~FLAG_UPDATE;  // Never GUI update
}


void
MFXIconComboBox::create() {
    FXPacker::create();
    pane->create();
}


void
MFXIconComboBox::detach() {
    FXPacker::detach();
    pane->detach();
}


void
MFXIconComboBox::destroy() {
    pane->destroy();
    FXPacker::destroy();
}


void
MFXIconComboBox::enable() {
    if (!isEnabled()) {
        FXPacker::enable();
        field->enable();
        button->enable();
    }
}


void
MFXIconComboBox::disable() {
    if (isEnabled()) {
        FXPacker::disable();
        field->disable();
        button->disable();
    }
}


FXint
MFXIconComboBox::getDefaultWidth() {
    FXint ww, pw;
    ww = field->getDefaultWidth() + button->getDefaultWidth() + (border << 1);
    pw = pane->getDefaultWidth();
    return FXMAX(ww, pw);
}


FXint
MFXIconComboBox::getDefaultHeight() {
    FXint th, bh;
    th = field->getDefaultHeight();
    bh = button->getDefaultHeight();
    return FXMAX(th, bh) + (border << 1);
}


void
MFXIconComboBox::layout() {
    FXint buttonWidth, textWidth, itemHeight;
    itemHeight = height - (border << 1);
    buttonWidth = button->getDefaultWidth();
    textWidth = width - buttonWidth - (border << 1);
    field->position(border, border, textWidth, itemHeight);
    button->position(border + textWidth, border, buttonWidth, itemHeight);
    pane->resize(width, pane->getDefaultHeight());
    flags &= ~FLAG_DIRTY;
}


long
MFXIconComboBox::onUpdFmText(FXObject*, FXSelector, void*) {
    return target && !isPaneShown() && target->tryHandle(this, FXSEL(SEL_UPDATE, message), NULL);
}


long
MFXIconComboBox::onFwdToText(FXObject* sender, FXSelector sel, void* ptr) {
    return field->handle(sender, sel, ptr);
}


long
MFXIconComboBox::onListClicked(FXObject*, FXSelector sel, void* ptr) {
    button->handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), NULL);
    if (FXSELTYPE(sel) == SEL_COMMAND) {
        field->setText(list->getItemText((FXint)(FXival)ptr));
        if (!(options & COMBOBOX_STATIC)) {
            // Select if editable
            field->selectAll();
        }
        if (target) {
            target->tryHandle(this, FXSEL(SEL_COMMAND, message), (void*)getText().text());
        }
    }
    return 1;
}


long
MFXIconComboBox::onTextButton(FXObject*, FXSelector, void*) {
    if (options & COMBOBOX_STATIC) {
        // Post the list
        button->handle(this, FXSEL(SEL_COMMAND, ID_POST), NULL);
        return 1;
    }
    return 0;
}


long
MFXIconComboBox::onTextChanged(FXObject*, FXSelector, void* ptr) {
    return target && target->tryHandle(this, FXSEL(SEL_CHANGED, message), ptr);
}


long
MFXIconComboBox::onTextCommand(FXObject*, FXSelector, void* ptr) {
    FXint index = list->getCurrentItem();
    if (!(options & COMBOBOX_STATIC)) {
        switch (options & COMBOBOX_INS_MASK) {
            case COMBOBOX_REPLACE:
                if (0 <= index) {
                    setItem(index, (FXchar*)ptr, getItemData(index));
                }
                break;
            case COMBOBOX_INSERT_BEFORE:
                if (0 <= index) {
                    insertItem(index, (FXchar*)ptr);
                }
                break;
            case COMBOBOX_INSERT_AFTER:
                if (0 <= index) {
                    insertItem(index + 1, (FXchar*)ptr);
                }
                break;
            case COMBOBOX_INSERT_FIRST:
                insertItem(0, (FXchar*)ptr);
                break;
            case COMBOBOX_INSERT_LAST:
                appendItem((FXchar*)ptr);
                break;
        }
    }
    return target && target->tryHandle(this, FXSEL(SEL_COMMAND, message), ptr);
}


long
MFXIconComboBox::onFocusSelf(FXObject* sender, FXSelector, void* ptr) {
    return field->handle(sender, FXSEL(SEL_FOCUS_SELF, 0), ptr);
}


long
MFXIconComboBox::onFocusUp(FXObject*, FXSelector, void*) {
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
MFXIconComboBox::onFocusDown(FXObject*, FXSelector, void*) {
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


// Mouse wheel
long MFXIconComboBox::onMouseWheel(FXObject*, FXSelector, void* ptr) {
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


// Return true if editable
FXbool MFXIconComboBox::isEditable() const {
    return field->isEditable();
}


// Set widget is editable or not
void MFXIconComboBox::setEditable(FXbool edit) {
    field->setEditable(edit);
}


// Set text
void MFXIconComboBox::setText(const FXString& text) {
    field->setText(text);
}


// Obtain text
FXString MFXIconComboBox::getText() const {
    return field->getText();
}


// Set number of text columns
void MFXIconComboBox::setNumColumns(FXint cols) {
    field->setNumColumns(cols);
}


// Get number of text columns
FXint MFXIconComboBox::getNumColumns() const {
    return field->getNumColumns();
}


// Get number of items
FXint MFXIconComboBox::getNumItems() const {
    return list->getNumItems();
}


// Get number of visible items
FXint MFXIconComboBox::getNumVisible() const {
    return list->getNumVisible();
}


// Set number of visible items
void MFXIconComboBox::setNumVisible(FXint nvis) {
    list->setNumVisible(nvis);
}


// Is item current
FXbool MFXIconComboBox::isItemCurrent(FXint index) const {
    return list->isItemCurrent(index);
}


// Change current item
void MFXIconComboBox::setCurrentItem(FXint index, FXbool notify) {
    FXint current = list->getCurrentItem();
    if (current != index) {
        list->setCurrentItem(index);
        list->makeItemVisible(index);
        if (0 <= index) {
            setText(list->getItemText(index));
        } else {
            setText(FXString::null);
        }
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_COMMAND, message), (void*)getText().text());
        }
    }
}


// Get current item
FXint MFXIconComboBox::getCurrentItem() const {
    return list->getCurrentItem();
}


// Retrieve item
FXString MFXIconComboBox::getItem(FXint index) const {
    return list->getItem(index)->getText();
}


// Replace text of item at index
FXint MFXIconComboBox::setItem(FXint index, const FXString& text, void* ptr) {
    if (index < 0 || list->getNumItems() <= index) {
        fxerror("%s::setItem: index out of range.\n", getClassName());
    }
    list->setItem(index, text, NULL, ptr);
    if (isItemCurrent(index)) {
        field->setText(text);
    }
    recalc();
    return index;
}


// Fill list by appending items from array of strings
FXint MFXIconComboBox::fillItems(const FXchar** strings) {
    FXint numberofitems = list->getNumItems();
    FXint n = list->fillItems(strings);
    if (numberofitems <= list->getCurrentItem()) {
        field->setText(list->getItemText(list->getCurrentItem()));
    }
    recalc();
    return n;
}


// Fill list by appending items from newline separated strings
FXint MFXIconComboBox::fillItems(const FXString& strings) {
    FXint numberofitems = list->getNumItems();
    FXint n = list->fillItems(strings);
    if (numberofitems <= list->getCurrentItem()) {
        field->setText(list->getItemText(list->getCurrentItem()));
    }
    recalc();
    return n;
}


// Insert item at index
FXint MFXIconComboBox::insertItem(FXint index, const FXString& text, void* ptr) {
    if (index < 0 || list->getNumItems() < index) {
        fxerror("%s::insertItem: index out of range.\n", getClassName());
    }
    list->insertItem(index, text, NULL, ptr);
    if (isItemCurrent(index)) {
        field->setText(text);
    }
    recalc();
    return index;
}



// Append item
FXint MFXIconComboBox::appendItem(const FXString& text, void* ptr) {
    FXint index = list->appendItem(text, NULL, ptr);
    if (isItemCurrent(getNumItems() - 1)) {
        field->setText(text);
    }
    recalc();
    return index;
}


FXint
MFXIconComboBox::appendIconItem(const FXString& text, FXIcon* icon, FXColor bgColor,  void* ptr) {
    FXint index  =  list->appendItem(new MFXListItem(text, icon, bgColor, ptr));
    if (isItemCurrent(getNumItems() - 1)) {
        field->setText(text);
    }
    recalc();
    return index;
}

// Prepend item
FXint MFXIconComboBox::prependItem(const FXString& text, void* ptr) {
    FXint index = list->prependItem(text, NULL, ptr);
    if (isItemCurrent(0)) {
        field->setText(text);
    }
    recalc();
    return index;
}


// Move item from oldindex to newindex
FXint MFXIconComboBox::moveItem(FXint newindex, FXint oldindex) {
    if (newindex < 0 || list->getNumItems() <= newindex || oldindex < 0 || list->getNumItems() <= oldindex) {
        fxerror("%s::moveItem: index out of range.\n", getClassName());
    }
    FXint current = list->getCurrentItem();
    list->moveItem(newindex, oldindex);
    if (current != list->getCurrentItem()) {
        current = list->getCurrentItem();
        if (0 <= current) {
            field->setText(list->getItemText(current));
        } else {
            field->setText(" ");
        }
    }
    recalc();
    return newindex;
}


// Remove given item
void MFXIconComboBox::removeItem(FXint index) {
    FXint current = list->getCurrentItem();
    list->removeItem(index);
    if (index == current) {
        current = list->getCurrentItem();
        if (0 <= current) {
            field->setText(list->getItemText(current));
        } else {
            field->setText(FXString::null);
        }
    }
    recalc();
}


// Remove all items
void MFXIconComboBox::clearItems() {
    field->setText(FXString::null);
    list->clearItems();
    recalc();
}


// Get item by name
FXint MFXIconComboBox::findItem(const FXString& text, FXint start, FXuint flgs) const {
    return list->findItem(text, start, flgs);
}


// Get item by data
FXint MFXIconComboBox::findItemByData(const void* ptr, FXint start, FXuint flgs) const {
    return list->findItemByData(ptr, start, flgs);
}


// Set item text
void MFXIconComboBox::setItemText(FXint index, const FXString& txt) {
    if (isItemCurrent(index)) {
        setText(txt);
    }
    list->setItemText(index, txt);
    recalc();
}


// Get item text
FXString MFXIconComboBox::getItemText(FXint index) const {
    return list->getItemText(index);
}


// Set item data
void MFXIconComboBox::setItemData(FXint index, void* ptr) const {
    list->setItemData(index, ptr);
}


// Get item data
void* MFXIconComboBox::getItemData(FXint index) const {
    return list->getItemData(index);
}


// Is the pane shown
FXbool MFXIconComboBox::isPaneShown() const {
    return pane->shown();
}


// Set font
void MFXIconComboBox::setFont(FXFont* fnt) {
    if (!fnt) {
        fxerror("%s::setFont: NULL font specified.\n", getClassName());
    }
    field->setFont(fnt);
    list->setFont(fnt);
    recalc();
}


// Obtain font
FXFont* MFXIconComboBox::getFont() const {
    return field->getFont();
}


// Change combobox style
void MFXIconComboBox::setComboStyle(FXuint mode) {
    FXuint opts = (options & ~COMBOBOX_MASK) | (mode & COMBOBOX_MASK);
    if (opts != options) {
        options = opts;
        if (options & COMBOBOX_STATIC) {
            field->setEditable(FALSE);                                // Non-editable
            list->setScrollStyle(SCROLLERS_TRACK | HSCROLLING_OFF);   // No scrolling
        } else {
            field->setEditable(TRUE);                                 // Editable
            list->setScrollStyle(SCROLLERS_TRACK | HSCROLLER_NEVER);  // Scrollable, but no scrollbar
        }
        recalc();
    }
}


// Get combobox style
FXuint MFXIconComboBox::getComboStyle() const {
    return (options & COMBOBOX_MASK);
}


// Set text justify style
void MFXIconComboBox::setJustify(FXuint style) {
    field->setJustify(style);
}


// Get text justify style
FXuint MFXIconComboBox::getJustify() const {
    return field->getJustify();
}


// Set window background color
void MFXIconComboBox::setBackColor(FXColor clr) {
    field->setBackColor(clr);
    list->setBackColor(clr);
}


// Get background color
FXColor MFXIconComboBox::getBackColor() const {
    return field->getBackColor();
}


// Set text color
void MFXIconComboBox::setTextColor(FXColor clr) {
    field->setTextColor(clr);
    list->setTextColor(clr);
}


// Return text color
FXColor MFXIconComboBox::getTextColor() const {
    return field->getTextColor();
}


// Set select background color
void MFXIconComboBox::setSelBackColor(FXColor clr) {
    field->setSelBackColor(clr);
    list->setSelBackColor(clr);
}


// Return selected background color
FXColor MFXIconComboBox::getSelBackColor() const {
    return field->getSelBackColor();
}


// Set selected text color
void MFXIconComboBox::setSelTextColor(FXColor clr) {
    field->setSelTextColor(clr);
    list->setSelTextColor(clr);
}


// Return selected text color
FXColor MFXIconComboBox::getSelTextColor() const {
    return field->getSelTextColor();
}


// Sort items using current sort function
void MFXIconComboBox::sortItems() {
    list->sortItems();
}


// Return sort function
FXListSortFunc MFXIconComboBox::getSortFunc() const {
    return list->getSortFunc();
}


// Change sort function
void MFXIconComboBox::setSortFunc(FXListSortFunc func) {
    list->setSortFunc(func);
}


// Set help text
void MFXIconComboBox::setHelpText(const FXString& txt) {
    field->setHelpText(txt);
}


const FXString&
MFXIconComboBox::getHelpText() const {
    return field->getHelpText();
}


void
MFXIconComboBox::setTipText(const FXString& txt) {
    field->setTipText(txt);
}


const FXString&
MFXIconComboBox::getTipText() const {
    return field->getTipText();
}


void
MFXIconComboBox::save(FXStream& store) const {
    FXPacker::save(store);
    store << field;
    store << button;
    store << list;
    store << pane;
}


void
MFXIconComboBox::load(FXStream& store) {
    FXPacker::load(store);
    store >> field;
    store >> button;
    store >> list;
    store >> pane;
}


MFXIconComboBox::~MFXIconComboBox() {
    delete pane;
    pane = (FXPopup*) - 1L;
    field = (FXTextField*) - 1L;
    button = (FXMenuButton*) - 1L;
    list = (FXList*) - 1L;
}
