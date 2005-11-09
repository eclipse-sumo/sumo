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
// Revision 1.6  2005/11/09 06:47:05  dkrajzew
// subwindows are now deleted on (re)loading the simulation
//
// Revision 1.5  2005/10/07 11:44:53  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 12:18:59  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/07/12 11:55:37  dkrajzew
// fonts are now drawn using polyfonts; dialogs have icons; searching for structures improved;
//
// Revision 1.2  2005/05/04 09:14:54  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.1  2004/12/16 12:12:59  dkrajzew
// first steps towards loading of selections between different applications
//
// Revision 1.3  2004/12/07 11:43:48  dksumo
// first steps towards reading of selected items
//
// Revision 1.2  2004/11/22 12:27:56  dksumo
// using the right class of the derivation tree
//
// Revision 1.1  2004/10/22 12:49:03  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.5  2004/08/02 11:28:57  dkrajzew
// ported to fox 1.2
//
// Revision 1.4  2004/07/02 08:08:32  dkrajzew
// global object selection added
//
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
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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
#include <microsim/MSVehicle.h>
#include <guisim/GUIEdge.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
    setIcon( GUIIconSubSys::getIcon(ICON_APP_SELECTOR) );
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
    for(std::vector<size_t>::const_iterator i=chosen.begin(); i!=chosen.end(); ++i) {
        GUIGlObject *object = gIDStorage.getObjectBlocking(*i);
        if(object!=0) {
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
    if(gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if(opendialog.execute()){
        gCurrentFolder = opendialog.getDirectory().text();
        string file = string(opendialog.getFilename().text());
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
    if(gCurrentFolder.length()!=0) {
        opendialog.setDirectory(gCurrentFolder.c_str());
    }
    if(opendialog.execute()){
        gCurrentFolder = opendialog.getDirectory().text();
        string file = string(opendialog.getFilename().text());
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
    for(i=0; i<no; i++) {
        if(myList->getItem(i)->isSelected()) {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


