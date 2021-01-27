/********************************************************************************
*                                                                               *
*                          M e n u C h e c k   W i d g e t                      *
*                                                                               *
*********************************************************************************
* Copyright (C) 2002,2006 by Jeroen van der Zijp.   All Rights Reserved.        *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************
* $Id: FXMenuCheckIcon.h,v 1.13 2006/01/22 17:58:06 fox Exp $                       *
********************************************************************************/
#pragma once
#include <config.h>


#include <fx.h>

/**
* The menu check widget is used to change a state in the
* application from a menu.  Menu checks may reflect
* the state of the application by graying out, becoming hidden,
* or by a check mark.
* When activated, a menu check sends a SEL_COMMAND to its target;
* the void* argument of the message contains the new state.
*/
class FXMenuCheckIcon : public FXMenuCommand {
	/// @brief FOX-declaration
	FXDECLARE(FXMenuCheckIcon)

public:
	/// @brief Construct a menu check
	FXMenuCheckIcon(FXComposite* p,const FXString& text,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=0);

	/// @brief Return default width
	virtual FXint getDefaultWidth();

	/// @brief Return default height
	virtual FXint getDefaultHeight();

	/// @brief Set check state (TRUE, FALSE or MAYBE)
	void setCheck(FXbool s=TRUE);

	/// @brief Get check state (TRUE, FALSE or MAYBE)
	FXbool getCheck() const;

	/// @brief Get the box background color
	FXColor getBoxColor() const;

	/// @brief Set the box background color
	void setBoxColor(FXColor clr);

	/// @brief Save menu to a stream
	virtual void save(FXStream& store) const;

	/// @brief Load menu from a stream
	virtual void load(FXStream& store);

	long onPaint(FXObject*, FXSelector, void*);
	long onButtonPress(FXObject*, FXSelector, void*);
	long onButtonRelease(FXObject*, FXSelector, void*);
	long onKeyPress(FXObject*, FXSelector, void*);
	long onKeyRelease(FXObject*, FXSelector, void*);
	long onHotKeyPress(FXObject*, FXSelector, void*);
	long onHotKeyRelease(FXObject*, FXSelector, void*);
	long onCheck(FXObject*, FXSelector, void*);
	long onUncheck(FXObject*, FXSelector, void*);
	long onUnknown(FXObject*, FXSelector, void*);
	long onCmdSetValue(FXObject*, FXSelector, void*);
	long onCmdSetIntValue(FXObject*, FXSelector, void*);
	long onCmdGetIntValue(FXObject*, FXSelector, void*);
	long onCmdAccel(FXObject*, FXSelector, void*);

protected:
	/// @brief default constructor
	FXMenuCheckIcon();

	/// @brief State of menu
	FXuchar check;

	/// @brief Box color
	FXColor boxColor;

private:
	/// @brief Invalidated copy constructor.
	FXMenuCheckIcon(const FXMenuCheckIcon&) = delete;

	/// @brief Invalidated assignment operator.
	FXMenuCheckIcon& operator=(const FXMenuCheckIcon&) = delete;
};