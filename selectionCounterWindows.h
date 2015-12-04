#ifndef SELECTIONWINDOWS_H
#define	SELECTIONWINDOWS_H



#include <QtCore>

#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>


#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include "ui_selectionCounterWindows.h"

#include "counterino.h"

namespace Ui {
class SelectionCounterWindows;
}


class SelectionCounterWindows: public QWidget
{
  Q_OBJECT
public:
  explicit SelectionCounterWindows(QWidget* parent=0);
  ~SelectionCounterWindows();
  
  
signals:
  void serialCounterSelected(Counterino* counterinoOut);
    
 
private slots:
  void updateTextBox(int index);
  void updateSerialList();
  void selectionSerial(bool);
  
private:
  
  Counterino *m_CounterinoOut; 
  QList<QSerialPortInfo> m_serialInfoList;
  Ui::SelectionCounterWindows *m_ui;
  
};

#endif  //SelectionCounterWindows.h

