#include "MFXMenuHeader.h"

MFXMenuHeader::MFXMenuHeader(FXComposite* p, FXFont *fnt,
                             const FXString& text,
                             FXIcon* ic, FXObject* tgt,
                             FXSelector sel,FXuint opts)
    : FXMenuCommand(p, text, ic, tgt, sel, opts)
{
    setFont(fnt);
}


MFXMenuHeader::~MFXMenuHeader()
{
}
