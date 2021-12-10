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
#define COMBOBOX_INS_MASK   (COMBOBOX_REPLACE | COMBOBOX_INSERT_BEFORE | COMBOBOX_INSERT_AFTER | COMBOBOX_INSERT_FIRST | COMBOBOX_INSERT_LAST)
#define COMBOBOX_MASK       (COMBOBOX_STATIC | COMBOBOX_INS_MASK)

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


MFXListItem::MFXListItem(const FXString& text, FXIcon* ic, FXColor backGroundColor, void* ptr):
    FXListItem(text, ic, ptr),
    myBackGroundColor(backGroundColor) {
}


void
MFXListItem::draw(const FXList* myList, FXDC& dc, FXint xx, FXint yy, FXint ww, FXint hh) {
    // almost the same code as FXListItem::draw except for using custom background color
    FXFont* font = myList->getFont();
    FXint ih = 0, th = 0;
    if (icon) {
        ih = icon->getHeight();
    }
    if (!label.empty()) {
        th = font->getFontHeight();
    }
    if (isSelected()) {
        dc.setForeground(myList->getSelBackColor());
    } else {
        dc.setForeground(myBackGroundColor);
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
            dc.setForeground(makeShadowColor(myList->getBackColor()));
        } else if (isSelected()) {
            dc.setForeground(myList->getSelTextColor());
        } else {
            dc.setForeground(myList->getTextColor());
        }
        dc.drawText(xx, yy + (hh - th) / 2 + font->getFontAscent(), label);
    }
}

const FXColor&
MFXListItem::getBackGroundColor() const {
    return myBackGroundColor;
}


MFXListItem::MFXListItem() :
    FXListItem("", nullptr),
    myBackGroundColor(FXRGB(0, 0, 0)) {
}


MFXIconComboBox::MFXIconComboBox(FXComposite* p, FXint cols, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h, FXint pl, FXint pr, FXint pt, FXint pb):
    FXPacker(p, opts, x, y, w, h, 0, 0, 0, 0, 0, 0) {
    flags |= FLAG_ENABLED;
    target = tgt;
    message = sel;
    myIconLabel = new FXLabel(this, "", nullptr, 0, 0, 0, 0, 0, pl, pr, pt, pb);
    myTextFieldIcon = new MFXTextFieldIcon(this, cols, this, MFXIconComboBox::ID_TEXT, 0, 0, 0, 0, 0, pl, pr, pt, pb);
    if (options & COMBOBOX_STATIC) {
        myTextFieldIcon->setEditable(FALSE);
    }
    myPane = new FXPopup(this, FRAME_LINE);
    myList = new FXList(myPane, this, MFXIconComboBox::ID_LIST, LIST_BROWSESELECT | LIST_AUTOSELECT | LAYOUT_FILL_X | LAYOUT_FILL_Y | SCROLLERS_TRACK | HSCROLLER_NEVER);
    if (options & COMBOBOX_STATIC) {
        myList->setScrollStyle(SCROLLERS_TRACK | HSCROLLING_OFF);
    }
    myButton = new FXMenuButton(this, FXString::null, NULL, myPane, FRAME_RAISED | FRAME_THICK | MENUBUTTON_DOWN | MENUBUTTON_ATTACH_RIGHT, 0, 0, 0, 0, 0, 0, 0, 0);
    myButton->setXOffset(border);
    myButton->setYOffset(border);
    flags &= ~FLAG_UPDATE;  // Never GUI update
}


MFXIconComboBox::~MFXIconComboBox() {
    delete myPane;
    myPane = (FXPopup*) - 1L;
    myIconLabel = (FXLabel*) - 1L;
    myTextFieldIcon = (MFXTextFieldIcon*) - 1L;
    myButton = (FXMenuButton*) - 1L;
    myList = (FXList*) - 1L;
}


void
MFXIconComboBox::create() {
    FXPacker::create();
    myPane->create();
}


void
MFXIconComboBox::detach() {
    FXPacker::detach();
    myPane->detach();
}


void
MFXIconComboBox::destroy() {
    myPane->destroy();
    FXPacker::destroy();
}


void
MFXIconComboBox::enable() {
    if (!isEnabled()) {
        FXPacker::enable();
        myIconLabel->enable();
        myTextFieldIcon->enable();
        myButton->enable();
    }
}


void
MFXIconComboBox::disable() {
    if (isEnabled()) {
        FXPacker::disable();
        myIconLabel->disable();
        myTextFieldIcon->disable();
        myButton->disable();
    }
}


FXint
MFXIconComboBox::getDefaultWidth() {
    FXint ww, pw;
    ww = myIconLabel->getDefaultWidth() + myTextFieldIcon->getDefaultWidth() + myButton->getDefaultWidth() + (border << 1);
    pw = myPane->getDefaultWidth();
    return FXMAX(ww, pw);
}


