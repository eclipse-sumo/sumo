/****************************************************************************/
// Eclipse SUMO,  Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License,  version 2
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
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_SETVALUE,          MFXListIcon::onCmdSetValue), 
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_SETINTVALUE,       MFXListIcon::onCmdSetIntValue), 
    FXMAPFUNC(SEL_COMMAND,              FXWindow::ID_GETINTVALUE,       MFXListIcon::onCmdGetIntValue), 
};


// Object implementation
FXIMPLEMENT(MFXListIcon, FXScrollArea, MFXListIconMap, ARRAYNUMBER(MFXListIconMap))

// ===========================================================================
// member method definitions
// ===========================================================================

MFXListIcon::MFXListIcon(FXComposite *p, FXObject* tgt, FXSelector sel, FXuint opts, FXint x, FXint y, FXint w, FXint h):
    FXScrollArea(p, opts, x, y, w, h) {
    flags |= FLAG_ENABLED;
    target = tgt;
    message = sel;
    anchor = -1;
    current = -1;
    extent = -1;
    cursor = -1;
    viewable = -1;
    font = getApp()->getNormalFont();
    textColor = getApp()->getForeColor();
    selbackColor = getApp()->getSelbackColor();
    seltextColor = getApp()->getSelforeColor();
    listWidth = 0;
    listHeight = 0;
    visible = 0;
    grabx = 0;
    graby = 0;
    state = FALSE;
}


MFXListIcon::~MFXListIcon() {
    getApp()->removeTimeout(this, ID_TIPTIMER);
    getApp()->removeTimeout(this, ID_LOOKUPTIMER);
    clearItems(FALSE);
    font = (FXFont*)-1L;
}


void
MFXListIcon::create() {
    FXScrollArea::create();
    for (int i = 0; i < items.no(); i++) {
        items[i]->create();
    }
    font->create();
}


