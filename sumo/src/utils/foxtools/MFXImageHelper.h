#ifndef MFXImageHelper_h
#define MFXImageHelper_h

#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <fx.h>

class MFXImageHelper {
public:
    static FXbool saveimage(FXApp *a, const std::string& file,
        int width, int height, FXColor *data);


};


#endif

