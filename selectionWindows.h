#ifndef SELECTIONWINDOWS_H
#define	SELECTIONWINDOWS_H



#include <QtCore>

#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>


#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include "ui_selectionWindows.h"

#include "spiedino.h"

namespace Ui {
class SelectionWindows;
}


class SelectionWindows: public QWidget
{
  Q_OBJECT
public:
  explicit SelectionWindows(QWidget* parent=0);
  ~SelectionWindows();
  
  
signals:
  void serialSelected(Spiedino* spiedinoOut);
    
 
private slots:
  void updateTextBox(int index);
  void updateSerialList();
  void selectionSerial(bool);
  
private:
  
  Spiedino *m_spiedoOut; 
  QList<QSerialPortInfo> m_serialInfoList;
  Ui::SelectionWindows *m_ui;
  
};

#endif  //SelectionWindows.h

