/********************************************************************************
*                                                                               *
*       R e a l - V a l u e d   S p i n n e r / D i a l    W i d g e t          *
*                                                                               *
*********************************************************************************
* Copyright (C) 2004 by Bill Baxter.         All Rights Reserved.               *
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************
* $Id$                   *
********************************************************************************/
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxkeys.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXRegistry.h"
#include "FXAccelTable.h"
#include "FXApp.h"
#include "FXLabel.h"
#include "FXTextField.h"
#include "FXDial.h"
#include "FXRealSpinDial.h"

#include <float.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS
/*
  Notes:
  - Based originally on Lyle's FXSpinner.
  - Can use with spin buttons, dial, or both, and with or without text
  - Three increment levels, fine, normal, and coarse.  Access different modes
    with CONTROL key (fine control) and SHIFT key (coarse mode).  Modifiers
    affect all of up/down keys, mousewheel, dial and spinbuttons.
  - Can specify display format for text either as a precision,showExponent pair
    or an sprintf format string.  (String format can include extra text like '$'!)
  - Wheel mouse increment/decrement in even multiples of fine/norm/coarse scales.
    (Key modifers sometimes require mouse motion to kick in because FOX doesn't
     have a [public] way to query the key state asynchronously.  Hacked extern to
     FOX's internal WIN32 function for querying this, so it works on Win32)
  - Dial warps the pointer at the edge of the screen so you don't run out of
    screen real estate.
*/
#define DIALINCR    160
#define DIALMULT    40
#define DIALWIDTH   12
#define BUTTONWIDTH 12
#define GAPWIDTH     1

#define INTMAX  2147483647
#define INTMIN  (-INTMAX-1)

#define SPINDIAL_MASK (SPINDIAL_CYCLIC|SPINDIAL_NOTEXT|SPINDIAL_NOBUTTONS|SPINDIAL_NODIAL|SPINDIAL_NOMAX|SPINDIAL_NOMIN|SPINDIAL_LOG)

using namespace FX;

/*******************************************************************************/
/*  Custom FXDial subclass                                                     */
/*******************************************************************************/
namespace FX {
class FXRealSpinDialDial : public FXDial {
    FXDECLARE(FXRealSpinDialDial)
protected:
    FXRealSpinDialDial() {}
private:
    FXRealSpinDialDial(const FXRealSpinDialDial&);
    FXRealSpinDialDial &operator=(const FXRealSpinDialDial&);
public:
    //long onDefault(FXObject*,FXSelector,void* );
    long onKey(FXObject*,FXSelector,void*);
    long onButtonPress(FXObject*,FXSelector,void*);
    long onButtonRelease(FXObject*,FXSelector,void*);
    long onRightButtonPress(FXObject*,FXSelector,void*);
    long onRightButtonRelease(FXObject*,FXSelector,void*);
    long onMotion(FXObject*,FXSelector,void*);
    long onAuto(FXObject*,FXSelector,void*);
    enum {
        ID_AUTOSPIN=FXDial::ID_LAST,
        ID_LAST
    };
public:

    /// Construct a dial widget
    FXRealSpinDialDial(FXComposite *p,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=DIAL_NORMAL,
                       FXint x=0,FXint y=0,FXint w=0,FXint h=0,
                       FXint pl=DEFAULT_PAD,FXint pr=DEFAULT_PAD,FXint pt=DEFAULT_PAD,FXint pb=DEFAULT_PAD):
            FXDial(p,tgt,sel,opts,x,y,w,h,pl,pr,pt,pb) {}

};

FXDEFMAP(FXRealSpinDialDial) FXSpinDialMap[]={
    FXMAPFUNC(SEL_KEYPRESS,0,  FXRealSpinDialDial::onKey),
    FXMAPFUNC(SEL_KEYRELEASE,0,  FXRealSpinDialDial::onKey),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,0,  FXRealSpinDialDial::onButtonPress),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,0,  FXRealSpinDialDial::onRightButtonRelease),
    FXMAPFUNC(SEL_RIGHTBUTTONPRESS,0,  FXRealSpinDialDial::onRightButtonPress),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE,0,  FXRealSpinDialDial::onButtonRelease),
    FXMAPFUNC(SEL_MOTION,0,  FXRealSpinDialDial::onMotion),

    FXMAPFUNC(SEL_TIMEOUT,FXRealSpinDialDial::ID_AUTOSPIN, FXRealSpinDialDial::onAuto),

    //FXMAPFUNC(SEL_KEYPRESS,0,  FXRealSpinDialDial::onKeyPress),
    //FXMAPFUNC(SEL_KEYRELEASE,0,FXRealSpinDialDial::onKeyRelease),
};
FXIMPLEMENT(FXRealSpinDialDial,FXDial,FXSpinDialMap,ARRAYNUMBER(FXSpinDialMap))
//FXIMPLEMENT(FXRealSpinDialDial,FXDial,0,0)