void
MFXListIcon::detach() {
    FXScrollArea::detach();
    for (int i = 0; i < items.no(); i++) {
        items[i]->detach();
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
    int filteredItems = (int)getFilteredItems().size();
    if (visible > filteredItems) {
        return filteredItems * (LINE_SPACING + FXMAX(font->getFontHeight(),  ICON_SIZE));
    } else {
        return visible * (LINE_SPACING + FXMAX(font->getFontHeight(),  ICON_SIZE));
    }
}


void
MFXListIcon::recalc() {
    FXScrollArea::recalc();
    flags |= FLAG_RECALC;
    cursor = -1;
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
    const auto filteredItems = getFilteredItems();
    // Determine line size for scroll bars
    if (0 < filteredItems.size()) {
        vertical->setLine(filteredItems[0]->getHeight(this));
        horizontal->setLine(filteredItems[0]->getWidth(this)/10);
    }
    update();
    // We were supposed to make this item viewable
    if (0 <= viewable) {
        makeItemVisible(viewable);
    }
    // No more dirty
    flags &= ~FLAG_DIRTY;
}


FXString
MFXListIcon::getItemText(FXint index) const {
    const auto filteredItems = getFilteredItems();
    if (index < 0 || (int)filteredItems.size() <= index) {
        fxerror("%s::getItemText: index out of range.\n", getClassName());
    }
    return filteredItems[index]->getText();
}


FXbool
MFXListIcon::isItemSelected(FXint index) const {
    const auto filteredItems = getFilteredItems();
    if (index < 0 || (int)filteredItems.size() <= index) {
        fxerror("%s::isItemSelected: index out of range.\n", getClassName());
    }
    return filteredItems[index]->isSelected();
}


FXbool
MFXListIcon::isItemCurrent(FXint index) const {
    return (0 <= index) && (index == current);
}


FXbool
MFXListIcon::isItemEnabled(FXint index) const {
    const auto filteredItems = getFilteredItems();
    if (index < 0 || (int)filteredItems.size() <= index) {
        fxerror("%s::isItemSelected: index out of range.\n", getClassName());
    }
    return filteredItems[index]->isEnabled();
}



FXbool
MFXListIcon::isItemVisible(FXint index) const {
    const auto filteredItems = getFilteredItems();
    if (index < 0 || (int)filteredItems.size() <= index) {
        fxerror("%s::isItemSelected: index out of range.\n", getClassName());
    }
    return (0 < (pos_y + filteredItems[index]->y + filteredItems[index]->getHeight(this))) &&
           ((pos_y + filteredItems[index]->y) < viewport_h);
}


void
MFXListIcon::makeItemVisible(FXint index) {
    register FXint y, h;
    const auto filteredItems = getFilteredItems();
    if (0 <= index && index < (int)filteredItems.size()) {
        // Remember for later
        viewable = index;
        // Was realized
        if (xid) {
            // Force layout if dirty
            if (flags & FLAG_RECALC) {
                layout();
            }
            y = pos_y;
            h = filteredItems[index]->getHeight(this);
            if (viewport_h <= y + filteredItems[index]->y+h) {
                y = viewport_h - filteredItems[index]->y-h;
            }
            if (y + filteredItems[index]->y <= 0) {
                y =- filteredItems[index]->y;
            }
            // Scroll into view
            setPosition(pos_x, y);
            // Done it
            viewable = -1;
        }
    }
}


FXint
MFXListIcon::getItemWidth(FXint index) const {
    const auto filteredItems = getFilteredItems();
    if (index < 0 || (int)filteredItems.size() <= index) {
        fxerror("%s::isItemSelected: index out of range.\n", getClassName());
    }
    return filteredItems[index]->getWidth(this);
}


FXint
MFXListIcon::getItemHeight(FXint index) const {
    const auto filteredItems = getFilteredItems();
    if (index < 0 || (int)filteredItems.size() <= index) {
        fxerror("%s::isItemSelected: index out of range.\n", getClassName());
    }
    return filteredItems[index]->getHeight(this);
}


FXint
MFXListIcon::getItemAt(FXint, FXint y) const {
    y -= pos_y;
    const auto filteredItems = getFilteredItems();
    for (int i = 0; i < (int)filteredItems.size(); i++) {
        if (filteredItems[i]->y <= y && y < filteredItems[i]->y + filteredItems[i]->getHeight(this)) {
            return i;
        }
    }
    return -1;
}


FXint
MFXListIcon::hitItem(FXint index, FXint x, FXint y) const {
    FXint ix, iy, hit = 0;
    const auto filteredItems = getFilteredItems();
    if (0 <= index && index < (int)filteredItems.size()) {
        x -= pos_x;
        y -= pos_y;
        ix = filteredItems[index]->x;
        iy = filteredItems[index]->y;
        hit = filteredItems[index]->hitItem(this, x - ix, y - iy);
    }
    return hit;
}


void
MFXListIcon::updateItem(FXint index) const {
    const auto filteredItems = getFilteredItems();
    if (0 <= index && index < (int)filteredItems.size()) {
        update(0, pos_y + filteredItems[index]->y, viewport_w, filteredItems[index]->getHeight(this));
    }
}


FXbool
MFXListIcon::selectItem(FXint index, FXbool notify) {
    const auto filteredItems = getFilteredItems();
    if (index < 0 || (int)filteredItems.size() <= index) {
        fxerror("%s::selectItem: index out of range.\n", getClassName());
    }
    if (!filteredItems[index]->isSelected()) {
        killSelection(notify);
        filteredItems[index]->setSelected(TRUE);
        updateItem(index);
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_SELECTED, message), (void*)(FXival)index);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}


FXbool
MFXListIcon::deselectItem(FXint index, FXbool notify) {
    const auto filteredItems = getFilteredItems();
    if (index < 0 || (int)filteredItems.size() <= index) {
        fxerror("%s::deselectItem: index out of range.\n", getClassName());
    }
    if (filteredItems[index]->isSelected()) {
        filteredItems[index]->setSelected(FALSE);
        updateItem(index);
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_DESELECTED, message), (void*)(FXival)index);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}


FXbool
MFXListIcon::toggleItem(FXint index, FXbool notify) {
    const auto filteredItems = getFilteredItems();
    if (index < 0 || (int)filteredItems.size() <= index) {
        fxerror("%s::toggleItem: index out of range.\n", getClassName());
    }
    if (!filteredItems[index]->isSelected()) {
        killSelection(notify);
        filteredItems[index]->setSelected(TRUE);
        updateItem(index);
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_SELECTED, message), (void*)(FXival)index);
        }
    } else {
        filteredItems[index]->setSelected(FALSE);
        updateItem(index);
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_DESELECTED, message), (void*)(FXival)index);
        }
    }
    return TRUE;
}


