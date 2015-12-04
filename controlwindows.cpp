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

#include "controlwindows.h"
#include "ui_controlwindows.h"
#include "QtWidgets/QFileDialog"


ControlWindows::ControlWindows()
{
    m_ui = new Ui::ControlWindows;
    m_ui->setupUi(this);
    
    m_timer.setSingleShot(false);
    m_clock.setSingleShot(false);
    m_clock.setInterval(2000);
    
    connect( m_ui->start, SIGNAL( clicked(bool) ),      this, SLOT( start() ));
    connect( &m_timer, SIGNAL( timeout() ),  this, SLOT( endStep()) );
    connect( this,     SIGNAL( endOfSet() ), this, SLOT(Save()) );
    
    
    
}

ControlWindows::ControlWindows(const ControlWindows& other)
{
    m_ui = new Ui::ControlWindows;
    m_ui->setupUi(this);
}

ControlWindows::~ControlWindows()
{
    delete m_ui;
}


void ControlWindows::getDevices(Spiedino* spiedino, Counterino* counterino)
{
m_spiedino=spiedino;
m_counterino=counterino;

}



void ControlWindows::start()
{
   
  m_ui->horizontalLayout->setEnabled(false);  // FIXME fare una funzione più precisa
  m_DeltaT = m_ui->spinIntegrationTime->value();
  m_dutyStep=m_ui->spinDutyStep->value();
  m_pol=m_ui->doubleSpinOutPol->value();
  
  m_timer.setInterval(m_DeltaT*1000);
  m_duty=0;
  m_spiedino->setDuty(m_duty);
  m_counterino->clear();
  
  
  
  
  startStep();
  
}

void ControlWindows::endStep()
{
 
  m_spiedino->stop();
  m_counterino->stop();
  disconnect( &m_clock, SIGNAL(timeout(QPrivateSignal)) , this, SLOT(update()));
  m_cache.append("#Duty: ").append( QString::number(m_duty ) ).append('\n');
  m_cache.append("#Conteggi: ").append( QString::number(m_counterino->getCount() ) ).append('\n');
  m_cache.append("#time[millis] rawMeasure\n");
  const Spiedino::AcquireData *bufferAcquireData=m_spiedino->getAcquireCache();
  
  for( unsigned int i=0; i < bufferAcquireData->time.size(); ++i){
    m_cache.append( QString::number(bufferAcquireData->time.at(i)       ) ).append(' ')
          .append( QString::number(bufferAcquireData->rawMeasure.at(i) ) ).append('\n') ;  
    
  }
  
  m_duty+=m_dutyStep;
  m_spiedino->clearAcquireCache();
  m_counterino->clear();
  
  
  
  if( m_duty >256 ){
    
    emit endOfSet();
    return;
    
  }
   
   startStep();
}




void ControlWindows::startStep()
{
  m_cache.append("#Polarizzatore: ").append( QString::number( m_ui->doubleSpinOutPol->value()  )).append('\n');
  m_cache.append("#Passo DutyCicle: ").append( QString::number( m_ui->spinDutyStep->value() ) ).append('\n');
  m_cache.append("#Tempo di integrazione: ").append( QString::number( m_ui->spinIntegrationTime->value()  ) ).append('\n');
  
  m_spiedino->setDuty(m_duty);
  m_timer.start();
  m_spiedino->acquire(0);
  m_counterino->start();
  connect( &m_clock, SIGNAL(timeout(QPrivateSignal)) , this, SLOT(update()));
  update();
}


void ControlWindows::update()
{
 m_ui->lcdNumber->display( m_counterino->getCount() );
 
 m_ui->progressBar->setValue( m_duty*1./256);
 m_clock.start();
}




void ControlWindows::Save()
{
  
  
  QString nameFile=QFileDialog::getSaveFileName(this,"Save File","./");
  QTextStream logStream(stdout);
  QFile fileLog;
  fileLog.setFileName(nameFile);
  fileLog.open(QIODevice::WriteOnly | QIODevice::Append);
  logStream.setDevice(&fileLog);
  logStream << m_cache << flush;
  fileLog.close();
  m_cache.clear();

  
}