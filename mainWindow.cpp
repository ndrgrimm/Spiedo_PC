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
 
 connect( m_ui->actionAddReader,   SIGNAL( triggered() )	               , this, SLOT( addReader() )				);
 connect( this,			   SIGNAL( blockingInterface() )	       , this, SLOT( blockInterface() )				);  // FIXME hanno davvero senso queste due righe?
 connect( this,			   SIGNAL( unBlockingInterface() )	       , this, SLOT( unBlockInterface() )			);  // FIXME hanno davvero senso queste due righe?
 connect( this,			   SIGNAL( foundEndDataBlock() )	       , this, SLOT( unBlockInterface() )			);
 connect( this,			   SIGNAL( foundEndDataBlock() )	       , this, SLOT( disconnectCache() )			);
 connect( m_selectionWindow,       SIGNAL( serialSelected(QSerialPort*) )      , this, SLOT( connectSerialPort(QSerialPort*) ) 		); 
 connect( this,			   SIGNAL(readyPerCentLine(int,double,double)) , this, SLOT(updateDisplay(int,double,double))		);
 connect( m_ui->m_autoButton,      SIGNAL( clicked(bool) )		       , this, SLOT( Scan() ) 					);
 connect( m_ui->m_acquireButton,   SIGNAL( clicked(bool) )		       , this, SLOT( Scan() ) 					);
 connect( m_ui->m_setDutyButton,   SIGNAL( clicked(bool) )		       , this, SLOT( SetDuty() )				);
 connect( m_ui->m_quitButton,      SIGNAL( clicked(bool) )		       , this, SLOT( m_close() ) 				);
 //connect( m_ui->m_saveButton,	   SIGNAL( clicked(bool) )		       , this, SLOT( save(QByteArray*) )			);
 connect( this,			   SIGNAL(foundEndDataBlock())		       , this, SLOT(askForSave())				);
 
 
 
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




  

void MainWindow::Acquire(int iLenghtOfSample, bool isLenghtATimer)
{

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


void MainWindow::m_close()
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
  m_selectionWindow=new SelectionWindows();
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
   
   
   
}


void MainWindow::blockInterface(){
  m_interfaceIsBlocked=true;
  m_ui->m_acquireButton->setEnabled(false);
  m_ui->m_setDutyButton->setEnabled(false);
  m_ui->m_spinAcquire->setEnabled(false);
  m_ui->m_autoButton->setEnabled(false);

}


//___________________Update Thing_________________
void MainWindow::updateFirstBuffer()
{
 //FIXME dispacciando i puntatori si ha che tutti accendono allo STESSO QByteArray, quindi 1. deve essere in sola lettura, due bisogna gestire bene la sua rimozione
 QByteArray tmp_data;
 QByteArray *buffer;
 tmp_data.append( m_serialPort->readAll() ); 
 bool isFirst=true;
 sm_streamlog << "tmp_data: " << tmp_data << endl;
 for( unsigned int i=0 ; i < tmp_data.size(); ++i){

   switch ( tmp_data.at(i) ){
     sm_streamlog << tmp_data.at(i) << endl;
     case '%':{
       sm_streamlog << "FIND % " << endl;
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
       buffer->replace('?','\n');
       QTextStream out(buffer);
       out >> duty >> mean >> sigma;
       sm_streamlog << duty << ' ' <<  mean << ' ' << sigma << endl; //FIXME DEBUG
      
      
       emit readyPerCentLine( duty, mean, sigma );
       tmp_data.remove(0,i+1);
       i=0;
       delete buffer;
       break;
     }
     case '!':{
       sm_streamlog << "FIND ! " << endl;

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
        buffer->replace('?','\n');
        QTextStream out(buffer);
        out >> time >> rawMeasure;
        emit readyEsclamationLine( time, rawMeasure);
	tmp_data.remove(0,i+1);
	i=0;
	delete buffer;
	break;
     }  
/*
 *      case '?':	//FIXME cosa molto sporca, da sistemare
       
	  if( isFirst){
	    
	     updateDisplay( new QByteArray(m_firstBuffer.append( tmp_data.mid( 0, i+1 ) ) ) );
	    m_firstBuffer.clear();
	    isFirst=false;

	  }else{
	    
	     updateDisplay( new QByteArray( tmp_data.mid( 0, i+1 ) ) );
	  }	
	  tmp_data.remove(0,i+1);
	  i=0;
	  break;
	  */
     case '$':
       tmp_data.remove(i,1);
       sm_streamlog << "FIND $" << endl;
       emit foundEndDataBlock();
     default:
       ;
   }
 
 }
 m_firstBuffer.append(tmp_data);
 
  
  
}

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
  sm_streamlog << time << ' ' << rawMeasure << endl;
  QTextStream out(&m_ScanCache);
  out << time << ' ' << rawMeasure << endl;

}
void MainWindow::updateScanCache(int duty, double mean, double sigma)
{
    sm_streamlog << "Updating Scan Cache..." << endl;
    sm_streamlog << duty << ' ' <<  mean << ' ' << sigma << endl;
    m_ScanCacheX.append(duty);
    m_ScanCacheY.append(mean);
    m_ScanCacheYSigma.append(sigma);
    m_ui->m_plot->graph(0)->setDataValueError(m_ScanCacheX,m_ScanCacheY,m_ScanCacheYSigma);
    m_ui->m_plot->replot();
    m_ScanCache.append( QString::number(duty) ).append(' ').append( QString::number(mean) ).append(' ').append( QString::number(sigma) ).append('\n');
    
}


void MainWindow::askForSave()
{
  
  if ( !m_ScanCache.isEmpty() ){
     save(&m_ScanCache);
  }
  if ( !m_AcquireCache.isEmpty() ){
     save(&m_AcquireCache);
  }

}