FXbool
MFXListIcon::killSelection(FXbool notify) {
    register FXbool changes = FALSE;
    register FXint i;
    for (i = 0; i < items.no(); i++) {
        if (items[i]->isSelected()) {
            items[i]->setSelected(FALSE);
            updateItem(i);
            changes = TRUE;
            if (notify && target) {
                target->tryHandle(this, FXSEL(SEL_DESELECTED, message), (void*)(FXival)i);
            }
        }
    }
    return changes;
}


long
MFXListIcon::onCmdSetValue(FXObject*, FXSelector, void* ptr) {
    setCurrentItem((FXint)(FXival)ptr);
    return 1;
}


long
MFXListIcon::onCmdGetIntValue(FXObject*, FXSelector, void* ptr) {
    *((FXint*)ptr) = getCurrentItem();
    return 1;
}


long
MFXListIcon::onCmdSetIntValue(FXObject*, FXSelector, void* ptr) {
    setCurrentItem(*((FXint*)ptr));
    return 1;
}


long
MFXListIcon::onEnter(FXObject* sender, FXSelector sel, void* ptr) {
    FXScrollArea::onEnter(sender, sel, ptr);
    getApp()->addTimeout(this, ID_TIPTIMER, getApp()->getMenuPause());
    cursor = -1;
    return 1;
}


long
MFXListIcon::onLeave(FXObject* sender, FXSelector sel, void* ptr) {
    FXScrollArea::onLeave(sender, sel, ptr);
    getApp()->removeTimeout(this, ID_TIPTIMER);
    cursor = -1;
    return 1;
}