//long FXRealSpinDialDial::onDefault(FXObject*o,FXSelector s,void*p )
//{
//  printf("DEFAULT!\n");
//  if (target) return target->handle(o,s,p);
//  return 0;
//}
long FXRealSpinDialDial::onKey(FXObject*o,FXSelector s,void*p) {
    if (target) return target->handle(o,s,p);
    return 0;
}
long FXRealSpinDialDial::onButtonPress(FXObject*o,FXSelector s,void*p) {
    grabKeyboard();
    return FXDial::onLeftBtnPress(o,s,p);
}
long FXRealSpinDialDial::onButtonRelease(FXObject*o,FXSelector s,void*p) {
    ungrabKeyboard();
    return FXDial::onLeftBtnRelease(o,s,p);
}
long FXRealSpinDialDial::onRightButtonPress(FXObject* /*o*/,FXSelector /*s*/,void*p) {
    if (isEnabled()) {
        grab();
        grabKeyboard();
        //if(target && target->handle(this,FXSEL(SEL_RIGHTBUTTONPRESS,message),ptr)) return 1;
        FXEvent *event = (FXEvent*)p;
        if (options&DIAL_HORIZONTAL)
            dragpoint=event->win_x;
        else
            dragpoint=event->win_y;
        getApp()->addTimeout(this,ID_AUTOSPIN,getApp()->getScrollSpeed());
    }
    return 1;
}
long FXRealSpinDialDial::onRightButtonRelease(FXObject* /*o*/,FXSelector /*s*/,void* /*p*/) {
    ungrab();
    ungrabKeyboard();
    getApp()->removeTimeout(this,ID_AUTOSPIN);
    if (isEnabled()) {
        //if(target && target->handle(this,FXSEL(SEL_RIGHTBUTTONRELEASE,message),p)) return 1;
    }
    return 1;

}
long FXRealSpinDialDial::onAuto(FXObject* /*o*/,FXSelector /*s*/,void* /*p*/) {
    getApp()->addTimeout(this,ID_AUTOSPIN,getApp()->getScrollSpeed());
    setValue(getValue()+int((dragpoint-dragpos)/float(5)));
    int v = getValue();
    if (target) target->handle(this,FXSEL(SEL_CHANGED,message),&v);
    return 1;
}

long FXRealSpinDialDial::onMotion(FXObject*o,FXSelector s,void*p) {
    if (!isEnabled()) return 0;
    if (target && target->handle(this,FXSEL(SEL_MOTION,message),p)) return 1;

    FXbool bJump=FALSE;
    FXEvent *e = (FXEvent*)p;
    if (!(flags&FLAG_PRESSED)) { // not doing clickdrag
        dragpos = e->win_y;
    }
    FXWindow *rootWin= getApp()->getRootWindow();
    FXint x = e->root_x, y = e->root_y;
    if (e->root_x >= rootWin->getWidth()-1)  {
        x-=40;
        dragpoint-=40;
        bJump=TRUE;
    } else if (e->root_x <= 10)                {
        x+=40;
        dragpoint+=40;
        bJump=TRUE;
    }
    if (e->root_y >= rootWin->getHeight()-1) {
        y-=40;
        dragpoint-=40;
        bJump=TRUE;
    } else if (e->root_y <= 10)                {
        y+=40;
        dragpoint+=40;
        bJump=TRUE;
    }
    if (bJump) {
        rootWin->setCursorPosition(x,y);
        return 1;
    } else
        return FXDial::onMotion(o,s,p);
}

}

/*******************************************************************************/
/*  Custom FXArrowButton subclass                                              */
/*******************************************************************************/
namespace FX {
class FXRealSpinDialBtn : public FXArrowButton {
    FXDECLARE(FXRealSpinDialBtn)
protected:
    FXRealSpinDialBtn() {}
private:
    FXRealSpinDialBtn(const FXRealSpinDialBtn&);
    FXRealSpinDialBtn &operator=(const FXRealSpinDialBtn&);
public:
    //long onDefault(FXObject*,FXSelector,void* );
    long onKey(FXObject*,FXSelector,void*);
    long onButtonPress(FXObject*,FXSelector,void*);
    long onButtonRelease(FXObject*,FXSelector,void*);
    enum {
        ID_AUTOSPIN=FXDial::ID_LAST,
        ID_LAST
    };
public:

    /// Construct a dial widget
    FXRealSpinDialBtn(FXComposite* p,FXObject* tgt=NULL,FXSelector sel=0,
                      FXuint opts=ARROW_NORMAL,
                      FXint x=0,FXint y=0,FXint w=0,FXint h=0,
                      FXint pl=DEFAULT_PAD,FXint pr=DEFAULT_PAD,FXint pt=DEFAULT_PAD,FXint pb=DEFAULT_PAD):
            FXArrowButton(p,tgt,sel,opts,x,y,w,h,pl,pr,pt,pb) {}

};

FXDEFMAP(FXRealSpinDialBtn) FXSpinDialBtnMap[]={
    FXMAPFUNC(SEL_KEYPRESS,0,  FXRealSpinDialBtn::onKey),
    FXMAPFUNC(SEL_KEYRELEASE,0,  FXRealSpinDialBtn::onKey),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,0,  FXRealSpinDialBtn::onButtonPress),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE,0,  FXRealSpinDialBtn::onButtonRelease),


    //FXMAPFUNC(SEL_KEYPRESS,0,  FXRealSpinDialBtn::onKeyPress),
    //FXMAPFUNC(SEL_KEYRELEASE,0,FXRealSpinDialBtn::onKeyRelease),
};
FXIMPLEMENT(FXRealSpinDialBtn,FXArrowButton,FXSpinDialBtnMap,ARRAYNUMBER(FXSpinDialBtnMap))
//FXIMPLEMENT(FXRealSpinDialBtn,FXDial,0,0)

