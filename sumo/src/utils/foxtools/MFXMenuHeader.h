#ifndef MFXMenuHeader_h
#define MFXMenuHeader_h

#include <fx.h>

class MFXMenuHeader : public FXMenuCommand {
public:
    MFXMenuHeader(FXComposite* p, FXFont *fnt, const FXString& text,FXIcon* ic=NULL,
        FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=0);
    ~MFXMenuHeader();
};


#endif

