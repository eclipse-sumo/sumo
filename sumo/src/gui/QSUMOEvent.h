#ifndef QSUMOEvent_h
#define QSUMOEvent_h

#include <qevent.h>
#include "GUIEvents.h"

class QSUMOEvent : public QEvent {
private:
    GUIEvent _ownType;
public:
    QSUMOEvent(GUIEvent ownType) 
        : QEvent(QEvent::User), _ownType(ownType) { }
    ~QSUMOEvent() { }
    GUIEvent getOwnType() const { return _ownType; }
};

#endif
