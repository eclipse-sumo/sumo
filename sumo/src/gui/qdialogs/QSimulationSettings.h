/****************************************************************************
** Form interface generated from reading ui file 'SimulationSettings.ui'
**
** Created: Tue Aug 19 11:19:33 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef QSIMULATIONSETTINGS_H
#define QSIMULATIONSETTINGS_H

#include <qvariant.h>
#include <qdialog.h>
#include <string>


class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTabWidget;
class QToolButton;
class QWidget;

class QSimulationSettings : public QDialog
{
    Q_OBJECT

public:
    QSimulationSettings( QWidget* parent = 0);
    ~QSimulationSettings();

    QPushButton* PushButton9;
    QPushButton* PushButton10;
    QTabWidget* TabWidget2;
    QWidget* tab;
    QLabel* TextLabel2;
    QLabel* TextLabel3;
    QSpinBox* BeginSpin;
    QSpinBox* EndSpin;
    QLabel* TextLabel1;
    QLineEdit* ConfigurationNameEdit;
    QToolButton* ConfigurationNameChooser;
    QWidget* tab_2;
    QLabel* TextLabel3_2;
    QLabel* TextLabel2_2;
    QLabel* TextLabel1_2;
    QLineEdit* NetworkNameEdit;
    QToolButton* NetworkNameChooser;
    QToolButton* RoutesNameChooser;
    QLineEdit* AdditionalNameEdit;
    QToolButton* AdditionalNameChooser;
    QLineEdit* RoutesNameEdit;
    QWidget* tab_3;
    QLabel* TextLabel4_2;
    QLabel* TextLabel6;
    QLabel* TextLabel5;
    QLineEdit* DumpNameEdit;
    QLineEdit* DumpIntervalsEdit;
    QLineEdit* RawNameEdit;
    QToolButton* RawNameChooser;
    QToolButton* DumpBaseChooser;
    QWidget* tab_4;
    QPushButton* PushButton3;
    QCheckBox* EnableActions;

public slots:
    virtual void pressedDumpChooser();
    virtual void pressedAdditionalChooser();
    virtual void pressedCancel();
    virtual void pressedConfigChooser();
    virtual void pressedEditActions();
    virtual void pressedNetworkChooser();
    virtual void pressedOK();
    virtual void pressedRawChooser();
    virtual void pressedRoutesChooser();

private:

    bool changed();
    bool validate();
    bool procSaveSettings();
    bool changed(const std::string &name, int val);
    bool changed(const std::string &name, const std::string &val);
    std::string getStringOptionSecure(const std::string &name);
    int getIntOptionSecure(const std::string &name);

    bool saveNewConfig();

};

#endif // QSIMULATIONSETTINGS_H
