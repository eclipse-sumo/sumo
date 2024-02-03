/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2024 German Aerospace Center (DLR) and others.
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
/// @file    MFXListIcon.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2023
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <utils/common/UtilExceptions.h>
#include <fxkeys.h>

#include "MFXListIcon.h"

// ===========================================================================
// Macross
// ===========================================================================

#define LINE_SPACING    4   // Line spacing between items
#define ICON_SIZE       16

#define LIST_MASK   (SELECT_MASK | LIST_AUTOSELECT)

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Map
FXDEFMAP(MFXListIcon) MFXListIconMap[] = {
    FXMAPFUNC(SEL_PAINT,                0,                              MFXListIcon::onPaint),
    FXMAPFUNC(SEL_ENTER,                0,                              MFXListIcon::onEnter),
    FXMAPFUNC(SEL_LEAVE,                0,                              MFXListIcon::onLeave),
    FXMAPFUNC(SEL_MOTION,               0,                              MFXListIcon::onMotion),
    FXMAPFUNC(SEL_TIMEOUT,              FXWindow::ID_AUTOSCROLL,        MFXListIcon::onAutoScroll),
    FXMAPFUNC(SEL_TIMEOUT,              MFXListIcon::ID_TIPTIMER,       MFXListIcon::onTipTimer),
    FXMAPFUNC(SEL_TIMEOUT,              MFXListIcon::ID_LOOKUPTIMER,    MFXListIcon::onLookupTimer),
    FXMAPFUNC(SEL_UNGRABBED,            0,                              MFXListIcon::onUngrabbed),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,      0,                              MFXListIcon::onLeftBtnPress),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE,    0,                              MFXListIcon::onLeftBtnRelease),
    FXMAPFUNC(SEL_RIGHTBUTTONPRESS,     0,                              MFXListIcon::onRightBtnPress),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   0,                              MFXListIcon::onRightBtnRelease),
    FXMAPFUNC(SEL_KEYPRESS,             0,                              MFXListIcon::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,           0,                              MFXListIcon::onKeyRelease),
    FXMAPFUNC(SEL_FOCUSIN,              0,                              MFXListIcon::onFocusIn),
    FXMAPFUNC(SEL_FOCUSOUT,             0,                              MFXListIcon::onFocusOut),
    FXMAPFUNC(SEL_CLICKED,              0,                              MFXListIcon::onClicked),
    FXMAPFUNC(SEL_DOUBLECLICKED,        0,                              MFXListIcon::onDoubleClicked),
    FXMAPFUNC(SEL_TRIPLECLICKED,        0,                              MFXListIcon::onTripleClicked),
    FXMAPFUNC(SEL_COMMAND,              0,                              MFXListIcon::onCommand),
    FXMAPFUNC(SEL_QUERY_TIP,            0,                              MFXListIcon::onQueryTip),
    FXMAPFUNC(SEL_QUERY_HELP,           0,                              MFXListIcon::onQueryHelp),
};


// Object implementation
FXIMPLEMENT(MFXListIcon, FXScrollArea, MFXListIconMap, ARRAYNUMBER(MFXListIconMap))

// ===========================================================================
// member method definitions
// ===========================================================================