//long FXRealSpinDialBtn::onDefault(FXObject*o,FXSelector s,void*p )
//{
//  printf("DEFAULT!\n");
//  if (target) return target->handle(o,s,p);
//  return 0;
//}
long FXRealSpinDialBtn::onKey(FXObject*o,FXSelector s,void*p) {
    if (target) return target->handle(o,s,p);
    return 0;
}
long FXRealSpinDialBtn::onButtonPress(FXObject*o,FXSelector s,void*p) {
    grabKeyboard();
    return FXArrowButton::onLeftBtnPress(o,s,p);
}
long FXRealSpinDialBtn::onButtonRelease(FXObject*o,FXSelector s,void*p) {
    ungrabKeyboard();
    return FXArrowButton::onLeftBtnRelease(o,s,p);
}


}


/*******************************************************************************/
/*  FXTextField subclass                                                       */
/*******************************************************************************/

namespace FX {
class FXRealSpinDialText : public FXTextField {
    FXDECLARE(FXRealSpinDialText)
protected:
    FXRealSpinDialText() {}
private:
    FXRealSpinDialText(const FXRealSpinDialText&);
    FXRealSpinDialText &operator=(const FXRealSpinDialText&);
public:
    long onCmdSetRealValue(FXObject*,FXSelector,void*);
    long onMotion(FXObject*,FXSelector,void*);
    enum {
        ID_LAST=FXTextField::ID_LAST
    };
    enum {
        FLAG_FMTSTRING = 0x1
    };
public:

    /// Construct a text widget
    FXRealSpinDialText(FXComposite* p,FXint ncols,FXObject* tgt=NULL,FXSelector sel=0,
                       FXuint opts=TEXTFIELD_NORMAL,
                       FXint x=0,FXint y=0,FXint w=0,FXint h=0,FXint
                       pl=DEFAULT_PAD,FXint pr=DEFAULT_PAD,FXint pt=DEFAULT_PAD,FXint pb=DEFAULT_PAD
                      ) :
            FXTextField(p,ncols,tgt,sel,opts,x,y,w,h,pl,pr,pt,pb),
            precision(3),
            exponent(FALSE),
            flags(0) {}

    void setNumberFormat(FXint prec, FXbool bExp=FALSE) {
        precision=prec;
        exponent=bExp;
        flags &= ~FLAG_FMTSTRING;
    }
    FXint getNumberFormatPrecision() const {
        return precision;
    }
    FXbool getNumberFormatExponent() const {
        return exponent;
    }
    void setFormatString(const FXchar *fmt) {
        fmtString = fmt;
        flags |= FLAG_FMTSTRING;
    }
    FXString getNumberFormatString() const {
        return fmtString;
    }

protected:
    FXint precision;
    FXbool exponent;
    FXString fmtString;
    FXuint flags;
};

FXDEFMAP(FXRealSpinDialText) FXSpinDialTextMap[]={
    FXMAPFUNC(SEL_MOTION, 0,  FXRealSpinDialText::onMotion),
    FXMAPFUNC(SEL_COMMAND, FXWindow::ID_SETREALVALUE,  FXRealSpinDialText::onCmdSetRealValue),
};
FXIMPLEMENT(FXRealSpinDialText,FXTextField,FXSpinDialTextMap,ARRAYNUMBER(FXSpinDialTextMap))

long FXRealSpinDialText::onMotion(FXObject*o,FXSelector s,void*ptr) {
    // Forward motion events so we can monitor key state.  We don't get the modifier
    // keys themselves if we aren't focused, so this seems the best we can do.
    if (!isEnabled()) return 0;
    if (target && target->handle(this,FXSEL(SEL_MOTION,message),ptr)) return 1;
    return FXTextField::onMotion(o,s,ptr);
}
long FXRealSpinDialText::onCmdSetRealValue(FXObject* /*o*/,FXSelector /*s*/,void*ptr) {
    //  setText(FXStringVal(*((FXdouble*)ptr)));
    if (flags & FLAG_FMTSTRING)
        setText(FXStringFormat(fmtString.text(),*((FXdouble*)ptr)));
    else {
        setText(FXStringVal(*((FXdouble*)ptr),precision,exponent));
    }
    return 1;
}

}

/*******************************************************************************/
/*  FXRealSpinDial                                                          */
/*******************************************************************************/

