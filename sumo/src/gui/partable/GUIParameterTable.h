#ifndef GUIParameterTable_h
#define GUIParameterTable_h

#include <qlistview.h>

class GUIApplicationWindow;
class GUIGlObject;

class GUIParameterTable : public QListView
{
    Q_OBJECT
public:
    GUIParameterTable( GUIApplicationWindow *app,
        GUIGlObject *o, QWidget * parent = 0, const char *name = 0 );
    ~GUIParameterTable();
protected:
    void contentsMousePressEvent( QMouseEvent * e );
//    void contentsMouseReleaseEvent( QMouseEvent * e );


signals:
    void mySelectionChanged();
    void mySelectionChanged( QListViewItem* );

private:
	GUIGlObject *myObject;
    QListViewItem* selected;
    GUIApplicationWindow *myApplication;
};


#endif