MFXListIcon::MFXListIcon(FXComposite* p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXScrollArea(p, opts, x, y, w, h) {
    flags |= FLAG_ENABLED;
    target = tgt;
    message = sel;
    font = getApp()->getNormalFont();
    textColor = getApp()->getForeColor();
    selbackColor = getApp()->getSelbackColor();
    seltextColor = getApp()->getSelforeColor();
}


MFXListIcon::~MFXListIcon() {
    getApp()->removeTimeout(this, ID_TIPTIMER);
    getApp()->removeTimeout(this, ID_LOOKUPTIMER);
    clearItems(FALSE);
    font = (FXFont*) - 1L;
}


void
MFXListIcon::create() {
    FXScrollArea::create();
    for (const auto& item : items) {
        item->create();
    }
    font->create();
}


void
MFXListIcon::detach() {
    FXScrollArea::detach();
    for (const auto& item : items) {
        item->detach();
    }
    font->detach();
}


bool
MFXListIcon::canFocus() const {
    return true;
}


void
MFXListIcon::setFocus() {
    FXScrollArea::setFocus();
    setDefault(TRUE);
}


void
MFXListIcon::killFocus() {
    FXScrollArea::killFocus();
    setDefault(MAYBE);
}


FXint
MFXListIcon::getDefaultWidth() {
    return FXScrollArea::getDefaultWidth();
}


FXint
MFXListIcon::getDefaultHeight() {
    if (visible > (int)itemFiltered.size()) {
        return (int)itemFiltered.size() * (LINE_SPACING + FXMAX(font->getFontHeight(),  ICON_SIZE));
    } else {
        return visible * (LINE_SPACING + FXMAX(font->getFontHeight(),  ICON_SIZE));
    }
}


void
MFXListIcon::recalc() {
    FXScrollArea::recalc();
    flags |= FLAG_RECALC;
    cursor = nullptr;
}


void
MFXListIcon::setNumVisible(FXint nvis) {
    if (nvis < 0) {
        nvis = 0;
    }
    if (visible != nvis) {
        visible = nvis;
        recalc();
    }
}


FXint
MFXListIcon::getContentWidth() {
    if (flags & FLAG_RECALC) {
        recompute();
    }
    return listWidth;
}


FXint
MFXListIcon::getContentHeight() {
    if (flags & FLAG_RECALC) {
        recompute();
    }
    return listHeight;
}


void
MFXListIcon::layout() {
    // Calculate contents
    FXScrollArea::layout();
    // Determine line size for scroll bars
    if (0 < (int)itemFiltered.size()) {
        vertical->setLine(itemFiltered[0]->getHeight(this));
        horizontal->setLine(itemFiltered[0]->getWidth(this) / 10);
    }
    update();
    // We were supposed to make this item viewable
    if (viewable) {
        makeItemVisible(viewable);
    }
    // No more dirty
    flags &= ~FLAG_DIRTY;
}


FXbool
MFXListIcon::isItemCurrent(FXint index) const {
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i] == currentItem) {
            return i == index;
        }
    }
    return false;
}


FXbool
MFXListIcon::isItemVisible(MFXListIconItem* item) const {
    return (0 < (pos_y + item->y + item->getHeight(this))) && ((pos_y + item->y) < viewport_h);
}


void
MFXListIcon::makeItemVisible(MFXListIconItem* item) {
    FXint y, h;
    // Remember for later
    viewable = item;
    // Was realized
    if (xid) {
        // Force layout if dirty
        if (flags & FLAG_RECALC) {
            layout();
        }
        y = pos_y;
        h = item->getHeight(this);
        if (viewport_h <= y + item->y + h) {
            y = viewport_h - item->y - h;
        }
        if (y + item->y <= 0) {
            y = -item->y;
        }
        // Scroll into view
        setPosition(pos_x, y);
        // Done it
        viewable = nullptr;
    }
}


void
MFXListIcon::makeItemVisible(FXint index) {
    makeItemVisible(items[index]);
}


FXint
MFXListIcon::getItemWidth(FXint index) const {
    if ((index < 0) || ((int)itemFiltered.size() <= index)) {
        fxerror("%s::isItemSelected: index out of range.\n", getClassName());
    }
    return itemFiltered[index]->getWidth(this);
}


FXint
MFXListIcon::getItemHeight(FXint index) const {
    if ((index < 0) || ((int)itemFiltered.size() <= index)) {
        fxerror("%s::isItemSelected: index out of range.\n", getClassName());
    }
    return itemFiltered[index]->getHeight(this);
}


MFXListIconItem*
MFXListIcon::getItemAt(FXint y) const {
    y -= pos_y;
    // continue depending if we're filtering
    if (filter.empty()) {
        for (int i = 0; i < (int)items.size(); i++) {
            if (items[i]->y <= y && y < items[i]->y + items[i]->getHeight(this)) {
                return items[i];
            }
        }
    } else {
        for (int i = 0; i < (int)itemFiltered.size(); i++) {
            if ((itemFiltered[i]->y <= y) && (y < itemFiltered[i]->y + itemFiltered[i]->getHeight(this))) {
                return itemFiltered[i];
            }
        }
    }
    return nullptr;
}


