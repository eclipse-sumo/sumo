/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2021 German Aerospace Center (DLR) and others.
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
/// @file    FXMenuButtonIcon.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2021
///
//
/****************************************************************************/

#pragma once
#include <config.h>


#include "fxheader.h"

/// @brief FXMenuButtonIcon (based on FXMenuButton)
class FXMenuButtonIcon : public FXLabel {
    FXDECLARE(FXMenuButtonIcon)
    protected:
    FXPopup *pane;                  // Pane to pop up
    FXint    offsetx;               // Shift attachment point x
    FXint    offsety;               // Shift attachment point y
    FXbool   state;                 // Pane was popped
    protected:
    FXMenuButtonIcon();
    private:
    FXMenuButtonIcon(const FXMenuButtonIcon&);
    FXMenuButtonIcon &operator=(const FXMenuButtonIcon&);
    public:
    long onPaint(FXObject*, FXSelector, void*);
    long onUpdate(FXObject*, FXSelector, void*);
    long onEnter(FXObject*, FXSelector, void*);
    long onLeave(FXObject*, FXSelector, void*);
    long onFocusIn(FXObject*, FXSelector, void*);
    long onFocusOut(FXObject*, FXSelector, void*);
    long onUngrabbed(FXObject*, FXSelector, void*);
    long onMotion(FXObject*, FXSelector, void*);
    long onLeftBtnPress(FXObject*, FXSelector, void*);
    long onLeftBtnRelease(FXObject*, FXSelector, void*);
    long onKeyPress(FXObject*, FXSelector, void*);
    long onKeyRelease(FXObject*, FXSelector, void*);
    long onHotKeyPress(FXObject*, FXSelector, void*);
    long onHotKeyRelease(FXObject*, FXSelector, void*);
    long onCmdPost(FXObject*, FXSelector, void*);
    long onCmdUnpost(FXObject*, FXSelector, void*);
    public:

    /// Constructor
    FXMenuButtonIcon(FXComposite* p, const FXString& text, FXIcon* ic=NULL, FXPopup* pup=NULL, FXuint opts=JUSTIFY_NORMAL|ICON_BEFORE_TEXT|MENUBUTTON_DOWN, FXint x=0, FXint y=0, FXint w=0, FXint h=0, FXint pl=DEFAULT_PAD, FXint pr=DEFAULT_PAD, FXint pt=DEFAULT_PAD, FXint pb=DEFAULT_PAD);

    /// Create server-side resources
    virtual void create();

    /// Detach server-side resources
    virtual void detach();

    /// Return default width
    virtual FXint getDefaultWidth();

    /// Return default height
    virtual FXint getDefaultHeight();

    /// Returns true because a menu button can receive focus
    virtual bool canFocus() const;

    /// Remove the focus from this window
    virtual void killFocus();

    /// Return true if window logically contains the given point
    virtual bool contains(FXint parentx, FXint parenty) const;

    /// Change the popup menu
    void setMenu(FXPopup *pup);

    /// Return current popup menu
    FXPopup* getMenu() const { return pane; }

    /// Set X offset where menu pops up relative to button
    void setXOffset(FXint offx){ offsetx=offx; }

    /// Return current X offset
    FXint getXOffset() const { return offsetx; }

    /// Set Y offset where menu pops up relative to button
    void setYOffset(FXint offy){ offsety=offy; }

    /// Return current Y offset
    FXint getYOffset() const { return offsety; }

    /// Change menu button style
    void setButtonStyle(FXuint style);

    /// Get menu button style
    FXuint getButtonStyle() const;

    /// Change popup style
    void setPopupStyle(FXuint style);

    /// Get popup style
    FXuint getPopupStyle() const;

    /// Change attachment
    void setAttachment(FXuint att);

    /// Get attachment
    FXuint getAttachment() const;

    /// Destructor
    virtual ~FXMenuButtonIcon();
};