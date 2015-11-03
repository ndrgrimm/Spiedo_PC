#include <QtCore/QtCore>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


QSerialPort* Connection(){
  
    
    QList<QSerialPortInfo> ListaSeriali_info = QSerialPortInfo::availablePorts();
    QTextStream out(stdout);
    QTextStream in(stdin);
    const QString blankString=QObject::tr("N\\A");
    QString description;
    QString manufacturer;
    QString serialNumber;	

    out << QObject::tr("Numero di seriali presenti: ") << ListaSeriali_info.count() << endl;
    if (ListaSeriali_info.count() == 0 ){
      out << QObject::tr("Arriverderci e grazie per tutto il pesce.") << endl;
      //QCoreApplication::exit(); //FIXIT: sistemare questo punto studiando bene QTApplication
      return 0;
    }
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
          << QObject::tr("Busy: ") << (Seriale_info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) << endl << endl;
      numberOfSerial++;
    
    }
    unsigned int indexSerial=0;
    do {
    out << QObject::tr("Seleziona Seriale.\n>>") << flush;
    in >> indexSerial;
    
    }while(indexSerial >= ListaSeriali_info.count());
    QSerialPort * tmp=new QSerialPort(ListaSeriali_info.at(indexSerial).systemLocation());
    // Load Arduino conf
   // tmp->setReadBufferSize(0);
    
    
    return tmp;
    
    
}



void ConfigurePort(QSerialPort *ToConfigure, _IO_FILE *out = stdout,
		   _IO_FILE *in = stdin){
   
    QTextStream std_out(out);
    QTextStream std_in(in);
    unsigned int baud=0;
    int ListBaud[]={9600, 115200};
    do {
    std_out << QObject::tr("Seleziona buad:") << endl
	<< QObject::tr("[0] 9600") << endl 
	<< QObject::tr("[1] 115200") << endl
	<< QObject::tr(">>") << flush
	;
    std_in >> baud;
    }while( baud >=2);
    
    ToConfigure->setBaudRate(ListBaud[baud]);
 
    ToConfigure->setDataBits(QSerialPort::Data8);
 
    ToConfigure->setParity(QSerialPort::NoParity);
 
    ToConfigure->setStopBits(QSerialPort::OneStop); 
   
    ToConfigure->setFlowControl(QSerialPort::NoFlowControl); 
  
  }