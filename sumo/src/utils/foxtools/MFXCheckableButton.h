#ifndef MFXCheckableButton_h
#define MFXCheckableButton_h

#include <fx.h>

class MFXCheckableButton : public FXButton
{
    FXDECLARE(MFXCheckableButton)
public:
    MFXCheckableButton(bool amChecked, FXComposite* p,const FXString& text,
        FXIcon* ic=NULL,FXObject* tgt=NULL,FXSelector sel=0,
        FXuint opts=BUTTON_NORMAL,
        FXint x=0,FXint y=0,FXint w=0,FXint h=0,
        FXint pl=DEFAULT_PAD,FXint pr=DEFAULT_PAD,FXint pt=DEFAULT_PAD,FXint pb=DEFAULT_PAD);
    ~MFXCheckableButton();
    bool amChecked() const;
    void setChecked(bool val);
    long onPaint(FXObject*,FXSelector,void*);
    long onUpdate(FXObject*,FXSelector,void*);


private:
    void buildColors();
    void setColors();

private:
    bool myAmChecked;
    FXColor myBackColor, myDarkColor, myHiliteColor, myShadowColor;

    bool myAmInitialised;

protected:
    MFXCheckableButton() { }
};

#endif


