/****************************************************************************
** Form interface generated from reading ui file 'd:\projects\sumo\sumo_about.ui'
**
** Created: Mon Mar 10 14:41:35 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef QABOUTSUMO_H
#define QABOUTSUMO_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QPushButton;

class QAboutSUMO : public QDialog
{
    Q_OBJECT

public:
    QAboutSUMO( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~QAboutSUMO();

    QLabel* PixmapLabel2;
    QLabel* PixmapLabel3;
    QLabel* TextLabel5;
    QLabel* TextLabel2;
    QPushButton* PushButton1;
    QLabel* TextLabel1;
    QLabel* TextLabel3;
    QLabel* TextLabel4;

protected:
    bool event( QEvent* );
};

#endif // QABOUTSUMO_H
