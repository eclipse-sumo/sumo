#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MFXMenuHeader.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

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