int
MFXListIcon::findItem(const FXString& text) const {
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i]->getText().text() == text) {
            return i;
        }
    }
    return -1;
}


FXint
MFXListIcon::hitItem(MFXListIconItem* item, FXint x, FXint y) const {
    FXint ix, iy, hit = 0;
    if (item) {
        x -= pos_x;
        y -= pos_y;
        ix = item->x;
        iy = item->y;
        hit = item->hitItem(this, x - ix, y - iy);
    }
    return hit;
}


void
MFXListIcon::updateItem(MFXListIconItem* item) const {
    update(0, pos_y + item->y, viewport_w, item->getHeight(this));
}


FXbool
MFXListIcon::selectItem(MFXListIconItem* item, FXbool notify) {
    if (!item->isSelected()) {
        killSelection(notify);
        item->setSelected(TRUE);
        updateItem(item);
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_SELECTED, message), nullptr);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}


FXbool
MFXListIcon::deselectItem(MFXListIconItem* item, FXbool notify) {
    if (item->isSelected()) {
        item->setSelected(FALSE);
        updateItem(item);
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_DESELECTED, message), nullptr);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}


FXbool
MFXListIcon::toggleItem(MFXListIconItem* item, FXbool notify) {
    if (!item->isSelected()) {
        killSelection(notify);
        item->setSelected(TRUE);
        updateItem(item);
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_SELECTED, message), nullptr);
        }
    } else {
        item->setSelected(FALSE);
        updateItem(item);
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_DESELECTED, message), nullptr);
        }
    }
    return TRUE;
}


FXbool
MFXListIcon::killSelection(FXbool notify) {
    FXbool changes = FALSE;
    FXint i;
    for (i = 0; i < (int)items.size(); i++) {
        if (items[i]->isSelected()) {
            items[i]->setSelected(FALSE);
            updateItem(items[i]);
            changes = TRUE;
            if (notify && target) {
                target->tryHandle(this, FXSEL(SEL_DESELECTED, message), (void*)(FXival)i);
            }
        }
    }
    return changes;
}


long
MFXListIcon::onEnter(FXObject* sender, FXSelector sel, void* ptr) {
    FXScrollArea::onEnter(sender, sel, ptr);
    getApp()->addTimeout(this, ID_TIPTIMER, getApp()->getMenuPause());
    cursor = nullptr;
    return 1;
}


long
MFXListIcon::onLeave(FXObject* sender, FXSelector sel, void* ptr) {
    FXScrollArea::onLeave(sender, sel, ptr);
    getApp()->removeTimeout(this, ID_TIPTIMER);
    cursor = nullptr;
    return 1;
}


long
MFXListIcon::onFocusIn(FXObject* sender, FXSelector sel, void* ptr) {
    FXScrollArea::onFocusIn(sender, sel, ptr);
    if (currentItem) {
        currentItem->setFocus(TRUE);
        updateItem(currentItem);
    }
    return 1;
}


long
MFXListIcon::onTipTimer(FXObject*, FXSelector, void*) {
    flags |= FLAG_TIP;
    return 1;
}


long
MFXListIcon::onQueryTip(FXObject* sender, FXSelector sel, void* ptr) {
    if (FXWindow::onQueryTip(sender, sel, ptr)) {
        return 1;
    }
    if (cursor && (flags & FLAG_TIP) && !(options & LIST_AUTOSELECT)) {    // No tip when autoselect!
        FXString string = cursor->getText();
        sender->handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), (void*) & string);
        return 1;
    }
    return 0;
}


long
MFXListIcon::onQueryHelp(FXObject* sender, FXSelector sel, void* ptr) {
    if (FXWindow::onQueryHelp(sender, sel, ptr)) {
        return 1;
    }
    if ((flags & FLAG_HELP) && !help.empty()) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), (void*) & help);
        return 1;
    }
    return 0;
}


