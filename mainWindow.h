#ifndef MAINWINDOWS_H
#define MAINWINDOWS_H

#include <QtCore/QtCore>
#include <QtCore/QDateTime>
#include <QtWidgets/QMainWindow>
#include <QWidget>



#include <QtSerialPort/QSerialPort>

#include "selectionWindows.h"

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
  /**
   * @brief This signal notify that the user has give the Read Command
   * 		to get a Measure form the Photodiode. Its get a 
   * 		string like: ' duty:Mean:Sigma#" get from 1000 sample.
   * @brief This signal notify that the user has give the Read Command
   * 		to get a Measure form the Photodiode. Its get a 
   * 		string like: ' duty:Mean:Sigma#" get from 1000 sample.
   * 
   * @return void
   */
  void recivedReadCommand();
  /**
   * @brief This signal notify that the user has give the Write Command
   * 		to set a duty cicle equal to iDutyCicle
   * 
   * @param iDutyCicle value of duty cicle that have to set.
   * 
   * @return void
   */
  void recivedtWriteCommand(int iDutyCicle);
  /**
   * @brief This signal notify that the user get the Auto Command
   * 		to get a run a read Command on all duty value
   * 		(form 0 to 255), getting a string like 
   * 		'duty:Mean:Sigma#' for each value.
   * 
   * @return void
   */
  void recivedAutoCommand();
  /**
   * @brief This signal notify that the user has given the scan Command
   * 		to get a raw and periodic mesure form the the photodiode
   * 		that are limited by a Lenght equal to iLenghtOfSample that
   * 		set a Time limit if isLenghtATimer is true, if not set a 
   * 		limit on the number of Sample.
   * 
   * @param iLenghtOfSample value of the limit to set on the scan
   * @param isLenghtATimer is the limit value a time limit?( if not, the limit is on the samples' number)
   * @return void
   */
  void recivedScanCommand( int iLenghtOfSample, bool isLenghtATimer);
  
  void blockingInterface();
  
  void unBlockingInterface();
  
private slots:
  
  /**
   * @brief slots that comunicate via Serial Port to set the Duty Cicle equal to iDutyCicle
   * 
   * @param iDutyCicle value of duty cicle that have to set.
   * @return void
   */
  void SetDuty(int iDutyCicle);
  /**
   * @brief slot that ask via Serial a measure from the photo-diode and recive a string like
   *	'duty:mean:sigma#'. The mean is made from a Sample of 1000 elements, sampled with a
   * 	frequency of 10kHz FIXME:Quanto è la frequenza?
   * 
   * @return void
   */
  void Read();
  /**
   * @brief slot that start the auto-scanning on all range of duty Cicle, for each value of
   *	 the duty it recive a string like: 'duty:mean:sigma#'. The mean is made from a 
   * 	Sample of 1000 elements, sampled with a frequency of 10 kHz FIXME: quanto è la frequnza
   * 	
   * 
   * @return void
   */
  void Auto();
  /**
   * @brief slot that ask via serial a periodic raw measure, user can set limit on the time of sampling or a 
   * 	limit on the number of the samples, via isLenghtATimer boolean variable.
   * 
   * @param iLenghtOfSample value of the limit to set on the scan
   * @param isLenghtATimer is the limit value a time limit?( if not, the limit is on the samples' number)
   * @return void
   */
  void Scan(int iLenghtOfSample, bool isLenghtATimer);
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
  void updateDisplay();
  
  void m_close();
  void save();
  
  
  void connectSerialPort( QSerialPort *serialPortSelected);
  
  void updateData();
  
  void blockInterface();
  
  void unBlockInterface();
  
private:
  
  bool m_interfaceIsBlocked;
  
  static QDateTime sm_datelog;
  static QTextStream sm_streamlog;
  
  
  int m_indexData1;
  int m_indexData2;
  QSerialPort * m_serialPort;
  Ui::MainWindow * m_ui;
  SelectionWindows * m_selectionWindow;
  QByteArray m_Data;

};
#endif //MainWindows.h