/****************************************************************************/
/// @file    GUIParameterTableItem.cpp
/// @author  Daniel Krajzewicz
/// @date
/// @version $Id: $
///
// A single line in the parameter window
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "GUIParameterTableItem.h"
#include <utils/common/ToString.h>
#include <utils/gui/images/GUIIconSubSys.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// method definitions
// ===========================================================================
GUIParameterTableItem::GUIParameterTableItem(FXTable *table, size_t pos,
        const std::string &name,
        bool dynamic,
        ValueSource<SUMOReal> *src)
        : myAmDynamic(dynamic), myName(name), myTablePosition(pos), mySource(src),
        myValue(src->getValue()), myTable(table)
{
    init(dynamic, toString<SUMOReal>(src->getValue()));
}


GUIParameterTableItem::GUIParameterTableItem(FXTable *table, size_t pos,
        const std::string &name,
        bool dynamic,
        SUMOReal value)
        : myAmDynamic(dynamic), myName(name), myTablePosition(pos), mySource(0),
        myValue(value), myTable(table)
{
    init(dynamic, toString<SUMOReal>(value));
}


GUIParameterTableItem::GUIParameterTableItem(FXTable *table, size_t pos,
        const std::string &name,
        bool dynamic,
        std::string value)
        : myAmDynamic(dynamic), myName(name), myTablePosition(pos), mySource(0),
        myValue(0), myTable(table)
{
    init(dynamic, value);
}


void
GUIParameterTableItem::init(bool dynamic, std::string value)
{
    myTable->setItemText(myTablePosition, 0, myName.c_str());
    myTable->setItemText(myTablePosition, 1, value.c_str());
    if (dynamic) {
        myTable->setItemIcon(myTablePosition, 2,
                             GUIIconSubSys::getIcon(ICON_YES));
    } else {
        myTable->setItemIcon(myTablePosition, 2,
                             GUIIconSubSys::getIcon(ICON_NO));
    }
//    myTable->setItemIconPosition(myTablePosition, 2, FXTableItem::ABOVE);
    myTable->setItemJustify(myTablePosition, 2,
                            FXTableItem::CENTER_X|FXTableItem::CENTER_Y);
}


GUIParameterTableItem::~GUIParameterTableItem()
{
    delete mySource;
}


bool
GUIParameterTableItem::dynamic() const
{
    return myAmDynamic;
}


const std::string &
GUIParameterTableItem::getName() const
{
    return myName;
}


void
GUIParameterTableItem::update()
{
    if (!dynamic()||mySource==0) {
        return;
    }
    SUMOReal value = mySource->getValue();
    if (value!=myValue) {
        myValue = value;
        myTable->setItemText(myTablePosition, 1,
                             toString<SUMOReal>(myValue).c_str());
    }
}


ValueSource<SUMOReal> *
GUIParameterTableItem::getSourceCopy() const
{
    if (mySource==0) {
        return 0;
    }
    return mySource->copy();
}



/****************************************************************************/

