#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "ui_controlwindows.h"

#include <QtCore/QDateTime>
#include <QThread>
#include <QtSerialPort/QtSerialPort>
#include <QtWidgets/QFileDialog>


QDateTime MainWindow::sm_datelog=QDateTime::currentDateTime();
QTextStream MainWindow::sm_streamlog(stdout);

MainWindow::MainWindow(QWidget* parent): 
QMainWindow(parent),
m_selectionReaderWindow(new SelectionWindows()),
m_selectionCounterWindow(new SelectionCounterWindows()),
m_controlWindows(new ControlWindows() ),
m_ui(new Ui::MainWindow)
{
 m_ui->setupUi(this);
 m_spiedino=0;
 m_counterino=0;
 m_indexAcquireCache=0;
 m_indexScanCache=0;
 m_scanCache=0;
 m_acquireCache=0;

 addReader();
 blockInterface();
 
 connect( m_ui->actionAddReader,   SIGNAL( triggered() )                        , this, SLOT( addReader() )     			);
 connect( m_ui->actionAddCounter,  SIGNAL( triggered(bool))                     , this, SLOT( addCounter() )                            );                              
 connect( m_ui->actionOpenControl, SIGNAL( triggered(bool))                     , this, SLOT( openControl() )                           );
 connect( m_ui->m_autoButton,      SIGNAL( clicked(bool) )                      , this, SLOT( Scan() )          			);
 connect( m_ui->m_acquireButton,   SIGNAL( clicked(bool) )                      , this, SLOT( Acquire() )       			);
 connect( m_ui->m_setDutyButton,   SIGNAL( clicked(bool) )                      , this, SLOT( SetDuty() )       			);
 connect( m_ui->m_quitButton,      SIGNAL( clicked(bool) )                      , this, SLOT( m_close() )       			);
 connect( m_ui->m_stopButton,	   SIGNAL( clicked(bool) )                      , this, SLOT( StopAcquire() )   			);
 
 connect( m_selectionReaderWindow, SIGNAL( serialSelected(Spiedino*) )          , this, SLOT( connectSpiedino(Spiedino*)        ) 	); 
 connect( this,                    SIGNAL( blockingInterface() )                , this, SLOT( blockInterface() )			);  // FIXME hanno davvero senso queste due righe?
 connect( this,                    SIGNAL( unBlockingInterface() )              , this, SLOT( unBlockInterface() )			);  // FIXME hanno davvero senso queste due righe?
 connect( m_selectionCounterWindow,SIGNAL( serialCounterSelected(Counterino*) ) , this, SLOT( connectCounterino(Counterino*))           );
 
}


MainWindow::~MainWindow()
{
 
  delete m_ui;

  delete m_selectionReaderWindow;

  if( !( m_spiedino == 0) ){
    delete m_spiedino;
  }
}


//###################SLOTS#################

void MainWindow::connectSpiedino(Spiedino* spiedinoSelected)
{

  if(! spiedinoSelected->isReady() ){
    m_selectionReaderWindow->show();
    emit blockingInterface();
    return;
    
  }
  m_spiedino=spiedinoSelected;
  connect( m_spiedino,              SIGNAL( foundEndDataBlock() )                , this, SLOT( askForSave() )                            );
  connect( m_spiedino,              SIGNAL( foundEndDataBlock() )                , this, SLOT( unBlockInterface() )                      );
  connect( m_spiedino,              SIGNAL( readyPerCentLine(int,double,double) ), this, SLOT( updateDisplay(int,double,double) )        );
  emit unBlockingInterface();

}

void MainWindow::connectCounterino(Counterino* counterinoSelected)
{

//   if(! Counterino->isReady() ){
//     m_selectionReaderWindow->show();
//     emit blockingInterface();
//     return;
//     
//   }
  int ErrorCode= counterinoSelected->initialize();
  if( ErrorCode != 0 ){
    sm_streamlog << "COUNTER ERROR: " << ErrorCode << endl;
    m_selectionReaderWindow->show();
    emit blockingInterface();
    return;
    
  }
  
  m_counterino=counterinoSelected;
  emit unBlockingInterface();

}



