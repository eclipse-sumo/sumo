#ifndef GUIApplicationWindow_h
#define GUIApplicationWindow_h

#include <string>
#include <iostream>
#include <qmainwindow.h>
#include <qworkspace.h>

class SUMOView;
class QPrinter;
class OptionsCont;
class GUINet;
class GUILoadThread;
class GUIRunThread;
class QSimulationLoadedEvent;
class QLabel;

class GUIApplicationWindow: public QMainWindow
{
    Q_OBJECT
public:
    GUIApplicationWindow();
    ~GUIApplicationWindow();
    void updateScreens();
private slots:
    void load();
    void print();
    void closeAllWindows();

    void start();
    void stop();
    void resume();
    void singleStep();

    void openNewWindow();

    void about();
    void aboutQt();

    void windowsMenuAboutToShow();
    void windowsMenuActivated( int id );
public slots:
    void setSimulationDelay(int value);
protected:
    bool event(QEvent *e);

private:
    void buildFileTools();
    void buildSimulationTools();
    void buildWindowsTools();
    void netLoaded(QSimulationLoadedEvent *ec);

    std::string _name;

    QPrinter *printer;
    QWorkspace* ws;
    QToolBar *fileTools;
    QToolBar *simTools;
    QToolBar *_winTools;
    QPopupMenu* windowsMenu;
    GUILoadThread *_loadThread;
    GUIRunThread *_runThread;
    QToolButton *_startSimButton, *_stopSimButton, *_resumeSimButton,
        *_singleStepButton;
    bool _wasStarted;
    QLabel *_simStepLabel;
};



#endif