FXint
MFXIconComboBox::getDefaultHeight() {
    FXint th, bh;
    th = myTextFieldIcon->getDefaultHeight();
    bh = myButton->getDefaultHeight();
    return FXMAX(th, bh) + (border << 1);
}


void
MFXIconComboBox::layout() {
    FXint buttonWidth, textWidth, itemHeight, iconSize;
    itemHeight = height - (border << 1);
    iconSize = itemHeight;
    buttonWidth = myButton->getDefaultWidth();
    textWidth = width - buttonWidth - iconSize - (border << 1);
    myIconLabel->position(border, border, iconSize, iconSize);
    myTextFieldIcon->position(border + iconSize, border, textWidth, itemHeight);
    myButton->position(border + textWidth + iconSize, border, buttonWidth, itemHeight);
    myPane->resize(width, myPane->getDefaultHeight());
    flags &= ~FLAG_DIRTY;
}


FXbool
MFXIconComboBox::isEditable() const {
    return myTextFieldIcon->isEditable();
}


void
MFXIconComboBox::setEditable(FXbool edit) {
    myTextFieldIcon->setEditable(edit);
}


FXString
MFXIconComboBox::getText() const {
    return myTextFieldIcon->getText();
}


void
MFXIconComboBox::setNumColumns(FXint cols) {
    myTextFieldIcon->setNumColumns(cols);
}


FXint
MFXIconComboBox::getNumColumns() const {
    return myTextFieldIcon->getNumColumns();
}


FXint
MFXIconComboBox::getNumItems() const {
    return myList->getNumItems();
}


FXint
MFXIconComboBox::getNumVisible() const {
    return myList->getNumVisible();
}


void
MFXIconComboBox::setNumVisible(FXint nvis) {
    myList->setNumVisible(nvis);
}


FXbool
MFXIconComboBox::isItemCurrent(FXint index) const {
    return myList->isItemCurrent(index);
}


