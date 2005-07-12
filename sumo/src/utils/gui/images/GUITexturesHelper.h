#ifndef GUITexturesHelper_h
#define GUITexturesHelper_h

#include <fx.h>
#include "GUITextures.h"



class GUITexturesHelper {
public:
    static void init(FXApp *a);
    static void drawTexturedBox(GUITexture which, double size);
    static void drawTexturedBox(GUITexture which,
        double sizeX1, double sizeY1, double sizeX2, double sizeY2);
    static void close();

private:
    static void assignTextures();

private:
    static bool myWasInitialised;
    static FXApp *myApp;
    static size_t myTextureIDs[TEXTURE_MAX];
    static FXImage *myTextures[TEXTURE_MAX];

};


#endif
