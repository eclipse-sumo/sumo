/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include "FXLinkLabel.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


FXint fxexecute(FXString link) {
#ifdef WIN32
    FXString quoted = FXPath::enquote(link);
    FXint ret =
        (int)ShellExecute(NULL,"open",quoted.text(),NULL,NULL,SW_SHOW) > 32;
    return ret;
#else
    FXString ext = FXFile::extension(link);
    FXString list;
    if (comparecase(link.section(':',0), "http")==0 ||
            comparecase(link.section(':',0), "ftp")==0 ||
            comparecase(ext, "htm")==0 || comparecase(ext, "html")==0 ||
            comparecase(ext, "php")==0 || comparecase(ext, "asp")==0)
        list = "mozilla-firefox\tmozilla\tnetscape\tkonqueror\tdillo\tlynx";
    else if (comparecase(ext, "pdf")==0)
        list = "acroread\tkghostview\tgpdf\txpdf";

    if (list.length()) {
        FXString software;
        FXint index=0;
        FXString path = FXFile::getExecPath();

        software = list.section("\t",index);
        while (!software.empty()) {
            software = FXFile::search(path, software);
            if (software.length())
                return system(FXString().format("%s \"%s\" >/dev/null 2>&1 & ",
                                                software.text(),link.text()).text())>0?0:1;
            index++;
            software = list.section("\t",index);
        }
    } else if (FXFile::isExecutable(link))
        return system((link + " >/dev/null 2>&1 & ").text()) > 0 ? 0:1;
    return 0;
#endif
}



FXDEFMAP(FXLinkLabel) FXLinkLabelMap[]={
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,0,FXLinkLabel::onLeftBtnPress),
    FXMAPFUNC(SEL_TIMEOUT,FXLinkLabel::ID_TIMER,FXLinkLabel::onTimer),
};
FXIMPLEMENT(FXLinkLabel,FXLabel,FXLinkLabelMap,ARRAYNUMBER(FXLinkLabelMap))


FXLinkLabel::FXLinkLabel(FXComposite* p,const FXString& text,FXIcon* ic,FXuint opts,FXint x,FXint y,FXint w,FXint h,FXint pl,FXint pr,FXint pt,FXint pb) :FXLabel(p,text,ic,opts,x,y,w,h,pl,pr,pt,pb) {
    setDefaultCursor(getApp()->getDefaultCursor(DEF_HAND_CURSOR));
    setTextColor(FXRGB(0,0,255));
}

FXLinkLabel::~FXLinkLabel() {
    getApp()->removeTimeout(this,ID_TIMER);
}

long FXLinkLabel::onLeftBtnPress(FXObject*,FXSelector,void*) {
    FXString link = getTipText();
    if (link.length()) {
        getApp()->beginWaitCursor();
        if (fxexecute(link))
            getApp()->addTimeout(this,ID_TIMER,2000); // 2 seconds of way cursor
        else {
            getApp()->endWaitCursor();
            getApp()->beep();
        }
    }
    return 1;
}

long FXLinkLabel::onTimer(FXObject*,FXSelector,void*) {
    getApp()->endWaitCursor();
    return 1;
}
