#ifndef GUISelectionLoader_h
#define GUISelectionLoader_h

#include <string>

class GUISelectionLoader {
public:
    static void loadSelection(const std::string &file);

protected:
    GUISelectionLoader();
    ~GUISelectionLoader();
};


#endif
