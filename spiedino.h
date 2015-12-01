#ifndef SPIEDINO_H
#define SPIEDINO_H

#include <QtCore/QtCore>
#include <QtCore/QDateTime>
#include <QtSerialPort/QSerialPort>

class Spiedino : public QObject  //FIXME: la faccio ereditare da Serial port o da QOBject?
{
  Q_OBJECT
  
public:
  Spiedino(QSerailPort serialPort = 0,QObject *parent=0);
  
  Spiedino(const QTextStream nameSerialPort, QObject *parent=0);
  
  Spiedino(const char* nameSerialPort, QObject *parent=0);

  ~Spiedino(); 
  
 
signals:

  void readyPerCentLine( int duty, double mean, double sigma);  // È stata letta una stringa con flag %
  
  void readyEsclamationLine( unsigned long time, unsigned int RawMeasure); //È stata letta una stringa con flag !
  
  void foundEndDataBlock(); // Si è ricevuto il flasg $ di fine blocco Dati
  
  void scanStarted(); // È stata avviato il comando Scan
  
  void acquireStarted(unsigned int iLenghtOfSample ); // È stato avviato il comando Acquire
  
  void warmUpStarted(); // È stato avviato il comando warmUp
  
  void ready(); // L'interfaccia è pronta
  
  void halt();
 
private slots:
  
  void updateFirstBuffer();
  
  void updateScanCache(int duty, double mean, double sigma);
  
  void updateAcquireCache( unsigned long time, unsigned int rawMeasure);
    
  void disconnetCache();
  
public:
  
  struct ScanData{
    QVector< int > duty;
    QVector< double > mean;
    QVector< double > sigma;
  };
  
  struct AcquireData{
    QVector< unsigned long > time;
    QVector< int > rawMeasure;
  };
  
  
  /**
   * @brief slots that comunicate via Serial Port to set the Duty Cicle equal to duty
   * 
   * @param iDutyCicle value of duty cicle that have to set.
   * @return void
   */
  void setDuty(unsigned int iDutyCicle );
  
  /**
   * @brief slot that start the auto-scanning on all range of duty Cicle, for each value of
   *     the duty it recive a string like: 'duty:mean:sigma#'. The mean is made from a 
   *    Sample of 1000 elements, sampled with a frequency of 10 kHz FIXME: quanto è la frequnza
   *    
   * 
   * @return void
   */
  void scan();
  
  /**
   * @brief slot that ask via serial a periodic raw measure, user can set limit on the number 
   * of the samples, if iLenghtOfSample == 0 then there is no number limit.
   * 
   * @param iLenghtOfSample value of the limit to set on the scan, if 0 sample =inf
   * @return void
   */
  void acquire(unsigned int iLenghtOfSample);
  
  void warmUp();
  
  void stop();
  
  void reset();
  
  void quit();
  
  bool handShake(QString synText);
  
  void clearScanCache();
  
  void clearAcquireCache();
  
  const ScanData *getScanCache();
  
  const AcquireData *getAcquireCache();
  
  bool isReady();               //TODO FARE MOLTA ATTENZIONE A COME LA SI SCRIVE
  
  
  
private:
  
  QByteArray m_firstBuffer;  
  
  QSerialPort * m_serialPort;
  
  ScanData m_scanCache;
  
  AcquireData m_acquireCache;

  bool m_handshakeDone;  
};





#endif //Spiedino.h
