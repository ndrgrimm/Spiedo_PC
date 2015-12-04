#include "spiedino.h"


#include <QtSerialPort/QtSerialPort>
#include <QtCore/QtCore>
#include <QtCore/QString>
#include <QtCore/QByteArray>

#include <iostream>


Spiedino::Spiedino(const char* nameSerialPort, QObject* parent):
QObject(parent),
m_scanCache(),
m_acquireCache(),
m_serialPort( new QSerialPort( QString(nameSerialPort), this ) )
{
  
  m_handshakeDone=false;
  m_serialPort->setBaudRate(QSerialPort::Baud115200);
  m_serialPort->setDataBits(QSerialPort::Data8);
  m_serialPort->setParity(QSerialPort::NoParity);
  m_serialPort->setStopBits(QSerialPort::OneStop); 
  m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
}

Spiedino::Spiedino(QSerialPort& serialPort, QObject* parent): 
QObject(parent),
m_scanCache(),
m_acquireCache(),
m_serialPort()
{
  serialPort.setParent(this);
  m_serialPort=&serialPort;
  m_handshakeDone=false;

  m_serialPort->setBaudRate(QSerialPort::Baud115200);
  m_serialPort->setDataBits(QSerialPort::Data8);
  m_serialPort->setParity(QSerialPort::NoParity);
  m_serialPort->setStopBits(QSerialPort::OneStop); 
  m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
}


Spiedino::Spiedino(const QString nameSerialPort, QObject* parent): 
QObject(parent),
m_scanCache(),
m_acquireCache(),
m_serialPort( new QSerialPort(nameSerialPort,this) )
{
  m_handshakeDone=false;

  m_serialPort->setBaudRate(QSerialPort::Baud115200);
  m_serialPort->setDataBits(QSerialPort::Data8);
  m_serialPort->setParity(QSerialPort::NoParity);
  m_serialPort->setStopBits(QSerialPort::OneStop); 
  m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
}


int Spiedino::handShake(QString synText)
{
  if( (m_serialPort == 0 ) || !( m_serialPort->open(QIODevice::ReadWrite) ) )
    return -1;
   
//   m_serialPort->open(QIODevice::ReadWrite);
  
  this->reset();
  
  QThread::sleep(1);
  QByteArray Data;
  char buf[1000];
  int leghtbuf=0;
  
  
  bool first=true;
  while( ( m_serialPort->waitForReadyRead(10000) ) ){
    
    if( m_serialPort->canReadLine() ){
      
      leghtbuf=m_serialPort->readLine(buf,sizeof(buf));
      if( first){
        first=false;
        
      }else{
        break;
      }
        
    }
  }
  
  
  
  QString sketchCode(buf);
  
  unsigned int limitCicle= (sketchCode.size() < synText.size())? sketchCode.size(): synText.size();
  
  bool resultHandshake=0;
  for( int i=0 ;  i< limitCicle; ++i){ // Attenzione alla lunghezza delle due stringhe
    resultHandshake+=!(sketchCode.at(i)==synText.at(i) );
    if( resultHandshake )
      break;
  }
  
  
  resultHandshake=( resultHandshake == 0 );
  
  if( resultHandshake ){
    
    m_serialPort->write("~");
    m_serialPort->flush();
    m_serialPort->clear();
    emit ready();
    m_handshakeDone=true;
    
    connect( m_serialPort, SIGNAL( readyRead()         ),       this, SLOT( updateFirstBuffer() ) );
    connect( this,         SIGNAL( foundEndDataBlock() ),       this, SLOT( disconnectCache()   ) );
  }
  
  return resultHandshake;
}


bool Spiedino::isReady()
{
  
  return  m_handshakeDone &&  (m_serialPort != 0 )  && m_serialPort->isReadable();
}


Spiedino::~Spiedino()
{
  
  delete m_serialPort;

}




