/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    MFXListIcon.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2023
///
//
/****************************************************************************/

#pragma once
#include <config.h>

#include "fxheader.h"


/// @brief A list item which allows for custom coloring
class MFXListIconItem : public FXListItem {
    /// @brief FOX declaration
    FXDECLARE(MFXListIconItem)

  friend class MFXListIcon;
protected:
  FXString  label;
  FXIcon   *icon;
  void     *data;
  FXuint    state;
  FXint     x,y;
private:
  MFXListIconItem(const MFXListIconItem&);
  MFXListIconItem& operator=(const MFXListIconItem&);
protected:
  MFXListIconItem():icon(NULL),data(NULL),state(0),x(0),y(0){}
  virtual void draw(const MFXListIcon* list,FXDC& dc,FXint x,FXint y,FXint w,FXint h);
  virtual FXint hitItem(const MFXListIcon* list,FXint x,FXint y) const;
public:
  enum {
    SELECTED  = 1,      /// Selected
    FOCUS     = 2,      /// Focus
    DISABLED  = 4,      /// Disabled
    DRAGGABLE = 8,      /// Draggable
    ICONOWNED = 16      /// Icon owned by item
    };
public:

  /// Construct new item with given text, icon, and user-data
  MFXListIconItem(const FXString& text,FXIcon* ic=NULL, FXColor backGroundColor = 0, void* ptr=NULL):label(text),icon(ic), myBackGroundColor(backGroundColor), data(ptr),state(0),x(0),y(0){}

  /// Change item's text label
  virtual void setText(const FXString& txt);

  /// Return item's text label
  const FXString& getText() const { return label; }

  /// Change item's icon, deleting the old icon if it was owned
  virtual void setIcon(FXIcon* icn,FXbool owned=FALSE);

  /// Return item's icon
  FXIcon* getIcon() const { return icon; }

  const FXColor& getBackGroundColor() const;

  /// Change item's user data
  void setData(void* ptr){ data=ptr; }

  /// Get item's user data
  void* getData() const { return data; }

  /// Make item draw as focused
  virtual void setFocus(FXbool focus);

  /// Return true if item has focus
  FXbool hasFocus() const { return (state&FOCUS)!=0; }

  /// Select item
  virtual void setSelected(FXbool selected);

  /// Return true if this item is selected
  FXbool isSelected() const { return (state&SELECTED)!=0; }

  /// Enable or disable item
  virtual void setEnabled(FXbool enabled);

  /// Return true if this item is enabled
  FXbool isEnabled() const { return (state&DISABLED)==0; }

  /// Make item draggable
  virtual void setDraggable(FXbool draggable);

  /// Return true if this item is draggable
  FXbool isDraggable() const { return (state&DRAGGABLE)!=0; }

  /// Return width of item as drawn in list
  virtual FXint getWidth(const MFXListIcon* list) const;

  /// Return height of item as drawn in list
  virtual FXint getHeight(const MFXListIcon* list) const;

  /// Create server-side resources
  virtual void create();

  /// Detach server-side resources
  virtual void detach();

  /// Destroy server-side resources
  virtual void destroy();

  /// Save to stream
  virtual void save(FXStream& store) const;

  /// Load from stream
  virtual void load(FXStream& store);

  /// Destroy item and free icons if owned
  virtual ~MFXListIconItem();

    /// @brief backGround color
    FXColor myBackGroundColor;
};