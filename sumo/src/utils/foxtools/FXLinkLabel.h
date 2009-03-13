#ifndef FXLinkLabel_h
#define FXLinkLabel_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>

/** A label which points to a link.
* This class behaves very much like the FXLabel class but it
* points to some link which is started when clicked.
*/
class FXLinkLabel : public FXLabel {
protected:
    // FOX stuff
    FXDECLARE(FXLinkLabel)
    FXLinkLabel() {}
public:
    /// Constructor.
    FXLinkLabel(FXComposite* p,const FXString& text,FXIcon* ic=0,FXuint opts=LABEL_NORMAL,FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint pl=DEFAULT_PAD,FXint pr=DEFAULT_PAD,FXint pt=DEFAULT_PAD,FXint pb=DEFAULT_PAD);

    /// Destructor.
    virtual ~FXLinkLabel();

    // FOX enum
    enum {
        ID_FIRST = FXLabel::ID_LAST,
        ID_TIMER,

        ID_LAST
    };

    // FOX messages
    long onLeftBtnPress(FXObject*,FXSelector,void*);
    long onTimer(FXObject*,FXSelector,void*);
};




#endif