long
MFXListIcon::onFocusOut(FXObject* sender, FXSelector sel, void* ptr) {
    FXScrollArea::onFocusOut(sender, sel, ptr);
    if (currentItem) {
        currentItem->setFocus(FALSE);
        updateItem(currentItem);
    }
    return 1;
}


long
MFXListIcon::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXDCWindow dc(this,  event);
    FXint y,  h;
    // Paint items
    y = pos_y;
    for (int i = 0; i < (int)itemFiltered.size(); i++) {
        h = itemFiltered[i]->getHeight(this);
        if (event->rect.y <= (y + h) && y < (event->rect.y + event->rect.h)) {
            itemFiltered[i]->draw(this,  dc,  pos_x,  y,  FXMAX(listWidth,  viewport_w),  h);
        }
        y += h;
    }
    // Paint blank area below items
    if (y < (event->rect.y + event->rect.h)) {
        dc.setForeground(backColor);
        dc.fillRectangle(event->rect.x,  y,  event->rect.w,  event->rect.y + event->rect.h - y);
    }
    return 1;
}


long
MFXListIcon::onLookupTimer(FXObject*, FXSelector, void*) {
    lookup = FXString::null;
    return 1;
}


long
MFXListIcon::onKeyPress(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXint index = getCurrentItemIndex();
    flags &= ~FLAG_TIP;
    if (!isEnabled()) {
        return 0;
    }
    if (target && target->tryHandle(this, FXSEL(SEL_KEYPRESS, message), ptr)) {
        return 1;
    }
    switch (event->code) {
        case KEY_Control_L:
        case KEY_Control_R:
        case KEY_Shift_L:
        case KEY_Shift_R:
        case KEY_Alt_L:
        case KEY_Alt_R:
            if (flags & FLAG_DODRAG) {
                handle(this, FXSEL(SEL_DRAGGED, 0), ptr);
            }
            return 1;
        case KEY_Page_Up:
        case KEY_KP_Page_Up:
            lookup = FXString::null;
            setPosition(pos_x, pos_y + verticalScrollBar()->getPage());
            return 1;
        case KEY_Page_Down:
        case KEY_KP_Page_Down:
            lookup = FXString::null;
            setPosition(pos_x, pos_y - verticalScrollBar()->getPage());
            return 1;
        case KEY_Up:
        case KEY_KP_Up:
            index -= 1;
            goto hop;
        case KEY_Down:
        case KEY_KP_Down:
            index += 1;
            goto hop;
        case KEY_Home:
        case KEY_KP_Home:
            index = 0;
            goto hop;
        case KEY_End:
        case KEY_KP_End:
            index = (int)itemFiltered.size() - 1;
hop:
            lookup = FXString::null;
            // continue depending of filter
            if (filter.empty()) {
                if (0 <= index && index < (int)items.size()) {
                    setCurrentItem(items[index], TRUE);
                    makeItemVisible(items[index]);
                }
            } else {
                if ((0 <= index) && (index < (int)itemFiltered.size())) {
                    setCurrentItem(itemFiltered[index], TRUE);
                    makeItemVisible(itemFiltered[index]);
                }
            }
            handle(this, FXSEL(SEL_CLICKED, 0), (void*)currentItem);
            if (currentItem && currentItem->isEnabled()) {
                handle(this, FXSEL(SEL_COMMAND, 0), (void*)currentItem);
            }
            return 1;
        case KEY_space:
        case KEY_KP_Space:
            lookup = FXString::null;
            if (currentItem && currentItem->isEnabled()) {
                toggleItem(currentItem, TRUE);
                setAnchorItem(currentItem);
            }
            handle(this, FXSEL(SEL_CLICKED, 0), (void*)currentItem);
            if (currentItem && currentItem->isEnabled()) {
                handle(this, FXSEL(SEL_COMMAND, 0), (void*)currentItem);
            }
            return 1;
        case KEY_Return:
        case KEY_KP_Enter:
            lookup = FXString::null;
            handle(this, FXSEL(SEL_DOUBLECLICKED, 0), (void*)currentItem);
            if (currentItem && currentItem->isEnabled()) {
                handle(this, FXSEL(SEL_COMMAND, 0), (void*)currentItem);
            }
            return 1;
        default:
            return 1;
    }
}


