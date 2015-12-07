
/*
 * Copyright 2015 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#include "counterino.h"

#include <QtSerialPort/QtSerialPort>
#include <QtCore/QString>



Counterino::Counterino(QSerialPort& serialPort, QObject* parent):
QObject(parent),
m_serialPort()
{
  serialPort.setParent(this);
  m_serialPort=&serialPort;
  
  m_serialPort->setBaudRate(QSerialPort::Baud9600);
  m_serialPort->setDataBits(QSerialPort::Data8);
  m_serialPort->setParity(QSerialPort::NoParity);
  m_serialPort->setStopBits(QSerialPort::OneStop); 
  m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
}


Counterino::Counterino(const char* nameSerialPort, QObject* parent): 
QObject(parent),
m_serialPort(new QSerialPort( QString( nameSerialPort ) ) )
{
  m_serialPort->setBaudRate(QSerialPort::Baud9600);
  m_serialPort->setDataBits(QSerialPort::Data8);
  m_serialPort->setParity(QSerialPort::NoParity);
  m_serialPort->setStopBits(QSerialPort::OneStop); 
  m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
}


Counterino::Counterino(const QString nameSerialPort, QObject* parent): 
QObject(parent),
m_serialPort( new QSerialPort(nameSerialPort) )
{

  m_serialPort->setBaudRate(QSerialPort::Baud9600);
  m_serialPort->setDataBits(QSerialPort::Data8);
  m_serialPort->setParity(QSerialPort::NoParity);
  m_serialPort->setStopBits(QSerialPort::OneStop); 
  m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
}


Counterino::~Counterino()
{
  delete m_serialPort;
}

Counterino& Counterino::operator=(const Counterino& other)
{
}

bool Counterino::operator==(const Counterino& other) const
{
}

Counterino::Counterino(const Counterino& other): QObject()
{

}



int  Counterino::checkBuffer()
{
   
  if( m_serialPort ==0 || ! m_serialPort->isOpen() )
    return -1;
  
  QTextStream streamOut(stdout);
  char buffer[11];
  while(m_serialPort->waitForReadyRead(1000)){}
  m_serialPort->read(buffer,11);
  buffer[10]='\0';
  streamOut << "buffer: " << buffer << '|' << endl;
  // checkSum
  int tmp_checksum=0;
  for ( unsigned int i =0; i < 7;++i){
    tmp_checksum+=buffer[i];
    streamOut << "tmp_checksum: " << tmp_checksum << endl;
  }
  char * check=&buffer[7];
  if( tmp_checksum%256 != atoi(check)){
   streamOut << "atoi(&buffer[1]): " << atoi(&buffer[1]) << endl;
   return atoi(&buffer[1]); 
  }
  
  // check Error Code
  const char *NoErrorCode="%000000069";
  
  bool isErrorCode=0;
  streamOut << "isErrorCode:NoErrorCode[i]:buffer[i]" << endl;
  for( int i=0; i< 10 ; ++i){
    isErrorCode+= ( NoErrorCode[i] != buffer[i]) ;
    streamOut << isErrorCode << ':' << NoErrorCode[i] << ':' << buffer[i] << endl;
    if( isErrorCode )
      break;
    
  }
  if( isErrorCode ){
    streamOut << "atoi( &buffer[1])" << atoi( &buffer[1]) << endl;
    return atoi( &buffer[1]); 
  }
    
  return 0;
    
}




// Command

int Counterino::initialize()
{
  if( (m_serialPort == 0 ) || !( m_serialPort->open(QIODevice::ReadWrite) ) )
    return -1;
  
  
  m_serialPort->write("INIT\n");
  unsigned int ErrorCode=0;
  if( ( ErrorCode = this->checkBuffer() )!= 0  )
    return -ErrorCode;
  
  
  m_serialPort->write("COMPUTER\n");
  if( ( ErrorCode = this->checkBuffer() ) != 0  )
    return -ErrorCode;
  
  m_serialPort->write("ENABLE_REMOTE\n");
  if( ( ErrorCode = this->checkBuffer() ) != 0  )
    return -ErrorCode;
  
  return 0;
}


int Counterino::reset()
{
m_serialPort->write("INIT\n");
  
  unsigned int ErrorCode=0;
  if( ErrorCode = this->checkBuffer() != 0  )
    return -ErrorCode;
  
  return 0;
}


int Counterino::start()
{
m_serialPort->write("START\n");
  
  unsigned int ErrorCode=0;
  if( ErrorCode = this->checkBuffer() != 0  )
    return -ErrorCode;
  
  return 0;
}

int Counterino::stop()
{
  m_serialPort->write("STOP\n");
  
  unsigned int ErrorCode=0;
  if( ErrorCode = this->checkBuffer() != 0  )
    return -ErrorCode;
  
  return 0;

}

int Counterino::getCount()
{
  QTextStream streamOut(stdout);
  
  streamOut << "getCount" << endl;
  m_serialPort->readAll();
  m_serialPort->write("SHOW_COUNT\n");
  m_serialPort->flush();
  m_serialPort->clear();
  char char_Counts[9];                  
  
  
  while( m_serialPort->waitForReadyRead(1000) ){}
  streamOut << m_serialPort->read(char_Counts,9) << endl;
   
  streamOut << "char_Counts: " << char_Counts << '|' << endl;
  char_Counts[8]='\0';
  streamOut << "char_Counts: " << char_Counts << '|' << endl;
  unsigned counts=atoi(char_Counts);
  unsigned int ErrorCode=0;
  if( ErrorCode = this->checkBuffer() != 0  )
    return -ErrorCode;
  
  return counts;

}



int Counterino::clear()
{
  m_serialPort->write("CLEAR_ALL\n");
  
  
  unsigned int ErrorCode=0;
  if( ErrorCode = this->checkBuffer() != 0  )
    return -ErrorCode;
  
  return 0;
}


