#include "GUIDetectorWrapper.h"
#include <gui/popup/QGLObjectPopupMenuItem.h>
#include <gui/popup/QGLObjectPopupMenu.h>
#include <gui/GUISUMOAbstractView.h>
#include <gui/partable/GUIParameterTableWindow.h>




GUIDetectorWrapper::GUIDetectorWrapper(GUIGlObjectStorage &idStorage,
                                       std::string id)
                                       : GUIGlObject(idStorage, id)
{
}


GUIDetectorWrapper::~GUIDetectorWrapper()
{
}

QGLObjectPopupMenu *
GUIDetectorWrapper::getPopUpMenu(GUIApplicationWindow &app,
                                 GUISUMOAbstractView &parent)
{
    QGLObjectPopupMenu *ret = new QGLObjectPopupMenu(app, parent, *this);
    int id = ret->insertItem(
        new QGLObjectPopupMenuItem(ret, getFullName().c_str(), true));
    ret->insertSeparator();
    // add view options
    id = ret->insertItem("Center", ret, SLOT(center()));
    ret->insertSeparator();
    id = ret->insertItem("Show Parameter", ret, SLOT(showPars()));
    ret->setItemEnabled(id, TRUE);
    // add views adding options
    ret->insertSeparator();
    id = ret->insertItem("Open ValueTracker");
    ret->setItemEnabled(id, FALSE);
    // add simulation options
    ret->insertSeparator();
    id = ret->insertItem("Close");
    ret->setItemEnabled(id, FALSE);
    return ret;
}


/*
Boundery
GUIDetectorWrapper::getBoundery() const
{
    return Boundery();
}

*/
