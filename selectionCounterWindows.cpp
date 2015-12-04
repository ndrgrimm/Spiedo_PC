#include "selectionCounterWindows.h"
#include "ui_selectionCounterWindows.h"


#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>


  
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>

#include "counterino.h"

SelectionCounterWindows::SelectionCounterWindows(QWidget* parent):
QWidget(parent),

m_serialInfoList(QSerialPortInfo::availablePorts()),
m_ui(new Ui::SelectionCounterWindows)
{
  m_ui->setupUi(this);
  m_CounterinoOut=0;
  updateSerialList();
  
  updateTextBox(0);
  
  connect( (m_ui->m_reloadList),     SIGNAL( clicked( bool ) ), 	    this, SLOT( updateSerialList() )	);
  connect( (m_ui->m_SerialList),     SIGNAL( highlighted(int) ),      	    this, SLOT( updateTextBox(int) )    );
  connect( (m_ui->m_selectionButton),SIGNAL( clicked( bool ) ),             this, SLOT( selectionSerial(bool) ) );
  connect( (m_ui->m_quitButton),     SIGNAL( clicked( bool ) ),             this, SLOT( close() )               );

  connect( this,                     SIGNAL( serialCounterSelected(Counterino*) ),     this, SLOT( close() ) );
  
  
}




void SelectionCounterWindows::selectionSerial(bool checked)
{
  
  
  
    m_CounterinoOut=new Counterino( m_serialInfoList.at( m_ui->m_SerialList->currentIndex() ).portName() );

      emit serialCounterSelected(m_CounterinoOut);
  
    
   return;
  
  
}



void SelectionCounterWindows::updateTextBox(int index)
{
      
      
      QString textUpdate;
      QTextStream out(&textUpdate);
     
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
        if( Serial_info.isBusy() ){	
          m_ui->m_selectionButton->setEnabled(false);
        }
        else{
          m_ui->m_selectionButton->setEnabled(true);
        }
	
        
      }
     
	  
}


void SelectionCounterWindows::updateSerialList(){
  
  m_ui->m_selectionButton->setEnabled(true);
  
  m_ui->m_SerialList->clear();
  QSerialPortInfo tmp_serialinfo;
  m_serialInfoList=QSerialPortInfo::availablePorts();
  if( m_serialInfoList.isEmpty() ){
    
    m_ui->m_textBox->setText("Nessuna porta seriale Collegata");
    return;
    
  }else{
    
    foreach( tmp_serialinfo, m_serialInfoList){
      
    m_ui->m_SerialList->addItem(tmp_serialinfo.portName());
  
    }
    updateTextBox(0);
  }
  
}







SelectionCounterWindows::~SelectionCounterWindows()
{
  delete m_ui;
}