namespace FX {

//  Message map
FXDEFMAP(FXRealSpinDial) FXRealSpinDialMap[]={
    FXMAPFUNC(SEL_KEYPRESS,0,  FXRealSpinDial::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,0,FXRealSpinDial::onKeyRelease),
    FXMAPFUNC(SEL_MOTION,0,FXRealSpinDial::onMotion),
    FXMAPFUNC(SEL_MOTION,FXRealSpinDial::ID_ENTRY,FXRealSpinDial::onMotion),
    FXMAPFUNC(SEL_MOTION,FXRealSpinDial::ID_DIAL,FXRealSpinDial::onMotion),
    FXMAPFUNC(SEL_UPDATE, FXRealSpinDial::ID_ENTRY,FXRealSpinDial::onUpdEntry),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_ENTRY,FXRealSpinDial::onCmdEntry),
    FXMAPFUNC(SEL_CHANGED,FXRealSpinDial::ID_ENTRY,FXRealSpinDial::onChgEntry),
    FXMAPFUNC(SEL_UPDATE, FXRealSpinDial::ID_DIAL,FXRealSpinDial::onUpdDial),
    FXMAPFUNC(SEL_CHANGED,FXRealSpinDial::ID_DIAL,FXRealSpinDial::onChgDial),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_DIAL,FXRealSpinDial::onCmdDial),
    FXMAPFUNC(SEL_MOUSEWHEEL,FXRealSpinDial::ID_ENTRY,FXRealSpinDial::onMouseWheel),
    FXMAPFUNC(SEL_MOUSEWHEEL,FXRealSpinDial::ID_DIAL,FXRealSpinDial::onMouseWheel),
    FXMAPFUNC(SEL_MOUSEWHEEL,FXRealSpinDial::ID_INCREMENT,FXRealSpinDial::onMouseWheel),
    FXMAPFUNC(SEL_MOUSEWHEEL,FXRealSpinDial::ID_DECREMENT,FXRealSpinDial::onMouseWheel),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_SETVALUE, FXRealSpinDial::onCmdSetValue),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_SETINTVALUE,FXRealSpinDial::onCmdSetIntValue),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_GETINTVALUE,FXRealSpinDial::onCmdGetIntValue),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_SETINTRANGE,FXRealSpinDial::onCmdSetIntRange),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_GETINTRANGE,FXRealSpinDial::onCmdGetIntRange),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_SETREALVALUE,FXRealSpinDial::onCmdSetRealValue),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_GETREALVALUE,FXRealSpinDial::onCmdGetRealValue),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_SETREALRANGE,FXRealSpinDial::onCmdSetRealRange),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_GETREALRANGE,FXRealSpinDial::onCmdGetRealRange),
    FXMAPFUNC(SEL_UPDATE, FXRealSpinDial::ID_INCREMENT,FXRealSpinDial::onUpdIncrement),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_INCREMENT,FXRealSpinDial::onCmdIncrement),
    FXMAPFUNC(SEL_UPDATE, FXRealSpinDial::ID_DECREMENT,FXRealSpinDial::onUpdDecrement),
    FXMAPFUNC(SEL_COMMAND,FXRealSpinDial::ID_DECREMENT,FXRealSpinDial::onCmdDecrement),
};


// Object implementation
FXIMPLEMENT(FXRealSpinDial,FXPacker,FXRealSpinDialMap,ARRAYNUMBER(FXRealSpinDialMap))


// Construct spinner out of two buttons and a text field
FXRealSpinDial::FXRealSpinDial() {
    flags=(flags|FLAG_ENABLED|FLAG_SHOWN)&~FLAG_UPDATE;
    textField=(FXRealSpinDialText*)-1L;
    dial=(FXDial*)-1L;
    upButton=(FXRealSpinDialBtn*)-1L;
    downButton=(FXRealSpinDialBtn*)-1L;
    range[0]=-DBL_MAX;
    range[1]= DBL_MAX;
    incr[0]=0.1;
    incr[1]=1.0;
    incr[2]=10;
    pos=1;
    dialpos=0;
}


// Construct spinner out of dial and a text field
FXRealSpinDial::FXRealSpinDial(FXComposite *p,FXint cols,FXObject *tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h,FXint pl,FXint pr,FXint pt,FXint pb):
        FXPacker(p,opts&~(FRAME_RIDGE),x,y,w,h,0,0,0,0,0,0) {
    flags=(flags|FLAG_ENABLED|FLAG_SHOWN)&~FLAG_UPDATE;
    target=tgt;
    message=sel;
    dial=new FXRealSpinDialDial(this,this,ID_DIAL,DIAL_VERTICAL, 0,0,0,0, 0,0,0,0);
    dial->setNotchSpacing(450);
    dial->setRevolutionIncrement(DIALINCR);
    upButton=new FXRealSpinDialBtn(this,this,ID_INCREMENT,FRAME_RAISED|FRAME_THICK|ARROW_UP|ARROW_REPEAT, 0,0,0,0, 0,0,0,0);
    downButton=new FXRealSpinDialBtn(this,this,ID_DECREMENT,FRAME_RAISED|FRAME_THICK|ARROW_DOWN|ARROW_REPEAT, 0,0,0,0, 0,0,0,0);
    textField=new FXRealSpinDialText(this,cols,this,ID_ENTRY,(opts&FRAME_RIDGE)|TEXTFIELD_REAL|JUSTIFY_RIGHT,0,0,0,0,pl,pr,pt,pb);
    textField->setText("0");
    range[0]=(options&SPINDIAL_NOMIN) ? -DBL_MAX : 0;
    range[1]=(options&SPINDIAL_NOMAX) ?  DBL_MAX : 100;
    dial->setRange(INTMIN,INTMAX);
    dialpos=dial->getValue();
    incr[0]=0.1;
    incr[1]=1.0;
    incr[2]=10;
    pos=0;
    keystate=0;
}


// Get default width
FXint FXRealSpinDial::getDefaultWidth() {
    FXint tw=0;
    if (!(options&SPINDIAL_NOTEXT)) tw=textField->getDefaultWidth();
    return tw+DIALWIDTH+GAPWIDTH+(border<<1);
}


// Get default height
FXint FXRealSpinDial::getDefaultHeight() {
    return textField->getDefaultHeight()+(border<<1);
}


// Create window
void FXRealSpinDial::create() {
    FXPacker::create();
}


// Enable the widget
void FXRealSpinDial::enable() {
    if (!(flags&FLAG_ENABLED)) {
        FXPacker::enable();
        textField->enable();
        dial->enable();
    }
}


// Disable the widget
void FXRealSpinDial::disable() {
    if (flags&FLAG_ENABLED) {
        FXPacker::disable();
        textField->disable();
        dial->disable();
    }
}


