#ifndef MFXImageHelper_h
#define MFXImageHelper_h

#include <string>
#include <vector>
#include <fx.h>

class MFXImageHelper {
public:
    static FXbool saveimage(FXApp *a, const std::string& file,
        int width, int height, FXColor *data);


};


#endif

