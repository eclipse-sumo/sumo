#ifndef GUISelectionLoader_h
#define GUISelectionLoader_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <string>

class GUISelectionLoader {
public:
    static void loadSelection(const std::string &file);

protected:
    GUISelectionLoader();
    ~GUISelectionLoader();
};


#endif