void Spiedino::updateFirstBuffer()
{
 QByteArray tmp_data;
 QByteArray *buffer;
 
 QTextStream out(stdout);
 tmp_data.append( m_serialPort->readAll() ); 
 out << "tmp_data: " << tmp_data << endl;
 bool isFirst=true;
 for( unsigned int i=0 ; i < tmp_data.size(); ++i){
   switch ( tmp_data.at(i) ){
     
     case '%':{
       double mean=0;
       double sigma=0;
       int duty=0;
       
       if( isFirst){
         buffer=new QByteArray( m_firstBuffer.append( tmp_data.mid( 0, i+1 ) ) );
         m_firstBuffer.clear();
         isFirst=false;
         
       }else{
         buffer=new QByteArray( tmp_data.mid( 0, i+1 ) );

       }
       buffer->replace(':',' ');
       buffer->replace('$','\n');
       QTextStream out(buffer);
       out >> duty >> mean >> sigma;
      
      
       emit readyPerCentLine( duty, mean, sigma );
       tmp_data.remove(0,i+1);
       i=-1;
       delete buffer;
       break;
     }
     case '!':{

       unsigned long time;
       unsigned int rawMeasure;
       
       if( isFirst){
         buffer= new QByteArray( m_firstBuffer.append( tmp_data.mid( 0, i+1 ) ) );
         m_firstBuffer.clear();
         isFirst=false;
         }else{
           buffer= new QByteArray( tmp_data.mid( 0, i+1 ) ) ;
           
        }
        buffer->replace(':',' ');
        buffer->replace('!','\n');
        
        QTextStream out(buffer);
        out >> time >> rawMeasure;
        emit readyEsclamationLine( time, rawMeasure);
        tmp_data.remove(0,i+1);
        i=-1;
        delete buffer;
        break;
     }  

     case '$':
       tmp_data.remove(i,1);
       emit foundEndDataBlock();
       i=-1;
     default:
       ;
   }
 
 }
 m_firstBuffer.append(tmp_data);
 
  
  
}

void Spiedino::updateAcquireCache( unsigned long time, unsigned int rawMeasure)
{ 
  m_acquireCache.time.append(time);
  m_acquireCache.rawMeasure.append( rawMeasure );

}

void Spiedino::updateScanCache(int duty, double mean, double sigma)
{
  
  m_scanCache.duty.append(duty);
  m_scanCache.mean.append(mean);
  m_scanCache.sigma.append(sigma);
   
}


void Spiedino::clearScanCache()
{
  m_scanCache.duty.clear();
  m_scanCache.mean.clear();
  m_scanCache.sigma.clear();
}


void Spiedino::clearAcquireCache()
{

  m_acquireCache.time.clear();
  m_acquireCache.rawMeasure.clear();
  
}


const Spiedino::ScanData* Spiedino::getScanCache()
{

  return &m_scanCache;
}

const Spiedino::AcquireData* Spiedino::getAcquireCache()
{

  return &m_acquireCache;
}

void Spiedino::disconnectCache()
{

  disconnect(this, SIGNAL( readyPerCentLine(int,double,double) ), this, SLOT( updateScanCache(int,double,double) ) );
  disconnect(this, SIGNAL( readyEsclamationLine(ulong,uint) ),    this, SLOT( updateAcquireCache(ulong,uint) ) );
}


void Spiedino::stop()
{
  m_serialPort->write("s");
  m_serialPort->flush();
  m_serialPort->clear();
  
}

void Spiedino::quit()
{
  m_serialPort->write("s");
  m_serialPort->flush();
  m_serialPort->clear();
  delete m_serialPort;
  m_handshakeDone=false;
  
}

void Spiedino::reset()
{

  if( this->isReady() ){
    m_serialPort->setDataTerminalReady( ! m_serialPort->isDataTerminalReady() );
    m_serialPort->setDataTerminalReady( ! m_serialPort->isDataTerminalReady() );
    
  }
}


void Spiedino::acquire(unsigned int iLenghtOfSample)
{
  
  connect( this, SIGNAL(readyEsclamationLine(ulong,uint)) , this, SLOT(updateAcquireCache(ulong,uint)));
  
  m_serialPort->write("t");
  m_serialPort->flush();
  m_serialPort->clear();
  QThread::sleep(1);
  
  if ( iLenghtOfSample == 0 ){
    m_serialPort->write( "g" );
  }else{
    m_serialPort->write( QString::number(iLenghtOfSample).toStdString().c_str() );
  }
  m_serialPort->flush();
  m_serialPort->clear();
  
}

void Spiedino::scan()
{
  
  connect( this, SIGNAL(readyPerCentLine(int,double,double)), this, SLOT(updateScanCache(int,double,double)) );
  
  m_serialPort->write("a");
  m_serialPort->flush();
  m_serialPort->clear();
}


void Spiedino::setDuty(unsigned int iDutyCicle)
{
  
  m_serialPort->write("w");
  m_serialPort->flush();
  m_serialPort->clear();
  QThread::sleep(1);
  m_serialPort->write( QString::number(iDutyCicle).toStdString().c_str() );
  m_serialPort->flush();
  m_serialPort->clear();
}

void Spiedino::warmUp()
{
  m_serialPort->write("r");
  m_serialPort->flush();
  m_serialPort->clear();
}