long
MFXListIcon::onFocusIn(FXObject* sender, FXSelector sel, void* ptr) {
    FXScrollArea::onFocusIn(sender, sel, ptr);
    if (0 <= current) {
        const auto filteredItems = getFilteredItems();
        FXASSERT(current < (int)filteredItems.size());
        filteredItems[current]->setFocus(TRUE);
        updateItem(current);
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
    if ((flags & FLAG_TIP) && !(options & LIST_AUTOSELECT) && (0 <= cursor)) {    // No tip when autoselect!
        FXString string = getFilteredItems()[cursor]->getText();
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
    if (0 <= current) {
        const auto filteredItems = getFilteredItems();
        FXASSERT(current < (int)filteredItems.size());
        filteredItems[current]->setFocus(FALSE);
        updateItem(current);
    }
    return 1;
}


long
MFXListIcon::onPaint(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXDCWindow dc(this,  event);
    FXint y,  h;
    const auto filteredItems = getFilteredItems();
    // Paint items
    y = pos_y;
    for (const auto &item : filteredItems) {
        h = item->getHeight(this);
        if (event->rect.y <= (y + h) && y < (event->rect.y + event->rect.h)) {
            item->draw(this,  dc,  pos_x,  y,  FXMAX(listWidth,  viewport_w),  h);
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
    FXint index = current;
    flags &= ~FLAG_TIP;
    if (!isEnabled()) {
        return 0;
    }
    if (target && target->tryHandle(this, FXSEL(SEL_KEYPRESS, message), ptr)) {
        return 1;
    }
    const auto filteredItems = getFilteredItems();
    switch(event->code) {
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
            index = (int)filteredItems.size() - 1;
            hop: lookup = FXString::null;
            if (0 <= index && index < (int)filteredItems.size()) {
                setCurrentItem(index, TRUE);
                makeItemVisible(index);
            }
            handle(this, FXSEL(SEL_CLICKED, 0), (void*)(FXival)current);
            if (0 <= current && filteredItems[current]->isEnabled()) {
                handle(this, FXSEL(SEL_COMMAND, 0), (void*)(FXival)current);
            }
            return 1;
        case KEY_space:
        case KEY_KP_Space:
            lookup = FXString::null;
            if (0 <= current && filteredItems[current]->isEnabled()) {
                toggleItem(current, TRUE);
                setAnchorItem(current);
            }
            handle(this, FXSEL(SEL_CLICKED, 0), (void*)(FXival)current);
            if (0 <= current && filteredItems[current]->isEnabled()) {
                handle(this, FXSEL(SEL_COMMAND, 0), (void*)(FXival)(FXival)current);
            }
            return 1;
        case KEY_Return:
        case KEY_KP_Enter:
            lookup = FXString::null;
            handle(this, FXSEL(SEL_DOUBLECLICKED, 0), (void*)(FXival)current);
            if (0 <= current && filteredItems[current]->isEnabled()) {
                handle(this, FXSEL(SEL_COMMAND, 0), (void*)(FXival)current);
            }
            return 1;
        default:
            if ((FXuchar)event->text[0] < ' ') {
                return 0;
            }
            if (event->state & (CONTROLMASK|ALTMASK)) {
                return 0;
            }
            if (!Ascii::isPrint(event->text[0])) {
                return 0;
            }
            lookup.append(event->text);
            getApp()->addTimeout(this, ID_LOOKUPTIMER, getApp()->getTypingSpeed());
            index = findItem(lookup, current, SEARCH_FORWARD|SEARCH_WRAP|SEARCH_PREFIX);
            if (0 <= index) {
                setCurrentItem(index, TRUE);
                makeItemVisible(index);
                setAnchorItem(index);
            }
            handle(this, FXSEL(SEL_CLICKED, 0), (void*)(FXival)current);
            if (0 <= current && filteredItems[current]->isEnabled()) {
                handle(this, FXSEL(SEL_COMMAND, 0), (void*)(FXival)current);
            }
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
    switch(event->code) {
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
    FXint oldcursor = cursor;
    FXuint flg = flags;

    // Kill the tip
    flags &= ~FLAG_TIP;

    // Kill the tip timer
    getApp()->removeTimeout(this, ID_TIPTIMER);

    // Right mouse scrolling
    if (flags & FLAG_SCROLLING) {
        setPosition(event->win_x-grabx, event->win_y-graby);
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
        FXint index = getItemAt(event->win_x, event->win_y);
        // Got an item different from before
        if (0 <= index && index != current) {
            // Make it the current item
            setCurrentItem(index, TRUE);
            return 1;
        }
    }

    // Reset tip timer if nothing's going on
    getApp()->addTimeout(this, ID_TIPTIMER, getApp()->getMenuPause());

    // Get item we're over
    cursor = getItemAt(event->win_x, event->win_y);

    // Force GUI update only when needed
    return (cursor != oldcursor)||(flg & FLAG_TIP);
}


long
MFXListIcon::onLeftBtnPress(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXint index, code;
    flags &= ~FLAG_TIP;
    handle(this, FXSEL(SEL_FOCUS_SELF, 0), ptr);
    if (isEnabled()) {
        const auto filteredItems = getFilteredItems();
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
        index = getItemAt(event->win_x, event->win_y);
        // No item
        if (index < 0) {
            return 1;
        }
        // Find out where hit
        code = hitItem(index, event->win_x, event->win_y);
        // Change current item
        setCurrentItem(index, TRUE);
        // Change item selection
        state = filteredItems[index]->isSelected();
        if (filteredItems[index]->isEnabled() && !state) {
            selectItem(index, TRUE);
        }
        // Start drag if actually pressed text or icon only
        if (code && filteredItems[index]->isSelected() && filteredItems[index]->isDraggable()) {
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
        const auto filteredItems = getFilteredItems();
        ungrab();
        stopAutoScroll();
        flags |= FLAG_UPDATE;
        flags &= ~(FLAG_PRESSED|FLAG_TRYDRAG|FLAG_DODRAG);
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
        if (0 <= current && filteredItems[current]->isEnabled()) {
            if (state) {
                deselectItem(current, TRUE);
            }
        }
        // Scroll to make item visibke
        makeItemVisible(current);

        // Update anchor
        setAnchorItem(current);

        // Generate clicked callbacks
        if (event->click_count == 1) {
        handle(this, FXSEL(SEL_CLICKED, 0), (void*)(FXival)current);
        }
        else if (event->click_count == 2) {
        handle(this, FXSEL(SEL_DOUBLECLICKED, 0), (void*)(FXival)current);
        }
        else if (event->click_count == 3) {
        handle(this, FXSEL(SEL_TRIPLECLICKED, 0), (void*)(FXival)current);
        }
        // Command callback only when clicked on item
        if (0 <= current && filteredItems[current]->isEnabled()) {
            handle(this, FXSEL(SEL_COMMAND, 0), (void*)(FXival)current);
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
    if (target && target->tryHandle(this, FXSEL(SEL_RIGHTBUTTONRELEASE, message), ptr)) return 1;
    return 1;
    }
    return 0;
}


long
MFXListIcon::onUngrabbed(FXObject* sender, FXSelector sel, void* ptr) {
    FXScrollArea::onUngrabbed(sender, sel, ptr);
    flags &= ~(FLAG_DODRAG|FLAG_TRYDRAG|FLAG_PRESSED|FLAG_CHANGED|FLAG_SCROLLING);
    flags |= FLAG_UPDATE;
    stopAutoScroll();
    return 1;
}


long
MFXListIcon::onCommand(FXObject*, FXSelector, void* ptr) {
    return target? target->tryHandle(this, FXSEL(SEL_COMMAND, message), ptr) : 0;
}


long
MFXListIcon::onClicked(FXObject*, FXSelector, void* ptr) {
    return target? target->tryHandle(this, FXSEL(SEL_CLICKED, message), ptr) : 0;
}


long
MFXListIcon::onDoubleClicked(FXObject*, FXSelector, void* ptr) {
    return target? target->tryHandle(this, FXSEL(SEL_DOUBLECLICKED, message), ptr) : 0;
}


long
MFXListIcon::onTripleClicked(FXObject*, FXSelector, void* ptr) {
    return target? target->tryHandle(this, FXSEL(SEL_TRIPLECLICKED, message), ptr) : 0;
}


void
MFXListIcon::setCurrentItem(FXint index, FXbool notify) {
    const auto filteredItems = getFilteredItems();
    if (index < -1 || filteredItems.size() <= index) {
        fxerror("%s::setCurrentItem: index out of range.\n", getClassName());
    }
    if (index != current) {
        // Deactivate old item
        if (0 <= current) {
            // No visible change if it doen't have the focus
            if (hasFocus()) {
                filteredItems[current]->setFocus(FALSE);
                updateItem(current);
            }
        }
        current = index;
        // Activate new item
        if (0 <= current) {
            // No visible change if it doen't have the focus
            if (hasFocus()) {
                filteredItems[current]->setFocus(TRUE);
                updateItem(current);
            }
        }
        // Notify item change
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)(FXival)current);
        }
    }
}


void
MFXListIcon::setAnchorItem(FXint index) {
    if (index < -1 || (int)getFilteredItems().size() <= index) {
        fxerror("%s::setAnchorItem: index out of range.\n", getClassName());
    }
    anchor = index;
    extent = index;
}


MFXListIconItem*
MFXListIcon::getItem(FXint index) const {
    const auto filteredItems = getFilteredItems();
    if (index < 0 || (int)filteredItems.size() <= index) {
        fxerror("%s::getItem: index out of range.\n", getClassName());
    }
    return filteredItems[index];
}


FXint
MFXListIcon::setItem(FXint index, MFXListIconItem* item, FXbool notify) {
    // Must have item
    if (!item) {
        fxerror("%s::setItem: item is NULL.\n", getClassName());
    }

    // Must be in range
    if (index < 0 || items.no() <= index) {
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

    // recompute (due filter)
    recompute();

    // Redo layout
    recalc();
    return index;
}


FXint
MFXListIcon::setItem(FXint index, const FXString &text, FXIcon *icon, void* ptr, FXbool notify) {
    return setItem(index, createItem(text, icon, ptr), notify);
}


FXint
MFXListIcon::insertItem(FXint index, MFXListIconItem* item, FXbool notify) {
    register FXint old = current;

    // Must have item
    if (!item) {
        fxerror("%s::insertItem: item is NULL.\n", getClassName());
    }

    // Must be in range
    if (index < 0 || items.no() < index) {
        fxerror("%s::insertItem: index out of range.\n", getClassName());
    }

    // Add item to list
    items.insert(index, item);

    // Adjust indices
    if (anchor >= index) anchor++;
    if (extent >= index) extent++;
    if (current >= index) current++;
    if (viewable >= index) viewable++;
    if (current < 0 && items.no() == 1) current = 0;

    // Notify item has been inserted
    if (notify && target) {
        target->tryHandle(this, FXSEL(SEL_INSERTED, message), (void*)(FXival)index);
    }

    // Current item may have changed
    if (old != current) {
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)(FXival)current);
        }
    }

    // Was new item
    if (0 <= current && current == index) {
        if (hasFocus()) {
            items[current]->setFocus(TRUE);
        }
    }

    // recompute (due filter)
    recompute();

    // Redo layout
    recalc();
    return index;
}


FXint
MFXListIcon::insertItem(FXint index, const FXString &text, FXIcon *icon, void* ptr, FXbool notify) {
    return insertItem(index, createItem(text, icon, ptr), notify);
}


FXint
MFXListIcon::appendItem(MFXListIconItem* item, FXbool notify) {
    return insertItem(items.no(), item, notify);
}


FXint
MFXListIcon::appendItem(const FXString &text, FXIcon *icon, void* ptr, FXbool notify) {
    return insertItem(items.no(), createItem(text, icon, ptr), notify);
}


void
MFXListIcon::clearItems(FXbool notify) {
    register FXint old = current;

    // Delete items
    for (FXint index = items.no()-1; 0 <= index; index--) {
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_DELETED, message), (void*)(FXival)index);
        }
        delete items[index];
    }

    // Free array
    items.clear();

    // Adjust indices
    current = -1;
    anchor = -1;
    extent = -1;
    viewable = -1;

    // Current item has changed
    if (old != -1) {
        if (notify && target) {
            target->tryHandle(this, FXSEL(SEL_CHANGED, message), (void*)(FXival)-1);
        }
    }

    // recompute (due filter)
    recompute();

    // Redo layout
    recalc();
}