long
MFXListIcon::onKeyRelease(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    if (!isEnabled()) {
        return 0;
    }
    if (target && target->tryHandle(this, FXSEL(SEL_KEYRELEASE, message), ptr)) {
        return 1;
    }
    switch (event->code) {
        case KEY_Shift_L:
        case KEY_Shift_R:
        case KEY_Control_L:
        case KEY_Control_R:
        case KEY_Alt_L:
        case KEY_Alt_R:
            if (flags & FLAG_DODRAG) {
                handle(this, FXSEL(SEL_DRAGGED, 0), ptr);
            }
            return 1;
    }
    return 0;
}


long
MFXListIcon::onAutoScroll(FXObject*,  FXSelector,  void*) {
    return 1;
}


long
MFXListIcon::onMotion(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    MFXListIconItem* oldcursor = cursor;
    FXuint flg = flags;

    // Kill the tip
    flags &= ~FLAG_TIP;

    // Kill the tip timer
    getApp()->removeTimeout(this, ID_TIPTIMER);

    // Right mouse scrolling
    if (flags & FLAG_SCROLLING) {
        setPosition(event->win_x - grabx, event->win_y - graby);
        return 1;
    }

    // Drag and drop mode
    if (flags & FLAG_DODRAG) {
        if (startAutoScroll(event, TRUE)) {
            return 1;
        }
        handle(this, FXSEL(SEL_DRAGGED, 0), ptr);
        return 1;
    }

    // Tentative drag and drop
    if ((flags & FLAG_TRYDRAG) && event->moved) {
        flags &= ~FLAG_TRYDRAG;
        if (handle(this, FXSEL(SEL_BEGINDRAG, 0), ptr)) {
            flags |= FLAG_DODRAG;
        }
        return 1;
    }

    // Normal operation
    if ((flags & FLAG_PRESSED) || (options & LIST_AUTOSELECT)) {
        // Start auto scrolling?
        if (startAutoScroll(event, FALSE)) {
            return 1;
        }
        // Find item
        auto element = getItemAt(event->win_y);
        // Got an item different from before
        if (element) {
            // Make it the current item
            setCurrentItem(element, TRUE);
            return 1;
        }
    }

    // Reset tip timer if nothing's going on
    getApp()->addTimeout(this, ID_TIPTIMER, getApp()->getMenuPause());

    // Get item we're over
    cursor = getItemAt(event->win_y);

    // Force GUI update only when needed
    return (cursor != oldcursor) || (flg & FLAG_TIP);
}


long
MFXListIcon::onLeftBtnPress(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXint code;
    flags &= ~FLAG_TIP;
    handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
    if (isEnabled()) {
        grab();
        flags &= ~FLAG_UPDATE;
        // First change callback
        if (target && target->tryHandle(this, FXSEL(SEL_LEFTBUTTONPRESS, message), ptr)) {
            return 1;
        }
        // Autoselect mode
        if (options & LIST_AUTOSELECT) {
            return 1;
        }
        // Locate item
        auto item = getItemAt(event->win_y);
        // No item
        if (item == nullptr) {
            return 1;
        }
        // Find out where hit
        code = hitItem(item, event->win_x, event->win_y);
        // Change current item
        setCurrentItem(item, TRUE);
        // Change item selection
        state = item->isSelected();
        if (item->isEnabled() && !state) {
            selectItem(item, TRUE);
        }
        // Start drag if actually pressed text or icon only
        if (code && item->isSelected() && item->isDraggable()) {
            flags |= FLAG_TRYDRAG;
        }
        flags |= FLAG_PRESSED;
        return 1;
    }
    return 0;
}