// Recompute layout
void FXRealSpinDial::layout() {
    FXint dialWidth,dialHeight,buttonWidth,buttonHeight,textHeight;

    textHeight=height-2*border;
    dialHeight=textHeight;
    buttonHeight=textHeight>>1;

    FXuint hideOpts = SPINDIAL_NOTEXT|SPINDIAL_NODIAL|SPINDIAL_NOBUTTONS;
    if ((options&hideOpts) == hideOpts) {
        flags&=~FLAG_DIRTY;
        return; // nothing to layout
    }

    FXint right = width-border;

    if (options&SPINDIAL_NOTEXT) {
        // Dial takes up the extra space if shown, otherwise spinbuttons
        if (!(options&SPINDIAL_NODIAL)) {
            // HAS DIAL
            int left=border;
            if (!(options&SPINDIAL_NOBUTTONS)) {
                FXint bw=BUTTONWIDTH;
                upButton->position(border,border,bw,buttonHeight);
                downButton->position(border,height-buttonHeight-border,bw,buttonHeight);
                left+=bw+GAPWIDTH;
            }
            dial->position(left,border,right-left,dialHeight);
        } else {
            upButton->position(border,border,right-border,buttonHeight);
            downButton->position(border,height-buttonHeight-border,right-border,buttonHeight);
        }
    } else {
        // dial/buttons are default width, text stretches to fill the rest
        dialWidth=buttonWidth=0;
        if (!(options&SPINDIAL_NODIAL)) {
            FXint w=DIALWIDTH;
            dial->position(right-w,border,w,dialHeight);
            right -= w+GAPWIDTH;
        }
        if (!(options&SPINDIAL_NOBUTTONS)) {
            FXint w=BUTTONWIDTH;
            upButton->position(right-w,border,w,buttonHeight);
            downButton->position(right-w,height-buttonHeight-border,w,buttonHeight);
            right -= w + GAPWIDTH;
        }
        textField->position(border,border,right-border,textHeight);
    }
    flags&=~FLAG_DIRTY;
}


// Respond to dial message
long FXRealSpinDial::onUpdDial(FXObject* sender,FXSelector,void*) {
    if (isEnabled() && ((options&SPINDIAL_CYCLIC) || (pos<=range[1])))
        sender->handle(this,FXSEL(SEL_COMMAND,ID_ENABLE),NULL);
    else
        sender->handle(this,FXSEL(SEL_COMMAND,ID_DISABLE),NULL);
    return 1;
}


// Respond to dial message
long FXRealSpinDial::onChgDial(FXObject* /*p*/,FXSelector /*sel*/,void* /*ptr*/) {
    if (!isEnabled()) return 0;
    FXdouble newpos;
    FXdouble inc;
    if (FXApp::instance()->getKeyState(CONTROLMASK))    inc = incr[0];
    else if (FXApp::instance()->getKeyState(SHIFTMASK)) inc = incr[2];
    else                         inc = incr[1];
    FXint dialdelta = dial->getValue()-dialpos;
    if (options&SPINDIAL_LOG) {
        newpos = pos * pow(inc , DIALMULT * FXdouble(dialdelta) / DIALINCR);
    } else {
        newpos = pos + DIALMULT * inc * (dialdelta) / DIALINCR;
    }
    // Now clamp newpos.
    if (dialdelta>0) {
        if (options&SPINDIAL_LOG) {
            if (options&SPINDIAL_CYCLIC && newpos>range[1]) {
                FXdouble lr0=log(range[0]), lr1=log(range[1]), lnp=log(newpos);
                newpos = exp(lr0 + fmod(lnp-lr0, lr1-lr0)) ;
            }
        } else {
            if (options&SPINDIAL_CYCLIC) {
                newpos = range[0] + fmod(newpos-range[0], range[1]-range[0]+1) ;
            }
        }
    } else {
        if (options&SPINDIAL_LOG) {
            if (options&SPINDIAL_CYCLIC  && newpos<range[0]) {
                FXdouble lr0=log(range[0]), lr1=log(range[1]), lpos=log(pos);
                FXdouble span=lr1-lr0;
                newpos = exp(lr0 + fmod(lpos-lr0+1+(span-inc), span));
            }
        } else {
            if (options&SPINDIAL_CYCLIC) {
                newpos = range[0] + fmod(pos+(range[1]-range[0]+1+(newpos-pos)-range[0]) , range[1]-range[0]+1);
            }
        }
    }
    setValue(newpos);
    if (target) target->handle(this,FXSEL(SEL_COMMAND,message),(void*)&pos);
    dialpos = dial->getValue();
    return 1;
}

// Respond to dial message
long FXRealSpinDial::onCmdDial(FXObject*,FXSelector /*sel*/,void*) {
    if (!isEnabled()) return 0;
    // if(target) target->handle(this,FXSEL(SEL_COMMAND,message),(void*)&pos);
    dialpos = dial->getValue() % DIALINCR;
    dial->setValue(dialpos);
    return 1;
}


// Respond to increment message
long FXRealSpinDial::onUpdIncrement(FXObject* sender,FXSelector,void*) {
    if (isEnabled() && ((options&REALSPIN_CYCLIC) || (pos<range[1])))
        sender->handle(this,FXSEL(SEL_COMMAND,ID_ENABLE),NULL);
    else
        sender->handle(this,FXSEL(SEL_COMMAND,ID_DISABLE),NULL);
    return 1;
}


