#ifndef GUINetBuilder_h
#define GUINetBuilder_h

#include <netload/NLNetBuilder.h>

class OptionsCont;
class GUINet;

class GUINetBuilder : public NLNetBuilder
{
public:
    GUINetBuilder(const OptionsCont &oc);
    ~GUINetBuilder();
    GUINet *buildGUINet();
};


#endif