void
MFXListIcon::setFilter(const FXString &value) {
    filter = value;
    recompute();
    recalc();
}


FXint
MFXListIcon::findItem(const FXString &text, FXint start, FXuint flgs) const {
    register FXCompareFunc comparefunc;
    register FXint index, len;
    if (0 < items.no()) {
        comparefunc = (flgs & SEARCH_IGNORECASE) ? (FXCompareFunc)comparecase : (FXCompareFunc)compare;
        len = (flgs & SEARCH_PREFIX)? text.length() : 2147483647;
        if (flgs & SEARCH_BACKWARD) {
            if (start < 0) {
                start = items.no()-1;
            }
            for (index = start; 0 <= index; index--) {
                if ((*comparefunc)(items[index]->getText(), text, len) == 0) {
                    return index;
                }
            }
            if (!(flgs & SEARCH_WRAP)) {
                return -1;
            }
            for (index = items.no()-1; start < index; index--) {
                if ((*comparefunc)(items[index]->getText(), text, len) == 0) {
                    return index;
                }
            }
        } else {
            if (start < 0) {
                start = 0;
            }
            for (index = start; index < items.no(); index++) {
                if ((*comparefunc)(items[index]->getText(), text, len) == 0) {
                    return index;
                }
            }
            if (!(flgs & SEARCH_WRAP)) {
                return -1;
            }
            for (index = 0; index < start; index++) {
                if ((*comparefunc)(items[index]->getText(), text, len) == 0) {
                    return index;
                }
            }
        }
    }
    return -1;
}


