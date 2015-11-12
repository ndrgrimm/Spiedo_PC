#include "selectionWindows.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>


  
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <iostream>


SelectionWindows::SelectionWindows(QWidget* parent):
QWidget(parent),
m_serialList(this),
m_textBox(this),
m_selectingButton(this),
m_serialInfoList(QSerialPortInfo::availablePorts())
{
  updateSerialList();
  updateTextBox(0);
  m_selectingButton.setText("Seleziona");
  connect( &m_serialList,SIGNAL( currentRowChanged(int) ),this, SLOT( updateTextBox(int) ) );
  connect( &m_selectingButton,SIGNAL( clicked(bool) ),this, SLOT( selectionSerial(bool) ) );
  connect( this, SIGNAL(serialSelected(QSerialPort*)),this, SLOT( close()) );
  
}




void SelectionWindows::selectionSerial(bool checked)
{
  
  QSerialPort* out=new QSerialPort( m_serialInfoList.at( m_serialList.currentRow() ) );
  
  emit serialSelected( out );
  
}



void SelectionWindows::updateTextBox(int index)
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
      }
      m_textBox.setText(textUpdate);
	  
}


void SelectionWindows::updateSerialList(){
  
  m_serialList.clear();
  QSerialPortInfo tmp_serialinfo;
  foreach( tmp_serialinfo, m_serialInfoList){
    
   m_serialList.addItem(tmp_serialinfo.portName());
   
  }
}

SelectionWindows::~SelectionWindows()
{

}