void
MFXIconComboBox::setCurrentItem(FXint index, FXbool notify) {
    FXint current = myList->getCurrentItem();
    if (current != index) {
        myList->setCurrentItem(index);
        myList->makeItemVisible(index);
        if (0 <= index) {
            // cast MFXListItem
            const MFXListItem* item = dynamic_cast<MFXListItem*>(myList->getItem(index));
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
MFXIconComboBox::getCurrentItem() const {
    return myList->getCurrentItem();
}


FXString
MFXIconComboBox::getItem(FXint index) const {
    return myList->getItem(index)->getText();
}


FXint
MFXIconComboBox::setIconItem(FXint index, const FXString& text, FXIcon* icon, FXColor bgColor, void* ptr) {
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
MFXIconComboBox::insertIconItem(FXint index, const FXString& text, FXIcon* icon, FXColor bgColor, void* ptr) {
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
MFXIconComboBox::appendIconItem(const FXString& text, FXIcon* icon, FXColor bgColor, void* ptr) {
    FXint index = myList->appendItem(new MFXListItem(text, icon, bgColor, ptr));
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
MFXIconComboBox::setItem(const FXString& text, FXIcon* icon) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        // cast MFXListItem
        const MFXListItem* item = dynamic_cast<MFXListItem*>(myList->getItem(i));
        // set icon and background color
        if (item && (item->getText() == text) && (item->getIcon() == icon)) {
            myList->setCurrentItem(i, FALSE);
            return true;
        }
    }
    return false;
}


FXint
MFXIconComboBox::prependItem(const FXString& text, void* ptr) {
    FXint index = myList->prependItem(text, NULL, ptr);
    if (isItemCurrent(0)) {
        myTextFieldIcon->setText(text);
        myTextFieldIcon->setBackColor(FXRGB(255,255,255));
        myIconLabel->setIcon(nullptr);
        myIconLabel->setBackColor(FXRGB(255,255,255));
    }
    recalc();
    return index;
}


FXint
MFXIconComboBox::moveItem(FXint newindex, FXint oldindex) {
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
        myIconLabel->setBackColor(FXRGB(255,255,255));
    }
    recalc();
    return newindex;
}


void
MFXIconComboBox::removeItem(FXint index) {
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
        myIconLabel->setBackColor(FXRGB(255,255,255));
    }
    recalc();
}


void
MFXIconComboBox::clearItems() {
    myTextFieldIcon->resetTextField();
    myList->clearItems();
    recalc();
}


FXint
MFXIconComboBox::findItem(const FXString& text, FXint start, FXuint flgs) const {
    return myList->findItem(text, start, flgs);
}


FXint
MFXIconComboBox::findItemByData(const void* ptr, FXint start, FXuint flgs) const {
    return myList->findItemByData(ptr, start, flgs);
}


FXString
MFXIconComboBox::getItemText(FXint index) const {
    return myList->getItemText(index);
}


void
MFXIconComboBox::setItemData(FXint index, void* ptr) const {
    myList->setItemData(index, ptr);
}


void*
MFXIconComboBox::getItemData(FXint index) const {
    return myList->getItemData(index);
}


FXbool
MFXIconComboBox::isPaneShown() const {
    return myPane->shown();
}


void
MFXIconComboBox::setFont(FXFont* fnt) {
    if (!fnt) {
        fxerror("%s::setFont: NULL font specified.\n", getClassName());
    }
    myTextFieldIcon->setFont(fnt);
    myList->setFont(fnt);
    recalc();
}


FXFont*
MFXIconComboBox::getFont() const {
    return myTextFieldIcon->getFont();
}


void
MFXIconComboBox::setComboStyle(FXuint mode) {
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
MFXIconComboBox::getComboStyle() const {
    return (options & COMBOBOX_MASK);
}


void
MFXIconComboBox::setJustify(FXuint style) {
    myTextFieldIcon->setJustify(style);
}


FXuint
MFXIconComboBox::getJustify() const {
    return myTextFieldIcon->getJustify();
}


void
MFXIconComboBox::setBackColor(FXColor clr) {
    myTextFieldIcon->setBackColor(clr);
    myIconLabel->setBackColor(clr);
    myList->setBackColor(clr);
}


FXColor
MFXIconComboBox::getBackColor() const {
    return myTextFieldIcon->getBackColor();
}


void
MFXIconComboBox::setTextColor(FXColor clr) {
    myTextFieldIcon->setTextColor(clr);
    myList->setTextColor(clr);
}


FXColor
MFXIconComboBox::getTextColor() const {
    return myTextFieldIcon->getTextColor();
}


void
MFXIconComboBox::setSelBackColor(FXColor clr) {
    myTextFieldIcon->setSelBackColor(clr);
    myList->setSelBackColor(clr);
}


FXColor
MFXIconComboBox::getSelBackColor() const {
    return myTextFieldIcon->getSelBackColor();
}


void
MFXIconComboBox::setSelTextColor(FXColor clr) {
    myTextFieldIcon->setSelTextColor(clr);
    myList->setSelTextColor(clr);
}


FXColor
MFXIconComboBox::getSelTextColor() const {
    return myTextFieldIcon->getSelTextColor();
}


void
MFXIconComboBox::sortItems() {
    myList->sortItems();
}


FXListSortFunc
MFXIconComboBox::getSortFunc() const {
    return myList->getSortFunc();
}


void
MFXIconComboBox::setSortFunc(FXListSortFunc func) {
    myList->setSortFunc(func);
}


void
MFXIconComboBox::setHelpText(const FXString& txt) {
    myTextFieldIcon->setHelpText(txt);
}


const FXString&
MFXIconComboBox::getHelpText() const {
    return myTextFieldIcon->getHelpText();
}


void
MFXIconComboBox::setTipText(const FXString& txt) {
    myTextFieldIcon->setTipText(txt);
}


const FXString&
MFXIconComboBox::getTipText() const {
    return myTextFieldIcon->getTipText();
}


long
MFXIconComboBox::onUpdFmText(FXObject*, FXSelector, void*) {
    return target && !isPaneShown() && target->tryHandle(this, FXSEL(SEL_UPDATE, message), NULL);
}


long
MFXIconComboBox::onFwdToText(FXObject* sender, FXSelector sel, void* ptr) {
    return myTextFieldIcon->handle(sender, sel, ptr);
}


long
MFXIconComboBox::onListClicked(FXObject*, FXSelector sel, void* ptr) {
    myButton->handle(this, FXSEL(SEL_COMMAND, ID_UNPOST), NULL);
    if (FXSELTYPE(sel) == SEL_COMMAND) {
        // cast MFXListItem
        const MFXListItem* item = dynamic_cast<MFXListItem*>(myList->getItem((FXint)(FXival)ptr));
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
MFXIconComboBox::onTextButton(FXObject*, FXSelector, void*) {
    if (options & COMBOBOX_STATIC) {
        // Post the myList
        myButton->handle(this, FXSEL(SEL_COMMAND, ID_POST), NULL);
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
MFXIconComboBox::onFocusSelf(FXObject* sender, FXSelector, void* ptr) {
    return myTextFieldIcon->handle(sender, FXSEL(SEL_FOCUS_SELF, 0), ptr);
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


MFXIconComboBox::MFXIconComboBox() {}