//___________________BUTTON_________________



void MainWindow::Scan()
{        
        sm_streamlog << "Scanning" << endl;

	emit blockingInterface();
        m_scanByteCache.append("#").append( sm_datelog.toString("dd\\MM\\yyyy hh:mm:ss") ).append("\n");
	m_scanByteCache.append("#Scanning on dutyCicle\n#duty\tmean\tsigma\n");

//         m_scanCache=m_spiedino->getScanCache();
// 	m_ui->m_plot->xAxis->setRange(0,255);
// 	m_ui->m_plot->yAxis->setRange(0,1023);
// 	m_ui->m_plot->addGraph(0);
// 	//m_ui->m_plot->graph(0)->setDataValueError(m_scanCache->duty,m_scanCache->mean,m_scanCache->sigma);
// 	m_ui->m_plot->replot();
//         connect( m_spiedino, SIGNAL(readyPerCentLine(int,double,double)), this,  SLOT(updatePlot()) );
// 	m_spiedino->scan();
        
        sm_streamlog << "prova counter: " <<  m_counterino->getCount() << endl;
        m_counterino->clear();
	
}

void MainWindow::Acquire()
{
  sm_streamlog << "Acquire" << endl;

//   emit blockingInterface();
//   
//   
//   m_acquireByteCache.append("#").append( sm_datelog.toString("dd\\MM\\yyyy hh:mm:ss") ).append("\n");
//   m_acquireByteCache.append("#Scanning on dutyCicle\n#duty\tmean\tsigma\n");
//   
//  
//   int numberOfSample=m_ui->m_spinAcquire->value();
//   m_acquireCache=m_spiedino->getAcquireCache();
  
//   m_ui->m_plot->xAxis->setRange(0,100);
//   m_ui->m_plot->xAxis->rescale();
//   m_ui->m_plot->addGraph(0);
//   m_ui->m_plot->graph(0)->setData(m_acquireCache->time,m_acquireCache->rawMeasure);
//   m_ui->m_plot->replot();
  
//   m_spiedino->acquire(numberOfSample);
  
  m_counterino->start();
  


}

void MainWindow::StopAcquire(){
 sm_streamlog << "Stop" << endl;


m_counterino->stop();

}


void MainWindow::SetDuty()
{
        sm_streamlog << "SetDuty" << endl;

    m_spiedino->setDuty(m_ui->m_spinDuty->value());

}

void MainWindow::WarmUp(){
}



void MainWindow::m_close()        //FIXME: è possibile scriverlo intercettando il segnale;
{
  sm_streamlog << "Quit" << endl;

  if( (! m_spiedino == 0 ) && m_spiedino->isReady() ){
    m_spiedino->quit();

  }
  close();
}


void MainWindow::addReader()
{
    sm_streamlog << "addReader" << endl;

  m_selectionReaderWindow->show();
  
}

void MainWindow::addCounter()
{
      sm_streamlog << "addCounter" << endl;

  m_selectionCounterWindow->show();
  
}

void MainWindow::openControl()
{

  sm_streamlog << "openControl" << endl;
  if( m_spiedino == 0 || ! m_spiedino->isReady() || m_counterino !=0 )
    return;
  
  
  disconnect( m_spiedino,              SIGNAL( foundEndDataBlock() )                , this, SLOT( askForSave() )                            );
  disconnect( m_spiedino,              SIGNAL( foundEndDataBlock() )                , this, SLOT( unBlockInterface() )                      );
  disconnect( m_spiedino,              SIGNAL( readyPerCentLine(int,double,double) ), this, SLOT( updateDisplay(int,double,double) )        );
  
  m_spiedino->clearAcquireCache();
  m_spiedino->clearScanCache();

 
}






