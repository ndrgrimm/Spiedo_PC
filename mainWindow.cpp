#include "mainWindow.h"
#include "ui_mainWindow.h"

#include <QtCore/QDateTime>
#include <QtSerialPort/QtSerialPort>
#include <QtWidgets/QFileDialog>


QDateTime MainWindow::sm_datelog=QDateTime::currentDateTime();
QTextStream MainWindow::sm_streamlog(stdout);

MainWindow::MainWindow(QWidget* parent): 
QMainWindow(parent),
m_Data(),
m_ui(new Ui::MainWindow),

m_selectionWindow(new SelectionWindows())
{
 m_ui->setupUi(this);
 m_serialPort=0;
 m_indexData1=0;
 m_indexData2=0;

 blockInterface();
 
 m_selectionWindow->show();
 connect (this,			   SIGNAL( blockingInterface() )	    , this, SLOT( blockInterface() )			);
 connect (this,			   SIGNAL( unBlockingInterface() )	    , this, SLOT( unBlockInterface())			);
 connect( m_selectionWindow,       SIGNAL( serialSelected(QSerialPort*) )   , this, SLOT( connectSerialPort(QSerialPort*) ) 	); 
 connect( m_ui->m_autoButton,      SIGNAL( clicked(bool) )		    , this, SLOT( Auto() ) 				);
 connect( m_ui->m_acquireButton,   SIGNAL( clicked(bool) )		    , this, SLOT( Scan(bool) ) 				);
 connect( m_ui->m_setDutyButton,   SIGNAL( clicked(bool) )		    , this, SLOT( SetDuty() )				);
 connect( m_ui->m_quitButton,      SIGNAL( clicked(bool) )		    , this, SLOT( m_close() ) 				);
 connect( m_ui->m_saveButton,	   SIGNAL( clicked(bool) )		    , this, SLOT( save() )				);
 
 
 
 
 
 connect( m_ui->m_quitButton, SIGNAL( clicked( bool ) ), this, SLOT( close() ) );
}


MainWindow::~MainWindow()
{

  delete m_ui;
  delete m_selectionWindow;
  
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
  connect( m_serialPort, SIGNAL( readyRead() ), this, SLOT( updateData() ) );

}


void MainWindow::updateData()
{
 QByteArray tmp_data;
 tmp_data.append( m_serialPort->readAll() );
 if( m_interfaceIsBlocked ){
   
   if( tmp_data.contains('$') ){
     
     emit unBlockingInterface();
     
   }
 }
 
 m_Data.append(tmp_data);
 if( tmp_data.contains('%') )
   updateDisplay();
 }



void MainWindow::Auto()
{        
	emit blockingInterface();
        m_Data.append("#").append( sm_datelog.toString("dd\\MM\\yyyy hh:mm:ss") ).append("\n");
	m_Data.append("#Scanning on dutyCicle\n#duty\tmean\tsigma\n");
	m_serialPort->write("a");
	m_serialPort->flush();
	m_serialPort->clear();

	
}
void MainWindow::Read()
{

}
void MainWindow::Scan(int iLenghtOfSample, bool isLenghtATimer)
{

}

void MainWindow::SetDuty(int iDutyCicle)
{

}


void MainWindow::save()
{
  if( m_Data.isEmpty() )
    return;
  
  emit blockingInterface();
  
  
  QString nameFile=QFileDialog::getSaveFileName(this,"Save File","./");
//   nameFile.append("spiedoGui_").append( sm_datelog.toString("ddMMyyhhmmss") ).append(".log");
  QTextStream logStream(stdout);
  QFile fileLog;
  fileLog.setFileName(nameFile);
  fileLog.open(QIODevice::WriteOnly|QIODevice::Append);
  logStream.setDevice(&fileLog);
  logStream << m_Data << flush;
  m_indexData1=0;
  m_indexData2=0;
  m_Data.clear();
  emit unBlockingInterface();
}

void MainWindow::m_close()
{
  m_serialPort->write("q");
  m_serialPort->flush();
  m_serialPort->clear();
  close();
}



void MainWindow::unBlockInterface()
{
   m_interfaceIsBlocked=false;
   m_ui->m_acquireButton->setEnabled(true);
   m_ui->m_setDutyButton->setEnabled(true);
   m_ui->m_saveButton->setEnabled(true);
   m_ui->m_spinAcquire->setEnabled(true);
   //m_ui->m_spinDuty->setEnabled(true);
   
   
}


void MainWindow::blockInterface(){
  m_interfaceIsBlocked=true;
  m_ui->m_acquireButton->setEnabled(false);
  m_ui->m_setDutyButton->setEnabled(false);
  m_ui->m_saveButton->setEnabled(false);
  m_ui->m_spinAcquire->setEnabled(false);
  //m_ui->m_spinDuty->setEnabled(false);
}



void MainWindow::updateDisplay()
{
    sm_streamlog << "updateDisplay Call" << endl;
    sm_streamlog << "m_indexData: " << m_indexData1 << ":" << m_indexData2 << endl;

    while(true){ 
	m_indexData1=m_Data.indexOf('%',m_indexData1);
	int tmp=m_Data.indexOf('%',m_indexData1+1);
	
	if ( tmp < 0 )						// check if there is another $ afther the one that I just found.
	  return;
	
	m_indexData2=tmp;
	if( m_indexData1 +1 != m_indexData2 )			//check if the situation is like [...]$[....]$[...]
		break;
			    
	  ++m_indexData1;						// if it's in $$ situation I move  m_indexData1 ->
    }    
    sm_streamlog << "m_indexData: " << m_indexData1 << ":" << m_indexData2 << endl;

    QByteArray tmp=m_Data.mid(m_indexData1, m_indexData2-m_indexData1 );
    sm_streamlog << "ciao!" << endl;
    int mean=0;
    double sigma=0;
    int duty=100;

    tmp.replace(':',' ');
    tmp.replace('%',' ');
    QTextStream out(tmp);
    sm_streamlog << tmp << endl;
    out >> duty >> mean >> sigma;
    sm_streamlog << duty << ' ' <<  mean << ' ' << sigma << endl;
    m_ui->m_spinDuty->setValue(duty);
    m_ui->m_diodeLcd->display(mean);
    m_ui->m_diodeStatusBar->setValue(mean);
    
    m_indexData1=m_indexData2;
   
    
    
}

void MainWindow::updatePlot()
{

}


