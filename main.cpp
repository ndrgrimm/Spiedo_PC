#include <QtCore>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtCore/QDateTime>

#include "Funzioni.h"
#include <fstream>


int main(int argc, char** argv){
  
  
  QCoreApplication app(argc, argv);
  QTextStream standard_out(stdout);
  QTextStream standard_in(stdin);
  QTextStream file_out(stdout);
  
  QDateTime logTime=QDateTime::currentDateTime();
  
  
  QSerialPort * SerialSelected=Connection();
  if( SerialSelected == 0 )  return 1;
  ConfigurePort(SerialSelected);
  
  standard_out << QObject::tr("Serial is Configured") << endl;

  if( !SerialSelected->open(QIODevice::ReadWrite) ) {
    standard_out << QObject::tr(" Failed to open port %1, error: %2").arg(SerialSelected->portName()).arg(SerialSelected->errorString()) << endl;
    return 1;
    
  }else{
  standard_out << QObject::tr("Serial is open.") << endl;
  }
  
  SerialSelected->setRequestToSend(true);
  SerialSelected->setRequestToSend(false);
  
  standard_out << QObject::tr("Serial is Configuration:") << endl
               << QObject::tr("Baud rate: ") <<   SerialSelected->baudRate()  << endl
	       << QObject::tr("DataBit: ") <<   SerialSelected->dataBits() << endl
	       << QObject::tr("Parity: ") << ( SerialSelected->parity() )<< endl
               << QObject::tr("Stop Bits: ") << ( SerialSelected->stopBits() )<< endl
	       << QObject::tr("FlowControl:  ") << ( SerialSelected->flowControl() )<< endl
	       
	       ;
  SerialSelected->setRequestToSend(true);
  SerialSelected->setRequestToSend(false);

                                                                                 
  char buf[64];                                                                    
  qint64 leghtbuf=0;
  while( ( SerialSelected->waitForReadyRead(10000) ) ){
    
    if( SerialSelected->canReadLine() ){
      leghtbuf=SerialSelected->readLine(buf,sizeof(buf));
      standard_out << buf << endl;
      if( leghtbuf >= 8 ) break;
      
    }
    standard_out << QObject::tr("Waiting for sketchCode") << endl;

  }
  
  if( leghtbuf == 0 ){
   standard_out << QObject::tr("Error: %1").arg(SerialSelected->errorString() ) << endl
		<< QObject::tr(" No sketchCode :(") << endl;
   return 1;
  }else{
    standard_out << QObject::tr("SketchCode recived!") << endl
		 << QObject::tr(" Code: %1 ").arg(buf) << endl
 		 << QObject::tr("Is it correct[Y/n]?\n>>") << flush
 		 ;
  }
  QString tmp;
  tmp=standard_in.readLine();
  if(  tmp[0] == 'n' ) return 0;
  SerialSelected->write("~");
  SerialSelected->flush();
  SerialSelected->waitForReadyRead(1000);
  SerialSelected->readAll();

  standard_out << QObject::tr("HandShake Terminate.") << endl;
  
  
  
  QByteArray Data;
  QString nameFile;
  char command=' ';
  bool exit=false;
  
  
  standard_out << QObject::tr("Controllo della Seriale:") << endl
		     << QObject::tr("\t[r] Campiona per 1000 volte e restiusce la seguente stringa:") << endl
		     << QObject::tr("\t\t dutyCicle:Mean:Mean^2") << endl
		     << QObject::tr("\t[w] Modifica il duty Cicle") << endl
		     << QObject::tr("\t[a] Fa una scansione completa sul duty Cicle da 0 a 255\n")
		     << QObject::tr("\t\t eseguendo ogni volta la lettura( vedi [r] )")<< endl
		     << QObject::tr("\t[h] restituisce questo OutPut") << endl
		     << QObject::tr("\t[s] Salva su File") << endl
		     << QObject::tr("\t[q] Salva  Esce") << endl
		     ;
  while(true){
    standard_out << ">>" << flush;
    standard_in >> command;
    switch(command){
      case 'r':
	SerialSelected->write("r");
	SerialSelected->flush();
	SerialSelected->clear();
	while( ( SerialSelected->waitForReadyRead(10000) ) ){
    
	  if( SerialSelected->canReadLine() ){
	   
	    standard_out << SerialSelected->readAll() << flush;
	    
	    break;
	  }
	}
	SerialSelected->clear();
	SerialSelected->readAll();

        break;
	
      case 'w':
	SerialSelected->write("w");
	SerialSelected->flush();
	standard_out << QObject::tr("Inserire il valore della duty Cicle\n>>") << flush;
	standard_in >> tmp;
	SerialSelected->write(tmp.toStdString().c_str());
	SerialSelected->flush();
	SerialSelected->readAll();
	break;
      case 'a':
        SerialSelected->write("a");
	SerialSelected->flush();
	SerialSelected->clear();

	Data.append("#").append(logTime.toString("ddMMyyyy hhmmss") ).append("#\n");
	while( ( SerialSelected->waitForReadyRead(10000) ) ){
    
	    Data.append( SerialSelected->readAll() );
	}
	SerialSelected->readAll();

	break;
      case 's':
	save:
	if ( Data.isEmpty() ){
	  standard_out << QObject::tr("Nothing to save") << endl;
	  break;
	}
	standard_out << QObject::tr("Nome del file: ") << flush;
	
	standard_in >> nameFile;
	file_out << Data << flush;
	break;
	
	
      case 'q':
	SerialSelected->write("q");
	SerialSelected->flush();
	SerialSelected->clear();

	exit=true;
	goto save;
	
	break;
	
      case 'h':
	standard_out << QObject::tr("Controllo della Seriale:") << endl
		     << QObject::tr("\t[r] Campiona per 1000 volte e restiusce la seguente stringa:") << endl
		     << QObject::tr("\t\t dutyCicle:Mean:Mean^2") << endl
		     << QObject::tr("\t[w] Modifica il duty Cicle") << endl
		     << QObject::tr("\t[a] Fa una scansione completa sul duty Cicle da 0 a 255\n")
		     << QObject::tr("\t\t eseguendo ogni volta la lettura( vedi [r] )")<< endl
		     << QObject::tr("\t[h] restituisce questo OutPut") << endl
		     << QObject::tr("\t[s] Salva su File") << endl
		     << QObject::tr("\t[q] Salva  Esce") << endl
		     ;
	break;
      default:
	if(exit) break;
    }
    if(exit) break;
    
    
  }
  return 0;
}
