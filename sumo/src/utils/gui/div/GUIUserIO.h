#ifndef GUIUserIO_h
#define GUIUserIO_h

#include <string>
#include <fx.h>


class GUIUserIO {
public:
    static void copyToClipboard(FXApp *app, const std::string &text);
};

#endif
