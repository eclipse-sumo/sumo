//---------------------------------------------------------------------------//
//                        GUIDialog_GLChosenEditor.cpp -
//  Editor for the  list of chosen objects
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 11.03.2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.3  2004/04/23 12:32:18  dkrajzew
// new layout
//
// Revision 1.2  2004/04/02 10:57:30  dkrajzew
// deselection of selected items added; saving of selected items names added
//
// Revision 1.1  2004/03/19 12:33:36  dkrajzew
// porting to FOX
//
// Revision 1.1  2004/03/19 12:32:26  dkrajzew
// porting to FOX
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <gui/GUIGlObject.h>
#include <guisim/GUINet.h>
#include "GUIDialog_GLChosenEditor.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIDialog_GLChosenEditor) GUIDialog_GLChosenEditorMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,       GUIDialog_GLChosenEditor::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,       GUIDialog_GLChosenEditor::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_DESELECT,   GUIDialog_GLChosenEditor::onCmdDeselect),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,      GUIDialog_GLChosenEditor::onCmdClear),
    FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,             GUIDialog_GLChosenEditor::onCmdClose),
};

FXIMPLEMENT(GUIDialog_GLChosenEditor, FXMainWindow, GUIDialog_GLChosenEditorMap, ARRAYNUMBER(GUIDialog_GLChosenEditorMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDialog_GLChosenEditor::GUIDialog_GLChosenEditor(GUIApplicationWindow *parent)
    : FXMainWindow(gFXApp, "List of Selected Items", NULL, NULL, DECOR_ALL, 0, 0, 300, 300),
    myParent(parent)
{
    FXHorizontalFrame *hbox =
        new FXHorizontalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,
        0,0,0,0);
    // build the list
    myList = new FXList(hbox, 0, 0,
        LAYOUT_FILL_X|LAYOUT_FILL_Y|LIST_MULTIPLESELECT);
    rebuildList();
    // build the layout
    FXVerticalFrame *layout = new FXVerticalFrame( hbox, LAYOUT_TOP,0,0,0,0,
        4,4,4,4);
    // "Load"
    new FXButton(layout, "Load\t\t", 0, this, MID_CHOOSEN_LOAD,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 3, 3);
    // "Save"
    new FXButton(layout, "Save\t\t", 0, this, MID_CHOOSEN_SAVE,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 3, 3);

    new FXHorizontalSeparator(layout,SEPARATOR_GROOVE|LAYOUT_FILL_X);

    // "Deselect Chosen"
    new FXButton(layout, "Deselect Chosen\t\t", 0, this, MID_CHOOSEN_DESELECT,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 3, 3);
    // "Clear List"
    new FXButton(layout, "Clear\t\t", 0, this, MID_CHOOSEN_CLEAR,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 3, 3);

    new FXHorizontalSeparator(layout,SEPARATOR_GROOVE|LAYOUT_FILL_X);

    // "Close"
    new FXButton(layout, "Close\t\t", 0, this, MID_CANCEL,
        ICON_BEFORE_TEXT|LAYOUT_FILL_X|FRAME_THICK|FRAME_RAISED,
        0, 0, 0, 0, 4, 4, 3, 3);
}


GUIDialog_GLChosenEditor::~GUIDialog_GLChosenEditor()
{
}


void
GUIDialog_GLChosenEditor::rebuildList()
{
    myList->clearItems();
    for(std::vector<size_t>::iterator i=gChosenObjects.begin(); i!=gChosenObjects.end(); ++i) {
        GUIGlObject *object = gSimInfo->net.getIDStorage().getObjectBlocking(*i);
        if(object!=0) {
            std::string name = object->getFullName();
            myList->appendItem(name.c_str());
            gSimInfo->net.getIDStorage().unblockObject(*i);
        }
    }
}


long
GUIDialog_GLChosenEditor::onCmdLoad(FXObject*,FXSelector,void*)
{
    return 1;
}


long
GUIDialog_GLChosenEditor::onCmdSave(FXObject*,FXSelector,void*)
{
    // get the new file name
    FXFileDialog opendialog(this,"Save List of selected Items");
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.sel.txt");
    if(gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if(opendialog.execute()){
        gCurrentFolder = opendialog.getDirectory().text();
		string file = string(opendialog.getFilename().text());
        ofstream strm(file.c_str());
        if(!strm.good()) {
            throw 1; //!!!!
        }
        // save selected objects
        for(std::vector<size_t>::iterator i=gChosenObjects.begin(); i!=gChosenObjects.end(); ++i) {
            GUIGlObject *object = gSimInfo->net.getIDStorage().getObjectBlocking(*i);
            if(object!=0) {
                std::string name = object->getFullName();
                strm << name << endl;
            }
        }
    }
    return 1;
}


long
GUIDialog_GLChosenEditor::onCmdDeselect(FXObject*,FXSelector,void*)
{
    size_t no = myList->getNumItems();
    vector<size_t> selected;
    size_t i;
    // collect selected items
    for(i=0; i<no; i++) {
        if(myList->getItem(i)->isSelected()) {
            selected.push_back(i);
        }
    }
    // remove items from list
    std::vector<size_t>::iterator j = gChosenObjects.begin();
    no = gChosenObjects.size();
    size_t removed = 0;
    for(i=0; i<no; ) {
        // if item shall be deselected
        if(i==*(selected.begin())-removed) {
            // remove from items-to-remove list
            selected.erase(selected.begin());
            // remove from selected items
            j = gChosenObjects.erase(j);
            no--;
            removed++;
        } else {
            // proceed
            i++;
            j++;
        }
    }
    // rebuild list
    rebuildList();
    myParent->update();
    return 1;
}



long
GUIDialog_GLChosenEditor::onCmdClear(FXObject*,FXSelector,void*)
{
    myList->clearItems();
    gChosenObjects.clear();
    return 1;
}



long
GUIDialog_GLChosenEditor::onCmdClose(FXObject*,FXSelector,void*)
{
    close(true);
    return 1;
}



FXbool
GUIDialog_GLChosenEditor::close(FXbool notify)
{
    return FXMainWindow::close(notify);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


