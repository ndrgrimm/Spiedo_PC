#ifndef SELECTIONWINDOWS_H
#define	SELECTIONWINDOWS_H


#include <QObject>
#include <QtWidgets/QWidget>
#include <QtCore>

#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>

#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>


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
  void quit(QSerialPort*);
  
private:
   
  
  QListWidget m_serialList;
  QPushButton m_selectingButton;
  QLabel m_textBox;
  QList<QSerialPortInfo> m_serialInfoList;
};

#endif  //SelectionWindows.h

