#ifndef GUIChooser_h
#define GUIChooser_h

#include <qmainwindow.h>
#include <string>
#include <vector>

class GUISUMOView;
class QHBoxLayout;
class QListBox;

class GUIChooser : public QMainWindow
{
    Q_OBJECT
public:
    enum ChooseableArtifact { 
        CHOOSEABLE_ARTIFACT_JUNCTIONS,
        CHOOSEABLE_ARTIFACT_EDGES,
        CHOOSEABLE_ARTIFACT_VEHICLES
    };
private:
    std::string _id;
    ChooseableArtifact _type;
    GUISUMOView *_parent;
public:
    GUIChooser(GUISUMOView *parent, ChooseableArtifact type,
        std::vector<std::string> &names);
    ~GUIChooser();
    std::string getID() const;
    void closeEvent ( QCloseEvent * );
public slots:
    void pressedOK();
    void pressedCancel();
private:
    void buildList(QHBoxLayout *vbox, std::vector<std::string> &names);
    void buildButtons(QHBoxLayout *vbox);
    QListBox *_list;
};

#endif