// Respond to increment message
long FXRealSpinDial::onCmdIncrement(FXObject*,FXSelector,void*) {
    if (!isEnabled()) return 0;
    FXint mode;
    if (keystate&CONTROLMASK)    mode = SPINDIAL_INC_FINE;
    else if (keystate&SHIFTMASK) mode = SPINDIAL_INC_COARSE;
    else                         mode = SPINDIAL_INC_NORMAL;
    increment(mode);
    if (target) target->handle(this,FXSEL(SEL_COMMAND,message),(void*)&pos);
    return 1;
}


// Disable decrement if at low end already
long FXRealSpinDial::onUpdDecrement(FXObject* sender,FXSelector,void*) {
    if (isEnabled() && ((options&REALSPIN_CYCLIC) || (range[0]<pos)))
        sender->handle(this,FXSEL(SEL_COMMAND,ID_ENABLE),NULL);
    else
        sender->handle(this,FXSEL(SEL_COMMAND,ID_DISABLE),NULL);
    return 1;
}


// Respond to decrement message
long FXRealSpinDial::onCmdDecrement(FXObject*,FXSelector,void*) {
    if (!isEnabled()) return 0;
    FXint mode;
    if (keystate&CONTROLMASK)    mode = SPINDIAL_INC_FINE;
    else if (keystate&SHIFTMASK) mode = SPINDIAL_INC_COARSE;
    else                         mode = SPINDIAL_INC_NORMAL;
    decrement(mode);
    if (target) target->handle(this,FXSEL(SEL_COMMAND,message),(void*)&pos);
    return 1;
}



// Update from text field
long FXRealSpinDial::onUpdEntry(FXObject*,FXSelector,void*) {
    return target && target->handle(this,FXSEL(SEL_UPDATE,message),NULL);
}

long FXRealSpinDial::onMouseWheel(FXObject* /*o*/,FXSelector /*s*/,void*p) {
    FXint mode;
    keystate = ((FXEvent*)p)->state;
    if (keystate&CONTROLMASK)    mode = SPINDIAL_INC_FINE;
    else if (keystate&SHIFTMASK) mode = SPINDIAL_INC_COARSE;
    else                         mode = SPINDIAL_INC_NORMAL;
    if (((FXEvent*)p)->code>0) increment(mode);
    else decrement(mode);
    if (target) target->handle(this,FXSEL(SEL_COMMAND,message),(void*)&pos);
    return 1;
}

// Text field changed
long FXRealSpinDial::onChgEntry(FXObject*,FXSelector,void*) {
    register FXdouble value=FXDoubleVal(textField->getText());
    if (value<range[0]) value=range[0];
    if (value>range[1]) value=range[1];
    if (value!=pos) {
        pos=value;
        if (target) target->handle(this,FXSEL(SEL_CHANGED,message),(void*)&pos);
    }
    return 1;
}


// Text field command
long FXRealSpinDial::onCmdEntry(FXObject*,FXSelector,void*) {
    textField->setText(FXStringVal(pos));       // Put back adjusted value
    if (target) target->handle(this,FXSEL(SEL_COMMAND,message),(void*)&pos);
    return 1;
}


// Keyboard press
long FXRealSpinDial::onKeyPress(FXObject* sender,FXSelector sel,void* ptr) {
    FXEvent* event=(FXEvent*)ptr;
    if (!isEnabled()) return 0;
    keystate=event->state;
    if (target && target->handle(this,FXSEL(SEL_KEYPRESS,message),ptr)) return 1;
    FXint mode;
    if (keystate&CONTROLMASK)    mode = SPINDIAL_INC_FINE;
    else if (keystate&SHIFTMASK) mode = SPINDIAL_INC_COARSE;
    else                         mode = SPINDIAL_INC_NORMAL;
    switch (event->code) {
    case KEY_Up:
    case KEY_KP_Up:
        increment(mode);
        if (target) target->handle(this,FXSEL(SEL_COMMAND,message),(void*)&pos);
        return 1;
    case KEY_Down:
    case KEY_KP_Down:
        decrement(mode);
        if (target) target->handle(this,FXSEL(SEL_COMMAND,message),(void*)&pos);
        return 1;
    default:
        return textField->handle(sender,sel,ptr);
    }
    return 0;
}


// Keyboard release
long FXRealSpinDial::onKeyRelease(FXObject* sender,FXSelector sel,void* ptr) {
    FXEvent* event=(FXEvent*)ptr;
    if (!isEnabled()) return 0;
    keystate=event->state;
    if (target && target->handle(this,FXSEL(SEL_KEYRELEASE,message),ptr)) return 1;
    switch (event->code) {
    case KEY_Up:
    case KEY_KP_Up:
    case KEY_Down:
    case KEY_KP_Down:
        return 1;
    default:
        return textField->handle(sender,sel,ptr);
    }
    return 0;
}

// Mouse motion
long FXRealSpinDial::onMotion(FXObject* /*sender*/,FXSelector /*sel*/,void* ptr) {
    if (!isEnabled()) return 0;
    keystate=((FXEvent*)ptr)->state;
    return 0;
}

// Update value from a message
long FXRealSpinDial::onCmdSetValue(FXObject*,FXSelector,void* ptr) {
    setValue((FXdouble)(size_t)ptr);
    return 1;
}


// Update value from a message
long FXRealSpinDial::onCmdSetIntValue(FXObject*,FXSelector,void* ptr) {
    setValue(FXdouble(*((FXint*)ptr)));
    return 1;
}


// Obtain value from spinner
long FXRealSpinDial::onCmdGetIntValue(FXObject*,FXSelector,void* ptr) {
    *((FXint*)ptr)=(FXint)getValue();
    return 1;
}


