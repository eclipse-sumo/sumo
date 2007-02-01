/****************************************************************************/
/// @file    GUIDialog_GLChosenEditor.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 11.03.2004
/// @version $Id: $
///
// Editor for the  list of chosen objects
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
#include <vector>
#include <iostream>
#include <fstream>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <guisim/GUINet.h>
#include "GUIDialog_GLChosenEditor.h"
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/gui/images/GUIIconSubSys.h>

#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <guisim/GUIEdge.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GUIDialog_GLChosenEditor) GUIDialog_GLChosenEditorMap[]=
    {
        FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,       GUIDialog_GLChosenEditor::onCmdLoad),
        FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,       GUIDialog_GLChosenEditor::onCmdSave),
        FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_DESELECT,   GUIDialog_GLChosenEditor::onCmdDeselect),
        FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,      GUIDialog_GLChosenEditor::onCmdClear),
        FXMAPFUNC(SEL_COMMAND,  MID_CANCEL,             GUIDialog_GLChosenEditor::onCmdClose),
    };

FXIMPLEMENT(GUIDialog_GLChosenEditor, FXMainWindow, GUIDialog_GLChosenEditorMap, ARRAYNUMBER(GUIDialog_GLChosenEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================
GUIDialog_GLChosenEditor::GUIDialog_GLChosenEditor(GUIMainWindow *parent,
        GUISelectedStorage *str)
        : FXMainWindow(gFXApp, "List of Selected Items", NULL, NULL, DECOR_ALL, 20,20,300, 300),
        myParent(parent), myStorage(str)
{
    myStorage->add2Update(this);
    FXHorizontalFrame *hbox =
        new FXHorizontalFrame(this, LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,
                              0,0,0,0);
    // build the list
    myList = new FXList(hbox, 0, 0,
                        LAYOUT_FILL_X|LAYOUT_FILL_Y|LIST_MULTIPLESELECT);
    rebuildList();
    // build the layout
    FXVerticalFrame *layout = new FXVerticalFrame(hbox, LAYOUT_TOP,0,0,0,0,
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
    setIcon(GUIIconSubSys::getIcon(ICON_APP_SELECTOR));
    myParent->addChild(this);
}


GUIDialog_GLChosenEditor::~GUIDialog_GLChosenEditor()
{
    myStorage->remove2Update(this);
    myParent->removeChild(this);
}


void
GUIDialog_GLChosenEditor::rebuildList()
{
    myList->clearItems();
    const std::vector<size_t> &chosen = gSelected.getAllSelected();
    for (std::vector<size_t>::const_iterator i=chosen.begin(); i!=chosen.end(); ++i) {
        GUIGlObject *object = gIDStorage.getObjectBlocking(*i);
        if (object!=0) {
            std::string name = object->getFullName();
            myList->appendItem(name.c_str());
            gIDStorage.unblockObject(*i);
        }
    }
}


long
GUIDialog_GLChosenEditor::onCmdLoad(FXObject*,FXSelector,void*)
{
    // get the new file name
    FXFileDialog opendialog(this,"Open List of Selected Items");
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("*.txt");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory().text();
        string file = opendialog.getFilename().text();
        myParent->loadSelection(file);
        rebuildList();
        update();
    }
    return 1;
}


long
GUIDialog_GLChosenEditor::onCmdSave(FXObject*,FXSelector,void*)
{
    // get the new file name
    FXFileDialog opendialog(this,"Save List of selected Items");
    opendialog.setSelectMode(SELECTFILE_ANY);
    opendialog.setPatternList("*.txt");
    if (gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if (opendialog.execute()) {
        gCurrentFolder = opendialog.getDirectory().text();
        string file = opendialog.getFilename().text();
        gSelected.save(-1, file);
    }
    return 1;
}


long
GUIDialog_GLChosenEditor::onCmdDeselect(FXObject*,FXSelector,void*)
{
    size_t no = myList->getNumItems();
    vector<size_t> selected;
    size_t i;
    // remove items from list
    std::vector<size_t> chosen = gSelected.getAllSelected();
    std::vector<size_t>::iterator j = chosen.begin();
    for (i=0; i<no; i++) {
        if (myList->getItem(i)->isSelected()) {
            gSelected.deselect(-1, *j);
            selected.push_back(i);
        } else {
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
    gSelected.clear();
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



/****************************************************************************/

