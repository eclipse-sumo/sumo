#include <iostream>
#include <qpopupmenu.h>
#include "GUISUMOAbstractView.h"
#include "GUIGlObject.h"
#include "QGLObjectPopupMenu.h"
#include "partable/GUIParameterTableWindow.h"

#ifndef WIN32
#include "QGLObjectPopupMenu.moc"
#endif

using namespace std;

QGLObjectPopupMenu::QGLObjectPopupMenu(GUIApplicationWindow *app,
                                       GUISUMOAbstractView *parent,
                                       GUIGlObject *o)
    : QPopupMenu(parent), myParent(parent), myObject(o),
    myApplication(app)
{
}


QGLObjectPopupMenu::~QGLObjectPopupMenu()
{
}


void
QGLObjectPopupMenu::center()
{
    myParent->centerTo(myObject->getType(), myObject->microsimID());
}


void
QGLObjectPopupMenu::showPars()
{
    GUIParameterTableWindow *win =
        new GUIParameterTableWindow(
            myApplication,
            myParent, myObject);
}