// Update range from a message
long FXRealSpinDial::onCmdSetIntRange(FXObject*,FXSelector,void* ptr) {
    FXdouble lo = (FXdouble)((FXint*)ptr)[0];
    FXdouble hi = (FXdouble)((FXint*)ptr)[1];
    setRange(lo,hi);
    return 1;
}


// Get range with a message
long FXRealSpinDial::onCmdGetIntRange(FXObject*,FXSelector,void* ptr) {
    ((FXdouble*)ptr)[0] = range[0];
    ((FXdouble*)ptr)[1] = range[1];
    return 1;
}


// Update value from a message
long FXRealSpinDial::onCmdSetRealValue(FXObject*,FXSelector,void* ptr) {
    setValue(*((FXdouble*)ptr));
    return 1;
}


// Obtain value from spinner
long FXRealSpinDial::onCmdGetRealValue(FXObject*,FXSelector,void* ptr) {
    *((FXdouble*)ptr)=getValue();
    return 1;
}


// Update range from a message
long FXRealSpinDial::onCmdSetRealRange(FXObject*,FXSelector,void* ptr) {
    setRange(((FXdouble*)ptr)[0],((FXdouble*)ptr)[1]);
    return 1;
}


// Get range with a message
long FXRealSpinDial::onCmdGetRealRange(FXObject*,FXSelector,void* ptr) {
    getRange(((FXdouble*)ptr)[0],((FXdouble*)ptr)[1]);
    return 1;
}



// Increment spinner
void FXRealSpinDial::increment(FXint incMode) {
    FXdouble inc = incr[incMode+1];
    FXdouble newpos;
    if (range[0]<range[1]) {
        if (options&SPINDIAL_LOG) {
            newpos = pos * inc;
            if (options&SPINDIAL_CYCLIC && newpos>range[1]) {
                // can have a huge magnitude disparity here, so better to work in log space
                FXdouble lr0=log(range[0]), lr1=log(range[1]), lnp=log(newpos);
                newpos = exp(lr0 + fmod(lnp-lr0, lr1-lr0)) ;
            }
        } else {
            newpos = pos + inc;
            if (options&SPINDIAL_CYCLIC) {
                newpos = range[0] + fmod(newpos-range[0], range[1]-range[0]+1) ;
            }
        }
        setValue(newpos);
    }
}


// Decrement spinner
void FXRealSpinDial::decrement(FXint incMode) {
    FXdouble inc = incr[incMode+1];
    FXdouble newpos;
    if (range[0]<range[1]) {
        if (options&SPINDIAL_LOG) {
            newpos = pos / inc;
            if (options&SPINDIAL_CYCLIC  && newpos<range[0]) {
                // can have a huge magnitude disparity here, so better to work in log space
                FXdouble lr0=log(range[0]), lr1=log(range[1]), lpos=log(pos);
                FXdouble span=lr1-lr0;
                newpos = exp(lr0 + fmod(lpos-lr0+1+(span-inc), span));
            }
        } else {
            newpos = pos - inc;
            if (options&SPINDIAL_CYCLIC) {
                newpos = range[0] + fmod(pos+(range[1]-range[0]+1+(newpos-pos)-range[0]) , range[1]-range[0]+1);
            }
        }
        setValue(newpos);
    }
}

// True if spinner is cyclic
FXbool FXRealSpinDial::isCyclic() const {
    return (options&SPINDIAL_CYCLIC)!=0;
}


// Set spinner cyclic mode
void FXRealSpinDial::setCyclic(FXbool cyclic) {
    if (cyclic) options|=SPINDIAL_CYCLIC;
    else options&=~SPINDIAL_CYCLIC;
}


// Set spinner range; this also revalidates the position,
void FXRealSpinDial::setRange(FXdouble lo,FXdouble hi) {
    if (lo>hi) {
        fxerror("%s::setRange: trying to set negative range.\n",getClassName());
    }
    if (range[0]!=lo || range[1]!=hi) {
        range[0]=lo;
        range[1]=hi;
        setValue(pos);
    }
}


// Set new value
void FXRealSpinDial::setValue(FXdouble value) {
    if (value<range[0]) value=range[0];
    if (value>range[1]) value=range[1];
    if (pos!=value) {
        textField->handle(this,FXSEL(SEL_COMMAND,ID_SETREALVALUE), &value);
        pos=value;
    }
}


// Change value increment
void FXRealSpinDial::setIncrement(FXdouble inc) {
    if (inc<0) {
        fxerror("%s::setIncrement: negative or zero increment specified.\n",getClassName());
    }
    incr[1]=inc;
}
void FXRealSpinDial::setFineIncrement(FXdouble inc) {
    if (inc<0) {
        fxerror("%s::setIncrement: negative or zero increment specified.\n",getClassName());
    }
    incr[0]=inc;
}
void FXRealSpinDial::setCoarseIncrement(FXdouble inc) {
    if (inc<0) {
        fxerror("%s::setIncrement: negative or zero increment specified.\n",getClassName());
    }
    incr[2]=inc;
}
void FXRealSpinDial::setIncrements(FXdouble fine,FXdouble inc,FXdouble coarse) {
    if (inc<0) {
        fxerror("%s::setIncrement: negative or zero increment specified.\n",getClassName());
    }
    incr[0]=fine;
    incr[1]=inc;
    incr[2]=coarse;
}


// True if text supposed to be visible
FXbool FXRealSpinDial::isTextVisible() const {
    return textField->shown();
}