void
MFXListIcon::setTextColor(FXColor clr) {
    if (textColor != clr) {
        textColor = clr;
        update();
    }
}


void
MFXListIcon::setHelpText(const FXString &text) {
    help = text;
}


MFXListIcon::MFXListIcon() {
    flags |= FLAG_ENABLED;
    anchor = -1;
    current = -1;
    extent = -1;
    cursor = -1;
    viewable = -1;
    font = (FXFont*)-1L;
    textColor = 0;
    selbackColor = 0;
    seltextColor = 0;
    listWidth = 0;
    listHeight = 0;
    visible = 0;
    grabx = 0;
    graby = 0;
    state = FALSE;
}


void
MFXListIcon::recompute() {
    // first filter items
    for (int i = 0; i < items.no(); i++) {
        items[i]->show = showItem(items[i]->getText().text());
    }
    register FXint x, y, w, h;
    x = 0;
    y = 0;
    listWidth = 0;
    listHeight = 0;
    const auto filteredItems = getFilteredItems();
    for (const auto &item : filteredItems) {
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
MFXListIcon::createItem(const FXString &text, FXIcon* icon, void* ptr) {
    return new MFXListIconItem(text, icon, 0, ptr);
}


bool
MFXListIcon::showItem(const std::string &itemName) const {
    if (filter.empty()) {
        return true;
    } else {
        const auto itemNameLower = tolowerString(itemName);
        const auto filterLower = tolowerString(filter.text());
        return itemNameLower.find(filterLower) != std::string::npos;
    }
}


std::vector<MFXListIconItem*>
MFXListIcon::getFilteredItems() const {
    std::vector<MFXListIconItem*> result;
    for (int i = 0; i < items.no(); i++) {
        if (items[i]->show) {
            result.push_back(items[i]);
        }
    }
    return result;
}


std::string
MFXListIcon::tolowerString(const std::string &str) const {
    std::string result;
    for (int i = 0; i < (int)str.size(); i++) {
        result.push_back((char)::tolower(str[i]));
    }
    return result;
}