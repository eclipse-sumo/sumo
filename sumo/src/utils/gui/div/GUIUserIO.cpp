/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)



/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "GUIUserIO.h"

#ifdef WIN32
#include <windows.h>


void
GUIUserIO::copyToClipboard(FXApp *app, const std::string &text)
{
    OpenClipboard((HWND) app->getRootWindow()->getFirst()->id());
    ::EmptyClipboard();
    HGLOBAL clipBuffer = GlobalAlloc(GMEM_DDESHARE, text.length()+1);
    char *buff = (char*)GlobalLock(clipBuffer);
    strcpy(buff, text.c_str());
    ::SetClipboardData(CF_TEXT, buff);
    CloseClipboard();
}


#else

void
GUIUserIO::copyToClipboard(FXApp *, const std::string &)
{
}

#endif
