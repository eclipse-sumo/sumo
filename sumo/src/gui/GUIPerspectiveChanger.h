#ifndef GUIPerspectiveChanger_h
#define GUIPerspectiveChanger_h

class QMouseEvent;
class GUIViewTraffic;

class GUIPerspectiveChanger {
protected:
    GUIViewTraffic &_callback;
public:
    GUIPerspectiveChanger(GUIViewTraffic &callBack);
    ~GUIPerspectiveChanger();
    virtual void mouseMoveEvent ( QMouseEvent * );
    virtual void mousePressEvent ( QMouseEvent * );
    virtual void mouseReleaseEvent ( QMouseEvent * );
private:
    void setZoomingFactor(double zoom);
    void setPosition(double xpos, double ypos);
    void setRotation(double rotation);
};

#endif
