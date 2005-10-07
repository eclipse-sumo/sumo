#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <fx.h>
#include "MFXImageHelper.h"
#include <sstream>

#include <cassert>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

using namespace std;

// smell: yellow (the save functions may have additional options, not regarded)
// Save file
FXbool
MFXImageHelper::saveimage(FXApp *a, const std::string& file,
        int width, int height, FXColor *data)
{
    FXString ext=FXFile::extension(file.c_str());
    FXFileStream stream;
    if(!stream.open(file.c_str(), FXStreamSave)){
        return false;
    }
    FXImage *img=NULL;
    if(comparecase(ext,"gif")==0){
        return fxsaveGIF(stream,
            data, width, height, false /* !!! "fast" */);
    } else if(comparecase(ext,"bmp")==0){
        return fxsaveBMP(stream,
            data, width, height);
    } else if(comparecase(ext,"xpm")==0){
        return fxsaveXPM(stream,
            data, width, height);
    } else if(comparecase(ext,"pcx")==0){
        return fxsavePCX(stream,
            data, width, height);
    } else if(comparecase(ext,"ico")==0 || comparecase(ext,"cur")==0){
        return fxsaveICO(stream,
            data, width, height);
    } else if(comparecase(ext,"tga")==0){
        return fxsaveTGA(stream,
            data, width, height);
    } else if(comparecase(ext,"rgb")==0){
        return fxsaveRGB(stream,
            data, width, height);
    } else if(comparecase(ext,"xbm")==0){
        return fxsaveXBM(stream,
            data, width, height);
    }
#ifdef HAVE_PNG_H
    else if(comparecase(ext,"png")==0){
        return fxsavePNG(stream,
            data, width, height);
    }
#endif
#ifdef HAVE_JPEG_H
    else if(comparecase(ext,"jpg")==0){
        return fxsaveJPG(stream,
            data, width, height);
    }
#endif
#ifdef HAVE_TIFF_H
    else if(comparecase(ext,"tif")==0 || comparecase(ext,"tiff")==0){
        return fxsaveTIF(stream,
            data, width, height);
    }
#endif
    else {
        throw 1;
    }

    throw 1; // not yet implemented
}


