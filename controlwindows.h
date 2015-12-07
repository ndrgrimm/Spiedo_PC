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

#ifndef CONTROLWINDOWS_H
#define CONTROLWINDOWS_H

#include "counterino.h"
#include "spiedino.h"
#include "ui_controlwindows.h"

#include <qt5/QtWidgets/QWidget>

#include <QtCore/QTimer>

namespace Ui
{
class ControlWindows;
}

class ControlWindows : public QWidget
{
    Q_OBJECT
public:
  
  ControlWindows();
  ControlWindows(const ControlWindows& other);
  ~ControlWindows();
  
  void setDevices( Spiedino * spiedino, Counterino *counterino);

  
signals:
  
  void endOfSet();

private slots:
  
  void start();
  
  void endStep();
  
  void startStep();
  
  void Save();
  
  void update();
  

private:
Ui::ControlWindows* m_ui;

unsigned int m_DeltaT;
unsigned int m_dutyStep;
double m_pol;

unsigned int m_duty;

Spiedino *m_spiedino;;
Counterino *m_counterino;

QByteArray m_cache;
QTimer m_timer;
QTimer m_clock;

};

#endif // CONTROLWINDOWS_H
