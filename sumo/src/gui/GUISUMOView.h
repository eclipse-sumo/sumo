#ifndef GUISUMOView_h
#define GUISUMOView_h

#include <string>
#include <vector>
#include <qmainwindow.h>
#include <qworkspace.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Boundery.h>
#include "GUIChooser.h"

class GUINet;
class QToolBar;
class GUIViewTraffic;
class QGUIToggleButton;

class GUISUMOView: public QMainWindow
{
    Q_OBJECT
public:
    GUISUMOView( QWidget* parent, const char* name, int wflags, GUINet &net );
    ~GUISUMOView();

    void load( const QString& fn );
    void save();
    void saveAs();
    void print( QPrinter* );

    void setView(GUIChooser::ChooseableArtifact type, const std::string &name);
    double getZoomingFactor() const;
    void setZoomingFactor(double val);
    bool showLegend() const;
    bool allowRotation() const;
public slots:
    void chooseJunction();
    void chooseEdge();
    void chooseVehicle();
    void recenterView();
    void toggleShowLegend();
    void toggleAllowRotation();
    void toggleBehaviour1();
    void toggleBehaviour2();
    void toggleBehaviour3();
//    void update();
protected:
    bool event(QEvent *e);
//    virtual void paintEvent ( QPaintEvent * ) ;
signals:
    void message(const QString&, int );

private:
    void buildSettingTools();
    void buildViewTools();
    void buildTrackingTools();
    void showValues(GUIChooser::ChooseableArtifact type, 
        std::vector<std::string> &names);

    double _zoomingFactor;
    QString filename;
    GUIViewTraffic *_view;
    QToolBar *_settingsTools;
    QToolBar *_viewTools;
    QToolBar *_trackingTools;
    QGUIToggleButton *_showLegendToggle, *_allowRotationToggle;
    QGUIToggleButton *_behaviourToggle1, *_behaviourToggle2, 
        *_behaviourToggle3;
    bool _showLegend;
    bool _allowRotation;
    GUIChooser *_chooser;
};


#endif
