#ifndef GUIParameterTableWindow_h
#define GUIParameterTableWindow_h

#include <qdialog.h>

class QListViewItem;
class GUIGlObject;
class QListView;
class GUIApplicationWindow;
class QPaintEvent;
class QListViewItem;
class QEvent;
class QResizeEvent;

class GUIParameterTableWindow : public QDialog
{
    Q_OBJECT
public:
    GUIParameterTableWindow( GUIApplicationWindow *app,
        QWidget * parent, GUIGlObject *o );

    ~GUIParameterTableWindow();

protected:
    bool event ( QEvent *e );
    void resizeEvent ( QResizeEvent * );

protected:

    GUIGlObject *myObject;
    QListView *myTable;
    double *myParameter;
    double *myParameterBuffer;
    GUIApplicationWindow *myApplication;
    QListViewItem **myItems;
};


#endif