long
MFXListIcon::onLeftBtnRelease(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXuint flg = flags;
    if (isEnabled()) {
        ungrab();
        stopAutoScroll();
        flags |= FLAG_UPDATE;
        flags &= ~(FLAG_PRESSED | FLAG_TRYDRAG | FLAG_DODRAG);
        // First chance callback
        if (target && target->tryHandle(this, FXSEL(SEL_LEFTBUTTONRELEASE, message), ptr)) {
            return 1;
        }
        // No activity
        if (!(flg & FLAG_PRESSED) && !(options & LIST_AUTOSELECT)) {
            return 1;
        }
        // Was dragging
        if (flg & FLAG_DODRAG) {
            handle(this, FXSEL(SEL_ENDDRAG, 0), ptr);
            return 1;
        }
        if (currentItem && currentItem->isEnabled()) {
            if (state) {
                deselectItem(currentItem, TRUE);
            }
        }
        // Scroll to make item visibke
        makeItemVisible(currentItem);
        // Update anchor
        setAnchorItem(currentItem);
        // Generate clicked callbacks
        if (event->click_count == 1) {
            handle(this, FXSEL(SEL_CLICKED, 0), (void*)currentItem);
        } else if (event->click_count == 2) {
            handle(this, FXSEL(SEL_DOUBLECLICKED, 0), (void*)currentItem);
        } else if (event->click_count == 3) {
            handle(this, FXSEL(SEL_TRIPLECLICKED, 0), (void*)currentItem);
        }
        // Command callback only when clicked on item
        if (currentItem && currentItem->isEnabled()) {
            handle(this, FXSEL(SEL_COMMAND, 0), (void*)currentItem);
        }
        return 1;
    }
    return 0;
}


long
MFXListIcon::onRightBtnPress(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    flags &= ~FLAG_TIP;
    handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
    if (isEnabled()) {
        grab();
        flags &= ~FLAG_UPDATE;
        if (target && target->tryHandle(this, FXSEL(SEL_RIGHTBUTTONPRESS, message), ptr)) {
            return 1;
        }
        flags |= FLAG_SCROLLING;
        grabx = event->win_x - pos_x;
        graby = event->win_y - pos_y;
        return 1;
    }
    return 0;
}


long
MFXListIcon::onRightBtnRelease(FXObject*, FXSelector, void* ptr) {
    if (isEnabled()) {
        ungrab();
        flags &= ~FLAG_SCROLLING;
        flags |= FLAG_UPDATE;
        if (target && target->tryHandle(this, FXSEL(SEL_RIGHTBUTTONRELEASE, message), ptr)) {
            return 1;
        }
        return 1;
    }
    return 0;
}


long
MFXListIcon::onUngrabbed(FXObject* sender, FXSelector sel, void* ptr) {
    FXScrollArea::onUngrabbed(sender, sel, ptr);
    flags &= ~(FLAG_DODRAG | FLAG_TRYDRAG | FLAG_PRESSED | FLAG_CHANGED | FLAG_SCROLLING);
    flags |= FLAG_UPDATE;
    stopAutoScroll();
    return 1;
}


long
MFXListIcon::onCommand(FXObject*, FXSelector, void* ptr) {
    return target ? target->tryHandle(this, FXSEL(SEL_COMMAND, message), ptr) : 0;
}


long
MFXListIcon::onClicked(FXObject*, FXSelector, void* ptr) {
    return target ? target->tryHandle(this, FXSEL(SEL_CLICKED, message), ptr) : 0;
}


long
MFXListIcon::onDoubleClicked(FXObject*, FXSelector, void* ptr) {
    return target ? target->tryHandle(this, FXSEL(SEL_DOUBLECLICKED, message), ptr) : 0;
}


long
MFXListIcon::onTripleClicked(FXObject*, FXSelector, void* ptr) {
    return target ? target->tryHandle(this, FXSEL(SEL_TRIPLECLICKED, message), ptr) : 0;
}


void
MFXListIcon::setCurrentItem(MFXListIconItem* item, FXbool notify) {
    if (item) {
        // Deactivate old item
        if (currentItem) {
            currentItem->setFocus(FALSE);
            updateItem(currentItem);
        }
        currentItem = item;
        // Activate new item
        if (currentItem) {
            currentItem->setFocus(TRUE);
            updateItem(currentItem);
        }
        // Notify item change
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)currentItem);
        }
    }
}


FXint
MFXListIcon::getCurrentItemIndex() const {
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i] == currentItem) {
            return i;
        }
    }
    return -1;
}


