#ifndef QGLObjectPopupMenu_h
#define QGLObjectPopupMenu_h

#include <qpopupmenu.h>

class GUISUMOAbstractView;
class GUIGlObject;
class GUIApplicationWindow;

class QGLObjectPopupMenu :
        public QPopupMenu
{
    Q_OBJECT

public:
    QGLObjectPopupMenu(GUIApplicationWindow *app,
        GUISUMOAbstractView *parent, GUIGlObject *o);
    ~QGLObjectPopupMenu();

public slots:
    void center();

    void showPars();

private:
    GUISUMOAbstractView *myParent; // !!! needed?
    GUIGlObject *myObject;
    GUIApplicationWindow *myApplication;
};

#endif

