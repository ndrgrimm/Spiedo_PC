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
  void serialSelected(QSerialPort* serial);
    
 
private slots:
  void updateTextBox(int index);
  void updateSerialList();
  void selectionSerial(bool);
  
private:
  
  QSerialPort *m_out; 
  QList<QSerialPortInfo> m_serialInfoList;
  Ui::SelectionWindows *m_ui;
  
};

#endif  //SelectionWindows.h

