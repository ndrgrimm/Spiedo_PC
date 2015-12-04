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

#ifndef COUNTERINO_H
#define COUNTERINO_H

#include <QtCore/QObject>
#include <QtSerialPort/QtSerialPort>



class Counterino : public QObject
{
    Q_OBJECT

public:
  
Counterino(QSerialPort &serialPort, QObject * parent=0);

Counterino(const QString nameSerialPort, QObject *parent=0);

Counterino(const char* nameSerialPort, QObject *parent=0);

Counterino(const Counterino& other);

~Counterino();


public:
  
  
  int initialize();
  
  int start();            // La funzione ritorna 0 se OK o il codice errore con segno negativo

  int stop();             // La funzione ritorna 0 se OK o il codice errore con segno negativo

  int reset();            // La funzione ritorna 0 se OK o il codice errore con segno negativo

  int clear();             // La funzione ritorna 0 se OK o il codice errore con segno negativo
  
  int getCount();         // La funzione ritorna un numero posito pari al conteggio  se OK o il codice errore con segno negativo




private:
  
  QSerialPort *m_serialPort;
  
   int checkBuffer();
  
  
  
Counterino& operator=(const Counterino& other);
bool operator==(const Counterino& other) const;
};

#endif // COUNTERINO_H