FXint
MFXListIcon::getViewableItem() const {
    // continue depending if we're filtering
    if (filter.empty()) {
        for (int i = 0; i < (int)items.size(); i++) {
            if (items[i] == viewable) {
                return i;
            }
        }
    } else {
        for (int i = 0; i < (int)itemFiltered.size(); i++) {
            if (itemFiltered[i] == viewable) {
                return i;
            }
        }
    }
    return -1;
}


void
MFXListIcon::setAnchorItem(MFXListIconItem* item) {
    int index = 0;
    // continue depending if we're filtering
    if (filter.empty()) {
        for (int i = 0; i < (int)items.size(); i++) {
            if (items[i] == item) {
                index = i;
            }
        }
    } else {
        for (int i = 0; i < (int)itemFiltered.size(); i++) {
            if (itemFiltered[i] == item) {
                index = i;
            }
        }
    }
    anchor = index;
    extent = index;
}


FXint
MFXListIcon::getAnchorItem() const {
    return anchor;
}


MFXListIconItem*
MFXListIcon::getCursorItem() const {
    return cursor;
}


MFXListIconItem*
MFXListIcon::getItem(FXint index) const {
    if (index < 0 || (int)items.size() <= index) {
        fxerror("%s::getItem: index out of range.\n", getClassName());
    }
    return items[index];
}


FXint
MFXListIcon::setItem(FXint index, MFXListIconItem* item, FXbool notify) {
    // Must have item
    if (!item) {
        fxerror("%s::setItem: item is NULL.\n", getClassName());
    }
    // Must be in range
    if (index < 0 || (int)items.size() <= index) {
        fxerror("%s::setItem: index out of range.\n", getClassName());
    }
    // Notify item will be replaced
    if (notify && target) {
        target->tryHandle(this, FXSEL(SEL_REPLACED, message), (void*)(FXival)index);
    }
    // Copy the state over
    item->state = items[index]->state;
    // Delete old
    delete items[index];
    // Add new
    items[index] = item;
    // apply filter
    setFilter(filter, nullptr);
    return index;
}


FXint
MFXListIcon::editItem(FXint index, const FXString& text, FXIcon* icon, void* ptr, FXbool notify) {
    return setItem(index, createItem(text, icon, ptr), notify);
}


FXint
MFXListIcon::insertItem(FXint index, MFXListIconItem* item, FXbool notify) {
    MFXListIconItem* old = currentItem;
    // Must have item
    if (!item) {
        fxerror("%s::insertItem: item is NULL.\n", getClassName());
    }
    // Must be in range
    if (index < 0 || (int)items.size() < index) {
        fxerror("%s::insertItem: index out of range.\n", getClassName());
    }
    // Add item to list
    items.insert(items.begin() + index, item);
    // Adjust indices
    if (anchor >= index) {
        anchor++;
    }
    if (extent >= index) {
        extent++;
    }
    if (getCurrentItemIndex() >= index) {
        currentItem = items[index];
    }
    if (getViewableItem() >= index) {
        viewable = items[index];
    }
    if ((currentItem == nullptr) && ((int)items.size() == 1)) {
        currentItem = items[0];
    }
    // Notify item has been inserted
    if (notify && target) {
        target->tryHandle(this, FXSEL(SEL_INSERTED, message), (void*)(FXival)index);
    }
    // Current item may have changed
    if (old != currentItem) {
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)currentItem);
        }
    }
    // Was new item
    if (currentItem && currentItem == items[index]) {
        if (hasFocus()) {
            currentItem->setFocus(TRUE);
        }
    }
    // apply filter
    setFilter(filter, nullptr);
    return index;
}


FXint
MFXListIcon::insertItem(FXint index, const FXString& text, FXIcon* icon, void* ptr, FXbool notify) {
    return insertItem(index, createItem(text, icon, ptr), notify);
}


FXint
MFXListIcon::appendItem(MFXListIconItem* item, FXbool notify) {
    return insertItem((int)items.size(), item, notify);
}


