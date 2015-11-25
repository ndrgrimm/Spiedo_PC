#include "selectionWindows.h"
#include "ui_selectionWindows.h"


#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>


  
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <iostream>


SelectionWindows::SelectionWindows(QWidget* parent):
QWidget(parent),
// m_serialList(this),
// m_textBox(this),
// m_selectingButton(this),
m_serialInfoList(QSerialPortInfo::availablePorts()),
m_ui(new Ui::SelectionWindows)
{
  m_ui->setupUi(this);
  m_out=0;
  updateSerialList();
  
  updateTextBox(0);
  
  connect( (m_ui->m_SerialList),     SIGNAL(activated(QString)),	    this, SLOT( updateSerialList() )  );
  connect( (m_ui->m_SerialList),     SIGNAL(currentIndexChanged(int)),      this, SLOT( updateTextBox(int) )  );
  
  connect( (m_ui->m_selectionButton),SIGNAL( clicked( bool ) ),             this, SLOT( selectionSerial(bool) ) );
  connect( (m_ui->m_quitButton),     SIGNAL( clicked( bool ) ),             this, SLOT( close() ) );
  
  connect( this,                     SIGNAL(serialSelected(QSerialPort*)),  this, SLOT( close() ) );
  
  
}




void SelectionWindows::selectionSerial(bool checked)
{
  if( m_out !=0 ){
    m_out->write("~");
    m_out->flush();
    m_out->readAll();
    emit serialSelected(m_out);
    return;
  }  

    
  m_out=new QSerialPort( m_serialInfoList.at( m_ui->m_SerialList->currentIndex() ) );
  m_out->setBaudRate(QSerialPort::Baud115200);
  m_out->setDataBits(QSerialPort::Data8);
  m_out->setParity(QSerialPort::NoParity);
  m_out->setStopBits(QSerialPort::OneStop); 
  m_out->setFlowControl(QSerialPort::NoFlowControl);
  m_out->open(QIODevice::ReadWrite);
  
  

  m_out->setRequestToSend(true);
  m_out->setRequestToSend(false);
  
  QByteArray Data;
  char buf[1000];
  int leghtbuf=0;
  

  
  while( ( m_out->waitForReadyRead(10000) ) ){
    
    if( m_out->canReadLine() ){
      leghtbuf=m_out->readLine(buf,sizeof(buf));
      if( leghtbuf >= 8 )
	break;     
    }
  }
  m_ui->m_textBox->setText( QObject::tr("SketchCode: %1\n").arg(buf) );
  m_ui->m_selectionButton->setText("Conferma");
   
  
  
}



void SelectionWindows::updateTextBox(int index)
{
  
  
      QString textUpdate;
      QTextStream out(&textUpdate);
      m_ui->m_selectionButton->setEnabled(false);
     
      if( m_serialInfoList.size() == 0 ){
	out << QObject::tr("Nessuna porta seriale Collegata") << endl;
      }else{
      
      const QSerialPortInfo& Serial_info=m_serialInfoList.at(index);
      QString description  = Serial_info.description();
      QString manufacturer = Serial_info.manufacturer();
      QString serialNumber = Serial_info.serialNumber();
      out
          << QObject::tr("Port: ") << Serial_info.portName() << endl
          << QObject::tr("Location: ") << Serial_info.systemLocation() << endl
          << QObject::tr("Description: ") << (!description.isEmpty() ? description : "N\\A") << endl
          << QObject::tr("Manufacturer: ") << (!manufacturer.isEmpty() ? manufacturer : "N\\A") << endl
          << QObject::tr("Serial number: ") << (!serialNumber.isEmpty() ? serialNumber : "N\\A") << endl
          << QObject::tr("Vendor Identifier: ") << (Serial_info.hasVendorIdentifier() ? QByteArray::number(Serial_info.vendorIdentifier(), 16) : "N\\A") << endl
          << QObject::tr("Product Identifier: ") << (Serial_info.hasProductIdentifier() ? QByteArray::number(Serial_info.productIdentifier(), 16) : "N\\A") << endl
          << QObject::tr("Busy: ") << (Serial_info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) << endl;
      
      m_ui->m_textBox->setText(textUpdate);
      if( ! Serial_info.isBusy() )	
	m_ui->m_selectionButton->setEnabled(true);
	
      }
	  
}


void SelectionWindows::updateSerialList(){
  
  m_ui->m_SerialList->clear();
  QSerialPortInfo tmp_serialinfo;
  foreach( tmp_serialinfo, m_serialInfoList){
    
   m_ui->m_SerialList->addItem(tmp_serialinfo.portName());
 
  }
  
  
}







SelectionWindows::~SelectionWindows()
{
  delete m_ui;
}




