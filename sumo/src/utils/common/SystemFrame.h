#ifndef SystemFrame_h
#define SystemFrame_h

class OptionsCont;

class SystemFrame
{
public:
    static bool init(bool gui, OptionsCont *oc=0);
    static void close(OptionsCont *oc);
};

#endif