FXint
MFXListIcon::appendItem(const FXString& text, FXIcon* icon, void* ptr, FXbool notify) {
    return insertItem((int)items.size(), createItem(text, icon, ptr), notify);
}


void
MFXListIcon::removeItem(FXint index, FXbool notify) {
    MFXListIconItem* old = currentItem;
    // Must be in range
    if ((index < 0) || ((int)items.size() <= index)) {
        fxerror("%s::removeItem: index out of range.\n", getClassName());
    }
    // Notify item will be deleted
    if (notify && target) {
        target->tryHandle(this, FXSEL(SEL_DELETED, message), (void*)(FXival)index);
    }
    // Delete item
    delete items[index];
    // Remove item from list
    items.erase(items.begin() + index);
    // Adjust indices
    if (anchor >= index) {
        anchor++;
    }
    if (extent >= index) {
        extent++;
    }
    if (getCurrentItemIndex() >= index) {
        currentItem = items[index];
    }
    if (getViewableItem() >= index) {
        viewable = items[index];
    }
    if ((currentItem == nullptr) && ((int)items.size() == 1)) {
        currentItem = items[0];
    }
    // Notify item has been inserted
    if (notify && target) {
        target->tryHandle(this, FXSEL(SEL_INSERTED, message), (void*)(FXival)index);
    }
    // Current item may have changed
    if (old != currentItem) {
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)currentItem);
        }
    }
    // Was new item
    if (currentItem && currentItem == items[index]) {
        if (hasFocus()) {
            currentItem->setFocus(TRUE);
        }
    }
    // apply filter
    setFilter(filter, nullptr);
}


void
MFXListIcon::clearItems(FXbool notify) {
    // Delete items
    for (FXint index = (int)items.size() - 1; 0 <= index; index--) {
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_DELETED, message), (void*)(FXival)index);
        }
        delete items[index];
    }
    // Free array
    items.clear();
    // Adjust indices
    anchor = -1;
    extent = -1;
    // Current item has changed
    if (currentItem) {
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)(FXival) - 1);
        }
        currentItem = nullptr;
    }
    viewable = nullptr;
    // apply filter
    setFilter(filter, nullptr);
}


void
MFXListIcon::setFilter(const FXString& value, FXLabel* label) {
    filter = value;
    // update item filtered
    itemFiltered.clear();
    for (int i = 0; i < (int)items.size(); i++) {
        items[i]->show = showItem(items[i]->getText());
        if (items[i]->show) {
            itemFiltered.push_back(items[i]);
        }
    }
    // check if show label
    if (label) {
        if (!value.empty() && ((int)itemFiltered.size() == 0)) {
            label->show();
        } else {
            label->hide();
        }
    }
    // recompute and recalc
    recompute();
    recalc();
}


void
MFXListIcon::setTextColor(FXColor clr) {
    if (textColor != clr) {
        textColor = clr;
        update();
    }
}


void
MFXListIcon::setHelpText(const FXString& text) {
    help = text;
}


FXString
MFXListIcon::tolowerString(const FXString& str) const {
    FXString result;
    for (int i = 0; i < str.count(); i++) {
        result.append((char)::tolower(str[i]));
    }
    return result;
}


MFXListIcon::MFXListIcon() {
    flags |= FLAG_ENABLED;
    font = (FXFont*) - 1L;
}


void
MFXListIcon::recompute() {
    FXint x, y, w, h;
    x = 0;
    y = 0;
    listWidth = 0;
    listHeight = 0;
    for (auto& item : itemFiltered) {
        // set position and size
        item->x = x;
        item->y = y;
        w = item->getWidth(this);
        h = item->getHeight(this);
        if (w > listWidth) {
            listWidth = w;
        }
        y += h;
    }
    listHeight = y;
    flags &= ~FLAG_RECALC;
}


MFXListIconItem*
MFXListIcon::createItem(const FXString& text, FXIcon* icon, void* ptr) {
    return new MFXListIconItem(text, icon, 0, ptr);
}


bool
MFXListIcon::showItem(const FXString& itemName) const {
    if (filter.empty()) {
        return true;
    } else {
        return tolowerString(itemName).find(tolowerString(filter)) != -1;
    }
}