// Change text visibility
void FXRealSpinDial::setTextVisible(FXbool shown) {
    FXuint opts=shown?(options&~SPINDIAL_NOTEXT):(options|SPINDIAL_NOTEXT);
    if (options!=opts) {
        options=opts;
        recalc();
    }
}


// Set the font used in the text field|
void FXRealSpinDial::setFont(FXFont *fnt) {
    textField->setFont(fnt);
}


// Return the font used in the text field
FXFont *FXRealSpinDial::getFont() const {
    return textField->getFont();
}


// Set help text
void FXRealSpinDial::setHelpText(const FXString&  text) {
    textField->setHelpText(text);
    dial->setHelpText(text);
    upButton->setHelpText(text);
    downButton->setHelpText(text);
}


// Get help text
FXString FXRealSpinDial::getHelpText() const {
    return textField->getHelpText();
}


// Set tip text
void FXRealSpinDial::setTipText(const FXString&  text) {
    textField->setTipText(text);
    dial->setTipText(text);
    upButton->setTipText(text);
    downButton->setTipText(text);
}



// Get tip text
FXString FXRealSpinDial::getTipText() const {
    return textField->getTipText();
}


// Change spinner style
void FXRealSpinDial::setSpinnerStyle(FXuint style) {
    FXuint opts=(options&~SPINDIAL_MASK) | (style&SPINDIAL_MASK);
    if (options!=opts) {
        if (opts&SPINDIAL_NOMIN) range[0]=-DBL_MAX;
        if (opts&SPINDIAL_NOMAX) range[1]=DBL_MAX;
        options=opts;
        recalc();
    }
}


// Get spinner style
FXuint FXRealSpinDial::getSpinnerStyle() const {
    return (options&SPINDIAL_MASK);
}


// Allow editing of the text field
void FXRealSpinDial::setEditable(FXbool edit) {
    textField->setEditable(edit);
}


// Return TRUE if text field is editable
FXbool FXRealSpinDial::isEditable() const {
    return textField->isEditable();
}

// Change color of the dial
void FXRealSpinDial::setDialColor(FXColor clr) {
    dial->setBackColor(clr);
}

// Return color of the dial
FXColor FXRealSpinDial::getDialColor() const {
    return dial->getBackColor();
}

// Change color of the up arrow
void FXRealSpinDial::setUpArrowColor(FXColor clr) {
    upButton->setArrowColor(clr);
}

// Return color of the up arrow
FXColor FXRealSpinDial::getUpArrowColor() const {
    return upButton->getArrowColor();
}

// Change color of the down arrow
void FXRealSpinDial::setDownArrowColor(FXColor clr) {
    downButton->setArrowColor(clr);
}

// Return color of the the down arrow
FXColor FXRealSpinDial::getDownArrowColor() const {
    return downButton->getArrowColor();
}


// Change text color
void FXRealSpinDial::setTextColor(FXColor clr) {
    textField->setTextColor(clr);
}

// Return text color
FXColor FXRealSpinDial::getTextColor() const {
    return textField->getTextColor();
}

// Change selected background color
void FXRealSpinDial::setSelBackColor(FXColor clr) {
    textField->setSelBackColor(clr);
}

// Return selected background color
FXColor FXRealSpinDial::getSelBackColor() const {
    return textField->getSelBackColor();
}

// Change selected text color
void FXRealSpinDial::setSelTextColor(FXColor clr) {
    textField->setSelTextColor(clr);
}

// Return selected text color
FXColor FXRealSpinDial::getSelTextColor() const {
    return textField->getSelTextColor();
}

// Changes the cursor color
void FXRealSpinDial::setCursorColor(FXColor clr) {
    textField->setCursorColor(clr);
}

// Return the cursor color
FXColor FXRealSpinDial::getCursorColor() const {
    return textField->getCursorColor();
}

void FXRealSpinDial::setNumberFormat(FXint  prec, FXbool  bExp) {
    textField->setNumberFormat(prec,bExp);
}

FXint FXRealSpinDial::getNumberFormatPrecision() const {
    return textField->getNumberFormatPrecision();
}

FXbool FXRealSpinDial::getNumberFormatExponent() const {
    return textField->getNumberFormatExponent();
}

void FXRealSpinDial::setFormatString(const FXchar  *fmt) {
    textField->setFormatString(fmt);
}

FXString FXRealSpinDial::getNumberFormatString() const {
    return textField->getNumberFormatString();
}

// Save object to stream
void FXRealSpinDial::save(FXStream& store) const {
    FXPacker::save(store);
    store << textField;
    store << dial;
    store << upButton;
    store << downButton;
    store << range[0] << range[1];
    store << incr[0] << incr[1] << incr[2];
    store << pos;
}


// Load object from stream
void FXRealSpinDial::load(FXStream& store) {
    FXPacker::load(store);
    store >> textField;
    store >> dial;
    store >> upButton;
    store >> downButton;
    store >> range[0] >> range[1];
    store >> incr[0] >> incr[1] >> incr[2];
    store >> pos;
}


// Destruct spinner:- trash it!
FXRealSpinDial::~FXRealSpinDial() {
    textField=(FXRealSpinDialText*)-1L;
    dial=(FXDial*)-1L;
    upButton=(FXRealSpinDialBtn*)-1L;
    downButton=(FXRealSpinDialBtn*)-1L;
}

}


void
FXRealSpinDial::selectAll() {
    textField->selectAll();
}



const FXDial &
FXRealSpinDial::getDial() const {
    return *dial;
}


