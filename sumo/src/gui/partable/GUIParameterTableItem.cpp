//---------------------------------------------------------------------------//
//                        GUIParameterTableItem.cpp -
//  A single line in the parameter window
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                :
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2003/11/12 14:09:12  dkrajzew
// clean up after recent changes; comments added
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <qlistview.h>
#include <qpixmap.h>
#include "GUIParameterTableItem.h"
#include <utils/convert/ToString.h>
#include <gui/icons/yes_no/yes.xpm>
#include <gui/icons/yes_no/no.xpm>


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIParameterTableItem::GUIParameterTableItem(QListView *table,
                                             const std::string &name,
                                             bool dynamic,
                                             ValueSource<double> *src)
    : QListViewItem(table, name.c_str(),
    toString<double>(src->getValue()).c_str()),
    myAmDynamic(dynamic), myName(name), mySource(src),
    myValue(src->getValue())
{
    if(dynamic) {
        setPixmap(2, QPixmap(yes_xpm));
    } else {
        setPixmap(2, QPixmap(no_xpm));
    }
}


GUIParameterTableItem::GUIParameterTableItem(QListView *table,
                                             const std::string &name,
                                             bool dynamic,
                                             double value)
    : QListViewItem(table, name.c_str(),
    toString<double>(value).c_str()),
    myAmDynamic(dynamic), myName(name), mySource(0),
    myValue(value)
{
    if(dynamic) {
        setPixmap(2, QPixmap(yes_xpm));
    } else {
        setPixmap(2, QPixmap(no_xpm));
    }
}


GUIParameterTableItem::GUIParameterTableItem(QListView *table,
                                             const std::string &name,
                                             bool dynamic,
                                             std::string value)
    : QListViewItem(table, name.c_str(), value.c_str()),
    myAmDynamic(dynamic), myName(name), mySource(0),
    myValue(0)
{
    if(dynamic) {
        setPixmap(2, QPixmap(yes_xpm));
    } else {
        setPixmap(2, QPixmap(no_xpm));
    }
}


GUIParameterTableItem::~GUIParameterTableItem()
{
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
    if(!dynamic()||mySource==0) {
        return;
    }
    double value = mySource->getValue();
    if(value!=myValue) {
        myValue = value;
        setText(1, toString<double>(value).c_str());
    }
}


ValueSource<double> *
GUIParameterTableItem::getSourceCopy() const
{
    if(mySource==0) {
        return 0;
    }
    return mySource->copy();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIParameterTableItem.icc"
//#endif

// Local Variables:
// mode:C++
// End:

