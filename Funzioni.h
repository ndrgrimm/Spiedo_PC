#include <QtCore/QtCore>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


QSerialPort* Collegamento(){
  
  
    QList<QSerialPortInfo> ListaSeriali_info = QSerialPortInfo::availablePorts();
    QTextStream out(stdout);
    QTextStream in(stdin);
    const QString blankString=QObject::tr("N\\A");
    QString description;
    QString manufacturer;
    QString serialNumber;	

    out << QObject::tr("Numero di seriali presenti: ") << ListaSeriali_info.count() << endl;
    //if (ListaSeriali_info.count() == 0 ) return 0;
    
    unsigned int numberOfSerial=0;
    foreach(const QSerialPortInfo &Seriale_info, ListaSeriali_info){
      
      description =  Seriale_info.description();
      manufacturer = Seriale_info.manufacturer();
      serialNumber = Seriale_info.serialNumber();
      out << endl
	  << numberOfSerial << endl
          << QObject::tr("Port: ") << Seriale_info.portName() << endl
          << QObject::tr("Location: ") << Seriale_info.systemLocation() << endl
          << QObject::tr("Description: ") << (!description.isEmpty() ? description : blankString) << endl
          << QObject::tr("Manufacturer: ") << (!manufacturer.isEmpty() ? manufacturer : blankString) << endl
          << QObject::tr("Serial number: ") << (!serialNumber.isEmpty() ? serialNumber : blankString) << endl
          << QObject::tr("Vendor Identifier: ") << (Seriale_info.hasVendorIdentifier() ? QByteArray::number(Seriale_info.vendorIdentifier(), 16) : blankString) << endl
          << QObject::tr("Product Identifier: ") << (Seriale_info.hasProductIdentifier() ? QByteArray::number(Seriale_info.productIdentifier(), 16) : blankString) << endl
          << QObject::tr("Busy: ") << (Seriale_info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) << endl;
      numberOfSerial++;
    
    }
    unsigned int indexSerial=0;
    out << QObject::tr("Quale Seriale Vuoi>>") << endl;
    in >> indexSerial;
    
    
    
    
    return &ListaSeriali_info.at(indexSerial);
    
    
}