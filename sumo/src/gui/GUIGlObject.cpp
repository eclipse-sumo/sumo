//---------------------------------------------------------------------------//
//                        GUIGlObject.cpp -
//  Base class for all objects that may be displayed within the openGL-gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.6  2003/06/06 10:28:45  dkrajzew
// new subfolder holding popup-menus was added due to link-dependencies under linux; QGLObjectPopupMenu*-classes were moved to "popup"
//
// Revision 1.5  2003/06/05 11:37:30  dkrajzew
// class templates applied
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <qlistview.h>
#include <string>
#include <stack>
#include <utils/convert/ToString.h>
#include "GUISUMOAbstractView.h"
#include <gui/popup/QGLObjectPopupMenu.h>
#include "GUIGlObject.h"
#include "GUIGlObjectStorage.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIGlObject::GUIGlObject(GUIGlObjectStorage &idStorage,
                         std::string fullName)
    : myFullName(fullName)
{
    idStorage.registerObject(this);
}


GUIGlObject::~GUIGlObject()
{
}


const std::string &
GUIGlObject::getFullName() const
{
    return myFullName;
}


size_t
GUIGlObject::getGlID() const
{
    return myGlID;
}


void
GUIGlObject::setGlID(size_t id)
{
    myGlID = id;
}



void
GUIGlObject::insertTableParameter(GUIParameterTableWindow *window,
                                  QListView *table,
                                  double *parameter,
                                  QListViewItem **vitems)
{
    fillTableParameter(parameter);


    std::stack<QListViewItem*> stack;
    QListViewItem *lvi = new QListViewItem(
        table, "id", getFullName().c_str());
    QListViewItem *plvi = lvi;
    stack.push(lvi);
    lvi->setOpen(TRUE);
    lvi->setExpandable(FALSE);

	int i = getTableParameterNo() - 1;
    for(; i>=0; i--) {
        const char * const item = getTableItem(i);
        TableType type = getTableType(i);
        switch(type)
        {
        case TT_DOUBLE:
            plvi==0
                ? lvi =  new QListViewItem(
                    table,
                    item,
                    toString<double>(parameter[i]).c_str())
                :  lvi =  new QListViewItem(
                    plvi,
                    item,
                    toString<double>(parameter[i]).c_str());
                vitems[i] = lvi;
            break;
        case TT_MENU_BEGIN:
            {
                const char *beginValue = getTableBeginValue(i);
                plvi==0
                    ? lvi =  new QListViewItem(
                        table,
                        item,
                        beginValue)
                    :  lvi =  new QListViewItem(
                        plvi,
                        item,
                        beginValue);
                stack.push(lvi);
                plvi = lvi;
                lvi->setOpen( FALSE );
                vitems[i] = lvi;
            }
            break;
        case TT_MENU_END:
            lvi = stack.top();
            stack.pop();
            if(stack.size()>0) {
                plvi = stack.top();
            } else {
                plvi = 0;
            }
            vitems[i] = 0;
            break;
        default:
            throw 1;
        }
    }
}


size_t
GUIGlObject::getTableParameterNo() const
{
    for(size_t i=0; ; i++) {
        const char * const item = getTableItem(i);
        if(item==0) {
            return i;
        }
    }
    throw 1;
    return 0;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIGlObject.icc"
//#endif

// Local Variables:
// mode:C++
// End:


