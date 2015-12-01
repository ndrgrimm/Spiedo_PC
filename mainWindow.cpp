#include "mainWindow.h"
#include "ui_mainWindow.h"

#include <QtCore/QDateTime>
#include <QThread>
#include <QtSerialPort/QtSerialPort>
#include <QtWidgets/QFileDialog>


QDateTime MainWindow::sm_datelog=QDateTime::currentDateTime();
QTextStream MainWindow::sm_streamlog(stdout);

MainWindow::MainWindow(QWidget* parent): 
QMainWindow(parent),
m_firstBuffer(),
m_ScanCache(),
m_selectionWindow(new SelectionWindows()),
m_ScanCacheX(256),
m_ScanCacheY(256),
m_ScanCacheYSigma(256),
m_AcquireCache(),
m_AcquireCacheX(),
m_AcquireCacheY(),
m_ui(new Ui::MainWindow)
{
 m_ui->setupUi(this);
 m_serialPort=0;
 m_indexData1=0;
 m_indexData2=0;
 addReader();
 blockInterface();
 
 connect( m_ui->actionAddReader,   SIGNAL( triggered() )                        , this, SLOT( addReader() )     			);
 connect( m_ui->m_autoButton,      SIGNAL( clicked(bool) )                      , this, SLOT( Scan() )          			);
 connect( m_ui->m_acquireButton,   SIGNAL( clicked(bool) )                      , this, SLOT( Acquire() )       			);
 connect( m_ui->m_setDutyButton,   SIGNAL( clicked(bool) )                      , this, SLOT( SetDuty() )       			);
 connect( m_ui->m_quitButton,      SIGNAL( clicked(bool) )                      , this, SLOT( m_close() )       			);
 connect( m_ui->m_stopButton,	   SIGNAL( clicked(bool) )                      , this, SLOT( stopAcquire() )   			);
 connect( this, 		   SIGNAL( foundEndDataBlock() )                , this, SLOT( askForSave() )    			);
 connect( this,                    SIGNAL( blockingInterface() )                , this, SLOT( blockInterface() )			);  // FIXME hanno davvero senso queste due righe?
 connect( this,                    SIGNAL( unBlockingInterface() )              , this, SLOT( unBlockInterface() )			);  // FIXME hanno davvero senso queste due righe?
 connect( this,                    SIGNAL( foundEndDataBlock() )                , this, SLOT( unBlockInterface() )			);
 connect( this,                    SIGNAL( foundEndDataBlock() )                , this, SLOT( disconnectCache() )			);
 connect( this,                    SIGNAL( readyPerCentLine(int,double,double) ), this, SLOT( updateDisplay(int,double,double) )		);
 connect( m_selectionWindow,       SIGNAL( serialSelected(QSerialPort*) )       , this, SLOT( connectSerialPort(QSerialPort*) ) 	); 
  
 
}


MainWindow::~MainWindow()
{

  delete m_ui;
  delete m_selectionWindow;
  delete m_serialPort;
}


//###################SLOTS#################

void MainWindow::connectSerialPort(QSerialPort *serialPortSelected)
{
  if(! serialPortSelected->isOpen()){
    m_selectionWindow->show();
    emit blockingInterface();
    return;
    
  }
  m_ui->centralWidget->setEnabled(false);
  m_ui->m_quitButton->setEnabled(true);
  m_serialPort=serialPortSelected;
  m_ui->centralWidget->setEnabled(true);
  m_serialPort->readAll();
  emit unBlockingInterface();
  connect( m_serialPort, SIGNAL( readyRead() ), this, SLOT( updateFirstBuffer() ) );

}



//___________________BUTTON_________________



void MainWindow::Scan()
{        
	emit blockingInterface();
	connect( this, SIGNAL(readyPerCentLine(int,double,double)) , this, SLOT(updateScanCache(int,double,double)) );
        m_ScanCache.append("#").append( sm_datelog.toString("dd\\MM\\yyyy hh:mm:ss") ).append("\n");
	m_ScanCache.append("#Scanning on dutyCicle\n#duty\tmean\tsigma\n");
	
	m_ui->m_plot->xAxis->setRange(0,255);
	m_ui->m_plot->yAxis->setRange(0,1023);
	m_ui->m_plot->addGraph(0);
	m_ui->m_plot->graph(0)->setDataValueError(m_ScanCacheX,m_ScanCacheY,m_ScanCacheYSigma);
	m_ui->m_plot->replot();
	m_serialPort->write("a");
	m_serialPort->flush();
	m_serialPort->clear();
	
	
}




  

