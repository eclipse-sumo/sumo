#ifndef GUIBaseColorer_h
#define GUIBaseColorer_h

template<class _T>
class GUIBaseColorer {
public:
    GUIBaseColorer() { }
    virtual ~GUIBaseColorer() { }
    virtual void setGlColor(const _T& i) const = 0;
};


#endif
