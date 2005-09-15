#ifndef MFXImageHelper_h
#define MFXImageHelper_h

#ifdef HAVE_CONFIG_H
#include <config.h>
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