void MainWindow::Acquire()
{
  emit blockingInterface();
  connect( this, SIGNAL(readyEsclamationLine(ulong,uint)) , this, SLOT(updateAcquireCache(ulong,uint)) );
  m_AcquireCache.append("#").append( sm_datelog.toString("dd\\MM\\yyyy hh:mm:ss") ).append("\n");
  m_AcquireCache.append("#Scanning on dutyCicle\n#duty\tmean\tsigma\n");
  m_serialPort->write("t");
  m_serialPort->flush();
  m_serialPort->clear();
 
  int numberOfSample=m_ui->m_spinAcquire->value();

  m_ui->m_plot->xAxis->setRange(0,100);
  m_ui->m_plot->yAxis->setRange(0,1023);
  m_ui->m_plot->addGraph(0);
  m_ui->m_plot->graph(0)->setData(m_AcquireCacheX,m_AcquireCacheY);
  m_ui->m_plot->replot();
  if ( numberOfSample <=0 ){
    m_serialPort->write("g");
    m_serialPort->flush();
    m_serialPort->clear();
    sm_streamlog << "go!" << endl;
  }else{
    m_serialPort->write(QString::number(numberOfSample).toStdString().c_str());
    m_serialPort->flush();
    m_serialPort->clear();
  }


}

void MainWindow::stopAcquire(){
  m_serialPort->write("s");
  m_serialPort->flush();
  m_serialPort->clear();

}


void MainWindow::SetDuty()
{
    m_serialPort->write("w");
    m_serialPort->flush();
    QThread::msleep(500);
    
    m_serialPort->write(QString::number(m_ui->m_spinDuty->value()).toStdString().c_str());
    m_serialPort->flush();
    m_serialPort->clear();
}


void MainWindow::save(QByteArray *cacheToSave)
{

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


void MainWindow::m_close()        //FIXME: è possibile scriverlo intercettando il segnale;
{
  if(m_serialPort !=0  && m_serialPort->isOpen() ){
    m_serialPort->write("q");
    m_serialPort->flush();
    m_serialPort->readAll();
    m_serialPort->clear();
    sm_streamlog << "pulito" << endl;
  }
  
  close();
}


void MainWindow::addReader()
{
  m_selectionWindow->show();
  
}





//___________________Interface Manager_____________
void MainWindow::unBlockInterface()
{
   m_interfaceIsBlocked=false;
   m_ui->m_acquireButton->setEnabled(true);
   m_ui->m_setDutyButton->setEnabled(true);
   m_ui->m_spinAcquire->setEnabled(true);
   m_ui->m_autoButton->setEnabled(true);
   m_ui->m_stopButton->setEnabled(true);
   
   
}


void MainWindow::blockInterface(){
  m_interfaceIsBlocked=true;
  m_ui->m_acquireButton->setEnabled(false);
  m_ui->m_setDutyButton->setEnabled(false);
  m_ui->m_spinAcquire->setEnabled(false);
  m_ui->m_autoButton->setEnabled(false);
  m_ui->m_stopButton->setEnabled(false);
}


//___________________Update Thing_________________
void MainWindow::updateFirstBuffer()
{
 //FIXME dispacciando i puntatori si ha che tutti accendono allo STESSO QByteArray, quindi 1. deve essere in sola lettura, due bisogna gestire bene la sua rimozione
 

void MainWindow::disconnectCache(){
 disconnect(this, SIGNAL( readyPerCentLine(int,double,double) ) , this, SLOT( updateScanCache(int,double,double) ));
 disconnect(this, SIGNAL( readyEsclamationLine(ulong,uint) )    , this, SLOT( updateAcquireCache(ulong,uint) ) );
 if( m_AcquireCache.isEmpty() && m_ScanCache.isEmpty())
   return;
 
 emit somethingToSave();
 
}

void MainWindow::updateDisplay( int duty, double mean, double sigma )
{
  sm_streamlog << "Updating Display..." << endl;
  sm_streamlog << duty << ' ' << mean << ' ' << sigma << endl;
 // m_ui->m_spinDuty->setValue(duty);
  m_ui->m_diodeLcd->display( int(mean) );
  m_ui->m_diodeStatusBar->setValue( int(mean) );
      
}


void MainWindow::updatePlot()
{


}


void MainWindow::updateAcquireCache(unsigned long time, unsigned int rawMeasure)
{
//   sm_streamlog << time << ' ' << rawMeasure << endl;
  m_AcquireCache.append( QString::number(time) ).append(' ').append( QString::number(rawMeasure) ).append('\n') ;
  m_AcquireCacheX.append( time );
  m_AcquireCacheY.append( rawMeasure );
  //m_ui->m_plot->xAxis->setRange( m_AcquireCache.at(0), m_AcquireCache.at( m_AcquireCache.size()-1 ) );
  m_ui->m_plot->graph(0)->setData(m_AcquireCacheX,m_AcquireCacheY);
  m_ui->m_plot->replot();
  
}



void MainWindow::askForSave()
{
  
  if ( !m_ScanCache.isEmpty() ){
     save(&m_ScanCache);
     m_ScanCacheX.clear();
     m_ScanCacheY.clear();
     m_ScanCacheYSigma.clear();
  }
  if ( !m_AcquireCache.isEmpty() ){
     save(&m_AcquireCache);
     m_AcquireCacheY.clear();
     m_AcquireCacheX.clear();
  }

}
