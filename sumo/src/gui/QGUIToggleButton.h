#ifndef QGUIToggleButton_h
#define QGUIToggleButton_h

#include <qtoolbutton.h>

class QGUIToggleButton : public QToolButton {
    Q_OBJECT
public:
    QGUIToggleButton(const QPixmap & pm, const QString & textLabel,
        const QString & grouptext, QObject * receiver,
        const char * slot, QToolBar * parent, const char * name,
        bool isOn)
        : QToolButton(pm, textLabel, grouptext, receiver, slot, parent, name)
    {
        setToggleButton(true);
        setOn(isOn);
    }

    ~QGUIToggleButton() { }

    void publicSetOn(bool value) 
    {
        setOn(value);
    }
};

#endif
