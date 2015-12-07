#ifndef MAINWINDOWS_H
#define MAINWINDOWS_H

#include <QtCore/QtCore>
#include <QtCore/QDateTime>
#include <QtWidgets/QMainWindow>
#include <QWidget>



#include <QtSerialPort/QSerialPort>

#include "selectionWindows.h"
#include "selectionCounterWindows.h"
#include "controlwindows.h"

#include "spiedino.h"
#include "counterino.h"


#include "ui_mainWindow.h"




namespace Ui {
  class MainWindow;
}


class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
   MainWindow(QWidget *parent=0);
  ~MainWindow();
signals:
   
  void blockingInterface();
  
  void unBlockingInterface();
  
  void enableControl();
  
  
  void blockingSpiedoInterface();
  void unBlockingSpiedoInterface();
  
  void blockingCounterInterface();
  void unBlockingCounterInterface();
  
  
  
  void somethingToSave();

 
  
private slots:
   
  /**
   * @brief slot that update the plot
   * 
   * @return void
   */
  void updatePlot();
  
  /**
   * @brief slot that update the Lcd and del status bar.
   * 
   * @return void
   */
  void updateDisplay( int duty, double mean, double sigma);
  
  void m_close();
  
  void save( QByteArray*CacheToSave);
  
  void addReader();
  
  void addCounter();
  
  void openControl();

  void connectSpiedino( Spiedino *spiedinoSelected );
  
  void connectCounterino( Counterino *counterinoSelected );
  
  void activeControl();
  
  void askForSave();
  
  void disconnectCache();
 
  
  
  
  
  
  void blockInterface();
  
  void unBlockInterface();
  
  void blockCounterInterface();
  
  void blockSpiedoInterface();
  
  void unBlockCounterInterface();
  
  void unBlockSpiedoInterface();
  
  
  // Buttom
  
  // Spiedo
  void Scan();
  
  void Acquire();
  
  void SetDuty();
  
  void WarmUp();
  
  void StopAcquire();
  
  //Counterino
  
  void StartCount();
  
  void StopCount();
  
  
private:
  
  bool m_interfaceIsBlocked;
  bool isAllConnect;
  
  static QDateTime sm_datelog;
  static QTextStream sm_streamlog;
  
  
  
  SelectionWindows *m_selectionReaderWindow;
  SelectionCounterWindows * m_selectionCounterWindow;
  ControlWindows *m_controlWindows;
  
  Spiedino * m_spiedino;
  Counterino *m_counterino;
  
  QByteArray m_scanByteCache;
  unsigned int m_indexScanCache;
  const Spiedino::ScanData *m_scanCache;
  
  QByteArray m_acquireByteCache;
  unsigned int m_indexAcquireCache;
  const Spiedino::AcquireData * m_acquireCache;
  
  QTimer *m_counterinoTimer;
  QTimer *m_spiedinoTimer;
  
  Ui::MainWindow * m_ui;
  


};
#endif //MainWindows.h