//___________________Interface Manager_____________
void MainWindow::unBlockInterface()
{
      sm_streamlog << "unBlockInterface" << endl;

   m_interfaceIsBlocked=false;
   m_ui->m_acquireButton->setEnabled(true);
   m_ui->m_setDutyButton->setEnabled(true);
   m_ui->m_spinAcquire->setEnabled(true);
   m_ui->m_autoButton->setEnabled(true);
  // m_ui->m_stopButton->setEnabled(true);
   
   
}


void MainWindow::blockInterface(){
      sm_streamlog << "blockingInterface" << endl;

  m_interfaceIsBlocked=true;
  m_ui->m_acquireButton->setEnabled(false);
  m_ui->m_setDutyButton->setEnabled(false);
  m_ui->m_spinAcquire->setEnabled(false);
  m_ui->m_autoButton->setEnabled(false);
  //m_ui->m_stopButton->setEnabled(false);
}


//___________________Update Thing_________________


void MainWindow::disconnectCache(){
      sm_streamlog << "disconnect Cache" << endl;

 disconnect(this, SIGNAL( readyPerCentLine(int,double,double) ) , this, SLOT( updatePlot() ) );
 disconnect(this, SIGNAL( readyEsclamationLine(ulong,uint) )    , this, SLOT( updatePlot() ) );
 if( m_acquireCache->time.isEmpty() && m_scanCache->duty.isEmpty() )
   return;
 
 emit somethingToSave();
 
}

void MainWindow::updateDisplay( int duty, double mean, double sigma )
{
  sm_streamlog << "updateDisplay" << endl;
  sm_streamlog << duty << ':' << mean << ':' << sigma << endl;
  m_ui->m_diodeLcd->display( int(mean) );
  m_ui->m_diodeStatusBar->setValue( int(mean) );
   
}



void MainWindow::updatePlot()
{

      sm_streamlog << "updatePlot" << endl;

  m_ui->m_plot->replot();

}


void MainWindow::askForSave()
{
  sm_streamlog << "askForSave" << endl;
  sm_streamlog << "m_scanCache" << m_scanCache << endl;
  sm_streamlog << "m_acquireCache" << m_acquireCache << endl;
  if ( m_scanCache !=0 && !m_scanCache->duty.isEmpty() ){
    
    for( unsigned int i=0; i< m_scanCache->duty.size() ; ++i){
      
      m_scanByteCache.append( QString::number(m_scanCache->duty.at(i)  ) ).append(' ')
                     .append( QString::number(m_scanCache->mean.at(i)  ) ).append(' ')
                     .append( QString::number(m_scanCache->sigma.at(i) ) ).append('\n');
      sm_streamlog << m_scanCache->duty.at(i) << ':'
                   << m_scanCache->mean.at(i) << ':'
                   << m_scanCache->sigma.at(i) << ':'
                   << endl;
    }
    save(&m_scanByteCache);
    m_spiedino->clearScanCache();
    m_scanByteCache.clear();
  }
  if ( m_acquireCache != 0 && !m_acquireCache->time.isEmpty() ){
    
    for(unsigned int i =0; i< m_acquireCache->time.size(); ++i){
     m_acquireByteCache.append( QString::number(m_acquireCache->time.at(i)       ) ).append(' ')
                       .append( QString::number(m_acquireCache->rawMeasure.at(i) ) ).append('\n') ; 
      sm_streamlog << m_acquireCache->time.at(i) << ':' << m_acquireCache->rawMeasure.at(i) << endl;
    }
      
     save(&m_acquireByteCache);
     m_spiedino->clearAcquireCache();
     m_acquireByteCache.clear();
  }

}

void MainWindow::save(QByteArray *cacheToSave)
{
    sm_streamlog << "save" << endl;

  emit blockingInterface();
  
  
  QString nameFile=QFileDialog::getSaveFileName(this,"Save File","./");
  QTextStream logStream(stdout);
  QFile fileLog;
  fileLog.setFileName(nameFile);
  fileLog.open(QIODevice::WriteOnly|QIODevice::Append);
  logStream.setDevice(&fileLog);
  logStream << *cacheToSave << flush;
  fileLog.close();
  cacheToSave->clear();

  emit unBlockingInterface();
}



